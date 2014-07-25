package de.fruitfly.ovr.structs;

public class TrackerState
{
    public TrackerState()
    {
        HeadPose.ThePose.Orientation.w = 1.0f;
    }

    public TrackerState(float PredictedPoseStatefPosefOrientationx,
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
                        int statusFlags
    )
    {
        HeadPose.ThePose.Orientation.x    = PredictedPoseStatefPosefOrientationx;
        HeadPose.ThePose.Orientation.y    = PredictedPoseStatefPosefOrientationy;
        HeadPose.ThePose.Orientation.z    = PredictedPoseStatefPosefOrientationz;
        HeadPose.ThePose.Orientation.w    = PredictedPoseStatefPosefOrientationw;
        HeadPose.ThePose.Position.x       = PredictedPoseStatefPosefPositionx;
        HeadPose.ThePose.Position.y       = PredictedPoseStatefPosefPositiony;
        HeadPose.ThePose.Position.z       = PredictedPoseStatefPosefPositionz;
        HeadPose.AngularVelocity.x     = PredictedVector3fAngularVelocityx;
        HeadPose.AngularVelocity.y     = PredictedVector3fAngularVelocityy;
        HeadPose.AngularVelocity.z     = PredictedVector3fAngularVelocityz;
        HeadPose.LinearVelocity.x      = PredictedVector3fLinearVelocityx;
        HeadPose.LinearVelocity.y      = PredictedVector3fLinearVelocityy;
        HeadPose.LinearVelocity.z      = PredictedVector3fLinearVelocityz;
        HeadPose.AngularAcceleration.x = PredictedVector3fAngularAccelerationx;
        HeadPose.AngularAcceleration.y = PredictedVector3fAngularAccelerationy;
        HeadPose.AngularAcceleration.z = PredictedVector3fAngularAccelerationz;
        HeadPose.LinearAcceleration.x  = PredictedVector3fLinearAccelerationx;
        HeadPose.LinearAcceleration.y  = PredictedVector3fLinearAccelerationy;
        HeadPose.LinearAcceleration.z  = PredictedVector3fLinearAccelerationz;
        HeadPose.TimeInSeconds         = PredictedTimeInSeconds;
        Temperature = temperature;
        StatusFlags = statusFlags;
    }
    
    // Predicted pose configuration at requested absolute time.
    // One can determine the time difference between predicted and actual
    // readings by comparing ovrPoseState.TimeInSeconds.
    public PoseStatef  HeadPose = new PoseStatef();

    // Sensor temperature reading, in degrees Celsius, as sample time.
    public float       Temperature;

    // Sensor status described by ovrStatusBits.
    public int         StatusFlags;
}
