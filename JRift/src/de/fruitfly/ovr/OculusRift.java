package de.fruitfly.ovr;

import de.fruitfly.ovr.enums.*;
import de.fruitfly.ovr.structs.*;

import java.text.DecimalFormat;

public class OculusRift //implements IVR
{
	private boolean initialized = false;

	private HmdParameters hmdParameters = new HmdParameters();
    private Posef lastEyePose[] = new Posef[3];
    private Posef lastHandPose[] = new Posef[2];
    private FullPoseState trackerPoseInfo = new FullPoseState();
    private ErrorInfo _lastErrorInfo = new ErrorInfo();

    public static String NOT_INITIALISED = "Not initialised";
    public String _initSummary = NOT_INITIALISED;

	private static boolean libraryLoaded = false;
	
	public OculusRift()
    {
        resetHMDInfo();
	}

    private void resetHMDInfo()
    {
        hmdParameters = new HmdParameters();
    }

    private void resetTrackerInfo()
    {
        lastEyePose[0] = new Posef();
        lastEyePose[1] = new Posef();
        lastEyePose[2] = new Posef();
        trackerPoseInfo = new FullPoseState();
    }

	public String getInitializationStatus()
	{
		return _initSummary;
	}

    public String getLastError()
    {
        return _getLastError().errorStr;
    }

    public static String getVersionString()
    {
        if (!libraryLoaded)
            return "Not loaded";

        return _getVersionString();
    }

	public boolean init()
    {
        OculusRift.LoadLibrary();

        if( !libraryLoaded )
        {
            _initSummary = "Load library failed";
            return false;
        }

        initialized = _initSubsystem();
        if( !initialized )
        {
            ErrorInfo error = _getLastError();
            _initSummary = error.errorStr;
            resetHMDInfo();
            return false;
        }

        hmdParameters = _getHmdParameters();
        _initSummary = "OK";
		
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

        _initSummary = NOT_INITIALISED;
        initialized = false;
    }

    public HmdParameters getHmdParameters()
    {
        return hmdParameters;
    }

    public void resetTracking()
    {
        _resetTracking();
        resetTrackerInfo();
    }

    public RenderTextureInfo getRenderTextureSizes(FovPort leftFov,
                                                   FovPort rightFov,
                                                   float renderScaleFactor)
    {
        return _getRenderTextureSize(leftFov.UpTan,
                leftFov.DownTan,
                leftFov.LeftTan,
                leftFov.RightTan,
                rightFov.UpTan,
                rightFov.DownTan,
                rightFov.LeftTan,
                rightFov.RightTan,
                renderScaleFactor);
    }

    public FullPoseState getTrackedPoses(long frameIndex)
    {
        if (!initialized)
            return new FullPoseState();

        trackerPoseInfo = _getTrackedPoses(frameIndex);


        if (trackerPoseInfo == null)
            trackerPoseInfo = new FullPoseState();

        // Account for the need for negated y position values
        trackerPoseInfo.leftEyePose.Position.y *= -1f;
        trackerPoseInfo.rightEyePose.Position.y *= -1f;
        trackerPoseInfo.centerEyePose.ThePose.Position.y *= -1f;

        lastEyePose[EyeType.ovrEye_Left.value()] = trackerPoseInfo.getEyePose(EyeType.ovrEye_Left);
        lastEyePose[EyeType.ovrEye_Right.value()] = trackerPoseInfo.getEyePose(EyeType.ovrEye_Right);
        lastEyePose[EyeType.ovrEye_Center.value()] = trackerPoseInfo.getEyePose(EyeType.ovrEye_Center);

        return trackerPoseInfo.clone();
    }

    public Vector3f getEyePos(EyeType eye)
    {
        if (!isInitialized())
            return new Vector3f();

        Posef pose = lastEyePose[eye.value()];
        return new Vector3f(pose.Position.x, pose.Position.y, pose.Position.z);
    }

