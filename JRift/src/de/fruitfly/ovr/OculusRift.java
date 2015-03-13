package de.fruitfly.ovr;

import de.fruitfly.ovr.enums.Axis;
import de.fruitfly.ovr.enums.EyeType;
import de.fruitfly.ovr.enums.HandedSystem;
import de.fruitfly.ovr.enums.RotateDirection;
import de.fruitfly.ovr.structs.*;

import java.io.File;

public class OculusRift //implements IOculusRift
{
	private boolean initialized = false;
    private boolean renderConfigured = false;

	private HmdDesc hmdDesc = new HmdDesc();
    private TrackerState trackerState = new TrackerState();
    private Posef lastPose[] = new Posef[2];

    public String _initSummary = "Not initialised";

	private static boolean libraryLoaded = false;
	
    protected EyeRenderParams erp = new EyeRenderParams();
	
	public OculusRift()
    {
        lastPose[0] = new Posef();
        lastPose[1] = new Posef();
        resetHMDInfo();
	}

    private void resetHMDInfo()
    {
        hmdDesc = new HmdDesc();
        trackerState = new TrackerState();
    }

	public String getInitializationStatus()
	{
		return _initSummary;
	}

    public static String getVersionString()
    {
        if (!libraryLoaded)
            return "Not loaded";

        return _getVersionString();
    }

	public boolean init( File nativeDir )
	{
		OculusRift.LoadLibrary();
		return init();
	}

	public static void initRenderingShim()
    {
        if (libraryLoaded)
            _initRenderingShim();
    }

	public boolean init()
	{
        _initSummary = "Load library failed";

        LoadLibrary();

        if( !libraryLoaded )
            return false;

        _initSummary = "Last initialisation attempt failed";

        if (!initialized)
		    initialized = _initSubsystem();

        if (initialized)
        {
            hmdDesc = _getHmdDesc();
            _initSummary = "OK";
        }
        else
        {
            resetHMDInfo();
        }
		
		return initialized;
	}
	
	public boolean isInitialized()
    {
		return initialized;
	}

    public void destroy()
    {
        if (initialized)
        {
            _destroySubsystem();
        }

        _initSummary = "Not initialised";
        initialized = false;
        renderConfigured = false;
    }

    public boolean getNextHmd()
    {
        return _getNextHmd();
    }

    public HmdDesc getHmdDesc()
    {
        return hmdDesc;
    }

    public TrackerState poll(double futureDelta)
    {
        if (initialized)
        {
            // Get tracking state as of now
            trackerState = _getTrackerState(futureDelta);
        }

        return trackerState;
    }

    public TrackerState getLastTrackerState()
    {
        return trackerState;
    }

    public void resetTracking()
    {
        _resetTracking();
    }

    public FovTextureInfo getFovTextureSize(FovPort leftFov,
                                            FovPort rightFov,
                                            float renderScaleFactor)
    {
        return _getFovTextureSize(leftFov.UpTan,
                                  leftFov.DownTan,
                                  leftFov.LeftTan,
                                  leftFov.RightTan,
                                  rightFov.UpTan,
                                  rightFov.DownTan,
                                  rightFov.LeftTan,
                                  rightFov.RightTan,
                                  renderScaleFactor);
    }

    public EyeRenderParams configureRendering(Sizei InTexture1Size,
                                              Sizei OutDisplaySize,
                                              GLConfig glConfig,
                                              FovPort LeftFov,
                                              FovPort RightFov)
    {
        if (!initialized)
            return null;

        erp = _configureRendering(true,
                                   InTexture1Size.w,
                                   InTexture1Size.h,
                                   glConfig.TexId,
                                   0,
                                   0,
                                   0,
                                   OutDisplaySize.w,
                                   OutDisplaySize.h,
                                   glConfig.Window,
                                   glConfig.Display,
                                   glConfig.VSyncEnabled,
                                   glConfig.MultiSampleCount,
                                   glConfig.useChromaticAbCorrection,
                                   glConfig.useTimewarp,
                                   glConfig.useVignette,
                                   glConfig.useLowPersistence,
                                   glConfig.mirrorDisplay,
                                   glConfig.useDisplayOverdrive,
                                   glConfig.useDynamicPrediction,
                                   glConfig.useHighQualityDistortion,
                                   LeftFov.UpTan,
                                   LeftFov.DownTan,
                                   LeftFov.LeftTan,
                                   LeftFov.RightTan,
                                   RightFov.UpTan,
                                   RightFov.DownTan,
                                   RightFov.LeftTan,
                                   RightFov.RightTan);

        if (erp != null)
            renderConfigured = true;

        return erp;
    }

