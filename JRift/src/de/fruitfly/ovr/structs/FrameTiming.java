package de.fruitfly.ovr.structs;

public class FrameTiming
{
    public FrameTiming() {}

    public FrameTiming(
            float deltaSeconds,
            double thisFrameSeconds,
            double timewarpPointSeconds,
            double nextFrameSeconds,
            double scanoutMidpointSeconds,
            double eye1ScanoutSeconds,
            double eye2ScanoutSeconds
        )
    {
        DeltaSeconds = deltaSeconds;
        ThisFrameSeconds = thisFrameSeconds;
        TimewarpPointSeconds = timewarpPointSeconds;
        NextFrameSeconds = nextFrameSeconds;
        ScanoutMidpointSeconds = scanoutMidpointSeconds;
        EyeScanoutSeconds[0] = eye1ScanoutSeconds;
        EyeScanoutSeconds[1] = eye2ScanoutSeconds;
    }

    // The amount of time that has passed since the previous frame returned
    // BeginFrameSeconds value, usable for movement scaling.
    // This will be clamped to no more than 0.1 seconds to prevent
    // excessive movement after pauses for loading or initialization.
    public float			DeltaSeconds;

    // It is generally expected that the following hold:
    // ThisFrameSeconds < TimewarpPointSeconds < NextFrameSeconds <
    // EyeScanoutSeconds[EyeOrder[0]] <= ScanoutMidpointSeconds <= EyeScanoutSeconds[EyeOrder[1]]

    // Absolute time value of when rendering of this frame began or is expected to
    // begin; generally equal to NextFrameSeconds of the previous frame. Can be used
    // for animation timing.
    public double			ThisFrameSeconds;
    // Absolute point when IMU expects to be sampled for this frame.
    public double			TimewarpPointSeconds;
    // Absolute time when frame Present + GPU Flush will finish, and the next frame starts.
    public double			NextFrameSeconds;

    // Time when when half of the screen will be scanned out. Can be passes as a prediction
    // value to ovrHmd_GetSensorState() go get general orientation.
    public double		    ScanoutMidpointSeconds;
    // Timing points when each eye will be scanned out to display. Used for rendering each eye.
    public double			EyeScanoutSeconds[] = new double[2];
}
