package de.fruitfly.ovr.structs;

import de.fruitfly.ovr.enums.EyeType;
import de.fruitfly.ovr.enums.HmdType;

public class HmdDesc
{
    public HmdDesc()
    {

    }

    public HmdDesc(
            int hmdType,
            String productName,
            String manufacturer,
            int hmdCaps,
            int trackingCaps,
            int distortionCaps,
            int resolutionW,
            int resolutionH,
            int windowPosX,
            int windowPosY,
            float defaultEyeFov1UpTan,
            float defaultEyeFov1DownTan,
            float defaultEyeFov1LeftTan,
            float defaultEyeFov1RightTan,
            float defaultEyeFov2UpTan,
            float defaultEyeFov2DownTan,
            float defaultEyeFov2LeftTan,
            float defaultEyeFov2RightTan,
            float maxEyeFov1UpTan,
            float maxEyeFov1DownTan,
            float maxEyeFov1LeftTan,
            float maxEyeFov1RightTan,
            float maxEyeFov2UpTan,
            float maxEyeFov2DownTan,
            float maxEyeFov2LeftTan,
            float maxEyeFov2RightTan,
            int eyeType1,
            int eyeType2,
            String displayDeviceName,
            long displayId,
            boolean isReal
        )
    {
        Type = HmdType.fromInteger(hmdType);
        ProductName = productName;
        Manufacturer = manufacturer;
        HmdCaps = hmdCaps;
        DistortionCaps = distortionCaps;
        TrackingCaps = trackingCaps;
        Resolution.w = resolutionW;
        Resolution.h = resolutionH;
        WindowsPos.x = windowPosX;
        WindowsPos.y = windowPosY;
        DefaultEyeFov[0] = new FovPort();
        DefaultEyeFov[0].UpTan = defaultEyeFov1UpTan;
        DefaultEyeFov[0].DownTan = defaultEyeFov1DownTan;
        DefaultEyeFov[0].LeftTan = defaultEyeFov1LeftTan;
        DefaultEyeFov[0].RightTan = defaultEyeFov1RightTan;
        DefaultEyeFov[1] = new FovPort();
        DefaultEyeFov[1].UpTan = defaultEyeFov2UpTan;
        DefaultEyeFov[1].DownTan = defaultEyeFov2DownTan;
        DefaultEyeFov[1].LeftTan = defaultEyeFov2LeftTan;
        DefaultEyeFov[1].RightTan = defaultEyeFov2RightTan;
        MaxEyeFov[0] = new FovPort();
        MaxEyeFov[0].UpTan = maxEyeFov1UpTan;
        MaxEyeFov[0].DownTan = maxEyeFov1DownTan;
        MaxEyeFov[0].LeftTan = maxEyeFov1LeftTan;
        MaxEyeFov[0].RightTan = maxEyeFov1RightTan;
        MaxEyeFov[1] = new FovPort();
        MaxEyeFov[1].UpTan = maxEyeFov2UpTan;
        MaxEyeFov[1].DownTan = maxEyeFov2DownTan;
        MaxEyeFov[1].LeftTan = maxEyeFov2LeftTan;
        MaxEyeFov[1].RightTan = maxEyeFov2RightTan;
        EyeRenderOrder[0] = EyeType.fromInteger(eyeType1);
        EyeRenderOrder[1] = EyeType.fromInteger(eyeType2);
        DisplayDeviceName = displayDeviceName;
        DisplayId = displayId;
        IsReal = isReal;
    }
    
    public HmdType Type = HmdType.ovrHmd_None;

    // Name string describing the product: "Oculus Rift DK1", etc.
    public String ProductName = new String();
    public String Manufacturer = new String();

    // Capability bits described by ovrHmdCapBits.
    public int HmdCaps;
    public int DistortionCaps;
    public int TrackingCaps;

    // Resolution of the entire HMD screen (for both eyes) in pixels.
    public Sizei    Resolution = new Sizei();
    // Where monitor window should be on screen or (0,0).
    public Vector2i WindowsPos = new Vector2i();

    // These define the recommended and maximum optical FOVs for the HMD.
    public FovPort  DefaultEyeFov[] = new FovPort[2];
    public FovPort  MaxEyeFov[] = new FovPort[2];

    // Preferred eye rendering order for best performance.
    // Can help reduce latency on sideways-scanned screens.
    public EyeType EyeRenderOrder[] = new EyeType[2];

    // Display that HMD should present on.
    // TBD: It may be good to remove this information relying on WidowPos instead.
    // Ultimately, we may need to come up with a more convenient alternative,
    // such as a API-specific functions that return adapter, ot something that will
    // work with our monitor driver.

    // Windows: "\\\\.\\DISPLAY3", etc. Can be used in EnumDisplaySettings/CreateDC.
    public String DisplayDeviceName = new String();
    // MacOS
    public long   DisplayId;

    // Is this a real, or debug (fake) device?
    public boolean IsReal = false;

