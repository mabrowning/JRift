#include "de_fruitfly_ovr_OculusRift.h"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#if defined(OVR_OS_WIN32)
#include "Windows.h"
#endif
#include "OVR_CAPI_GL.h"
#include "GL\CAPI_GLE_GL.h"

using namespace OVR;

ovrHmd              _pHmd             = 0;
ovrHmdDesc          _hmdDesc; 
int                 _hmdIndex         = -1;
bool                _initialised      = false;
bool                _renderConfigured = false;
bool                _realDevice       = false;
ovrSwapTextureSet*  _pSwapTextureSet[2];
ovrGLTexture*       _pMirrorTexture   = 0;

ovrTrackingState    _hmdState;
ovrPosef            _eyeRenderPose[2];
ovrGLTexture        _GLEyeTexture[2];
ovrEyeRenderDesc    _EyeRenderDesc[2];
double              _sensorSampleTime = 0.0;

ovrResult           _lastOvrResult = ovrSuccess;

std::map<ovrErrorType,   std::string> _errorMap;
std::map<ovrSuccessType, std::string> _successMap;

//sizes from last ovr_CreateSwapTextureSetGL
int _lwidth=0;
int _lheight=0;
int _rwidth=0;
int _rheight=0;

const bool          LogDebug = false;

const Vector3f		UpVector(0.0f, 1.0f, 0.0f);
const Vector3f		ForwardVector(0.0f, 0.0f, -1.0f);

// JNI class / method caching
static jclass       eyeRenderParams_Class                = 0;
static jmethodID    eyeRenderParams_constructor_MethodID = 0;
static jclass       frameTiming_Class                    = 0;
static jmethodID    frameTiming_constructor_MethodID     = 0;
static jclass       posef_Class                          = 0;
static jmethodID    posef_constructor_MethodID           = 0;
static jclass       trackerState_Class                   = 0;
static jmethodID    trackerState_constructor_MethodID    = 0;
static jclass       sizei_Class                          = 0;
static jmethodID    sizei_constructor_MethodID           = 0;
static jclass       fovTextureInfo_Class                 = 0;
static jmethodID    fovTextureInfo_constructor_MethodID  = 0;
static jclass       hmdDesc_Class                        = 0;
static jmethodID    hmdDesc_constructor_MethodID         = 0;
static jclass       vector3f_Class                       = 0;
static jmethodID    vector3f_constructor_MethodID        = 0;
static jclass       matrix4f_Class                       = 0;
static jmethodID    matrix4f_constructor_MethodID        = 0;
static jclass       userProfileData_Class                = 0;
static jmethodID    userProfileData_constructor_MethodID = 0;
static jclass       fullPoseState_Class                  = 0;
static jmethodID    fullPoseState_constructor_MethodID   = 0;
static jclass       swapTextureSet_Class                 = 0;
static jmethodID    swapTextureSet_constructor_MethodID  = 0;
static jclass       eulerOrient_Class                    = 0;
static jmethodID    eulerOrient_constructor_MethodID     = 0;
static jclass       arrayListClass						 = 0;
static jmethodID    method_arrayList_init                = 0;
static jmethodID    method_arrayList_add                 = 0;
static jclass       integerClass                         = 0;
static jmethodID    method_integer_init                  = 0;

static jfieldID     field_swapTextureSet_leftEyeTextureIds   = 0;
static jfieldID     field_swapTextureSet_rightEyeTextureIds  = 0;



JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1initSubsystem(JNIEnv *env, jobject jobj) 
{
    DEBUGLOG("Initialising Oculus Rift subsystem...");
	initOvrResultMaps();

	Reset();

    if (!CacheJNIGlobals(env))
    {
        return false;
    }

	// Initialise LibOVR - use default init params for now
	ovrInitParams initParams;
	memset(&initParams, 0, sizeof(ovrInitParams));
	_lastOvrResult = ovr_Initialize(&initParams);

	if (OVR_FAILURE(_lastOvrResult)) 
	{
		printf("Unable to initialise LibOVR! ovr_Initialize() returned error code '%s' (%d)\n", getOvrResultString(_lastOvrResult).c_str(), _lastOvrResult);
		return false;
	}

    // Create HMD
    if (CreateHmdAndConfigureTracker())
        _initialised = true;
	
	if (!_initialised)
	{
		printf("Unable to create Oculus Rift device interface!\n");
	}

	InitRenderConfig();
	
	return _initialised;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroySubsystem(JNIEnv *env, jobject jobj) 
{
	printf("Destroying Oculus Rift device interface.\n");	

	if (_initialised)
	{
		Reset();
	}
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getHmdDesc(JNIEnv *env, jobject) 
{
	if (!_initialised) 
        return 0;

    jstring productName = env->NewStringUTF( _hmdDesc.ProductName == NULL ? "" : _hmdDesc.ProductName );
    jstring manufacturer = env->NewStringUTF( _hmdDesc.Manufacturer == NULL ? "" : _hmdDesc.Manufacturer );
    jstring displayDeviceName = env->NewStringUTF( "Not set" );
    
    ClearException(env);

	// TODO: Sync with updated hmdDesc struct
    jobject jHmdDesc = env->NewObject(hmdDesc_Class, hmdDesc_constructor_MethodID,
                                      (int)_hmdDesc.Type,
                                      productName,
                                      manufacturer,
                                      (int)_hmdDesc.AvailableHmdCaps,
                                      (int)_hmdDesc.AvailableTrackingCaps,
                                      0, // Distortion caps no longer used, remove
                                      _hmdDesc.Resolution.w,
                                      _hmdDesc.Resolution.h,
                                      0, // Window pos no longer used, remove
                                      0, // Window pos no longer used, remove
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
                                      0, // Eye render order no longer used, remove
                                      0, // Eye render order no longer used, remove
                                      displayDeviceName,  // DisplayDeviceName no longer used, remove
                                      0, // Display ID no longer used, remove
                                      _realDevice
            );

    env->DeleteLocalRef( productName );
    env->DeleteLocalRef( manufacturer );
    env->DeleteLocalRef( displayDeviceName );

    if (jHmdDesc == 0) PrintNewObjectException(env, "HmdDesc");

    return jHmdDesc;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1resetTracking(JNIEnv *env, jobject) 
{
	if (!_initialised)
		return;

    ovr_RecenterPose(_pHmd);
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getFovTextureSize(
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
    Sizei recommendedTex0Size = ovr_GetFovTextureSize(_pHmd, ovrEye_Left,  leftFov, RenderScaleFactor);
    Sizei recommendedTex1Size = ovr_GetFovTextureSize(_pHmd, ovrEye_Right, rightFov, RenderScaleFactor);
    Sizei RenderTargetSize;
    RenderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
    RenderTargetSize.h = (std::max) ( recommendedTex0Size.h, recommendedTex1Size.h );

    ClearException(env);

    jobject jfovTextureInfo = env->NewObject(fovTextureInfo_Class, fovTextureInfo_constructor_MethodID,
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

    if (jfovTextureInfo == 0) PrintNewObjectException(env, "FovTextureInfo");

    return jfovTextureInfo;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1createSwapTextureSet(
	JNIEnv *env, 
	jobject,
	jint lwidth,
	jint lheight,
    jint rwidth,
    jint rheight
	)
{
	if (!_initialised)
		return 0;
	
	DestroySwapTextureSet();

	boolean result = true;
	_lwidth=lwidth;
	_lheight=lheight;
	_rwidth=rwidth;
	_rheight=rheight;
	if (result && ovr_CreateSwapTextureSetGL(_pHmd, GL_SRGB8_ALPHA8, lwidth, lheight, &_pSwapTextureSet[0]) != ovrSuccess)
	{
		result = false;	
	}
	if (result && ovr_CreateSwapTextureSetGL(_pHmd, GL_SRGB8_ALPHA8, rwidth, rheight, &_pSwapTextureSet[1]) != ovrSuccess)
	{
		result = false;	
	}

	if (!result)
	{
		printf("Unable to create swap texture set!\n");
		DestroySwapTextureSet();
	}

	if (result)
	{
		// Construct a new SwapTextureSet object
		ClearException(env);
		jobject jswapTextureSet = env->NewObject(swapTextureSet_Class, swapTextureSet_constructor_MethodID);
		if (jswapTextureSet == 0) PrintNewObjectException(env, "SwapTextureSet");

		jobject leftEyeTextureIds = env->GetObjectField(jswapTextureSet, field_swapTextureSet_leftEyeTextureIds);
		jobject rightEyeTextureIds = env->GetObjectField(jswapTextureSet, field_swapTextureSet_rightEyeTextureIds);

		// Add the texture IDs
		for (int i = 0; i < _pSwapTextureSet[0]->TextureCount; i++)
		{
			ovrGLTexture* tex = (ovrGLTexture*)&_pSwapTextureSet[0]->Textures[i];
			jobject texIdInt = env->NewObject(integerClass, method_integer_init, (jint)tex->OGL.TexId);
			jboolean jbool = env->CallBooleanMethod(leftEyeTextureIds, method_arrayList_add, texIdInt);
		}
		for (int i = 0; i < _pSwapTextureSet[1]->TextureCount; i++)
		{
			ovrGLTexture* tex = (ovrGLTexture*)&_pSwapTextureSet[1]->Textures[i];
			jobject texIdInt = env->NewObject(integerClass, method_integer_init, (jint)tex->OGL.TexId);
			jboolean jbool = env->CallBooleanMethod(rightEyeTextureIds, method_arrayList_add, texIdInt);
		}
		return jswapTextureSet;
	}

	return 0;
}

JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1setCurrentSwapTextureIndex
(JNIEnv *env,
 jobject,
 jint index)
{
    if (!_initialised)
    {
        return false;
    }
    
    if (_pSwapTextureSet[0] == 0 || _pSwapTextureSet[1] == 0)
    {
        return false;
    }
    
    if (index < 0 ||
        index > (_pSwapTextureSet[0]->TextureCount - 1) ||
        index > (_pSwapTextureSet[1]->TextureCount - 1))
    {
        return false;
    }
    
    _pSwapTextureSet[0]->CurrentIndex = index;
    _pSwapTextureSet[1]->CurrentIndex = index;
    
    return true;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroySwapTextureSet
(JNIEnv *env, jobject)
{
    DestroySwapTextureSet();
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

	if (ovr_CreateMirrorTextureGL(_pHmd, GL_SRGB8_ALPHA8, width, height, (ovrTexture**)&_pMirrorTexture) != ovrSuccess)
	{
		printf("Unable to create mirror texture!\n");
		_pMirrorTexture = 0;
		return -1;
	}

	// Just return the texture ID
	return _pMirrorTexture->OGL.TexId;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroyMirrorTexture
(JNIEnv *env, jobject)
{
    DestroyMirrorTexture();
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1resetRenderConfig(JNIEnv *env, jobject)
{
	ResetRenderConfig();
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getEyePoses(
	JNIEnv *env, 
	jobject, 
	jlong FrameIndex
	)
{
    if (!_initialised)
        return 0;

	// Use mandated view offsets
	ovrVector3f ViewOffsets[2] = { _EyeRenderDesc[0].HmdToEyeViewOffset,
                                   _EyeRenderDesc[1].HmdToEyeViewOffset };

	// Get eye poses at our predicted display times
	double ftiming = ovr_GetPredictedDisplayTime(_pHmd, FrameIndex);
    _sensorSampleTime = ovr_GetTimeInSeconds();
    _hmdState = ovr_GetTrackingState(_pHmd, ftiming, ovrTrue);
    ovr_CalcEyePoses(_hmdState.HeadPose.ThePose, ViewOffsets, _eyeRenderPose);

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
								 _hmdState.HeadPose.ThePose.Orientation.x,   
								 _hmdState.HeadPose.ThePose.Orientation.y,  
								 _hmdState.HeadPose.ThePose.Orientation.z,   
								 _hmdState.HeadPose.ThePose.Orientation.w,   
								 _hmdState.HeadPose.ThePose.Position.x,      
								 _hmdState.HeadPose.ThePose.Position.y,      
								 _hmdState.HeadPose.ThePose.Position.z,      
								 _hmdState.HeadPose.AngularVelocity.x,    
								 _hmdState.HeadPose.AngularVelocity.y,    
								 _hmdState.HeadPose.AngularVelocity.z,    
								 _hmdState.HeadPose.LinearVelocity.x,     
								 _hmdState.HeadPose.LinearVelocity.y,     
								 _hmdState.HeadPose.LinearVelocity.z,     
								 _hmdState.HeadPose.AngularAcceleration.x,
								 _hmdState.HeadPose.AngularAcceleration.y,
								 _hmdState.HeadPose.AngularAcceleration.z,
								 _hmdState.HeadPose.LinearAcceleration.x, 
								 _hmdState.HeadPose.LinearAcceleration.y, 
								 _hmdState.HeadPose.LinearAcceleration.z, 
								 _hmdState.HeadPose.TimeInSeconds,        
								 _hmdState.RawSensorData.Temperature,
								 _hmdState.StatusFlags
								 );
    if (jfullposestate == 0) PrintNewObjectException(env, "FullPoseState");

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

    if (!_renderConfigured)
    {
        printf("getMatrix4fProjection() - ERROR: Render config not set!\n");
        return 0;
    }

    ovrFovPort fov;
    fov.UpTan = EyeFovPortUpTan;
    fov.DownTan = EyeFovPortDownTan;
    fov.LeftTan = EyeFovPortLeftTan;
    fov.RightTan = EyeFovPortRightTan;

    Matrix4f proj = ovrMatrix4f_Projection(fov, nearClip, farClip, true); // true = RH for OGL

    jobject jproj = env->NewObject(matrix4f_Class, matrix4f_constructor_MethodID,
                                   proj.M[0][0], proj.M[0][1], proj.M[0][2], proj.M[0][3],
                                   proj.M[1][0], proj.M[1][1], proj.M[1][2], proj.M[1][3],
                                   proj.M[2][0], proj.M[2][1], proj.M[2][2], proj.M[2][3],
                                   proj.M[3][0], proj.M[3][1], proj.M[3][2], proj.M[3][3]
                                   );

    return jproj;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1submitFrame(JNIEnv *env, jobject)
{
    if (!_initialised)
        return;

    if (!_renderConfigured)
    {
        printf("endFrame() - ERROR: Render config not set!\n");
        return;
    }

    ovrViewScaleDesc viewScaleDesc;
    viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;  // TODO: pass in as param
    viewScaleDesc.HmdToEyeViewOffset[0] = _EyeRenderDesc[0].HmdToEyeViewOffset;
    viewScaleDesc.HmdToEyeViewOffset[1] = _EyeRenderDesc[1].HmdToEyeViewOffset;

    
    ovrLayerEyeFov ld;
    ld.Header.Type  = ovrLayerType_EyeFov;
    ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
    
    for (int eye = 0; eye < 2; ++eye)
    {
        ld.ColorTexture[eye] = _pSwapTextureSet[eye];
        ld.Fov[eye]          = _hmdDesc.DefaultEyeFov[eye];
        ld.RenderPose[eye]   = _eyeRenderPose[eye];
        ld.SensorSampleTime  = _sensorSampleTime;
    }
    ld.Viewport[0]     = Recti(0,0,_lwidth,_lheight);
    ld.Viewport[1]     = Recti(0,0,_rwidth,_rheight);

    ovrLayerHeader* layers = &ld.Header;
    ovrResult result = ovr_SubmitFrame(_pHmd, 0, &viewScaleDesc, &layers, 1);
	// TODO: Return result
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
    if (!LookupJNIGlobal(env,
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

	float playerHeight = ovr_GetFloat( _pHmd, OVR_KEY_PLAYER_HEIGHT, OVR_DEFAULT_PLAYER_HEIGHT);
	float eyeHeight    = ovr_GetFloat( _pHmd, OVR_KEY_EYE_HEIGHT,    OVR_DEFAULT_EYE_HEIGHT); 
	float ipd          = ovr_GetFloat( _pHmd, OVR_KEY_IPD,           OVR_DEFAULT_IPD); 
	std::string gender = ovr_GetString(_pHmd, OVR_KEY_GENDER,        OVR_DEFAULT_GENDER);
    std::string name   = ovr_GetString(_pHmd, OVR_KEY_NAME,          "No Profile");

	jstring jname   = env->NewStringUTF(name.c_str());
    jstring jgender = env->NewStringUTF(gender.c_str());

	jobject profileData = env->NewObject(userProfileData_Class, userProfileData_constructor_MethodID,
		playerHeight,
		eyeHeight,
		ipd,
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

void ResetRenderConfig()
{
/*
    if (_initialised)
    {
        ovr_ConfigureRendering(_pHmd, 0, 0, 0, 0); //TODO_ continue here
    }

    // Reset texture data
	_GLEyeTexture[0].OGL.Header.API                   = ovrRenderAPI_None;
    _GLEyeTexture[0].OGL.Header.TextureSize.w         = 0;
	_GLEyeTexture[0].OGL.Header.TextureSize.h         = 0;
    _GLEyeTexture[0].OGL.Header.RenderViewport.Pos.x  = 0;
    _GLEyeTexture[0].OGL.Header.RenderViewport.Pos.y  = 0;
    _GLEyeTexture[0].OGL.Header.RenderViewport.Size.w = 0;
    _GLEyeTexture[0].OGL.Header.RenderViewport.Size.h = 0;
	_GLEyeTexture[0].OGL.TexId                        = 0;
    _GLEyeTexture[1] = _GLEyeTexture[0];
*/
    _renderConfigured = false;
}

void InitRenderConfig()
{
	_EyeRenderDesc[0] = ovr_GetRenderDesc(_pHmd, ovrEye_Left, _hmdDesc.DefaultEyeFov[0]);
    _EyeRenderDesc[1] = ovr_GetRenderDesc(_pHmd, ovrEye_Right, _hmdDesc.DefaultEyeFov[1]);
    _renderConfigured = true;
}

bool CreateHmdAndConfigureTracker()
{
    _pHmd = 0;

    bool result = false;
    _realDevice = false;

	ovrGraphicsLuid luid;

	// Get HMD
	if (ovr_Create(&_pHmd, &luid) == ovrSuccess)
	{
		printf("Oculus Rift device found!\n");
        _realDevice = true; // TODO: How to detect real versus debug device?
		result = true;
	}
/*	else 
	{

		// TODO: No debug device


		// Create debug Rift
        _hmdIndex = -1;
		if (ovr_CreateDebug(ovrHmd_DK2, &_pHmd) == ovrSuccess)
		{
			printf("No Oculus Rift devices found, creating dummy device...\n");
			result = true;
		}
	}
*/

	if (result)
	{
		_hmdDesc = ovr_GetHmdDesc(_pHmd);

		// Log description
		LogHmdDesc(_pHmd);

		// Configure tracking
        int trackerResult = ovr_ConfigureTracking(_pHmd,
			    ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position,
			    0);

		if (trackerResult != ovrSuccess)
		{
			// Initialised successfully
			printf("FAILED to configure tracker!");
            result = false;
		}
	}

    return result;
}

void DEBUGLOG(std::string s)
{
	if (LogDebug)
		printf("DEBUG: %s\n", s.c_str());
}

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

void LogHmdDesc(ovrHmd pHmd)
{
	// Chuck out some basic HMD info
	printf(" Product Name:      %s\n", _hmdDesc.ProductName);
	printf(" Manufacturer:      %s\n", _hmdDesc.Manufacturer);
	printf(" Native Resolution: %d X %d\n", _hmdDesc.Resolution.w, _hmdDesc.Resolution.h);
}

void Reset()
{
	if (_initialised)
	{
        // Reset render config
        ResetRenderConfig();

		// Destroy textures
		DestroySwapTextureSet();
		DestroyMirrorTexture();

		// Cleanup HMD
		if (_pHmd)
			ovr_Destroy(_pHmd);

		// Shutdown LibOVR
		ovr_Shutdown();
	}

	_pHmd = 0;
	_hmdIndex = -1;

	_pSwapTextureSet[0] = 0;
	_pSwapTextureSet[1] = 0;
	_pMirrorTexture     = 0;

    _eyeRenderPose[0].Orientation.x = 0.0;
    _eyeRenderPose[0].Orientation.y = 0.0;
    _eyeRenderPose[0].Orientation.z = 0.0;
    _eyeRenderPose[0].Orientation.w = 0.0;
    _eyeRenderPose[0].Position.x = 0.0;
    _eyeRenderPose[0].Position.y = 0.0;
    _eyeRenderPose[0].Position.z = 0.0;
    _eyeRenderPose[1] = _eyeRenderPose[0];

	_initialised = false;
    _realDevice = false;
}

void DestroySwapTextureSet()
{
	if (!_initialised)
	{
		_pSwapTextureSet[0] = 0;
		_pSwapTextureSet[1] = 0;
		return;
	}

	if (_pSwapTextureSet[0] != 0)
	{		    
		ovr_DestroySwapTextureSet(_pHmd, _pSwapTextureSet[0]);
		_pSwapTextureSet[0] = 0;
	}
	if (_pSwapTextureSet[1] != 0)
	{
		ovr_DestroySwapTextureSet(_pHmd, _pSwapTextureSet[1]);
		_pSwapTextureSet[1] = 0;
	}
}

void DestroyMirrorTexture()
{
	if (!_initialised)
	{
		_pMirrorTexture = 0;
		return;
	}

	if (_pMirrorTexture != 0)
	{
		ovr_DestroyMirrorTexture(_pHmd, (ovrTexture*)_pMirrorTexture);
		_pMirrorTexture = 0;
	}
}

bool CacheJNIGlobals(JNIEnv *env)
{
    if (!LookupJNIGlobal(env,
                         sizei_Class,
                         "de/fruitfly/ovr/structs/Sizei",
                         sizei_constructor_MethodID,
                         "(II)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         fovTextureInfo_Class,
                         "de/fruitfly/ovr/structs/FovTextureInfo",
                         fovTextureInfo_constructor_MethodID,
                         "(IIIIIIIIF)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         hmdDesc_Class,
                         "de/fruitfly/ovr/structs/HmdDesc",
                         hmdDesc_constructor_MethodID,
                         "(ILjava/lang/String;Ljava/lang/String;IIIIIIIFFFFFFFFFFFFFFFFIILjava/lang/String;JZ)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         trackerState_Class,
                         "de/fruitfly/ovr/structs/TrackerState",
                         trackerState_constructor_MethodID,
                         "(FFFFFFFFFFFFFFFFFFFDFI)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         vector3f_Class,
                         "de/fruitfly/ovr/structs/Vector3f",
                         vector3f_constructor_MethodID,
                         "(FFF)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         eyeRenderParams_Class,
                         "de/fruitfly/ovr/EyeRenderParams",
                         eyeRenderParams_constructor_MethodID,
                         "(IIIIIFFFFIIIIFFFFFIIIIIFFFFIIIIFFFFFF)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         posef_Class,
                         "de/fruitfly/ovr/structs/Posef",
                         posef_constructor_MethodID,
                         "(FFFFFFF)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         frameTiming_Class,
                         "de/fruitfly/ovr/structs/FrameTiming",
                         frameTiming_constructor_MethodID,
                         "(FDDDDDD)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         matrix4f_Class,
                         "de/fruitfly/ovr/structs/Matrix4f",
                         matrix4f_constructor_MethodID,
                         "(FFFFFFFFFFFFFFFF)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         userProfileData_Class,
                         "de/fruitfly/ovr/UserProfileData",
                         userProfileData_constructor_MethodID,
                         "(FFFLjava/lang/String;ZLjava/lang/String;)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         fullPoseState_Class,
                         "de/fruitfly/ovr/structs/FullPoseState",
                         fullPoseState_constructor_MethodID,
                         "(JFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDFI)V"))
    {
        return false;
    }

	if (!LookupJNIGlobal(env,
                         swapTextureSet_Class,
                         "de/fruitfly/ovr/structs/SwapTextureSet",
                         swapTextureSet_constructor_MethodID,
                         "()V"))
    {
        return false;
    }
	field_swapTextureSet_leftEyeTextureIds = env->GetFieldID(swapTextureSet_Class, "leftEyeTextureIds", "Ljava/util/ArrayList;");
	if (field_swapTextureSet_leftEyeTextureIds == 0)
    {
		printf("Failed to find field 'Ljava/util/ArrayList;' leftEyeTextureIds");
        return false; 
    }
	field_swapTextureSet_rightEyeTextureIds = env->GetFieldID(swapTextureSet_Class, "rightEyeTextureIds", "Ljava/util/ArrayList;");
	if (field_swapTextureSet_rightEyeTextureIds == 0)
    {
		printf("Failed to find field 'Ljava/util/ArrayList;' rightEyeTextureIds");
        return false; 
    }

	// Lookup some standard java classes / methods
	if (!LookupJNIGlobal(env,
                         arrayListClass,
                         "java/util/ArrayList",
                         method_arrayList_init,
                         "()V"))
    {
        return false;
    }

	method_arrayList_add = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");
	if (method_arrayList_add == 0) 
	{
		printf("Failed to find method 'java/util/ArrayList' add(Ljava/lang/Object;)Z");
		return false;
	}

	if (!LookupJNIGlobal(env,
                         integerClass,
                         "java/lang/Integer",
                         method_integer_init,
                         "(I)V"))
    {
        return false;
    }

    return true;
}

bool LookupJNIGlobal(JNIEnv *env,
                     jclass& clazz,
                     std::string className,
                     jmethodID& method,
                     std::string constructorSignature)
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
		method = env->GetMethodID(clazz, "<init>", constructorSignature.c_str());
        if (method == 0)
        {
            printf("Failed to find constuctor method for class '%s' with signature: %s", 
                className.c_str(), constructorSignature.c_str());
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

void initOvrResultMaps()
{
    /* General errors */
    _errorMap[ovrError_MemoryAllocationFailure       ] = "ovrError_MemoryAllocationFailure";   
    _errorMap[ovrError_SocketCreationFailure         ] = "ovrError_SocketCreationFailure";
    _errorMap[ovrError_InvalidSession                ] = "ovrError_InvalidSession";   
    _errorMap[ovrError_Timeout                       ] = "ovrError_Timeout";   
    _errorMap[ovrError_NotInitialized                ] = "ovrError_NotInitialized";   
    _errorMap[ovrError_InvalidParameter              ] = "ovrError_InvalidParameter";   
    _errorMap[ovrError_ServiceError                  ] = "ovrError_ServiceError";   
    _errorMap[ovrError_NoHmd                         ] = "ovrError_NoHmd";   
    _errorMap[ovrError_AudioReservedBegin            ] = "ovrError_AudioReservedBegin";   
    _errorMap[ovrError_AudioDeviceNotFound           ] = "ovrError_AudioDeviceNotFound";   
    _errorMap[ovrError_AudioComError                 ] = "ovrError_AudioComError";   
    _errorMap[ovrError_AudioReservedEnd              ] = "ovrError_AudioReservedEnd";   
    _errorMap[ovrError_Initialize                    ] = "ovrError_Initialize";   
    _errorMap[ovrError_LibLoad                       ] = "ovrError_LibLoad (No runtime found)";   
    _errorMap[ovrError_LibVersion                    ] = "ovrError_LibVersion (Runtime version incompatibility)";   
    _errorMap[ovrError_ServiceConnection             ] = "ovrError_ServiceConnection";   
    _errorMap[ovrError_ServiceVersion                ] = "ovrError_ServiceVersion";   
    _errorMap[ovrError_IncompatibleOS                ] = "ovrError_IncompatibleOS";   
    _errorMap[ovrError_DisplayInit                   ] = "ovrError_DisplayInit";  
    _errorMap[ovrError_ServerStart                   ] = "ovrError_ServerStart";  
    _errorMap[ovrError_Reinitialization              ] = "ovrError_Reinitialization";  
    _errorMap[ovrError_MismatchedAdapters            ] = "ovrError_MismatchedAdapters";  
    _errorMap[ovrError_LeakingResources              ] = "ovrError_LeakingResources";  
    _errorMap[ovrError_ClientVersion                 ] = "ovrError_ClientVersion";  
    _errorMap[ovrError_OutOfDateOS                   ] = "ovrError_OutOfDateOS";  
    _errorMap[ovrError_OutOfDateGfxDriver            ] = "ovrError_OutOfDateGfxDriver";  
    _errorMap[ovrError_IncompatibleGPU               ] = "ovrError_IncompatibleGPU";  
    _errorMap[ovrError_NoValidVRDisplaySystem        ] = "ovrError_NoValidVRDisplaySystem";  
    _errorMap[ovrError_InvalidBundleAdjustment       ] = "ovrError_InvalidBundleAdjustment";  
    _errorMap[ovrError_USBBandwidth                  ] = "ovrError_USBBandwidth";  
    _errorMap[ovrError_USBEnumeratedSpeed            ] = "ovrError_USBEnumeratedSpeed";  
    _errorMap[ovrError_ImageSensorCommError          ] = "ovrError_ImageSensorCommError";  
    _errorMap[ovrError_GeneralTrackerFailure         ] = "ovrError_GeneralTrackerFailure";  
    _errorMap[ovrError_ExcessiveFrameTruncation      ] = "ovrError_ExcessiveFrameTruncation";  
    _errorMap[ovrError_ExcessiveFrameSkipping        ] = "ovrError_ExcessiveFrameSkipping";  
    _errorMap[ovrError_SyncDisconnected              ] = "ovrError_SyncDisconnected";  
    _errorMap[ovrError_TrackerMemoryReadFailure      ] = "ovrError_TrackerMemoryReadFailure";  
    _errorMap[ovrError_TrackerMemoryWriteFailure     ] = "ovrError_TrackerMemoryWriteFailure";  
    _errorMap[ovrError_TrackerFrameTimeout           ] = "ovrError_TrackerFrameTimeout";  
    _errorMap[ovrError_TrackerTruncatedFrame         ] = "ovrError_TrackerTruncatedFrame";  
    _errorMap[ovrError_HMDFirmwareMismatch           ] = "ovrError_HMDFirmwareMismatch";  
    _errorMap[ovrError_TrackerFirmwareMismatch       ] = "ovrError_TrackerFirmwareMismatch";  
    _errorMap[ovrError_BootloaderDeviceDetected      ] = "ovrError_BootloaderDeviceDetected";  
    _errorMap[ovrError_TrackerCalibrationError       ] = "ovrError_TrackerCalibrationError";  
    _errorMap[ovrError_ControllerFirmwareMismatch    ] = "ovrError_ControllerFirmwareMismatch";  
    _errorMap[ovrError_Incomplete                    ] = "ovrError_Incomplete";  
    _errorMap[ovrError_Abandoned                     ] = "ovrError_Abandoned";  
    _errorMap[ovrError_DisplayLost                   ] = "ovrError_DisplayLost";  
    _errorMap[ovrError_RuntimeException              ] = "ovrError_RuntimeException";

    _successMap[ovrSuccess                           ] = "ovrSuccess"; 
    _successMap[ovrSuccess_NotVisible                ] = "ovrSuccess_NotVisible";
    _successMap[ovrSuccess_HMDFirmwareMismatch       ] = "ovrSuccess_HMDFirmwareMismatch";
    _successMap[ovrSuccess_TrackerFirmwareMismatch   ] = "ovrSuccess_TrackerFirmwareMismatch";
    _successMap[ovrSuccess_ControllerFirmwareMismatch] = "ovrSuccess_ControllerFirmwareMismatch";
}

std::string getOvrResultString(ovrResult ovrResult)
{
	std::string sError = "<unknown ovrResult code>";
	
	if (OVR_SUCCESS(ovrResult))
	{
		std::map<ovrSuccessType, std::string>::const_iterator it = _successMap.find((ovrSuccessType)ovrResult);
		if (it != _successMap.end()) 
		{
			sError = it->second;
		}
	}
	else
	{
		std::map<ovrErrorType, std::string>::const_iterator it = _errorMap.find((ovrErrorType)ovrResult);
		if (it != _errorMap.end()) 
		{
			sError = it->second;
		}
	}

	return sError;
}
