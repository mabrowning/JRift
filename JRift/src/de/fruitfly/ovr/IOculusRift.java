package de.fruitfly.ovr;
import java.io.File;

public interface IOculusRift {

    public final float MAXPITCH = (90 * 0.98f);
    public final float MAXROLL = (180 * 0.98f);

    public enum AspectCorrectionType
    {
        CORRECTION_NONE          (0),
        CORRECTION_16_9_TO_16_10 (1),
        CORRECTION_16_10_TO_16_9 (2),
        CORRECTION_AUTO          (3);

        private int value;

        AspectCorrectionType(int value)
        {
            this.value = value;
        }

        public int getValue()
        {
            return value;
        }
    }

	//public String getInitializationStatus();
    public String getVersion();
	public boolean init(File nativeDir);
	public boolean init();
	public boolean isInitialized();
	public void poll();
    //public float getYawDegrees_LH();
    //public float getPitchDegrees_LH();
    //public float getRollDegrees_LH();
    //public float[] getOrientationQuaternion_xyzw();

	//public HMDInfo getHMDInfo();
//    public EyeRenderParams getEyeRenderParams(int viewPortWidth, int viewPortHeight);
//    public EyeRenderParams getEyeRenderParams(int viewPortX, int viewPortY, int viewPortWidth, int viewPortHeight, float nearClip, float farClip);
//    public EyeRenderParams getEyeRenderParams(int viewPortX,
//                                              int viewPortY,
//                                              int viewPortWidth,
//                                              int viewPortHeight,
//                                              float clipNear,
//                                              float clipFar,
//                                              float eyeToScreenDistanceScaleFactor,
//                                              float lensSeparationScaleFactor);
//    public EyeRenderParams getEyeRenderParams(int viewPortX,
//                                              int viewPortY,
//                                              int viewPortWidth,
//                                              int viewPortHeight,
//                                              float clipNear,
//                                              float clipFar,
//                                              float eyeToScreenDistanceScaleFactor,
//                                              float lensSeparationScaleFactor,
//                                              float distortionFitX,
//                                              float distortionFitY,
//                                              AspectCorrectionType aspectCorrectionType);
	public void destroy();
//    public void setIPD(float ipd);
//    public float getIPD();
//    public void setPrediction(float delta, boolean enable);
}