    public EyeRenderParams configureRenderingDualTexture(Sizei InTexture1Size,
                                                         Sizei InTexture2Size,
                                                         Sizei OutDisplaySize,
                                                         GLConfig glConfig,
                                                         FovPort LeftFov,
                                                         FovPort RightFov)
    {
        if (!initialized)
            return null;

        erp = _configureRendering(false,
                                   InTexture1Size.w,
                                   InTexture1Size.h,
                                   glConfig.TexId,
                                   InTexture2Size.w,
                                   InTexture2Size.h,
                                   glConfig.TexId2,
                                   OutDisplaySize.w,
                                   OutDisplaySize.h,
                                   glConfig.Window,
                                   glConfig.Display,
                                   glConfig.VSyncEnabled,
                                   glConfig.MultiSampleCount,
                                   glConfig.useChromaticAbCorrection,
                                   glConfig.useTimewarp,
                                   glConfig.useVignette,
                                   glConfig.useLowPersistence,
                                   glConfig.mirrorDisplay,
                                   glConfig.useDisplayOverdrive,
                                   glConfig.useDynamicPrediction,
                                   glConfig.useHighQualityDistortion,
                                   LeftFov.UpTan,
                                   LeftFov.DownTan,
                                   LeftFov.LeftTan,
                                   LeftFov.RightTan,
                                   RightFov.UpTan,
                                   RightFov.DownTan,
                                   RightFov.LeftTan,
                                   RightFov.RightTan);

        if (erp != null)
            renderConfigured = true;

        return erp;
    }

    public void resetRenderConfig()
    {
        if (!initialized)
            return;

        _resetRenderConfig();
        renderConfigured = false;
    }

    public boolean isRenderConfigured()
    {
        return renderConfigured;
    }

    public FrameTiming beginFrameGetTiming()
    {
        if (!initialized || !renderConfigured)
            return new FrameTiming();

        return _beginFrame(0);
    }

    public FrameTiming beginFrameGetTiming(int frameIndex)
    {
        if (!initialized || !renderConfigured)
            return new FrameTiming();

        return _beginFrame(frameIndex);
    }

    public Posef getEyePose(EyeType eye)
    {
        if (!initialized || !renderConfigured)
            return new Posef();

        lastPose[eye.value()] = _getEyePose(eye.value());
        return lastPose[eye.value()];
    }

    public FullPoseState getEyePoses(int frameIndex)
    {
        if (!initialized || !renderConfigured)
            return new FullPoseState();

        return _getEyePoses(frameIndex,
                erp.Eyes[0].ViewAdjust.x,
                erp.Eyes[0].ViewAdjust.y,
                erp.Eyes[0].ViewAdjust.z,
                erp.Eyes[1].ViewAdjust.x,
                erp.Eyes[1].ViewAdjust.y,
                erp.Eyes[1].ViewAdjust.z);
    }

    public Vector3f getEyePos(EyeType eye)
    {
        if (!isInitialized())
            return new Vector3f();

        Posef pose = lastPose[eye.value()];
        return new Vector3f(pose.Position.x, pose.Position.y, pose.Position.z);
    }

    public Matrix4f getMatrix4fProjection(FovPort fov,
                                          float nearClip,
                                          float farClip)
    {
        if (!initialized || !renderConfigured)
            return null;

        return _getMatrix4fProjection(fov.UpTan,
                                      fov.DownTan,
                                      fov.LeftTan,
                                      fov.RightTan,
                                      nearClip,
                                      farClip);
    }

