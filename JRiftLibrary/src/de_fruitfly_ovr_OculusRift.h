#include <jni.h>
#include <string>
#include <memory>
#include "OVR_Math.h"

/* Header for class de_fruitfly_ovr_OculusRift */

#ifndef _Included_de_fruitfly_ovr_OculusRift
#define _Included_de_fruitfly_ovr_OculusRift
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    initSubsystem
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1initSubsystem
    (JNIEnv *, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    destroySubsystem
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroySubsystem
  (JNIEnv *, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    getHmdDesc
 * Signature: (F)Lde/fruitfly/ovr/struct/HmdDesc;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getHmdDesc
    (JNIEnv *, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    getNextHmd
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1getNextHmd
    (JNIEnv *, jobject); 

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    getTrackerState
 * Signature: (D)Lde/fruitfly/ovr/structs/TrackerState;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getTrackerState
    (JNIEnv *, jobject, jdouble);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    resetTracking
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1resetTracking
    (JNIEnv *, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    getFovTextureSize
 * Signature: (FFFFFFFFF)Lde/fruitfly/ovr/struct/FovTextureInfo;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getFovTextureSize(
	JNIEnv *, 
	jobject, 
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat
	);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    createSwapTextureSet
 * Signature: (II)Lde/fruitfly/ovr/SwapTextureSet;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1createSwapTextureSet(
	JNIEnv *env, 
	jobject,
	jint width,
	jint height
	);
    
/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    setCurrentSwapTextureIndex
 * Signature: (I)Z;
 */
JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1setCurrentSwapTextureIndex
    (JNIEnv *env,
     jobject,
     jint index);
    
/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    destroySwapTextureSet
 * Signature: ()V;
 */
JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroySwapTextureSet
    (JNIEnv *env, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    createMirrorTexture
 * Signature: (II)I;
 */
JNIEXPORT jint JNICALL Java_de_fruitfly_ovr_OculusRift__1createMirrorTexture(
	JNIEnv *env, 
	jobject,
	jint width,
	jint height
	);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    destroyMirrorTexture
 * Signature: ()V;
 */
JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroyMirrorTexture
    (JNIEnv *env, jobject);
    
/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    configureRendering
 * Signature: (ZIIIIIJJZIZZZZZZZFFFFFFFFF)Lde/fruitfly/ovr/EyeRenderParams;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1configureRendering(
	JNIEnv *, 
	jobject,
	jboolean,
    jint,
    jint,
	jint,
	jint,
	jint,
	jint,
	jint,
	jint,
	jlong, 
	jlong,
	jboolean,
    jint,
    jboolean,
    jboolean,
    jboolean,
	jboolean,
    jboolean,
	jboolean,
	jboolean,
	jboolean,
	jboolean,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat,
	jfloat
	);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    resetRenderConfig
 * Signature: ()V;
 */
JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1resetRenderConfig
	(JNIEnv *env, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    getEyePoses
 * Signature: (IFFFFFF)Lde/fruitfly/ovr/FullPoseState;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getEyePoses(
	JNIEnv *env, 
	jobject, 
	jint FrameIndex,
	jfloat HmdToLeftEyeViewOffsetX,
	jfloat HmdToLeftEyeViewOffsetY,
	jfloat HmdToLeftEyeViewOffsetZ,
	jfloat HmdToRightEyeViewOffsetX,
	jfloat HmdToRightEyeViewOffsetY,
	jfloat HmdToRightEyeViewOffsetZ);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    getMatrix4fProjection
 * Signature: (FFFFFF)Lde/fruitfly/ovr/structs/Matrix4f;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getMatrix4fProjection(
    JNIEnv *, 
    jobject, 
    jfloat,
    jfloat,
    jfloat,
    jfloat,
    jfloat,
    jfloat);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    submitFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1submitFrame
    (JNIEnv *, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    _convertQuatToEuler
 * Signature: (FFFFFIIIII)[Lde/fruitfly/ovr/structs/EulerOrient;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1convertQuatToEuler
  (JNIEnv *, 
  jobject, 
  jfloat, 
  jfloat, 
  jfloat, 
  jfloat, 
  jfloat, 
  jint, 
  jint, 
  jint, 
  jint, 
  jint);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    _getUserProfileData
 * Signature: L de/fruitfly/ovr/UserProfileData
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getUserProfileData(
   JNIEnv *env, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    _getVersionString
 * Signature: ()Ljava/lang/String
 */
JNIEXPORT jstring JNICALL Java_de_fruitfly_ovr_OculusRift__1getVersionString(
   JNIEnv *env, jobject);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    _getCurrentTimeSecs
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_de_fruitfly_ovr_OculusRift__1getCurrentTimeSecs(
   JNIEnv *env, jobject);


/* Helpers */
void DEBUGLOG(std::string s);
void LogHmdDesc(ovrHmd pHmd);
void Reset();
void ResetRenderConfig();
void DestroySwapTextureSet();
void DestroyMirrorTexture();
bool CacheJNIGlobals(JNIEnv *env);
bool CreateHmdAndConfigureTracker(int hmdIndex);
bool LookupJNIGlobal(JNIEnv *env,
                     jclass& clazz,
                     std::string className,
                     jmethodID& method,
                     std::string constructorSignature);
void ClearException(JNIEnv *env);
void PrintNewObjectException(JNIEnv *env, std::string objectName);
void SetEulerEnumValues(int firstRotationAxis,
					    int secondRotationAxis,
					    int thirdRotationAxis,
					    int rotationDir,
					    int hand,
					    OVR::Axis& A1,
						OVR::Axis& A2,
					    OVR::Axis& A3,
					    OVR::RotateDirection& D,
					    OVR::HandedSystem& S);
void SetAxisEnum(int value, OVR::Axis& A);

#ifdef __cplusplus
}
#endif
#endif
