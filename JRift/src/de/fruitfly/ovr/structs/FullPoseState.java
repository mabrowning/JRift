package de.fruitfly.ovr.structs;

import de.fruitfly.ovr.enums.EyeType;

public class FullPoseState
{
    public long frameIndex = 0;
    public Posef leftEyePose = new Posef();
    public Posef rightEyePose = new Posef();
    public Posef leftHandPose = new Posef();
    public Posef rightHandPose = new Posef();
    public Posef cameraPose = new Posef();
    public Posef levelledCameraPose = new Posef();
    public PoseStatef centerEyePose = new PoseStatef();
    public float temperature;
    public int   hmdStatusFlags;
    public int   leftHandStatusFlags;
    public int   rightHandStatusFlags;
    public int   counter;

    public FullPoseState() {}

    public FullPoseState(long frameIndex,
                         float Lquatx,
                         float Lquaty,
                         float Lquatz,
                         float Lquatw,
                         float Lposx,
                         float Lposy,
                         float Lposz,
                         float Rquatx,
                         float Rquaty,
                         float Rquatz,
                         float Rquatw,
                         float Rposx,
                         float Rposy,
                         float Rposz,
                         float PredictedPoseStatefPosefOrientationx,
                         float PredictedPoseStatefPosefOrientationy,
                         float PredictedPoseStatefPosefOrientationz,
                         float PredictedPoseStatefPosefOrientationw,
                         float PredictedPoseStatefPosefPositionx,
                         float PredictedPoseStatefPosefPositiony,
                         float PredictedPoseStatefPosefPositionz,
                         float PredictedVector3fAngularVelocityx,
                         float PredictedVector3fAngularVelocityy,
                         float PredictedVector3fAngularVelocityz,
                         float PredictedVector3fLinearVelocityx,
                         float PredictedVector3fLinearVelocityy,
                         float PredictedVector3fLinearVelocityz,
                         float PredictedVector3fAngularAccelerationx,
                         float PredictedVector3fAngularAccelerationy,
                         float PredictedVector3fAngularAccelerationz,
                         float PredictedVector3fLinearAccelerationx,
                         float PredictedVector3fLinearAccelerationy,
                         float PredictedVector3fLinearAccelerationz,
                         double PredictedTimeInSeconds,
                         float temp,
                         int statusFlags,
                         float Cameraquatx,
                         float Cameraquaty,
                         float Cameraquatz,
                         float Cameraquatw,
                         float Cameraposx,
                         float Cameraposy,
                         float Cameraposz,
                         float LevelledCameraquatx,
                         float LevelledCameraquaty,
                         float LevelledCameraquatz,
                         float LevelledCameraquatw,
                         float LevelledCameraposx,
                         float LevelledCameraposy,
                         float LevelledCameraposz,
                         float LHandquatx,
                         float LHandquaty,
                         float LHandquatz,
                         float LHandquatw,
                         float LHandposx,
                         float LHandposy,
                         float LHandposz,
                         int lhandstatusFlags,
                         float RHandquatx,
                         float RHandquaty,
                         float RHandquatz,
                         float RHandquatw,
                         float RHandposx,
                         float RHandposy,
                         float RHandposz,
                         int rhandstatusFlags,
                         int count)
    {
        this.frameIndex = frameIndex;

        leftEyePose.Orientation.x  = Lquatx;
        leftEyePose.Orientation.y  = Lquaty;
        leftEyePose.Orientation.z  = Lquatz;
        leftEyePose.Orientation.w  = Lquatw;
        leftEyePose.Position.x     = Lposx;
        leftEyePose.Position.y     = Lposy;
        leftEyePose.Position.z     = Lposz;

        rightEyePose.Orientation.x = Rquatx;
        rightEyePose.Orientation.y = Rquaty;
        rightEyePose.Orientation.z = Rquatz;
        rightEyePose.Orientation.w = Rquatw;
        rightEyePose.Position.x    = Rposx;
        rightEyePose.Position.y    = Rposy;
        rightEyePose.Position.z    = Rposz;

        // Center eye pose
        centerEyePose.ThePose.Orientation.x    = PredictedPoseStatefPosefOrientationx;
        centerEyePose.ThePose.Orientation.y    = PredictedPoseStatefPosefOrientationy;
        centerEyePose.ThePose.Orientation.z    = PredictedPoseStatefPosefOrientationz;
        centerEyePose.ThePose.Orientation.w    = PredictedPoseStatefPosefOrientationw;
        centerEyePose.ThePose.Position.x       = PredictedPoseStatefPosefPositionx;
        centerEyePose.ThePose.Position.y       = PredictedPoseStatefPosefPositiony;
        centerEyePose.ThePose.Position.z       = PredictedPoseStatefPosefPositionz;

        centerEyePose.AngularVelocity.x     = PredictedVector3fAngularVelocityx;
        centerEyePose.AngularVelocity.y     = PredictedVector3fAngularVelocityy;
        centerEyePose.AngularVelocity.z     = PredictedVector3fAngularVelocityz;
        centerEyePose.LinearVelocity.x      = PredictedVector3fLinearVelocityx;
        centerEyePose.LinearVelocity.y      = PredictedVector3fLinearVelocityy;
        centerEyePose.LinearVelocity.z      = PredictedVector3fLinearVelocityz;
        centerEyePose.AngularAcceleration.x = PredictedVector3fAngularAccelerationx;
        centerEyePose.AngularAcceleration.y = PredictedVector3fAngularAccelerationy;
        centerEyePose.AngularAcceleration.z = PredictedVector3fAngularAccelerationz;
        centerEyePose.LinearAcceleration.x  = PredictedVector3fLinearAccelerationx;
        centerEyePose.LinearAcceleration.y  = PredictedVector3fLinearAccelerationy;
        centerEyePose.LinearAcceleration.z  = PredictedVector3fLinearAccelerationz;
        centerEyePose.TimeInSeconds         = PredictedTimeInSeconds;

        temperature = temp;
        hmdStatusFlags = statusFlags;

        cameraPose.Orientation.x  = Cameraquatx;
        cameraPose.Orientation.y  = Cameraquaty;
        cameraPose.Orientation.z  = Cameraquatz;
        cameraPose.Orientation.w  = Cameraquatw;
        cameraPose.Position.x     = Cameraposx;
        cameraPose.Position.y     = Cameraposy;
        cameraPose.Position.z     = Cameraposz;

        levelledCameraPose.Orientation.x  = LevelledCameraquatx;
        levelledCameraPose.Orientation.y  = LevelledCameraquaty;
        levelledCameraPose.Orientation.z  = LevelledCameraquatz;
        levelledCameraPose.Orientation.w  = LevelledCameraquatw;
        levelledCameraPose.Position.x     = LevelledCameraposx;
        levelledCameraPose.Position.y     = LevelledCameraposy;
        levelledCameraPose.Position.z     = LevelledCameraposz;

        leftHandPose.Orientation.x  = LHandquatx;
        leftHandPose.Orientation.y  = LHandquaty;
        leftHandPose.Orientation.z  = LHandquatz;
        leftHandPose.Orientation.w  = LHandquatw;
        leftHandPose.Position.x     = LHandposx;
        leftHandPose.Position.y     = LHandposy;
        leftHandPose.Position.z     = LHandposz;
        leftHandStatusFlags         = lhandstatusFlags;

        rightHandPose.Orientation.x = RHandquatx;
        rightHandPose.Orientation.y = RHandquaty;
        rightHandPose.Orientation.z = RHandquatz;
        rightHandPose.Orientation.w = RHandquatw;
        rightHandPose.Position.x    = RHandposx;
        rightHandPose.Position.y    = RHandposy;
        rightHandPose.Position.z    = RHandposz;
        rightHandStatusFlags        = rhandstatusFlags;

        counter = count;
    }

