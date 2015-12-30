package de.fruitfly.ovr;

import de.fruitfly.ovr.enums.*;
import de.fruitfly.ovr.structs.*;

import java.io.File;
import java.text.DecimalFormat;

public class OculusRift //implements IOculusRift
{
	private boolean initialized = false;

	private HmdDesc hmdDesc = new HmdDesc();
    private TrackerState trackerState = new TrackerState();
    private Posef lastPose[] = new Posef[3];
    private FullPoseState fps = new FullPoseState();

    public String _initSummary = "Not initialised";

	private static boolean libraryLoaded = false;
	
    protected EyeRenderParams erp = new EyeRenderParams();
	
	public OculusRift()
    {
        resetHMDInfo();
	}

    private void resetHMDInfo()
    {
        hmdDesc = new HmdDesc();
    }

    private void resetTrackerInfo()
    {
        lastPose[0] = new Posef();
        lastPose[1] = new Posef();
        lastPose[2] = new Posef();
        fps = new FullPoseState();
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

	public boolean init()
	{
        _initSummary = "Load library failed";

        OculusRift.LoadLibrary();

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
    }

    public HmdDesc getHmdDesc()
    {
        return hmdDesc;
    }

    public void resetTracking()
    {
        _resetTracking();
        resetTrackerInfo();
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

    public FullPoseState getEyePoses(long frameIndex)
    {
        if (!initialized)
            return new FullPoseState();

        fps = _getEyePoses(frameIndex);

        if (fps == null)
            fps = new FullPoseState();

        // Account for the need for negated y position values
        fps.leftEyePose.Position.y *= -1f;
        fps.rightEyePose.Position.y *= -1f;
        fps.trackerState.HeadPose.ThePose.Position.y *= -1f;

        lastPose[EyeType.ovrEye_Left.value()] = fps.getPose(EyeType.ovrEye_Left);
        lastPose[EyeType.ovrEye_Right.value()] = fps.getPose(EyeType.ovrEye_Right);
        lastPose[EyeType.ovrEye_Center.value()] = fps.getPose(EyeType.ovrEye_Center);

        return fps.clone();
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
        if (!initialized)
            return null;

        return _getMatrix4fProjection(fov.UpTan,
                                      fov.DownTan,
                                      fov.LeftTan,
                                      fov.RightTan,
                                      nearClip,
                                      farClip);
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

    public UserProfileData getUserProfile()
    {
        if (!isInitialized())
            return null;

        return _getUserProfileData();
    }

    public static double getCurrentTimeSeconds()
    {
        double time = 0f;

        if (libraryLoaded)
            time = _getCurrentTimeSecs();

        return time;
    }

    public SwapTextureSet createSwapTextureSet(int lwidth, int lheight, int rwidth, int rheight)
    {
        if (!isInitialized())
            return null;

        return _createSwapTextureSet(lwidth, lheight, rwidth, rheight);
    }
    
    public boolean setCurrentSwapTextureIndex(int idx)
    {
    	return _setCurrentSwapTextureIndex(idx);
    }

    public int createMirrorTexture(int width, int height)
    {
        if (!isInitialized())
            return -1;

        return _createMirrorTexture(width, height);
    }

    public void submitFrame()
    {
        if (!isInitialized())
            return;

        _submitFrame();
    }

    // Native declarations

	protected native boolean         _initSubsystem();
    protected native void            _destroySubsystem();

    protected native ErrorInfo       _getLastError();

    protected native boolean         _getNextHmd();
    protected native HmdDesc         _getHmdDesc();

    protected native void            _resetTracking();
    protected native SwapTextureSet  _createSwapTextureSet(int lwidth,
                                                           int lheight,
                                                           int rwidth,
                                                           int rheight);
    protected native boolean         _setCurrentSwapTextureIndex(int idx);
    protected native int             _createMirrorTexture(int width,
                                                          int height);
    protected native FovTextureInfo  _getFovTextureSize(float LeftFovUpTan,
                                                        float LeftFovDownTan,
                                                        float LeftFovLeftTan,
                                                        float LeftFovRightTan,
                                                        float RightFovUpTan,
                                                        float RightFovDownTan,
                                                        float RightFovLeftTan,
                                                        float RightFovRightTan,
                                                        float RenderScaleFactor);

    protected native FullPoseState   _getEyePoses(long frameIndex);
    protected native Matrix4f        _getMatrix4fProjection(float EyeFovPortUpTan,
                                                            float EyeFovPortDownTan,
                                                            float EyeFovPortLeftTan,
                                                            float EyeFovPortRightTan,
                                                            float nearClip,
                                                            float farClip);

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

    protected native ErrorInfo       _submitFrame();
    protected native UserProfileData _getUserProfileData();
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
        long frameIndex = 0;

        // Will need to add the natives dir to your Java VM args: -Djava.library.path="<path to natives dir>"

        // Load the JRift library
        OculusRift.LoadLibrary();
        OculusRift or = new OculusRift();

        // Initialise the Rift
        if (!or.init())
        {
            System.out.println("Failed to initialise OR lib");
            return;
        }

        // Get the HMD information
        HmdDesc hmdDesc = or.getHmdDesc();
        System.out.println(hmdDesc.toString());

        // Determine render target size based on recommended sizes calculated by the Oculus SDK
        FovTextureInfo recommendedFovTextureSize = or.getFovTextureSize(hmdDesc.DefaultEyeFov[0], hmdDesc.DefaultEyeFov[1], 1.0f);
        System.out.println("Render target size: " + recommendedFovTextureSize.CombinedTextureResolution.w + "x" + recommendedFovTextureSize.CombinedTextureResolution.h);

        // Setup render parameters
        GLConfig glConfig = new GLConfig();

        // TODO: Update!

        while (or.isInitialized())
        {
            ///frameIndex++;

            // Get tracker and eye pose information before beginFrame - if rendering configured
            FullPoseState eyePoses = or.getEyePoses(frameIndex);
            Posef leyePose = eyePoses.leftEyePose;
            Posef reyePose = eyePoses.rightEyePose;

            // If you need a Quatf to Euler conversion...
            EulerOrient Leuler = or.getEulerAnglesDeg(leyePose.Orientation.inverted(),
                                               1.0f,
                                               Axis.Axis_Y,
                                               Axis.Axis_X,
                                               Axis.Axis_Z,
                                               HandedSystem.Handed_L,
                                               RotateDirection.Rotate_CCW);
            EulerOrient Reuler = or.getEulerAnglesDeg(reyePose.Orientation.inverted(),
                    1.0f,
                    Axis.Axis_Y,
                    Axis.Axis_X,
                    Axis.Axis_Z,
                    HandedSystem.Handed_L,
                    RotateDirection.Rotate_CCW);

            Vector3f Lpos = eyePoses.leftEyePose.Position;
            Vector3f Rpos = eyePoses.rightEyePose.Position;

            // In game render loop, would call (needed to have called configureRendering first):
            //or._beginFrame(frameIndex);

            //EyeType firstEyeToRender = hmdDesc.EyeRenderOrder[0];

            // Pseudo code
            //<"RenderEye(eyePoses.getPose(firstEyeToRender))">     (should be <5ms for DK2)

            //EyeType secondEyeToRender = hmdDesc.EyeRenderOrder[1];

            // Pseudo code
            //<"RenderEye(eyePoses.getPose(secondEyeToRender))">     (should be <5ms for DK2)

            //or.endFrame();

            try {

                System.out.println("\n");
                dumpPose("L Eye: ", Leuler, Lpos);
                dumpPose("R Eye: ", Reuler, Rpos);

                // Obviously you wouldn't sleep in your normal poll / render loop! You'd poll every frame.
                Thread.sleep(1000);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        or.destroy();
    }

    public static void dumpPose(String prefix, EulerOrient euler, Vector3f pos)
    {
        DecimalFormat fmt = new DecimalFormat("+#,000.0000;-#");
        System.out.println(prefix + "Yaw: " + fmt.format(euler.yaw) + "\u0176 Pitch: " + fmt.format(euler.pitch) + "\u0176 Roll: " + fmt.format(euler.roll) + "\u0176" +
                " PosX: " + fmt.format(pos.x) + "m PosY: " + fmt.format(pos.y) + "m PosZ: " + fmt.format(pos.z) + "m");
    }
}
