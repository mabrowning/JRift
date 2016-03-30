#include "de_fruitfly_ovr_OculusRift.h"

#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#if defined(OVR_OS_WIN32)
#include "Windows.h"
#endif
#include "OVR_CAPI_GL.h"
#include "GL\CAPI_GLE_GL.h"

using namespace OVR;

ovrSession          _pHmdSession        = 0;
ovrHmdDesc          _hmdDesc; 
bool                _initialised        = false;
bool                _performedFirstInit = false;

const std::string   UNKNOWN_RUNTIME_VER = "<Unknown>";
const std::string   _NO_ERROR           = "<No error>";
std::string         _sRuntimeVersion    = UNKNOWN_RUNTIME_VER;

const int32_t ovrSuccess_SessionShouldQuit  = 9999;

struct ErrorInfo 
{
	ErrorInfo()
	{
		sError = _NO_ERROR;
		ovr_result = ovrSuccess;
		Success = true;
		UnqualifiedSuccess = true;
	}

	std::string sError;
	ovrResult   ovr_result;
	bool        Success;
	bool        UnqualifiedSuccess;
};

ErrorInfo            _lastError;
ovrTextureSwapChain  _pRenderTextureSet[2];
ovrSizei             _RenderTextureSize[2];
ovrMirrorTexture     _pMirrorTexture = 0;
GLuint               _mirrorTexId = -1;
ovrTextureSwapChain  _pDepthTextureSet[2];
//ovrGLTexture         _DepthTexture[2];

ovrTrackingState    _hmdTrackerState;
ovrTrackerPose      _hmdTrackerPoses;
ovrTrackerDesc      _trackerDesc;
uint32_t            _trackerCount;
ovrInputState       _inputState;
ovrPosef            _eyeRenderPose[2];
//ovrGLTexture        _GLEyeTexture[2];
ovrEyeRenderDesc    _EyeRenderDesc[2];
double              _sensorSampleTime  = 0.0;
ovrTimewarpProjectionDesc _PosTimewarpProjectionDesc;
float               _worldScale;

std::map<int32_t, std::string> _ErrorMap;
std::map<int32_t, std::string> _SuccessMap;

const Vector3f		UpVector(0.0f, 1.0f, 0.0f);
const Vector3f		ForwardVector(0.0f, 0.0f, -1.0f);

// JNI class / method caching
static jclass       eyeRenderParams_Class                   = 0;
static jmethodID    eyeRenderParams_constructor_MethodID    = 0;
static jclass       frameTiming_Class                       = 0;
static jmethodID    frameTiming_constructor_MethodID        = 0;
static jclass       posef_Class                             = 0;
static jmethodID    posef_constructor_MethodID              = 0;
static jclass       trackerState_Class                      = 0;
static jmethodID    trackerState_constructor_MethodID       = 0;
static jclass       sizei_Class                             = 0;
static jmethodID    sizei_constructor_MethodID              = 0;
static jclass       renderTextureInfo_Class                 = 0;
static jmethodID    renderTextureInfo_constructor_MethodID  = 0;
static jclass       hmdDesc_Class                           = 0;
static jmethodID    hmdDesc_constructor_MethodID            = 0;
static jclass       vector3f_Class                          = 0;
static jmethodID    vector3f_constructor_MethodID           = 0;
static jclass       matrix4f_Class                          = 0;
static jmethodID    matrix4f_constructor_MethodID           = 0;
static jclass       userProfileData_Class                   = 0;
static jmethodID    userProfileData_constructor_MethodID    = 0;
static jclass       fullPoseState_Class                     = 0;
static jmethodID    fullPoseState_constructor_MethodID      = 0;
static jclass       renderTextureSet_Class                  = 0;
static jmethodID    renderTextureSet_constructor_MethodID   = 0;
static jclass       eulerOrient_Class                       = 0;
static jmethodID    eulerOrient_constructor_MethodID        = 0;
static jclass       arrayListClass						    = 0;
static jmethodID    method_arrayList_init                   = 0;
static jmethodID    method_arrayList_add                    = 0;
static jclass       integerClass                            = 0;
static jmethodID    method_integer_init                     = 0;
static jclass       errorInfo_Class                         = 0;
static jmethodID    errorInfo_constructor_MethodID          = 0;
static jclass       quatf_Class                             = 0;
static jmethodID    quatf_constructor_MethodID              = 0;

static jfieldID     field_renderTextureSet_leftEyeTextureIds  = 0;
static jfieldID     field_renderTextureSet_rightEyeTextureIds = 0;