    public Matrix4f getProjectionMatrix(FovPort fov,
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

    public RenderTextureSet createRenderTextureSet(int lwidth, int lheight, int rwidth, int rheight)
    {
        if (!isInitialized())
            return null;

        RenderTextureSet renderTextureSet = _createRenderTextureSet(lwidth, lheight, rwidth, rheight);
        if (renderTextureSet == null) {
            _lastErrorInfo = _getLastError();
        }
        return renderTextureSet;
    }
    
    public boolean setCurrentRenderTextureInfo(int index, int textureIdx, int depthId, int depthWidth, int depthHeight)
    {
        if (!isInitialized())
            return false;

    	return _setCurrentRenderTextureInfo(index, textureIdx, depthId, depthWidth, depthHeight);
    }

    public void deleteRenderTextures()
    {
        if (!isInitialized())
            return;

        _destroyRenderTextureSet();
    }

    public int createMirrorTexture(int width, int height)
    {
        if (!isInitialized())
            return -1;

        int ret = _createMirrorTexture(width, height);
        if (ret == -1) {
            _lastErrorInfo = _getLastError();
        }
        return ret;
    }

    public void deleteMirrorTexture()
    {
        if (!isInitialized())
            return;

        _destroyMirrorTexture();
    }

    public ErrorInfo submitFrame()
    {
        if (!isInitialized())
            return null;

        ErrorInfo ei = _submitFrame(1f); // TODO: How best to set render params, layer stuff?
        if (ei != null && !ei.unqualifiedSuccess)
        {
            System.out.println("submitFrame: " + ei.errorStr);
        }
        return ei;
    }

    // Native declarations

	protected native boolean         _initSubsystem();
    protected native void            _destroySubsystem();

    protected native ErrorInfo       _getLastError();

    protected native HmdParameters   _getHmdParameters();

    protected native void            _resetTracking();
    protected native RenderTextureSet _createRenderTextureSet(int lwidth,
                                                              int lheight,
                                                              int rwidth,
                                                              int rheight);
    protected native boolean         _setCurrentRenderTextureInfo(int index,
                                                                  int textureIdx,
                                                                  int depthId,
                                                                  int depthWidth,
                                                                  int depthHeight);
    protected native void            _destroyRenderTextureSet();
    protected native int             _createMirrorTexture(int width,
                                                          int height);
    protected native void            _destroyMirrorTexture();
    protected native RenderTextureInfo _getRenderTextureSize(float LeftFovUpTan,
                                                             float LeftFovDownTan,
                                                             float LeftFovLeftTan,
                                                             float LeftFovRightTan,
                                                             float RightFovUpTan,
                                                             float RightFovDownTan,
                                                             float RightFovLeftTan,
                                                             float RightFovRightTan,
                                                             float RenderScaleFactor);

    protected native FullPoseState   _getTrackedPoses(long frameIndex);
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

    protected native ErrorInfo       _submitFrame(float worldScale);
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
        HmdParameters hmdDesc = or.getHmdParameters();
        System.out.println(hmdDesc.toString());

        // Determine render target size based on recommended sizes calculated by the Oculus SDK
        RenderTextureInfo renderTextureSize = or.getRenderTextureSizes(hmdDesc.DefaultEyeFov[0], hmdDesc.DefaultEyeFov[1], 1.0f);
        System.out.println("Render target size: " + renderTextureSize.CombinedTextureResolution.w + "x" + renderTextureSize.CombinedTextureResolution.h);

        // Setup render parameters
        GLConfig glConfig = new GLConfig();

        // TODO: Update!

        while (or.isInitialized())
        {
            ///frameIndex++;

            // Get tracker and eye pose information before beginFrame - if rendering configured
            FullPoseState trackedPoses = or.getTrackedPoses(frameIndex);
            Posef leyePose = trackedPoses.leftEyePose;
            Posef reyePose = trackedPoses.rightEyePose;

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

            Vector3f Lpos = trackedPoses.leftEyePose.Position;
            Vector3f Rpos = trackedPoses.rightEyePose.Position;

            // In game render loop, would call (needed to have called configureRendering first):
            //or._beginFrame(frameIndex);

            //EyeType firstEyeToRender = hmdParameters.EyeRenderOrder[0];

            // Pseudo code
            //<"RenderEye(eyePoses.getEyePose(firstEyeToRender))">     (should be <5ms for DK2)

            //EyeType secondEyeToRender = hmdParameters.EyeRenderOrder[1];

            // Pseudo code
            //<"RenderEye(eyePoses.getEyePose(secondEyeToRender))">     (should be <5ms for DK2)

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
