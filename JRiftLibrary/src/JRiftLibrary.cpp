#include "de_fruitfly_ovr_OculusRift.h"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#if defined(OVR_OS_WIN32)
#include "Windows.h"
#endif
#include "OVR_CAPI_GL.h"

using namespace OVR;

ovrHmd              _pHmd             = 0;
ovrHmdDesc          _hmdDesc; 
int                 _hmdIndex         = -1;
bool                _initialised      = false;
bool                _renderConfigured = false;
bool                _realDevice       = false;
ovrSwapTextureSet*  _pSwapTextureSet[2];
ovrGLTexture*       _pMirrorTexture   = 0;

ovrPosef            _eyeRenderPose[2];
ovrGLTexture        _GLEyeTexture[2];

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

	Reset();

    if (!CacheJNIGlobals(env))
    {
        return false;
    }

	// Initialise LibOVR - use default init params for now
	ovrInitParams initParams;
	memset(&initParams, 0, sizeof(ovrInitParams));
	if (ovr_Initialize(&initParams) != ovrSuccess) 
	{
		printf("Unable to initialise LibOVR!\n");
		return false;
	}

    // Create HMD
    if (CreateHmdAndConfigureTracker())
        _initialised = true;
	
	if (!_initialised)
	{
		printf("Unable to create Oculus Rift device interface!\n");
	}

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
                                      (int)_pHmd->EyeRenderOrder[0],
                                      (int)_hmdDesc.EyeRenderOrder[1],
                                      displayDeviceName,  // DisplayDeviceName no longer used, remove,
                                      0, // Display ID no longer used, remove,
                                      _realDevice
            );

    env->DeleteLocalRef( productName );
    env->DeleteLocalRef( manufacturer );
    env->DeleteLocalRef( displayDeviceName );

    if (jHmdDesc == 0) PrintNewObjectException(env, "HmdDesc");

    return jHmdDesc;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getTrackerState(JNIEnv *env, jobject, jdouble time) 
{
	if (!_initialised) 
        return 0;

	// Get sensorstate at the specified time in the future from now (0.0 means 'now')
	ovrTrackingState ss = ovr_GetTrackingState(_pHmd, time);

    ClearException(env);

    jobject jss = env->NewObject(trackerState_Class, trackerState_constructor_MethodID,
                                 ss.HeadPose.ThePose.Orientation.x,   
                                 ss.HeadPose.ThePose.Orientation.y,  
                                 ss.HeadPose.ThePose.Orientation.z,   
                                 ss.HeadPose.ThePose.Orientation.w,   
                                 ss.HeadPose.ThePose.Position.x,      
                                 ss.HeadPose.ThePose.Position.y,      
                                 ss.HeadPose.ThePose.Position.z,      
                                 ss.HeadPose.AngularVelocity.x,    
                                 ss.HeadPose.AngularVelocity.y,    
                                 ss.HeadPose.AngularVelocity.z,    
                                 ss.HeadPose.LinearVelocity.x,     
                                 ss.HeadPose.LinearVelocity.y,     
                                 ss.HeadPose.LinearVelocity.z,     
                                 ss.HeadPose.AngularAcceleration.x,
                                 ss.HeadPose.AngularAcceleration.y,
                                 ss.HeadPose.AngularAcceleration.z,
                                 ss.HeadPose.LinearAcceleration.x, 
                                 ss.HeadPose.LinearAcceleration.y, 
                                 ss.HeadPose.LinearAcceleration.z, 
                                 ss.HeadPose.TimeInSeconds,        
                                 ss.RawSensorData.Temperature,
                                 ss.StatusFlags
                                 );

    if (jss == 0) PrintNewObjectException(env, "TrackerState");

    return jss;
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
									_pHmd->Resolution.w,
									_pHmd->Resolution.h,
                                    (float)RenderScaleFactor
                                    );

    if (jfovTextureInfo == 0) PrintNewObjectException(env, "FovTextureInfo");

    return jfovTextureInfo;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1createSwapTextureSet(
	JNIEnv *env, 
	jobject,
	jint width,
	jint height
	)
{
	if (!_initialised)
		return 0;

	DestroySwapTextureSet();

	boolean result = true;

	if (result && ovr_CreateSwapTextureSetGL(_pHmd, GL_RGBA, width, height, &_pSwapTextureSet[0]) != ovrSuccess)
	{
		result = false;	
	}
	if (result && ovr_CreateSwapTextureSetGL(_pHmd, GL_RGBA, width, height, &_pSwapTextureSet[1]) != ovrSuccess)
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

	if (ovr_CreateMirrorTextureGL(_pHmd, GL_RGBA, width, height, (ovrTexture**)&_pMirrorTexture) != ovrSuccess)
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

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1configureRendering(
	JNIEnv *env, 
	jobject, 
	jboolean UsesInputTexture1Only,
	jint InTexture1Width,
	jint InTexture1Height,
	jint InTexture1GLId,
	jint InTexture2Width,
	jint InTexture2Height,
	jint InTexture2GLId,
	jint OutDisplayWidth,
	jint OutDisplayHeight,
	jlong Win, 
	jlong Displ,
	jboolean VSyncEnabled,
    jint MultiSample,
    jboolean UseTimewarp,
	jboolean UseTimewarpJitDelay,
	jboolean UseVignette,
	jboolean UseLowPersistence,
    jboolean MirrorDisplay,
    jboolean UseDisplayOverdrive,
	jboolean DynamicPrediction,
	jboolean UseHighQualityDistortion,
	jboolean UseProfileNoSpinWaits,
	jfloat leftFovUpTan,
	jfloat leftFovDownTan,
	jfloat leftFovLeftTan,
	jfloat leftFovRightTan,
	jfloat rightFovUpTan,
	jfloat rightFovDownTan,
	jfloat rightFovLeftTan,
	jfloat rightFovRightTan,
	jfloat worldScale
	)
{
	if (!_initialised)
		return 0;

    // Initialize eye rendering information for ovrHmd_Configure.
    // The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
	
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

    ovrRecti   EyeRenderViewport[2];
	ovrFovPort eyeFov[2] = { leftFov, rightFov } ;

	if (UsesInputTexture1Only) // Same texture used, has both views
	{
		EyeRenderViewport[0].Pos  = Vector2i(0,0);
		EyeRenderViewport[0].Size = Sizei(InTexture1Width / 2, InTexture1Height);
		EyeRenderViewport[1].Pos  = Vector2i((InTexture1Width + 1) / 2, 0);
		EyeRenderViewport[1].Size = EyeRenderViewport[0].Size;

		// Setup GL texture data.
		_GLEyeTexture[0].OGL.Header.API            = ovrRenderAPI_OpenGL;
		_GLEyeTexture[0].OGL.Header.TextureSize.w  = InTexture1Width;
		_GLEyeTexture[0].OGL.Header.TextureSize.h  = InTexture1Height;
//		_GLEyeTexture[0].OGL.Header.RenderViewport = EyeRenderViewport[0];
		_GLEyeTexture[0].OGL.TexId                 = (GLuint)InTexture1GLId;
    
		// Right eye uses the same texture, but different rendering viewport.
		_GLEyeTexture[1] = _GLEyeTexture[0];
//		_GLEyeTexture[1].OGL.Header.RenderViewport = EyeRenderViewport[1];
	}
	else // Uses individual input textures for each view
	{
		EyeRenderViewport[0].Pos  = Vector2i(0,0);
		EyeRenderViewport[0].Size = Sizei(InTexture1Width, InTexture1Height);
		EyeRenderViewport[1].Pos  = Vector2i(0, 0);
		EyeRenderViewport[1].Size = Sizei(InTexture2Width, InTexture2Height);

		// Setup GL texture data.
		_GLEyeTexture[0].OGL.Header.API            = ovrRenderAPI_OpenGL;
		_GLEyeTexture[0].OGL.Header.TextureSize.w  = InTexture1Width;
		_GLEyeTexture[0].OGL.Header.TextureSize.h  = InTexture1Height;
//		_GLEyeTexture[0].OGL.Header.RenderViewport = EyeRenderViewport[0];
		_GLEyeTexture[0].OGL.TexId                 = (GLuint)InTexture1GLId;

		_GLEyeTexture[1].OGL.Header.API            = ovrRenderAPI_OpenGL;
		_GLEyeTexture[1].OGL.Header.TextureSize.w  = InTexture2Width;
		_GLEyeTexture[1].OGL.Header.TextureSize.h  = InTexture2Height;
//		_GLEyeTexture[1].OGL.Header.RenderViewport = EyeRenderViewport[1];
		_GLEyeTexture[1].OGL.TexId                 = (GLuint)InTexture2GLId;
	}

	// Configure OpenGL. 
	ovrGLConfig cfg; 
	cfg.OGL.Header.API            = ovrRenderAPI_OpenGL; 
	cfg.OGL.Header.BackBufferSize = Sizei(OutDisplayWidth, OutDisplayHeight); 
	cfg.OGL.Header.Multisample    = MultiSample; 

	// Cast context pointers to 32 / 64 bit as appropriate
#if defined(OVR_OS_WIN32)
    cfg.OGL.Window = (HWND)(intptr_t)Win;
	cfg.OGL.DC     = ::GetDC(cfg.OGL.Window);
#elif defined(OVR_OS_LINUX)
    cfg.OGL.Disp   = (_XDisplay*)(intptr_t)Displ;
#endif
	 
	unsigned int DistortionCaps = 0;
    if (UseTimewarp)
        DistortionCaps |= ovrDistortionCap_TimeWarp;
	if (UseTimewarpJitDelay)
        DistortionCaps |= ovrDistortionCap_TimewarpJitDelay;
	if (UseVignette)
        DistortionCaps |= ovrDistortionCap_Vignette;
    if (UseDisplayOverdrive)  
        DistortionCaps |= ovrDistortionCap_Overdrive;
	if (UseHighQualityDistortion)
		DistortionCaps |= ovrDistortionCap_HqDistortion;
	if (UseProfileNoSpinWaits)
        DistortionCaps |= ovrDistortionCap_ProfileNoSpinWaits;

#if defined(OVR_OS_LINUX)
    DistortionCaps |= ovrDistortionCap_LinuxDevFullscreen;
#endif

	//DistortionCaps |= ovrDistortionCap_SRGB;
    
	ovrEyeRenderDesc EyeRenderDesc[2];

    // Set VSync and low persistence etc.
    unsigned int HmdCaps = 0;
    if (!VSyncEnabled)
        HmdCaps |= ovrHmdCap_NoVSync;
    if (UseLowPersistence)
        HmdCaps |= ovrHmdCap_LowPersistence;
    if (!MirrorDisplay)
        HmdCaps |= ovrHmdCap_NoMirrorToWindow;
    if (DynamicPrediction)  
        HmdCaps |= ovrHmdCap_DynamicPrediction;

	ovr_SetEnabledCaps(_pHmd, HmdCaps); 

    // Configure render setup
    ovrBool result = ovr_ConfigureRendering(_pHmd, &cfg.Config, DistortionCaps, eyeFov, EyeRenderDesc);
	if (!result)
	{
		printf("ovrHmd_ConfigureRendering() - ERROR: failure\n");
        ResetRenderConfig();
		return 0;
	}
	
    _renderConfigured = true;

	// Set worldScale
	EyeRenderDesc[0].HmdToEyeViewOffset.x *= worldScale;
	EyeRenderDesc[0].HmdToEyeViewOffset.y *= worldScale;
    EyeRenderDesc[0].HmdToEyeViewOffset.z *= worldScale;
	EyeRenderDesc[1].HmdToEyeViewOffset.x *= worldScale;
	EyeRenderDesc[1].HmdToEyeViewOffset.y *= worldScale;
    EyeRenderDesc[1].HmdToEyeViewOffset.z *= worldScale;

	jobject eyeRenderDesc = env->NewObject(eyeRenderParams_Class, eyeRenderParams_constructor_MethodID,
                                           EyeRenderDesc[0].Eye,
                                           EyeRenderViewport[0].Pos.x,
                                           EyeRenderViewport[0].Pos.y,
                                           EyeRenderViewport[0].Size.w,
                                           EyeRenderViewport[0].Size.h,
                                           EyeRenderDesc[0].Fov.UpTan,
                                           EyeRenderDesc[0].Fov.DownTan,
                                           EyeRenderDesc[0].Fov.LeftTan,
                                           EyeRenderDesc[0].Fov.RightTan,
                                           EyeRenderDesc[0].DistortedViewport.Pos.x, 
                                           EyeRenderDesc[0].DistortedViewport.Pos.y,
                                           EyeRenderDesc[0].DistortedViewport.Size.w,
                                           EyeRenderDesc[0].DistortedViewport.Size.h,
                                           EyeRenderDesc[0].PixelsPerTanAngleAtCenter.x,
                                           EyeRenderDesc[0].PixelsPerTanAngleAtCenter.y,
                                           EyeRenderDesc[0].HmdToEyeViewOffset.x,
                                           EyeRenderDesc[0].HmdToEyeViewOffset.y,
                                           EyeRenderDesc[0].HmdToEyeViewOffset.z,
                                           EyeRenderDesc[1].Eye,
                                           EyeRenderViewport[1].Pos.x,
                                           EyeRenderViewport[1].Pos.y,
                                           EyeRenderViewport[1].Size.w,
                                           EyeRenderViewport[1].Size.h,
                                           EyeRenderDesc[1].Fov.UpTan,
                                           EyeRenderDesc[1].Fov.DownTan,
                                           EyeRenderDesc[1].Fov.LeftTan,
                                           EyeRenderDesc[1].Fov.RightTan,
                                           EyeRenderDesc[1].DistortedViewport.Pos.x, 
                                           EyeRenderDesc[1].DistortedViewport.Pos.y,
                                           EyeRenderDesc[1].DistortedViewport.Size.w,
                                           EyeRenderDesc[1].DistortedViewport.Size.h,
                                           EyeRenderDesc[1].PixelsPerTanAngleAtCenter.x,
                                           EyeRenderDesc[1].PixelsPerTanAngleAtCenter.y,
                                           EyeRenderDesc[1].HmdToEyeViewOffset.x,
                                           EyeRenderDesc[1].HmdToEyeViewOffset.y,
                                           EyeRenderDesc[1].HmdToEyeViewOffset.z,
										   worldScale
										);

    return eyeRenderDesc;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1resetRenderConfig(JNIEnv *env, jobject)
{
	ResetRenderConfig();
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getEyePoses(
	JNIEnv *env, 
	jobject, 
	jint FrameIndex,
	jfloat HmdToLeftEyeViewOffsetX,
	jfloat HmdToLeftEyeViewOffsetY,
	jfloat HmdToLeftEyeViewOffsetZ,
	jfloat HmdToRightEyeViewOffsetX,
	jfloat HmdToRightEyeViewOffsetY,
	jfloat HmdToRightEyeViewOffsetZ
	)
{
    if (!_initialised)
        return 0;

	ovrVector3f ViewOffsets[2];
	ViewOffsets[0].x = HmdToLeftEyeViewOffsetX;
	ViewOffsets[0].y = HmdToLeftEyeViewOffsetY;
	ViewOffsets[0].z = HmdToLeftEyeViewOffsetZ;
	ViewOffsets[1].x = HmdToRightEyeViewOffsetX;
	ViewOffsets[1].y = HmdToRightEyeViewOffsetY;
	ViewOffsets[1].z = HmdToRightEyeViewOffsetZ;

	ovrTrackingState ss;

	// Get both eye poses, and the tracking state in one hit
	ovr_GetEyePoses(_pHmd, (unsigned int)FrameIndex, ViewOffsets, _eyeRenderPose, &ss);

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
								 ss.HeadPose.ThePose.Orientation.x,   
								 ss.HeadPose.ThePose.Orientation.y,  
								 ss.HeadPose.ThePose.Orientation.z,   
								 ss.HeadPose.ThePose.Orientation.w,   
								 ss.HeadPose.ThePose.Position.x,      
								 ss.HeadPose.ThePose.Position.y,      
								 ss.HeadPose.ThePose.Position.z,      
								 ss.HeadPose.AngularVelocity.x,    
								 ss.HeadPose.AngularVelocity.y,    
								 ss.HeadPose.AngularVelocity.z,    
								 ss.HeadPose.LinearVelocity.x,     
								 ss.HeadPose.LinearVelocity.y,     
								 ss.HeadPose.LinearVelocity.z,     
								 ss.HeadPose.AngularAcceleration.x,
								 ss.HeadPose.AngularAcceleration.y,
								 ss.HeadPose.AngularAcceleration.z,
								 ss.HeadPose.LinearAcceleration.x, 
								 ss.HeadPose.LinearAcceleration.y, 
								 ss.HeadPose.LinearAcceleration.z, 
								 ss.HeadPose.TimeInSeconds,        
								 ss.RawSensorData.Temperature,
								 ss.StatusFlags
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
    
    ovrLayerEyeFov ld;
    ld.Header.Type  = ovrLayerType_EyeFov;
    ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
    
    for (int eye = 0; eye < 2; eye++)
    {
        ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
        ld.Viewport[eye]     = Recti(eyeRenderTexture[eye]->GetSize());
        ld.Fov[eye]          = HMD->DefaultEyeFov[eye];
        ld.RenderPose[eye]   = EyeRenderPose[eye];
    }
    
    ovrLayerHeader* layers = &ld.Header;
    ovrResult result = ovr_SubmitFrame(HMD, 0, &viewScaleDesc, &layers, 1);

    // Let OVR do distortion rendering, present and flush/sync
    //ovrHmd_EndFrame(_pHmd, _eyeRenderPose, &_GLEyeTexture[0].Texture);
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

	float playerHeight = ovrHmd_GetFloat( _pHmd, OVR_KEY_PLAYER_HEIGHT, OVR_DEFAULT_PLAYER_HEIGHT);
	float eyeHeight    = ovrHmd_GetFloat( _pHmd, OVR_KEY_EYE_HEIGHT,    OVR_DEFAULT_EYE_HEIGHT); 
	float ipd          = ovrHmd_GetFloat( _pHmd, OVR_KEY_IPD,           OVR_DEFAULT_IPD); 
	std::string gender = ovrHmd_GetString(_pHmd, OVR_KEY_GENDER,        OVR_DEFAULT_GENDER);
    std::string name   = ovrHmd_GetString(_pHmd, OVR_KEY_NAME,          "No Profile");

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
    if (_initialised)
    {
        ovr_ConfigureRendering(_pHmd, 0, 0, 0, 0);
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

    _renderConfigured = false;
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
        _realDevice = true;
		result = true;

		_hmdDesc = ovr_GetHmdDesc(_pHmd);
	}
	else 
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

	if (result)
	{
		// Log description
		LogHmdDesc(_pHmd);

        // Set initial hmd caps
		ovr_SetEnabledCaps(_pHmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

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
                         "(IFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDFI)V"))
    {
        return false;
    }
	field_swapTextureSet_leftEyeTextureIds = env->GetFieldID(swapTextureSet_Class, "leftEyeTextureIds", "Lde/fruitfly/ovr/structs/SwapTextureSet;");
	if (field_swapTextureSet_leftEyeTextureIds == 0)
    {
		printf("Failed to find field 'Lde/fruitfly/ovr/structs/SwapTextureSet;' leftEyeTextureIds");
        return false; 
    }
	field_swapTextureSet_rightEyeTextureIds = env->GetFieldID(swapTextureSet_Class, "rightEyeTextureIds", "Lde/fruitfly/ovr/structs/SwapTextureSet;");
	if (field_swapTextureSet_rightEyeTextureIds == 0)
    {
		printf("Failed to find field 'Lde/fruitfly/ovr/structs/SwapTextureSet;' rightEyeTextureIds");
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

	// Lookup some standard java classes / methods
    arrayListClass = env->FindClass("java/util/ArrayList");
	if (arrayListClass == 0) 
	{
		printf("Failed to find class 'java/util/ArrayList'");
		return false;
	}

	method_arrayList_init = env->GetMethodID(arrayListClass, "<init>", "()V");
	if (method_arrayList_init == 0) 
	{
		printf("Failed to find method 'java/util/ArrayList' <init>()V");
		return false;
	}

	method_arrayList_add = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");
	if (method_arrayList_add == 0) 
	{
		printf("Failed to find method 'java/util/ArrayList' add(Ljava/lang/Object;)Z");
		return false;
	}

	jclass integerClass = env->FindClass("java/lang/Integer");
	if (integerClass == 0) 
	{
		printf("Failed to find class 'java/lang/Integer'");
		return false;
	}
    jmethodID method_integer_init = env->GetMethodID(integerClass, "<init>", "(I)V");
	if (method_integer_init == 0) 
	{
		printf("Failed to find method 'java/lang/Integer' <init>(I)V");
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