    public String toString()
    {
        StringBuilder sb = new StringBuilder();

        sb.append("Type:              ").append(HmdType.toString(Type)).append("\n");
        sb.append("ProductName:       ").append(ProductName).append("\n");
        sb.append("Manufacturer:      ").append(Manufacturer).append("\n");
        sb.append("Hmd capability bits:\n").append(HmdDesc.HmdCapsToString(HmdCaps));
        sb.append("Distortion capability bits:\n").append(HmdDesc.DistortionCapsToString(DistortionCaps));
        sb.append("Tracker capability bits:\n").append(HmdDesc.TrackingCapsToString(TrackingCaps));
        sb.append("Resolution:        ").append(Resolution.w).append("x").append(Resolution.h).append("\n");
        sb.append("EyeRenderOrder:    ").append(EyeType.toString(EyeRenderOrder[0])).append(", ").append(EyeType.toString(EyeRenderOrder[1])).append("\n");
        sb.append("DisplayDeviceName: ").append(DisplayDeviceName).append("\n");
        sb.append("DisplayId:         ").append(DisplayId).append("\n");
        sb.append("Real?              ").append((IsReal ? "YES" : "NO")).append("\n");

        return sb.toString();
    }

    // HMD capability bits reported by device.
    // Read-only flags.
    public static int ovrHmdCap_Present           = 0x0001;   //  This HMD exists (as opposed to being unplugged).
    public static int ovrHmdCap_Available         = 0x0002;   //  HMD and is sensor is available for use
                                                              //  (if not owned by another app).

    // These flags are intended for use with the new driver display mode.
    public static int ovrHmdCap_ExtendDesktop     = 0x0004;   // Read only, means display driver is in compatibility mode.

    public static int ovrHmdCap_DisplayOff        = 0x0040;   // Turns off Oculus HMD screen and output.
    public static int ovrHmdCap_NoMirrorToWindow  = 0x2000;   // Disables mirrowing of HMD output to the window;
                                                              // may improve rendering performance slightly.

    // Modifiable flags (through ovrHmd_SetEnabledCaps).
    public static int ovrHmdCap_LowPersistence    = 0x0080;   //  Supports low persistence mode.
    public static int ovrHmdCap_LatencyTest       = 0x0100;   //  Supports pixel reading for continuous latency testing.
    public static int ovrHmdCap_DynamicPrediction = 0x0200;   //  Adjust prediction dynamically based on DK2 Latency.

    // Support rendering without VSync for debugging
    public static int ovrHmdCap_NoVSync           = 0x1000;
    public static int ovrHmdCap_NoRestore         = 0x4000;

    // These bits can be modified by ovrHmd_SetEnabledCaps.
    public static int ovrHmdCap_Writable_Mask     = 0x1380;

    // Tracking capability bits reported by device.
    // Used with ovrHmd_ConfigureTracking.
    public static int ovrTrackingCap_Orientation    = 0x0010;   //  Supports orientation tracking (IMU).
    public static int ovrTrackingCap_MagYawCorrection  = 0x0020;   //  Supports yaw correction through magnetometer or other means.
    public static int ovrTrackingCap_Position       = 0x0040;   //  Supports positional tracking.

    // Distortion capability bits reported by device.
    // Used with ovrHmd_ConfigureRendering and ovrHmd_CreateDistortionMesh.
    public static int ovrDistortion_Chromatic = 0x01;    //	Supports chromatic aberration correction.
    public static int ovrDistortion_TimeWarp  = 0x02;    //	Supports timewarp.
    public static int ovrDistortion_Vignette  = 0x08;    //	Supports vignetting around the edges of the view.

    public static String HmdCapsToString(int caps)
    {
        StringBuilder sb = new StringBuilder();

        if ((caps & ovrHmdCap_Present) != 0)
            sb.append(" ovrHmdCap_Present\n");

        if ((caps & ovrHmdCap_Available) != 0)
            sb.append(" ovrHmdCap_Available\n");

        if ((caps & ovrHmdCap_LowPersistence) != 0)
            sb.append(" ovrHmdCap_LowPersistence\n");

        if ((caps & ovrHmdCap_LatencyTest) != 0)
            sb.append(" ovrHmdCap_LatencyTest\n");

        if ((caps & ovrHmdCap_DynamicPrediction) != 0)
            sb.append(" ovrHmdCap_DynamicPrediction\n");

        if ((caps & ovrHmdCap_NoVSync) != 0)
            sb.append(" ovrHmdCap_NoVSync\n");

        if ((caps & ovrHmdCap_ExtendDesktop) != 0)
            sb.append(" ovrHmdCap_ExtendDesktop\n");

        return sb.toString();
    }

    public static String TrackingCapsToString(int caps)
    {
        StringBuilder sb = new StringBuilder();

        if ((caps & ovrTrackingCap_Orientation) != 0)
            sb.append(" ovrTrackingCap_Orientation\n");

        if ((caps & ovrTrackingCap_MagYawCorrection) != 0)
            sb.append(" ovrTrackingCap_MagYawCorrection\n");

        if ((caps & ovrTrackingCap_Position) != 0)
            sb.append(" ovrTrackingCap_Position\n");

        return sb.toString();
    }

    public static String DistortionCapsToString(int caps)
    {
        StringBuilder sb = new StringBuilder();

        if ((caps & ovrDistortion_Chromatic) != 0)
            sb.append(" ovrDistortion_Chromatic\n");

        if ((caps & ovrDistortion_TimeWarp) != 0)
            sb.append(" ovrDistortion_TimeWarp\n");

        if ((caps & ovrDistortion_Vignette) != 0)
            sb.append(" ovrDistortion_Vignette\n");

        return sb.toString();
    }

    public boolean isDirectMode()
    {
        if ((HmdCaps & ovrHmdCap_ExtendDesktop) != 0)
            return false;

        return true;
    }

    public boolean isExtendedMode()
    {
        return !isDirectMode();
    }
}