    public void endFrame()
    {
        if (!initialized || !renderConfigured)
            return;

        _endFrame();
    }

    public static EulerOrient getEulerAnglesDeg(Quatf quat,
                                                float scale,
                                                Axis rotationAxis1,
                                                Axis rotationAxis2,
                                                Axis rotationAxis3,
                                                HandedSystem hand,
                                                RotateDirection rotationDir)
    {
        if( !libraryLoaded )
            return null;

        EulerOrient eulerAngles = _convertQuatToEuler(quat.x, quat.y, quat.z, quat.w, scale,
                rotationAxis1.value(),
                rotationAxis2.value(),
                rotationAxis3.value(),
                hand.value(),
                rotationDir.value());

        eulerAngles.yaw = (float)Math.toDegrees(eulerAngles.yaw);
        eulerAngles.pitch = (float)Math.toDegrees(eulerAngles.pitch);
        eulerAngles.roll = (float)Math.toDegrees(eulerAngles.roll);

        return eulerAngles;
    }

    public static Matrix4f getViewFromEyePose(float yawOffsetRads,
                                              float pitchOffsetRads,
                                              Vector3f headPos,
                                              Quatf orientation,
                                              Vector3f position,
                                              Vector3f viewAdjust)
    {
        if( !libraryLoaded )
            return null;

        return _getViewFromEyePose(yawOffsetRads,
                pitchOffsetRads,
                headPos.x,
                headPos.y,
                headPos.z,
                orientation.x,
                orientation.y,
                orientation.z,
                orientation.w,
                position.x,
                position.y,
                position.z,
                viewAdjust.x,
                viewAdjust.y,
                viewAdjust.z);
    }

    public UserProfileData getUserProfile()
    {
        if (!isInitialized())
            return null;

        return _getUserProfileData();
    }

    public void dismissHSW()
    {
        if (!isInitialized())
            return;

        _dismissHSW();
    }

    public static double getCurrentTimeSeconds()
    {
        double time = 0f;

        if (libraryLoaded)
            time = _getCurrentTimeSecs();

        return time;
    }

    // Native declarations

    protected native static void     _initRenderingShim();
	protected native boolean         _initSubsystem();
    protected native void            _destroySubsystem();

    protected native boolean         _getNextHmd();
    protected native HmdDesc         _getHmdDesc();

    protected native TrackerState    _getTrackerState(double timeFromNow);
    protected native void            _resetTracking();

    protected native FovTextureInfo  _getFovTextureSize(float LeftFovUpTan,
                                                        float LeftFovDownTan,
                                                        float LeftFovLeftTan,
                                                        float LeftFovRightTan,
                                                        float RightFovUpTan,
                                                        float RightFovDownTan,
                                                        float RightFovLeftTan,
                                                        float RightFovRightTan,
                                                        float RenderScaleFactor);
    protected native EyeRenderParams _configureRendering(boolean UsesInputTexture1Only,
                                                         int InTexture1Width,
                                                         int InTexture1Height,
                                                         int InTexture1GLId,
                                                         int InTexture2Width,
                                                         int InTexture2Height,
                                                         int InTexture2GLId,
                                                         int OutDisplayWidth,
                                                         int OutDisplayHeight,
                                                         long pWindow,
                                                         long pDisplay,
                                                         boolean VSyncEnabled,
                                                         int MultiSample,
                                                         boolean useChromaticAbCorrection,
                                                         boolean useTimewarp,
                                                         boolean useVignette,
                                                         boolean useLowPersistence,
                                                         boolean mirrorDisplay,
                                                         boolean useDisplayOverdrive,
                                                         boolean useDynamicPrediction,
                                                         boolean useHighQualityDistortion,
                                                         float LeftFovUpTan,
                                                         float LeftFovDownTan,
                                                         float LeftFovLeftTan,
                                                         float LeftFovRightTan,
                                                         float RightFovUpTan,
                                                         float RightFovDownTan,
                                                         float RightFovLeftTan,
                                                         float RightFovRightTan);
    protected native void            _resetRenderConfig();

