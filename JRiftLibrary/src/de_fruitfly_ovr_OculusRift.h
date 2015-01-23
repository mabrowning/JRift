#include <jni.h>
#include <string>
#include <memory>
#include "OVR.h"

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
 * Method:    configureRendering
 * Signature: (IIIIIJJZIZZZZZZZFFFFFFFF)Lde/fruitfly/ovr/EyeRenderParams;
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
 * Method:    beginFrame
 * Signature: (I)Lde/fruitfly/ovr/structs/FrameTiming;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1beginFrame
    (JNIEnv *, jobject, jint);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    getEyePose
 * Signature: (I)Lde/fruitfly/ovr/structs/Posef;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getEyePose
    (JNIEnv *, jobject, jint);

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
 * Method:    getViewFromEyePose
 * Signature: (FFFFFFFFFFFFFFF)Lde/fruitfly/ovr/structs/Matrix4f;
 */
JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getViewFromEyePose(
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
    jfloat,
    jfloat,
    jfloat,
    jfloat,
	jfloat,
    jfloat,
    jfloat);

/*
 * Class:     de_fruitfly_ovr_OculusRift
 * Method:    endFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1endFrame
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
 * Method:    _initRenderingShim
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1initRenderingShim(
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
