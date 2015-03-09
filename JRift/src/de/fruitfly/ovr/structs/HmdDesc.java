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

    // WARNING: Oculus seem to change these on a regular basis, occasionally breaking backwards compatibility!

    // HMD capability bits reported by device.
    // Read-only flags.
    public static int ovrHmdCap_Present           = 0x0001;   //  This HMD exists (as opposed to being unplugged).
    public static int ovrHmdCap_Available         = 0x0002;   //  HMD and is sensor is available for use
                                                              //  (if not owned by another app).
    public static int ovrHmdCap_Captured          = 0x0004;   /// Set to 'true' if we captured ownership of this HMD.

    // These flags are intended for use with the new driver display mode.
    public static int ovrHmdCap_ExtendDesktop     = 0x0008;   // (read only) Means the display driver is in compatibility mode.

    public static int ovrHmdCap_NoMirrorToWindow  = 0x2000;   // Disables mirroring of HMD output to the window;
                                                              // may improve rendering performance slightly.
    public static int ovrHmdCap_DisplayOff        = 0x0040;   // Turns off Oculus HMD screen and output.

    // Modifiable flags (through ovrHmd_SetEnabledCaps).
    public static int ovrHmdCap_LowPersistence    = 0x0080;   //  Supports low persistence mode.
    public static int ovrHmdCap_DynamicPrediction = 0x0200;   //  Adjust prediction dynamically based on DK2 Latency.
    public static int ovrHmdCap_NoVSync           = 0x1000;   //  Support rendering without VSync for debugging

    // These bits can be modified by ovrHmd_SetEnabledCaps.
    public static int ovrHmdCap_Writable_Mask     = 0x1380;
    public static int ovrHmdCap_Service_Mask      = 0x23F0;

    // Tracking capability bits reported by device.
    // Used with ovrHmd_ConfigureTracking.
    public static int ovrTrackingCap_Orientation       = 0x0010;   //  Supports orientation tracking (IMU).
    public static int ovrTrackingCap_MagYawCorrection  = 0x0020;   //  Supports yaw correction through magnetometer or other means.
    public static int ovrTrackingCap_Position          = 0x0040;   //  Supports positional tracking.
    /// Overrides the other flags. Indicates that the application
    /// doesn't care about tracking settings. This is the internal
    /// default before ovrHmd_ConfigureTracking is called.
    public static int ovrTrackingCap_Idle              = 0x0100;

    // Distortion capability bits reported by device.
    // Used with ovrHmd_ConfigureRendering and ovrHmd_CreateDistortionMesh.
    public static int ovrDistortion_Chromatic                      = 0x01;    // Supports chromatic aberration correction.
    public static int ovrDistortion_TimeWarp                       = 0x02;    // Supports timewarp.
    public static int ovrDistortion_Vignette                       = 0x08;    // Supports vignetting around the edges of the view.
    public static int ovrDistortionCap_NoRestore                   = 0x10;    // Do not save and restore the graphics state when rendering distortion.
    public static int ovrDistortionCap_FlipInput                   = 0x20;    // Flip the vertical texture coordinate of input images.
    public static int ovrDistortionCap_SRGB                        = 0x40;    // Assume input images are in sRGB gamma-corrected color space.
    public static int ovrDistortionCap_Overdrive                   = 0x80;    // Overdrive brightness transitions to reduce artifacts on DK2+ displays
    public static int ovrDistortionCap_HqDistortion                = 0x100;   // High-quality sampling of distortion buffer for anti-aliasing
    public static int ovrDistortionCap_LinuxDevFullscreen          = 0x200;   // Indicates window is fullscreen on a device when set. The SDK will automatically apply distortion mesh rotation if needed.
    public static int ovrDistortionCap_ProfileNoTimewarpSpinWaits  = 0x10000; // Use when profiling with timewarp to remove false positives


    public static String HmdCapsToString(int caps)
    {
        StringBuilder sb = new StringBuilder();

        if ((caps & ovrHmdCap_Present) != 0)
            sb.append(" ovrHmdCap_Present\n");

        if ((caps & ovrHmdCap_Available) != 0)
            sb.append(" ovrHmdCap_Available\n");

        if ((caps & ovrHmdCap_Captured) != 0)
            sb.append(" ovrHmdCap_Captured\n");

        if ((caps & ovrHmdCap_LowPersistence) != 0)
            sb.append(" ovrHmdCap_LowPersistence\n");

        if ((caps & ovrHmdCap_DynamicPrediction) != 0)
            sb.append(" ovrHmdCap_DynamicPrediction\n");

        if ((caps & ovrHmdCap_NoVSync) != 0)
            sb.append(" ovrHmdCap_NoVSync\n");

        if ((caps & ovrHmdCap_ExtendDesktop) != 0)
            sb.append(" ovrHmdCap_ExtendDesktop\n");

        if ((caps & ovrHmdCap_ExtendDesktop) != 0)
            sb.append(" ovrHmdCap_ExtendDesktop\n");

        if ((caps & ovrHmdCap_DisplayOff) != 0)
            sb.append(" ovrHmdCap_DisplayOff\n");

        if ((caps & ovrHmdCap_NoMirrorToWindow) != 0)
            sb.append(" ovrHmdCap_NoMirrorToWindow\n");

        if ((caps & ovrHmdCap_Writable_Mask) != 0)
            sb.append(" ovrHmdCap_Writable_Mask\n");

        if ((caps & ovrHmdCap_Service_Mask) != 0)
            sb.append(" ovrHmdCap_Service_Mask\n");

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

        if ((caps & ovrTrackingCap_Idle) != 0)
            sb.append(" ovrTrackingCap_Idle\n");

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

        if ((caps & ovrDistortionCap_NoRestore) != 0)
            sb.append(" ovrDistortionCap_NoRestore\n");

        if ((caps & ovrDistortionCap_FlipInput) != 0)
            sb.append(" ovrDistortionCap_FlipInput\n");

        if ((caps & ovrDistortionCap_SRGB) != 0)
            sb.append(" ovrDistortionCap_SRGB\n");

        if ((caps & ovrDistortionCap_Overdrive) != 0)
            sb.append(" ovrDistortionCap_Overdrive\n");

        if ((caps & ovrDistortionCap_HqDistortion) != 0)
            sb.append(" ovrDistortionCap_HqDistortion\n");

        if ((caps & ovrDistortionCap_LinuxDevFullscreen) != 0)
            sb.append(" ovrDistortionCap_LinuxDevFullscreen\n");

        if ((caps & ovrDistortionCap_ProfileNoTimewarpSpinWaits) != 0)
            sb.append(" ovrDistortionCap_ProfileNoTimewarpSpinWaits\n");

        return sb.toString();
    }

    public boolean isDirectMode()
    {
        if (IsReal)
            return false;

        if ((HmdCaps & ovrHmdCap_ExtendDesktop) != 0)
            return false;

        return true;
    }

    public boolean isExtendedMode()
    {
        return !isDirectMode();
    }
}