    protected native FrameTiming     _beginFrame(int frameIndex);
    protected native Posef           _getEyePose(int eye);
    protected native FullPoseState   _getEyePoses(int frameIndex,
                                                  float leftEyeViewAdjustX,
                                                  float leftEyeViewAdjustY,
                                                  float leftEyeViewAdjustZ,
                                                  float rightEyeViewAdjustX,
                                                  float rightEyeViewAdjustY,
                                                  float rightEyeViewAdjustZ);
    protected native Matrix4f        _getMatrix4fProjection(float EyeFovPortUpTan,
                                                            float EyeFovPortDownTan,
                                                            float EyeFovPortLeftTan,
                                                            float EyeFovPortRightTan,
                                                            float nearClip,
                                                            float farClip);
    protected native static Matrix4f _getViewFromEyePose(float yawOffsetRads,
                                                         float pitchOffsetRads,
                                                         float headPosX,
                                                         float headPosY,
                                                         float headPosZ,
                                                         float orientX,
                                                         float orientY,
                                                         float orientZ,
                                                         float orientW,
                                                         float PosX,
                                                         float PosY,
                                                         float PosZ,
                                                         float viewAdjustX,
                                                         float viewAdjustY,
                                                         float viewAdjustZ);
    protected native void            _endFrame();

    protected native static EulerOrient _convertQuatToEuler(float quatx,
                                                            float quaty,
                                                            float quatz,
                                                            float quatw,
                                                            float scale,
                                                            int rot1,
                                                            int rot2,
                                                            int rot3,
                                                            int hand,
                                                            int rotationDir);

    protected native UserProfileData _getUserProfileData();
    protected native void            _dismissHSW();
    protected native static String   _getVersionString();
    protected native static double   _getCurrentTimeSecs();

    public static void LoadLibrary()
    {
        if( libraryLoaded ) return;
        String os = System.getProperty("os.name");
        boolean is64bit = System.getProperty("sun.arch.data.model").equalsIgnoreCase("64");

        //Launcher takes care of extracting natives
        if( is64bit )
        {
            System.loadLibrary("JRiftLibrary64");
            System.out.println("Loaded JRift native library (64bit)");
        }
        else
        {
            System.loadLibrary("JRiftLibrary");
            System.out.println("Loaded JRift native library (32bit)");
        }

        libraryLoaded = true;
    }

    public static void main(String[] args)
    {
        OculusRift.LoadLibrary();
        OculusRift or = new OculusRift();

        if (!or.init())
        {
            System.out.println("Failed to initialise OR lib");
            return;
        }

        HmdDesc hmdDesc = or.getHmdDesc();
        System.out.println(hmdDesc.toString());

        FovTextureInfo recommendedFovTextureSize = or.getFovTextureSize(hmdDesc.DefaultEyeFov[0], hmdDesc.DefaultEyeFov[1], 1.0f);
        System.out.println("Render target size: " + recommendedFovTextureSize.CombinedTextureResolution.w + "x" + recommendedFovTextureSize.CombinedTextureResolution.h);

        while (or.isInitialized())
        {
            or.poll(0.0d);
            TrackerState state = or.getLastTrackerState();
            EulerOrient euler = or.getEulerAnglesDeg(state.HeadPose.ThePose.Orientation,
                                               1.0f,
                                               Axis.Axis_Y,
                                               Axis.Axis_X,
                                               Axis.Axis_Z,
                                               HandedSystem.Handed_L,
                                               RotateDirection.Rotate_CCW);

            Vector3f pos = state.HeadPose.ThePose.Position;

            System.out.println("Yaw: " + euler.yaw + " Pitch: " + euler.pitch + " Roll: " + euler.roll + " PosX: " + pos.x+ " PosY: " + pos.y + " PosZ: " + pos.z);

            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        or.destroy();
    }
}
