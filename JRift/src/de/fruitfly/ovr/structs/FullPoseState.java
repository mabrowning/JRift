package de.fruitfly.ovr.structs;

import de.fruitfly.ovr.enums.EyeType;
import de.fruitfly.ovr.structs.Posef;
import de.fruitfly.ovr.structs.TrackerState;

public class FullPoseState
{
    public int frameIndex = 0;
    public Posef leftEyePose = new Posef();
    public Posef rightEyePose = new Posef();
    public TrackerState trackerState = new TrackerState();

    public FullPoseState() {}

    public FullPoseState(int frameIndex,
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
                         float temperature,
                         int statusFlags)
    {
        this.frameIndex = frameIndex;

        leftEyePose.Orientation.x = Lquatx;
        leftEyePose.Orientation.y = Lquaty;
        leftEyePose.Orientation.z = Lquatz;
        leftEyePose.Orientation.w = Lquatw;
        leftEyePose.Position.x = Lposx;
        leftEyePose.Position.y = Lposy;
        leftEyePose.Position.z = Lposz;

        rightEyePose.Orientation.x = Rquatx;
        rightEyePose.Orientation.y = Rquaty;
        rightEyePose.Orientation.z = Rquatz;
        rightEyePose.Orientation.w = Rquatw;
        rightEyePose.Position.x = Rposx;
        rightEyePose.Position.y = Rposy;
        rightEyePose.Position.z = Rposz;

        trackerState.HeadPose.ThePose.Orientation.x    = PredictedPoseStatefPosefOrientationx;
        trackerState.HeadPose.ThePose.Orientation.y    = PredictedPoseStatefPosefOrientationy;
        trackerState.HeadPose.ThePose.Orientation.z    = PredictedPoseStatefPosefOrientationz;
        trackerState.HeadPose.ThePose.Orientation.w    = PredictedPoseStatefPosefOrientationw;
        trackerState.HeadPose.ThePose.Position.x       = PredictedPoseStatefPosefPositionx;
        trackerState.HeadPose.ThePose.Position.y       = PredictedPoseStatefPosefPositiony;
        trackerState.HeadPose.ThePose.Position.z       = PredictedPoseStatefPosefPositionz;
        trackerState.HeadPose.AngularVelocity.x     = PredictedVector3fAngularVelocityx;
        trackerState.HeadPose.AngularVelocity.y     = PredictedVector3fAngularVelocityy;
        trackerState.HeadPose.AngularVelocity.z     = PredictedVector3fAngularVelocityz;
        trackerState.HeadPose.LinearVelocity.x      = PredictedVector3fLinearVelocityx;
        trackerState.HeadPose.LinearVelocity.y      = PredictedVector3fLinearVelocityy;
        trackerState.HeadPose.LinearVelocity.z      = PredictedVector3fLinearVelocityz;
        trackerState.HeadPose.AngularAcceleration.x = PredictedVector3fAngularAccelerationx;
        trackerState.HeadPose.AngularAcceleration.y = PredictedVector3fAngularAccelerationy;
        trackerState.HeadPose.AngularAcceleration.z = PredictedVector3fAngularAccelerationz;
        trackerState.HeadPose.LinearAcceleration.x  = PredictedVector3fLinearAccelerationx;
        trackerState.HeadPose.LinearAcceleration.y  = PredictedVector3fLinearAccelerationy;
        trackerState.HeadPose.LinearAcceleration.z  = PredictedVector3fLinearAccelerationz;
        trackerState.HeadPose.TimeInSeconds         = PredictedTimeInSeconds;
        trackerState.Temperature = temperature;
        trackerState.StatusFlags = statusFlags;
    }

    public Posef getPose(EyeType eye)
    {
        if (eye == EyeType.ovrEye_Right)
            return rightEyePose;
        else
            return leftEyePose;
    }
}
