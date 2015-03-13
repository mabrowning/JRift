package de.fruitfly.ovr;

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

    public FullPoseState(int frameIndex, Posef leftEyePose, Posef rightEyePose, TrackerState trackerState)
    {
        this.frameIndex = frameIndex;
        this.leftEyePose = leftEyePose;
        this.rightEyePose = rightEyePose;
        this.trackerState = trackerState;
    }

    public Posef getPose(EyeType eye)
    {
        if (eye == EyeType.ovrEye_Right)
            return rightEyePose;
        else
            return leftEyePose;
    }
}