/* 
Initialises 
   - the LibOVR client -> RT connection
   - the HMD device session
   - gets the HMD render parameters
*/
JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1initSubsystem(JNIEnv *env, jobject jobj) 
{
	jobject errorInfo = 0;
	ovrGraphicsLuid luid;
	ovrInitParams initParams;
	memset(&initParams, 0, sizeof(ovrInitParams));
	ovrResult ovr_result = ovrSuccess;

	// Do any lib first init
	if (!LibFirstInit(env))
	{
		SetGenericErrorInfo(env, "Failed libFirstInit()");
		return false;
	}


	// Ensure we have a clean state
	Reset();


	// Initialise LibOVR with default params
	ovr_result = ovr_Initialize(&initParams);
	if (OVR_FAILURE(ovr_result)) 
	{
		_SetErrorInfo(env, "Unable to initialise LibOVR!", ovr_result);
		return false;
	}


	// Get RT version
	_sRuntimeVersion = ovr_GetVersionString();
	printf("Initialised LibOVR! Client SDK version %s, Runtime version %s\n", OVR_VERSION_STRING, _sRuntimeVersion.c_str());


	// Create the HMD session (HMD must be present or a debug device enabled)
	ovr_result = ovr_Create(&_pHmdSession, &luid);
	if (OVR_FAILURE(ovr_result))
	{
		_SetErrorInfo(env, "Unable to connect to HMD!", ovr_result);
		return false;
	}


	// Get the HMD and HMD tracker configuration parameters
	_hmdDesc = ovr_GetHmdDesc(_pHmdSession);
	_trackerCount = ovr_GetTrackerCount(_pHmdSession);  // TODO: Support multiple trackers, and poll when needed, not at startup
	if (_trackerCount > 0)
		_trackerDesc = ovr_GetTrackerDesc(_pHmdSession, 0); 
	ovr_SetTrackingOriginType(_pHmdSession, ovrTrackingOrigin_EyeLevel); // TODO: Allow selection of ovrTrackingOrigin_FloorLevel for standing experiences?


	// Get initial HMD render params - this may change at any time however
	_EyeRenderDesc[0] = ovr_GetRenderDesc(_pHmdSession, ovrEye_Left,  _hmdDesc.DefaultEyeFov[0]);
    _EyeRenderDesc[1] = ovr_GetRenderDesc(_pHmdSession, ovrEye_Right, _hmdDesc.DefaultEyeFov[1]);


	printf("Rift device found!\n");
	printf(" Product Name:      %s\n", _hmdDesc.ProductName);
	printf(" Manufacturer:      %s\n", _hmdDesc.Manufacturer);
	printf(" Native Resolution: %d X %d\n", _hmdDesc.Resolution.w, _hmdDesc.Resolution.h);


	_initialised = true;


	_SetErrorInfo(env, "Initialised Rift successfully!", ovr_result);
	return true;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getLastError(JNIEnv *env, jobject jobj) 
{
	return GetLastErrorInfo(env);
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroySubsystem(JNIEnv *env, jobject jobj) 
{
	printf("Destroying Oculus Rift device interface.\n");	
	Reset();
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getHmdParameters(JNIEnv *env, jobject) 
{
	if (!_initialised) 
        return 0;

    jstring productName = env->NewStringUTF( _hmdDesc.ProductName == NULL ? "" : _hmdDesc.ProductName );
    jstring manufacturer = env->NewStringUTF( _hmdDesc.Manufacturer == NULL ? "" : _hmdDesc.Manufacturer );
	jstring serialnumber = env->NewStringUTF( _hmdDesc.SerialNumber == NULL ? "" : _hmdDesc.SerialNumber );
    
    ClearException(env);

    jobject jHmdDesc = env->NewObject(hmdDesc_Class, hmdDesc_constructor_MethodID,
                                      (int)_hmdDesc.Type,
									  productName,
                                      manufacturer,
									  (int)_hmdDesc.VendorId,
									  (int)_hmdDesc.ProductId,
									  serialnumber,
									  (int)_hmdDesc.FirmwareMajor,
									  (int)_hmdDesc.FirmwareMinor,
									  _trackerDesc.FrustumHFovInRadians,
									  _trackerDesc.FrustumVFovInRadians,
									  _trackerDesc.FrustumNearZInMeters,
									  _trackerDesc.FrustumFarZInMeters,
                                      (int)_hmdDesc.AvailableHmdCaps,
									  (int)_hmdDesc.DefaultHmdCaps,
                                      (int)_hmdDesc.AvailableTrackingCaps,
                                      (int)_hmdDesc.DefaultTrackingCaps,
                                      _hmdDesc.DefaultEyeFov[0].UpTan,
                                      _hmdDesc.DefaultEyeFov[0].DownTan,
                                      _hmdDesc.DefaultEyeFov[0].LeftTan,
                                      _hmdDesc.DefaultEyeFov[0].RightTan,
                                      _hmdDesc.DefaultEyeFov[1].UpTan,
                                      _hmdDesc.DefaultEyeFov[1].DownTan,
                                      _hmdDesc.DefaultEyeFov[1].LeftTan,
                                      _hmdDesc.DefaultEyeFov[1].RightTan,
                                      _hmdDesc.MaxEyeFov[0].UpTan,
                                      _hmdDesc.MaxEyeFov[0].DownTan,
                                      _hmdDesc.MaxEyeFov[0].LeftTan,
                                      _hmdDesc.MaxEyeFov[0].RightTan,
                                      _hmdDesc.MaxEyeFov[1].UpTan,
                                      _hmdDesc.MaxEyeFov[1].DownTan,
                                      _hmdDesc.MaxEyeFov[1].LeftTan,
                                      _hmdDesc.MaxEyeFov[1].RightTan,
									  _hmdDesc.Resolution.w,
                                      _hmdDesc.Resolution.h,
									  _hmdDesc.DisplayRefreshRate
									  );

    env->DeleteLocalRef( productName );
    env->DeleteLocalRef( manufacturer );
    env->DeleteLocalRef( serialnumber );

    if (jHmdDesc == 0) PrintNewObjectException(env, "HmdParameters");

    return jHmdDesc;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1resetTracking(JNIEnv *env, jobject) 
{
	ResetTracking();
}

void ResetTracking()
{
	if (!_initialised)
		return;

    ovr_RecenterTrackingOrigin(_pHmdSession);
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1configureRenderer(
	JNIEnv *env,
    jobject,
    jfloat worldScale)
{
	if (!_initialised)
		return;

	_worldScale = worldScale;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getRenderTextureSize(
	JNIEnv *env, 
	jobject, 
	jfloat leftFovUpTan,
	jfloat leftFovDownTan,
	jfloat leftFovLeftTan,
	jfloat leftFovRightTan,
	jfloat rightFovUpTan,
	jfloat rightFovDownTan,
	jfloat rightFovLeftTan,
	jfloat rightFovRightTan,
	jfloat RenderScaleFactor)
{
	if (!_initialised)
		return 0;

	ovrFovPort leftFov;
	ovrFovPort rightFov;
	leftFov.UpTan     = leftFovUpTan;
	leftFov.DownTan   = leftFovDownTan;
	leftFov.LeftTan   = leftFovLeftTan;
	leftFov.RightTan  = leftFovRightTan;
	rightFov.UpTan    = rightFovUpTan;
	rightFov.DownTan  = rightFovDownTan;
	rightFov.LeftTan  = rightFovLeftTan;
	rightFov.RightTan = rightFovRightTan;

	// A RenderScaleFactor of 1.0f signifies default (non-scaled) operation
    Sizei recommendedTex0Size = ovr_GetFovTextureSize(_pHmdSession, ovrEye_Left,  leftFov, RenderScaleFactor);
    Sizei recommendedTex1Size = ovr_GetFovTextureSize(_pHmdSession, ovrEye_Right, rightFov, RenderScaleFactor);
    Sizei RenderTargetSize;
    RenderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
    RenderTargetSize.h = (std::max) ( recommendedTex0Size.h, recommendedTex1Size.h );

    ClearException(env);

    jobject jrenderTextureInfo = env->NewObject(renderTextureInfo_Class, renderTextureInfo_constructor_MethodID,
									recommendedTex0Size.w,
									recommendedTex0Size.h,
									recommendedTex1Size.w,
									recommendedTex1Size.h,
                                    RenderTargetSize.w,
                                    RenderTargetSize.h,
									_hmdDesc.Resolution.w,
									_hmdDesc.Resolution.h,
                                    (float)RenderScaleFactor
                                    );

    if (jrenderTextureInfo == 0) PrintNewObjectException(env, "RenderTextureInfo");

    return jrenderTextureInfo;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1createRenderTextureSet(
	JNIEnv *env, 
	jobject,
	jint lwidth,
	jint lheight,
    jint rwidth,
    jint rheight
	)
{
	ovrResult ovr_result = ovrSuccess;

	if (!_initialised)
		return 0;
	
	DestroyRenderTextureSet();

	boolean Result = true;

	_RenderTextureSize[0].w = lwidth;
	_RenderTextureSize[0].h = lheight;
	_RenderTextureSize[1].w = rwidth;
	_RenderTextureSize[1].h = rheight;

	ovrTextureSwapChainDesc ldesc = {};
	ldesc.Type = ovrTexture_2D;
	ldesc.ArraySize = 1;
	ldesc.Width = lwidth;
	ldesc.Height = lheight;
	ldesc.MipLevels = 1;
	ldesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	ldesc.SampleCount = 1;
	ldesc.StaticImage = ovrFalse;

	ovrTextureSwapChainDesc rdesc = {};
	rdesc.Type = ovrTexture_2D;
	rdesc.ArraySize = 1;
	rdesc.Width = rwidth;
	rdesc.Height = rheight;
	rdesc.MipLevels = 1;
	rdesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	rdesc.SampleCount = 1;
	rdesc.StaticImage = ovrFalse;

	// Construct a new RenderTextureSet object
	ClearException(env);
	jobject jrenderTextureSet = env->NewObject(renderTextureSet_Class, renderTextureSet_constructor_MethodID);
	if (jrenderTextureSet == 0) PrintNewObjectException(env, "RenderTextureSet");

	jobject leftEyeTextureIds = env->GetObjectField(jrenderTextureSet, field_renderTextureSet_leftEyeTextureIds);
	jobject rightEyeTextureIds = env->GetObjectField(jrenderTextureSet, field_renderTextureSet_rightEyeTextureIds);

	// Create the texture sets

	// Left eye
	ovr_result = ovr_CreateTextureSwapChainGL(_pHmdSession, &ldesc, &_pRenderTextureSet[0]);
	if (OVR_FAILURE(ovr_result))
	{
		Result = false;	
	}

	if (Result)
	{
		int length = 0;
        ovr_GetTextureSwapChainLength(_pHmdSession, _pRenderTextureSet[0], &length);
        for (int i = 0; i < length; ++i)
        {
            GLuint chainTexId;
            ovr_GetTextureSwapChainBufferGL(_pHmdSession, _pRenderTextureSet[0], i, &chainTexId);
			jobject texIdInt = env->NewObject(integerClass, method_integer_init, (jint)chainTexId);
		    env->CallBooleanMethod(leftEyeTextureIds, method_arrayList_add, texIdInt);
		}
	}

	// Right eye
	if (Result)
	{
		ovr_result = ovr_CreateTextureSwapChainGL(_pHmdSession, &rdesc, &_pRenderTextureSet[1]);
		if (OVR_FAILURE(ovr_result))
		{
			Result = false;	
		}

		if (Result)
		{
			int length = 0;
			ovr_GetTextureSwapChainLength(_pHmdSession, _pRenderTextureSet[1], &length);
			for (int i = 0; i < length; ++i)
			{
				GLuint chainTexId;
				ovr_GetTextureSwapChainBufferGL(_pHmdSession, _pRenderTextureSet[0], i, &chainTexId);
				jobject texIdInt = env->NewObject(integerClass, method_integer_init, (jint)chainTexId);
				env->CallBooleanMethod(rightEyeTextureIds, method_arrayList_add, texIdInt);
			}
		}	
	}

	if (!Result)
	{
		_SetErrorInfo(env, "Unable to create render texture set!", ovr_result);

		DestroyRenderTextureSet();
		env->DeleteLocalRef(jrenderTextureSet);
		return 0;
	}

	_SetErrorInfo(env, "Created render texture set successfully!", ovr_result);
	return jrenderTextureSet;
}

JNIEXPORT jint JNICALL Java_de_fruitfly_ovr_OculusRift__1getCurrentEyeRenderTextureId(
	JNIEnv *env,
    jobject,
    jint Eye)
{
    if (!_initialised)
    {
        return 0;
    }

	GLuint curTexId = 0;

	// Normalise eye
	ovrEyeType eye = ovrEye_Left;
	if (Eye == 1)
		eye = ovrEye_Right;

	// Get current eye render texture
	if (_pRenderTextureSet[eye] != 0)
	{
		int curIndex = 0;
		ovr_GetTextureSwapChainCurrentIndex(_pHmdSession, _pRenderTextureSet[eye], &curIndex);
		ovr_GetTextureSwapChainBufferGL(_pHmdSession, _pRenderTextureSet[eye], curIndex, &curTexId);
	}

	return curTexId;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1commitCurrentEyeRenderTexture(
	JNIEnv *env,
    jobject,
    jint Eye)
{
    if (!_initialised)
    {
        return;
    }

	// Normalise eye
	ovrEyeType eye = ovrEye_Left;
	if (Eye == 1)
		eye = ovrEye_Right;

	// Commit current eye render texture
	if (_pRenderTextureSet[eye] != 0)
	{
		ovr_CommitTextureSwapChain(_pHmdSession, _pRenderTextureSet[eye]);
	}
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroyRenderTextureSet
(JNIEnv *env, jobject)
{
    DestroyRenderTextureSet();
}

JNIEXPORT jint JNICALL Java_de_fruitfly_ovr_OculusRift__1createMirrorTexture(
	JNIEnv *env, 
	jobject,
	jint width,
	jint height
	)
{
	if (!_initialised)
		return 0;

	DestroyMirrorTexture();

	ovrMirrorTextureDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.Width = width;
    desc.Height = height;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	ovrResult ovr_result = ovr_CreateMirrorTextureGL(_pHmdSession, &desc, &_pMirrorTexture);
	if (OVR_FAILURE(ovr_result))
	{
		_SetErrorInfo(env, "Unable to create mirror texture!", ovr_result);

		_pMirrorTexture = 0;
		return -1;
	}

	_SetErrorInfo(env, "Created mirror texture successfully!", ovr_result);

	// Just return the texture ID
    ovr_GetMirrorTextureBufferGL(_pHmdSession, _pMirrorTexture, &_mirrorTexId);
	return _mirrorTexId;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroyMirrorTexture
(JNIEnv *env, jobject)
{
    DestroyMirrorTexture();
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getTrackedPoses(
	JNIEnv *env, 
	jobject, 
	jlong FrameIndex
	)
{
    if (!_initialised)
        return 0;

	// Update render params
	_EyeRenderDesc[0] = ovr_GetRenderDesc(_pHmdSession, ovrEye_Left,  _hmdDesc.DefaultEyeFov[0]);
    _EyeRenderDesc[1] = ovr_GetRenderDesc(_pHmdSession, ovrEye_Right, _hmdDesc.DefaultEyeFov[1]);

	// Use mandated view offsets
	ovrVector3f ViewOffsets[2] = { _EyeRenderDesc[0].HmdToEyeOffset,
                                   _EyeRenderDesc[1].HmdToEyeOffset };

	// Get eye poses at our predicted display times
	double ftiming = ovr_GetPredictedDisplayTime(_pHmdSession, FrameIndex);
    _sensorSampleTime = ovr_GetTimeInSeconds();
	//ovr_GetInputState(_pHmdSession, ovrControllerType_All, &_inputState);
    _hmdTrackerState = ovr_GetTrackingState(_pHmdSession, ftiming, ovrTrue);
	if (_trackerCount > 0)
		_hmdTrackerPoses = ovr_GetTrackerPose(_pHmdSession, 0); // TODO: Get all tracker states

    ovr_CalcEyePoses(_hmdTrackerState.HeadPose.ThePose, ViewOffsets, _eyeRenderPose);
	double PredictedDisplayTime = ovr_GetPredictedDisplayTime(_pHmdSession, FrameIndex);

    ClearException(env);
	jobject jfullposestate = env->NewObject(fullPoseState_Class, fullPoseState_constructor_MethodID,
                                 FrameIndex,
								 _eyeRenderPose[0].Orientation.x,
								 _eyeRenderPose[0].Orientation.y,
								 _eyeRenderPose[0].Orientation.z,
								 _eyeRenderPose[0].Orientation.w,
								 _eyeRenderPose[0].Position.x,
								 _eyeRenderPose[0].Position.y,
								 _eyeRenderPose[0].Position.z,
								 _eyeRenderPose[1].Orientation.x,
								 _eyeRenderPose[1].Orientation.y,
								 _eyeRenderPose[1].Orientation.z,
								 _eyeRenderPose[1].Orientation.w,
								 _eyeRenderPose[1].Position.x,
								 _eyeRenderPose[1].Position.y,
								 _eyeRenderPose[1].Position.z,
								 _hmdTrackerState.HeadPose.ThePose.Orientation.x,   
								 _hmdTrackerState.HeadPose.ThePose.Orientation.y,  
								 _hmdTrackerState.HeadPose.ThePose.Orientation.z,   
								 _hmdTrackerState.HeadPose.ThePose.Orientation.w,   
								 _hmdTrackerState.HeadPose.ThePose.Position.x,      
								 _hmdTrackerState.HeadPose.ThePose.Position.y,      
								 _hmdTrackerState.HeadPose.ThePose.Position.z,      
								 _hmdTrackerState.HeadPose.AngularVelocity.x,    
								 _hmdTrackerState.HeadPose.AngularVelocity.y,    
								 _hmdTrackerState.HeadPose.AngularVelocity.z,    
								 _hmdTrackerState.HeadPose.LinearVelocity.x,     
								 _hmdTrackerState.HeadPose.LinearVelocity.y,     
								 _hmdTrackerState.HeadPose.LinearVelocity.z,     
								 _hmdTrackerState.HeadPose.AngularAcceleration.x,
								 _hmdTrackerState.HeadPose.AngularAcceleration.y,
								 _hmdTrackerState.HeadPose.AngularAcceleration.z,
								 _hmdTrackerState.HeadPose.LinearAcceleration.x, 
								 _hmdTrackerState.HeadPose.LinearAcceleration.y, 
								 _hmdTrackerState.HeadPose.LinearAcceleration.z, 
								 _hmdTrackerState.HeadPose.TimeInSeconds,        
								 0, // TODO: Remove
								 _hmdTrackerState.StatusFlags,
								 _hmdTrackerPoses.Pose.Orientation.x,   
								 _hmdTrackerPoses.Pose.Orientation.y,  
								 _hmdTrackerPoses.Pose.Orientation.z,   
								 _hmdTrackerPoses.Pose.Orientation.w,   
								 _hmdTrackerPoses.Pose.Position.x,      
								 _hmdTrackerPoses.Pose.Position.y,      
								 _hmdTrackerPoses.Pose.Position.z,
								 _hmdTrackerPoses.LeveledPose.Orientation.x,   
								 _hmdTrackerPoses.LeveledPose.Orientation.y,  
								 _hmdTrackerPoses.LeveledPose.Orientation.z,   
								 _hmdTrackerPoses.LeveledPose.Orientation.w,   
								 _hmdTrackerPoses.LeveledPose.Position.x,      
								 _hmdTrackerPoses.LeveledPose.Position.y,      
								 _hmdTrackerPoses.LeveledPose.Position.z,
								 _hmdTrackerState.HandPoses[0].ThePose.Orientation.x,   
								 _hmdTrackerState.HandPoses[0].ThePose.Orientation.y,  
								 _hmdTrackerState.HandPoses[0].ThePose.Orientation.z,   
								 _hmdTrackerState.HandPoses[0].ThePose.Orientation.w,   
								 _hmdTrackerState.HandPoses[0].ThePose.Position.x,      
								 _hmdTrackerState.HandPoses[0].ThePose.Position.y,      
								 _hmdTrackerState.HandPoses[0].ThePose.Position.z,
								 _hmdTrackerState.HandStatusFlags[0],
								 _hmdTrackerState.HandPoses[1].ThePose.Orientation.x,   
								 _hmdTrackerState.HandPoses[1].ThePose.Orientation.y,  
								 _hmdTrackerState.HandPoses[1].ThePose.Orientation.z,   
								 _hmdTrackerState.HandPoses[1].ThePose.Orientation.w,   
								 _hmdTrackerState.HandPoses[1].ThePose.Position.x,      
								 _hmdTrackerState.HandPoses[1].ThePose.Position.y,      
								 _hmdTrackerState.HandPoses[1].ThePose.Position.z,
								 _hmdTrackerState.HandStatusFlags[1],
								 0, // TODO: Remove
								 PredictedDisplayTime);
	
	return jfullposestate;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getMatrix4fProjection(
    JNIEnv *env, 
    jobject, 
    jfloat EyeFovPortUpTan,
    jfloat EyeFovPortDownTan,
    jfloat EyeFovPortLeftTan,
    jfloat EyeFovPortRightTan,
    jfloat nearClip,
    jfloat farClip)
{
    if (!_initialised)
        return 0;

    ovrFovPort fov;
    fov.UpTan    = EyeFovPortUpTan;
    fov.DownTan  = EyeFovPortDownTan;
    fov.LeftTan  = EyeFovPortLeftTan;
    fov.RightTan = EyeFovPortRightTan;

	unsigned int projectionModifier = ovrProjection_ClipRangeOpenGL;
    Matrix4f proj = ovrMatrix4f_Projection(fov, nearClip, farClip, projectionModifier); // RH for OGL (by default)
	_PosTimewarpProjectionDesc = ovrTimewarpProjectionDesc_FromProjection(proj, projectionModifier);

    ClearException(env);
    jobject jproj = env->NewObject(matrix4f_Class, matrix4f_constructor_MethodID,
                                   proj.M[0][0], proj.M[0][1], proj.M[0][2], proj.M[0][3],
                                   proj.M[1][0], proj.M[1][1], proj.M[1][2], proj.M[1][3],
                                   proj.M[2][0], proj.M[2][1], proj.M[2][2], proj.M[2][3],
                                   proj.M[3][0], proj.M[3][1], proj.M[3][2], proj.M[3][3]
                                   );
	if (jproj == 0) PrintNewObjectException(env, "Matrix4f");

    return jproj;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1submitFrame(
	JNIEnv *env,
	jobject)
{
    if (!_initialised)
        return 0;

    ovrViewScaleDesc viewScaleDesc;
    viewScaleDesc.HmdSpaceToWorldScaleInMeters = _worldScale;   
    viewScaleDesc.HmdToEyeOffset[0] = _EyeRenderDesc[0].HmdToEyeOffset;
    viewScaleDesc.HmdToEyeOffset[1] = _EyeRenderDesc[1].HmdToEyeOffset;
  
    ovrLayer_Union EyeLayer;
	
	// TODO: Get this working when Oculus get pos track timewarp working again
	//ovrGLTexture* pDepthTexture[2];
	//pDepthTexture[0] = (ovrGLTexture*)&_pDepthTextureSet[0].Textures[0];
	//pDepthTexture[1] = (ovrGLTexture*)&_pDepthTextureSet[1].Textures[0];
	//bool HasDepth = pDepthTexture[0]->OGL.TexId == -1 ? false : true;

    //EyeLayer.Header.Type  = HasDepth == true ? ovrLayerType_EyeFovDepth : ovrLayerType_EyeFov;
	EyeLayer.Header.Type  = ovrLayerType_EyeFov;
    EyeLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
    
    for (int eye = 0; eye < 2; ++eye)
    {
        EyeLayer.EyeFov.ColorTexture[eye] = _pRenderTextureSet[eye];
        EyeLayer.EyeFov.Fov[eye]          = _hmdDesc.DefaultEyeFov[eye];
        EyeLayer.EyeFov.RenderPose[eye]   = _eyeRenderPose[eye];
        EyeLayer.EyeFov.SensorSampleTime  = _sensorSampleTime;
		EyeLayer.EyeFov.Viewport[eye]     = Recti(0,0,_RenderTextureSize[eye].w,_RenderTextureSize[eye].h);

		//if (HasDepth)
		//{
		//	EyeLayer.EyeFovDepth.DepthTexture[eye] = &_pDepthTextureSet[eye];
        //    EyeLayer.EyeFovDepth.ProjectionDesc    = _PosTimewarpProjectionDesc;
		//}
    }

    ovrLayerHeader* layers = &EyeLayer.Header;
    ovrResult ovr_result = ovr_SubmitFrame(_pHmdSession, 0, &viewScaleDesc, &layers, 1);

	// Get session status (TODO: Move to a better place)
    ovrSessionStatus sessionStatus;
    ovr_GetSessionStatus(_pHmdSession, &sessionStatus);
    if (sessionStatus.ShouldQuit)
        ovr_result = ovrSuccess_SessionShouldQuit;
    if (sessionStatus.ShouldRecenter)
        ResetTracking();

	if (OVR_FAILURE(ovr_result))
	{
		_SetErrorInfo(env, "Failed to submit frame!", ovr_result);
	}
	else
	{
		_SetErrorInfo(env, "Submitted frame successfully!", ovr_result);
	}
	
	return GetLastErrorInfo(env);
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1convertQuatToEuler
  (JNIEnv *env, 
  jobject, 
  jfloat quaternion_x, 
  jfloat quaternion_y, 
  jfloat quaternion_z, 
  jfloat quaternion_w, 
  jfloat scale, 
  jint firstRotationAxis, 
  jint secondRotationAxis, 
  jint thirdRotationAxis, 
  jint hand, 
  jint rotationDir)
{
	Quatf quat(quaternion_x, quaternion_y, quaternion_z, quaternion_w);
    ovrVector3f euler;
	Axis A1, A2, A3;
	RotateDirection D;
	HandedSystem S;

	SetEulerEnumValues(firstRotationAxis,
					   secondRotationAxis,
					   thirdRotationAxis,
					   rotationDir,
					   hand,
					   A1,
					   A2,
					   A3,
					   D,
					   S);
	
	if (scale != 1.0f)
		quat = quat.PowNormalized(scale);

	// Yes - this next bit is ridiculous. Why did they use templates? Or am
    // I missing the easy way to do this?!
	if (S == Handed_R)
	{
		if (D == Rotate_CCW)
		{
			// Handed_R, Rotate_CCW
			if (A1 == Axis_X)
			{
				if (A2 == Axis_Y)
				{
					quat.GetEulerAngles<Axis_X, Axis_Y, Axis_Z, Rotate_CCW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Z)
				{
					quat.GetEulerAngles<Axis_X, Axis_Z, Axis_Y, Rotate_CCW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
			}
			else if (A1 == Axis_Y)
			{
				if (A2 == Axis_X)
				{
					quat.GetEulerAngles<Axis_Y, Axis_X, Axis_Z, Rotate_CCW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Z)
				{
					quat.GetEulerAngles<Axis_Y, Axis_Z, Axis_X, Rotate_CCW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
			}
			else if (A1 == Axis_Z)
			{
				if (A2 == Axis_X)
				{
					quat.GetEulerAngles<Axis_Z, Axis_X, Axis_Y, Rotate_CCW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Y)
				{
					quat.GetEulerAngles<Axis_Z, Axis_Y, Axis_X, Rotate_CCW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
			}
		}
		else
		{
			// Handed_R, Rotate_CW
			if (A1 == Axis_X)
			{
				if (A2 == Axis_Y)
				{
					quat.GetEulerAngles<Axis_X, Axis_Y, Axis_Z, Rotate_CW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Z)
				{
					quat.GetEulerAngles<Axis_X, Axis_Z, Axis_Y, Rotate_CW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
			}
			else if (A1 == Axis_Y)
			{
				if (A2 == Axis_X)
				{
					quat.GetEulerAngles<Axis_Y, Axis_X, Axis_Z, Rotate_CW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Z)
				{
					quat.GetEulerAngles<Axis_Y, Axis_Z, Axis_X, Rotate_CW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
			}
			else if (A1 == Axis_Z)
			{
				if (A2 == Axis_X)
				{
					quat.GetEulerAngles<Axis_Z, Axis_X, Axis_Y, Rotate_CW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Y)
				{
					quat.GetEulerAngles<Axis_Z, Axis_Y, Axis_X, Rotate_CW, Handed_R>(&euler.x, &euler.y, &euler.z);
				}
			}
		}
	}
	else if (S == Handed_L)
	{
		if (D == Rotate_CCW)
		{
			// Handed_L, Rotate_CCW
			if (A1 == Axis_X)
			{
				if (A2 == Axis_Y)
				{
					quat.GetEulerAngles<Axis_X, Axis_Y, Axis_Z, Rotate_CCW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Z)
				{
					quat.GetEulerAngles<Axis_X, Axis_Z, Axis_Y, Rotate_CCW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
			}
			else if (A1 == Axis_Y)
			{
				if (A2 == Axis_X)
				{
					quat.GetEulerAngles<Axis_Y, Axis_X, Axis_Z, Rotate_CCW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Z)
				{
					quat.GetEulerAngles<Axis_Y, Axis_Z, Axis_X, Rotate_CCW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
			}
			else if (A1 == Axis_Z)
			{
				if (A2 == Axis_X)
				{
					quat.GetEulerAngles<Axis_Z, Axis_X, Axis_Y, Rotate_CCW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Y)
				{
					quat.GetEulerAngles<Axis_Z, Axis_Y, Axis_X, Rotate_CCW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
			}
		}
		else
		{
			// Handed_L, Rotate_CW
			if (A1 == Axis_X)
			{
				if (A2 == Axis_Y)
				{
					quat.GetEulerAngles<Axis_X, Axis_Y, Axis_Z, Rotate_CW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Z)
				{
					quat.GetEulerAngles<Axis_X, Axis_Z, Axis_Y, Rotate_CW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
			}
			else if (A1 == Axis_Y)
			{
				if (A2 == Axis_X)
				{
					quat.GetEulerAngles<Axis_Y, Axis_X, Axis_Z, Rotate_CW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Z)
				{
					quat.GetEulerAngles<Axis_Y, Axis_Z, Axis_X, Rotate_CW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
			}
			else if (A1 == Axis_Z)
			{
				if (A2 == Axis_X)
				{
					quat.GetEulerAngles<Axis_Z, Axis_X, Axis_Y, Rotate_CW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
				else if (A2 == Axis_Y)
				{
					quat.GetEulerAngles<Axis_Z, Axis_Y, Axis_X, Rotate_CW, Handed_L>(&euler.x, &euler.y, &euler.z);
				}
			}
		}
	}

    // Cache JNI objects here to prevent the need for initialisation
    if (!LookupJNIConstructorGlobal(env,
                         eulerOrient_Class,
                         "de/fruitfly/ovr/structs/EulerOrient",
                         eulerOrient_constructor_MethodID,
                         "(FFF)V"))
    {
        return 0;
    }

    jobject jeulerOrient = env->NewObject(eulerOrient_Class, eulerOrient_constructor_MethodID,
                                       euler.x,
                                       euler.y,
                                       euler.z
                                       );

	return jeulerOrient;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getUserProfileData(
   JNIEnv *env, jobject)
{
	if (!_initialised) 
        return 0;

/*
#define OVR_KEY_USER                        "User"
#define OVR_KEY_NAME                        "Name"
#define OVR_KEY_GENDER                      "Gender"
#define OVR_KEY_PLAYER_HEIGHT               "PlayerHeight"
#define OVR_KEY_EYE_HEIGHT                  "EyeHeight"
#define OVR_KEY_IPD                         "IPD"
#define OVR_KEY_NECK_TO_EYE_DISTANCE        "NeckEyeDistance"
#define OVR_KEY_EYE_RELIEF_DIAL             "EyeReliefDial"
#define OVR_KEY_EYE_TO_NOSE_DISTANCE        "EyeToNoseDist"
#define OVR_KEY_MAX_EYE_TO_PLATE_DISTANCE   "MaxEyeToPlateDist"
#define OVR_KEY_EYE_CUP                     "EyeCup"
#define OVR_KEY_CUSTOM_EYE_RENDER           "CustomEyeRender"

#define OVR_DEFAULT_GENDER                  "Male"
#define OVR_DEFAULT_PLAYER_HEIGHT           1.778f
#define OVR_DEFAULT_EYE_HEIGHT              1.675f
#define OVR_DEFAULT_IPD                     0.064f
#define OVR_DEFAULT_NECK_TO_EYE_HORIZONTAL  0.09f
#define OVR_DEFAULT_NECK_TO_EYE_VERTICAL    0.15f
#define OVR_DEFAULT_EYE_RELIEF_DIAL         3
*/

	float playerHeight = ovr_GetFloat( _pHmdSession, OVR_KEY_PLAYER_HEIGHT, OVR_DEFAULT_PLAYER_HEIGHT);
	float eyeHeight    = ovr_GetFloat( _pHmdSession, OVR_KEY_EYE_HEIGHT,    OVR_DEFAULT_EYE_HEIGHT); 
	//float ipd          = ovr_GetFloat( _pHmdSession, OVR_KEY_IPD,           OVR_DEFAULT_IPD); 
	std::string gender = ovr_GetString(_pHmdSession, OVR_KEY_GENDER,        OVR_DEFAULT_GENDER);
    std::string name   = ovr_GetString(_pHmdSession, OVR_KEY_NAME,          "No Profile");

	jstring jname   = env->NewStringUTF(name.c_str());
    jstring jgender = env->NewStringUTF(gender.c_str());

	jobject profileData = env->NewObject(userProfileData_Class, userProfileData_constructor_MethodID,
		playerHeight,
		eyeHeight,
		0,//ipd,
		jgender,
		true, // Always the default profile?
		jname
	);

    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jgender);

	return profileData;
}

JNIEXPORT jstring JNICALL Java_de_fruitfly_ovr_OculusRift__1getVersionString(
   JNIEnv *env, jobject)
{
    std::string version = ovr_GetVersionString();
    return env->NewStringUTF(version.c_str());
}

JNIEXPORT jdouble JNICALL Java_de_fruitfly_ovr_OculusRift__1getCurrentTimeSecs(
   JNIEnv *env, jobject)
{
    return ovr_GetTimeInSeconds();
}


/**** HELPERS ****/

void ClearException(JNIEnv *env)
{
    env->ExceptionClear();
}

void PrintNewObjectException(JNIEnv *env, std::string objectName)
{
    printf("Failed to create object '%s'", objectName.c_str());
    env->ExceptionDescribe();
    env->ExceptionClear();
}

void Reset()
{
	if (_initialised)
	{
		// Destroy render textures
		DestroyRenderTextureSet();

		// Destroy mirror texture
		DestroyMirrorTexture();

		// Cleanup HMD
		if (_pHmdSession)
			ovr_Destroy(_pHmdSession);

		// Shutdown LibOVR
		ovr_Shutdown();
	}

	_pHmdSession = 0;

	_pRenderTextureSet[0] = 0;
	_pRenderTextureSet[1] = 0;
	_pMirrorTexture     = 0;

    _eyeRenderPose[0].Orientation.x = 0.0;
    _eyeRenderPose[0].Orientation.y = 0.0;
    _eyeRenderPose[0].Orientation.z = 0.0;
    _eyeRenderPose[0].Orientation.w = 1.0;
    _eyeRenderPose[0].Position.x = 0.0;
    _eyeRenderPose[0].Position.y = 0.0;
    _eyeRenderPose[0].Position.z = 0.0;
    _eyeRenderPose[1] = _eyeRenderPose[0];

	_sRuntimeVersion            = UNKNOWN_RUNTIME_VER;
	_lastError.sError             = "No error";
	_lastError.ovr_result         = ovrSuccess;
	_lastError.Success            = OVR_SUCCESS(ovrSuccess);
	_lastError.UnqualifiedSuccess = OVR_UNQUALIFIED_SUCCESS(ovrSuccess);

	_worldScale = 1.0;

	_initialised = false;
}

void DestroyRenderTextureSet()
{
	if (!_initialised)
	{
		_pRenderTextureSet[0] = 0;
		_pRenderTextureSet[1] = 0;
		return;
	}

	if (_pRenderTextureSet[0] != 0)
	{		    
		ovr_DestroyTextureSwapChain(_pHmdSession, _pRenderTextureSet[0]);
		_pRenderTextureSet[0] = 0;
	}
	if (_pRenderTextureSet[1] != 0)
	{
		ovr_DestroyTextureSwapChain(_pHmdSession, _pRenderTextureSet[1]);
		_pRenderTextureSet[1] = 0;
	}
}

void DestroyMirrorTexture()
{
	if (!_initialised)
	{
		_pMirrorTexture = 0;
		_mirrorTexId = -1;
		return;
	}

	if (_pMirrorTexture != 0)
	{
		ovr_DestroyMirrorTexture(_pHmdSession, _pMirrorTexture);
		_pMirrorTexture = 0;
		_mirrorTexId = -1;
	}
}

bool CacheJNIGlobals(JNIEnv *env)
{
	bool Success = true;

    if (!LookupJNIConstructorGlobal(env,
                         sizei_Class,
                         "de/fruitfly/ovr/structs/Sizei",
                         sizei_constructor_MethodID,
                         "(II)V"))
    {
        Success = false;
    }

    if (!LookupJNIConstructorGlobal(env,
                         renderTextureInfo_Class,
                         "de/fruitfly/ovr/structs/RenderTextureInfo",
                         renderTextureInfo_constructor_MethodID,
                         "(IIIIIIIIF)V"))
    {
        Success = false;
    }

    if (!LookupJNIConstructorGlobal(env,
                         hmdDesc_Class,
                         "de/fruitfly/ovr/structs/HmdParameters",
                         hmdDesc_constructor_MethodID,
                         "(ILjava/lang/String;Ljava/lang/String;IILjava/lang/String;IIFFFFIIIIFFFFFFFFFFFFFFFFIIF)V"))
    {
        Success = false;
    }

    if (!LookupJNIConstructorGlobal(env,
                         vector3f_Class,
                         "de/fruitfly/ovr/structs/Vector3f",
                         vector3f_constructor_MethodID,
                         "(FFF)V"))
    {
        Success = false;
    }

    if (!LookupJNIConstructorGlobal(env,
                         eyeRenderParams_Class,
                         "de/fruitfly/ovr/EyeRenderParams",
                         eyeRenderParams_constructor_MethodID,
                         "(IIIIIFFFFIIIIFFFFFIIIIIFFFFIIIIFFFFFF)V"))
    {
        Success = false;
    }

    if (!LookupJNIConstructorGlobal(env,
                         posef_Class,
                         "de/fruitfly/ovr/structs/Posef",
                         posef_constructor_MethodID,
                         "(FFFFFFF)V"))
    {
        Success = false;
    }

    if (!LookupJNIConstructorGlobal(env,
                         matrix4f_Class,
                         "de/fruitfly/ovr/structs/Matrix4f",
                         matrix4f_constructor_MethodID,
                         "(FFFFFFFFFFFFFFFF)V"))
    {
        Success = false;
    }

    if (!LookupJNIConstructorGlobal(env,
                         userProfileData_Class,
                         "de/fruitfly/ovr/UserProfileData",
                         userProfileData_constructor_MethodID,
                         "(FFFLjava/lang/String;ZLjava/lang/String;)V"))
    {
        Success = false;
    }                   

    if (!LookupJNIConstructorGlobal(env,
                         quatf_Class,
                         "de/fruitfly/ovr/structs/Quatf",
                         quatf_constructor_MethodID,
                         "(FFFF)V"))
    {
        Success = false;
    }  

    if (!LookupJNIConstructorGlobal(env,
                         fullPoseState_Class,
                         "de/fruitfly/ovr/structs/FullPoseState",
                         fullPoseState_constructor_MethodID,
                         "(JFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDFIFFFFFFFFFFFFFFFFFFFFFIFFFFFFFIID)V"))
    {
        Success = false;
    }

	if (!LookupJNIConstructorGlobal(env,
                         renderTextureSet_Class,
                         "de/fruitfly/ovr/structs/RenderTextureSet",
                         renderTextureSet_constructor_MethodID,
                         "()V"))
    {
        Success = false;
    }

	if (!LookupJNIConstructorGlobal(env,
                         errorInfo_Class,
                         "de/fruitfly/ovr/structs/ErrorInfo",
                         errorInfo_constructor_MethodID,
                         "(Ljava/lang/String;IZZ)V"))
    {
        Success = false;
    }

    if (!LookupJNIFieldGlobal(env,
                         renderTextureSet_Class,
                         "de/fruitfly/ovr/structs/RenderTextureSet",
                         field_renderTextureSet_leftEyeTextureIds,
                         "Ljava/util/ArrayList;",
						 "leftEyeTextureIds"))
    {
        Success = false;
    }

    if (!LookupJNIFieldGlobal(env,
                         renderTextureSet_Class,
                         "de/fruitfly/ovr/structs/RenderTextureSet",
                         field_renderTextureSet_rightEyeTextureIds,
                         "Ljava/util/ArrayList;",
						 "rightEyeTextureIds"))
    {
        Success = false;
    }


	// Lookup some standard java classes / methods
	if (!LookupJNIConstructorGlobal(env,
                         arrayListClass,
                         "java/util/ArrayList",
                         method_arrayList_init,
                         "()V"))
    {
        Success = false;
    }

    if (!LookupJNIMethodGlobal(env,
                         arrayListClass,
                         "java/util/ArrayList",
                         method_arrayList_add,
                         "(Ljava/lang/Object;)Z",
						 "add"))
    {
        Success = false;
    }

	if (!LookupJNIConstructorGlobal(env,
                         integerClass,
                         "java/lang/Integer",
                         method_integer_init,
                         "(I)V"))
    {
        Success = false;
    }

    return Success;
}

bool LookupJNIConstructorGlobal(JNIEnv *env,
                     jclass& clazz,
                     std::string className,
                     jmethodID& method,
                     std::string Signature)
{
	std::string methodName = "<init>";
	return LookupJNIMethodGlobal(env, clazz, className, method, Signature, methodName);
}

bool LookupJNIMethodGlobal(JNIEnv *env,
                     jclass& clazz,
                     std::string className,
                     jmethodID& method,
                     std::string Signature,
					 std::string MethodName)
{
    if (clazz == NULL)
	{
		jclass localClass = env->FindClass(className.c_str());
        if (localClass == 0)
        {
            printf("Failed to find class '%s'", className.c_str());
            return false;
        }

		clazz = (jclass)env->NewGlobalRef(localClass);
		env->DeleteLocalRef(localClass);
	}

	if (method == NULL)
	{
		method = env->GetMethodID(clazz, MethodName.c_str(), Signature.c_str());
        if (method == 0)
        {
            printf("Failed to find method '%s' for class '%s' with signature: %s", 
                MethodName.c_str(), className.c_str(), Signature.c_str());
            return false;
        }
	}

    return true;
}

bool LookupJNIFieldGlobal(JNIEnv *env,
                     jclass& clazz,
                     std::string className,
                     jfieldID& field,
					 std::string Signature,
					 std::string FieldName)
{
    if (clazz == NULL)
	{
		jclass localClass = env->FindClass(className.c_str());
        if (localClass == 0)
        {
            printf("Failed to find class '%s'", className.c_str());
            return false;
        }

		clazz = (jclass)env->NewGlobalRef(localClass);
		env->DeleteLocalRef(localClass);
	}

	if (field == NULL)
	{
		field = env->GetFieldID(clazz, FieldName.c_str(), Signature.c_str());
        if (field == 0)
        {
            printf("Failed to find field '%s' for class '%s' with signature: %s", 
                FieldName.c_str(), className.c_str(), Signature.c_str());
            return false;
        }
	}

    return true;
}

void SetEulerEnumValues(int firstRotationAxis,
					    int secondRotationAxis,
					    int thirdRotationAxis,
					    int rotationDir,
					    int hand,
					    Axis& A1,
						Axis& A2,
					    Axis& A3,
					    RotateDirection& D,
					    HandedSystem& S)
{
	SetAxisEnum(firstRotationAxis, A1);
	SetAxisEnum(secondRotationAxis, A2);
	SetAxisEnum(thirdRotationAxis, A3);

	switch (rotationDir)
	{
	case 1:
		D = Rotate_CCW;
		break;
	default:
		D = Rotate_CW;
	}

	switch (hand)
	{
	case 1:
		S = Handed_R;
		break;
	default:
		S = Handed_L;
	}
}

void SetAxisEnum(int value, Axis& A)
{
	switch (value)
	{
	case 0:
		A = Axis_X;
		break;
	case 1:
		A = Axis_Y;
		break;
	default:
		A = Axis_Z;
	}
}

bool LibFirstInit(JNIEnv *env)
{
	bool Success = true;
	if (!_performedFirstInit)
	{
		InitOvrResultMaps();
		InitOvrDepthTextureSets();

		Success = CacheJNIGlobals(env);
		if (Success) 
		{
			_performedFirstInit = true;
		}
	}
	return Success;
}

void InitOvrDepthTextureSets()
{
//	for (int i = 0; i < 2; i++)
//	{
//		_pDepthTextureSet[i].TextureCount = 1;
//		_pDepthTextureSet[i].CurrentIndex = 0;
//		_pDepthTextureSet[i].Textures = (ovrTexture*)&_DepthTexture[i];
//	}
}

void InitOvrResultMaps()
{
	_ErrorMap.clear();
	_SuccessMap.clear();

    /* General errors */
    _ErrorMap[ovrError_MemoryAllocationFailure       ] = "ovrError_MemoryAllocationFailure";   
    _ErrorMap[ovrError_SocketCreationFailure         ] = "ovrError_SocketCreationFailure";
    _ErrorMap[ovrError_InvalidSession                ] = "ovrError_InvalidSession";   
    _ErrorMap[ovrError_Timeout                       ] = "ovrError_Timeout";   
    _ErrorMap[ovrError_NotInitialized                ] = "ovrError_NotInitialized";   
    _ErrorMap[ovrError_InvalidParameter              ] = "ovrError_InvalidParameter";   
    _ErrorMap[ovrError_ServiceError                  ] = "ovrError_ServiceError";   
    _ErrorMap[ovrError_NoHmd                         ] = "ovrError_NoHmd (Connect HMD or enable debug HMD device)";   
    _ErrorMap[ovrError_AudioReservedBegin            ] = "ovrError_AudioReservedBegin";   
    _ErrorMap[ovrError_AudioDeviceNotFound           ] = "ovrError_AudioDeviceNotFound";   
    _ErrorMap[ovrError_AudioComError                 ] = "ovrError_AudioComError";   
    _ErrorMap[ovrError_AudioReservedEnd              ] = "ovrError_AudioReservedEnd";   
    _ErrorMap[ovrError_Initialize                    ] = "ovrError_Initialize";   
    _ErrorMap[ovrError_LibLoad                       ] = "ovrError_LibLoad (No runtime found)";   
    _ErrorMap[ovrError_LibVersion                    ] = "ovrError_LibVersion (Runtime version incompatibility)";   
    _ErrorMap[ovrError_ServiceConnection             ] = "ovrError_ServiceConnection";   
    _ErrorMap[ovrError_ServiceVersion                ] = "ovrError_ServiceVersion";   
    _ErrorMap[ovrError_IncompatibleOS                ] = "ovrError_IncompatibleOS";   
    _ErrorMap[ovrError_DisplayInit                   ] = "ovrError_DisplayInit (GPU does not meet minimum requirements?)";  
    _ErrorMap[ovrError_ServerStart                   ] = "ovrError_ServerStart";  
    _ErrorMap[ovrError_Reinitialization              ] = "ovrError_Reinitialization";  
    _ErrorMap[ovrError_MismatchedAdapters            ] = "ovrError_MismatchedAdapters";  
    _ErrorMap[ovrError_LeakingResources              ] = "ovrError_LeakingResources";  
    _ErrorMap[ovrError_ClientVersion                 ] = "ovrError_ClientVersion";  
    _ErrorMap[ovrError_OutOfDateOS                   ] = "ovrError_OutOfDateOS";  
    _ErrorMap[ovrError_OutOfDateGfxDriver            ] = "ovrError_OutOfDateGfxDriver";  
    _ErrorMap[ovrError_IncompatibleGPU               ] = "ovrError_IncompatibleGPU";  
    _ErrorMap[ovrError_NoValidVRDisplaySystem        ] = "ovrError_NoValidVRDisplaySystem";  
    _ErrorMap[ovrError_InvalidBundleAdjustment       ] = "ovrError_InvalidBundleAdjustment";  
    _ErrorMap[ovrError_USBBandwidth                  ] = "ovrError_USBBandwidth";  
    _ErrorMap[ovrError_USBEnumeratedSpeed            ] = "ovrError_USBEnumeratedSpeed";  
    _ErrorMap[ovrError_ImageSensorCommError          ] = "ovrError_ImageSensorCommError";  
    _ErrorMap[ovrError_GeneralTrackerFailure         ] = "ovrError_GeneralTrackerFailure";  
    _ErrorMap[ovrError_ExcessiveFrameTruncation      ] = "ovrError_ExcessiveFrameTruncation";  
    _ErrorMap[ovrError_ExcessiveFrameSkipping        ] = "ovrError_ExcessiveFrameSkipping";  
    _ErrorMap[ovrError_SyncDisconnected              ] = "ovrError_SyncDisconnected";  
    _ErrorMap[ovrError_TrackerMemoryReadFailure      ] = "ovrError_TrackerMemoryReadFailure";  
    _ErrorMap[ovrError_TrackerMemoryWriteFailure     ] = "ovrError_TrackerMemoryWriteFailure";  
    _ErrorMap[ovrError_TrackerFrameTimeout           ] = "ovrError_TrackerFrameTimeout";  
    _ErrorMap[ovrError_TrackerTruncatedFrame         ] = "ovrError_TrackerTruncatedFrame";  
    _ErrorMap[ovrError_HMDFirmwareMismatch           ] = "ovrError_HMDFirmwareMismatch";  
    _ErrorMap[ovrError_TrackerFirmwareMismatch       ] = "ovrError_TrackerFirmwareMismatch";  
    _ErrorMap[ovrError_BootloaderDeviceDetected      ] = "ovrError_BootloaderDeviceDetected";  
    _ErrorMap[ovrError_TrackerCalibrationError       ] = "ovrError_TrackerCalibrationError";  
    _ErrorMap[ovrError_ControllerFirmwareMismatch    ] = "ovrError_ControllerFirmwareMismatch";  
    _ErrorMap[ovrError_Incomplete                    ] = "ovrError_Incomplete";  
    _ErrorMap[ovrError_Abandoned                     ] = "ovrError_Abandoned";  
    _ErrorMap[ovrError_DisplayLost                   ] = "ovrError_DisplayLost";  
    _ErrorMap[ovrError_RuntimeException              ] = "ovrError_RuntimeException";

    _SuccessMap[ovrSuccess                           ] = "ovrSuccess"; 
    _SuccessMap[ovrSuccess_NotVisible                ] = "ovrSuccess_NotVisible";
    _SuccessMap[ovrSuccess_HMDFirmwareMismatch       ] = "ovrSuccess_HMDFirmwareMismatch";
    _SuccessMap[ovrSuccess_TrackerFirmwareMismatch   ] = "ovrSuccess_TrackerFirmwareMismatch";
    _SuccessMap[ovrSuccess_ControllerFirmwareMismatch] = "ovrSuccess_ControllerFirmwareMismatch";

	// Added for (and defined in) JRift
	_SuccessMap[ovrSuccess_SessionShouldQuit]          = "ovrSuccess_SessionShouldQuit";
}

void _SetErrorInfo(JNIEnv *env, const char* error, ovrResult ovr_result)
{	
	/* 
	TODO: Use 
		ovrErrorInfo errorInfo;
		ovr_GetLastErrorInfo(&errorInfo);
	when I can get it working...
	*/

	std::stringstream s;
	std::string sOvrError = "<unknown ovrResult code>";
	
	if (OVR_SUCCESS(ovr_result))
	{
		std::map<int32_t, std::string>::const_iterator it = _SuccessMap.find((int32_t)ovr_result);
		if (it != _SuccessMap.end()) 
		{
			sOvrError = it->second;
		}
	}
	else
	{
		std::map<int32_t, std::string>::const_iterator it = _ErrorMap.find((int32_t)ovr_result);
		if (it != _ErrorMap.end()) 
		{
			sOvrError = it->second;
		}
	}

	if (strlen(error) > 0)
	{
		s << error << " [Client SDK version " << OVR_VERSION_STRING << 
			", Runtime version " << _sRuntimeVersion.c_str() << "]: ";
	}
	s << sOvrError;

	_lastError.sError             = s.str();
	_lastError.ovr_result         = ovr_result;
	_lastError.Success            = OVR_SUCCESS(ovr_result);
	_lastError.UnqualifiedSuccess = OVR_UNQUALIFIED_SUCCESS(ovr_result);
}

void SetGenericErrorInfo(JNIEnv *env, const char* error)
{	
    _SetErrorInfo(env, error, ovrError_Initialize);
}

jobject GetLastErrorInfo(JNIEnv *env)
{
	if (!_performedFirstInit)
	{
		return 0;
	}

	ClearException(env);

	jstring jerrorStr = env->NewStringUTF( _lastError.sError.c_str() );
	jobject errorInfo = env->NewObject(errorInfo_Class, errorInfo_constructor_MethodID,
                                      jerrorStr,
									  (int)_lastError.ovr_result,
									  _lastError.Success,
									  _lastError.UnqualifiedSuccess);
	env->DeleteLocalRef( jerrorStr );
	if (errorInfo == 0) PrintNewObjectException(env, "ErrorInfo");
	return errorInfo;
}