    public Posef getEyePose(EyeType eye)
    {
        if (eye == EyeType.ovrEye_Right)
            return rightEyePose;
        else if (eye == EyeType.ovrEye_Left)
            return leftEyePose;
        else
            return centerEyePose.ThePose;
    }

    public FullPoseState clone()
    {
        FullPoseState cfps = new FullPoseState(
                frameIndex,
                leftEyePose.Orientation.x,
                leftEyePose.Orientation.y,
                leftEyePose.Orientation.z,
                leftEyePose.Orientation.w,
                leftEyePose.Position.x,
                leftEyePose.Position.y,
                leftEyePose.Position.z,
                rightEyePose.Orientation.x,
                rightEyePose.Orientation.y,
                rightEyePose.Orientation.z,
                rightEyePose.Orientation.w,
                rightEyePose.Position.x,
                rightEyePose.Position.y,
                rightEyePose.Position.z,
                centerEyePose.ThePose.Orientation.x,
                centerEyePose.ThePose.Orientation.y,
                centerEyePose.ThePose.Orientation.z,
                centerEyePose.ThePose.Orientation.w,
                centerEyePose.ThePose.Position.x,
                centerEyePose.ThePose.Position.y,
                centerEyePose.ThePose.Position.z,
                centerEyePose.AngularVelocity.x    ,
                centerEyePose.AngularVelocity.y    ,
                centerEyePose.AngularVelocity.z    ,
                centerEyePose.LinearVelocity.x     ,
                centerEyePose.LinearVelocity.y     ,
                centerEyePose.LinearVelocity.z     ,
                centerEyePose.AngularAcceleration.x,
                centerEyePose.AngularAcceleration.y,
                centerEyePose.AngularAcceleration.z,
                centerEyePose.LinearAcceleration.x ,
                centerEyePose.LinearAcceleration.y ,
                centerEyePose.LinearAcceleration.z ,
                centerEyePose.TimeInSeconds        ,
                temperature,
                hmdStatusFlags,
                cameraPose.Orientation.x,
                cameraPose.Orientation.y,
                cameraPose.Orientation.z,
                cameraPose.Orientation.w,
                cameraPose.Position.x,
                cameraPose.Position.y,
                cameraPose.Position.z,
                levelledCameraPose.Orientation.x,
                levelledCameraPose.Orientation.y,
                levelledCameraPose.Orientation.z,
                levelledCameraPose.Orientation.w,
                levelledCameraPose.Position.x,
                levelledCameraPose.Position.y,
                levelledCameraPose.Position.z,
                leftHandPose.Orientation.x,
                leftHandPose.Orientation.y,
                leftHandPose.Orientation.z,
                leftHandPose.Orientation.w,
                leftHandPose.Position.x,
                leftHandPose.Position.y,
                leftHandPose.Position.z,
                leftHandStatusFlags,
                rightHandPose.Orientation.x,
                rightHandPose.Orientation.y,
                rightHandPose.Orientation.z,
                rightHandPose.Orientation.w,
                rightHandPose.Position.x,
                rightHandPose.Position.y,
                rightHandPose.Position.z,
                rightHandStatusFlags,
                counter);

        return cfps;
    }
}
