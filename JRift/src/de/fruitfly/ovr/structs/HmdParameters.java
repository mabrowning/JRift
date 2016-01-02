package de.fruitfly.ovr.structs;

import de.fruitfly.ovr.enums.EyeType;
import de.fruitfly.ovr.enums.HmdType;

public class HmdParameters
{
    public HmdParameters() {}

    public HmdParameters(
            int hmdType,
            String productName,
            String manufacturer,
            int vendorid,
            int productid,
            String serialnumber,
            int firmwaremajor,
            int firmwareminor,
            float camerafrustumhfovnradians,
            float camerafrustumvfovinradians,
            float camerafrustumnearzinmeters,
            float camerafrustumfarzinmeters,
            int availablehmdCaps,
            int defaulthmdCaps,
            int availabletrackingCaps,
            int defaulttrackingCaps,
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
            int resolutionW,
            int resolutionH,
            float displayrefreshrate
        )
    {
        Type = HmdType.fromInteger(hmdType);
        ProductName = productName;
        Manufacturer = manufacturer;
        VendorId = vendorid;
        ProductId = productid;
        SerialNumber = serialnumber;
        FirmwareMajor = firmwaremajor;
        FirmwareMinor = firmwareminor;
        CameraFrustumHFovInRadians = camerafrustumhfovnradians;
        CameraFrustumVFovInRadians = camerafrustumvfovinradians;
        CameraFrustumNearZInMeters = camerafrustumnearzinmeters;
        CameraFrustumFarZInMeters = camerafrustumfarzinmeters;
        AvailableHmdCaps = availablehmdCaps;
        DefaultHmdCaps = defaulthmdCaps;
        AvailableTrackingCaps = availabletrackingCaps;
        DefaultTrackingCaps = defaulttrackingCaps;
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
        Resolution.w = resolutionW;
        Resolution.h = resolutionH;
        DisplayRefreshRate = displayrefreshrate;

        isSet = true;
    }
    
    public HmdType Type              = HmdType.ovrHmd_None;
    public String ProductName        = new String();
    public String Manufacturer       = new String();
    public int VendorId              = 0;
    public int ProductId             = 0;
    public String SerialNumber       = new String();
    public int FirmwareMajor         = 0;
    public int FirmwareMinor         = 0;
    public float CameraFrustumHFovInRadians = 0f;
    public float CameraFrustumVFovInRadians = 0f;
    public float CameraFrustumNearZInMeters = 0f;
    public float CameraFrustumFarZInMeters  = 0f;
    public int AvailableHmdCaps      = 0;
    public int DefaultHmdCaps        = 0;
    public int AvailableTrackingCaps = 0;
    public int DefaultTrackingCaps   = 0;
    public FovPort DefaultEyeFov[]   = new FovPort[2];
    public FovPort MaxEyeFov[]       = new FovPort[2];
    public Sizei Resolution          = new Sizei();
    public float DisplayRefreshRate  = 0f;
    boolean isSet = false;

    public boolean isReal()
    {
        // Do we have a real HMD attached, or a debug 'device'?

        if (!isSet) {
            return false;
        }

        if ((AvailableHmdCaps & ovrHmdCap_DebugDevice) != 0) {
            return false;
        }

        return true;
    }

    public String toString()
    {
        StringBuilder sb = new StringBuilder();

        sb.append("Type:                       ").append(HmdType.toString(Type)).append("\n");
        sb.append("ProductName:                ").append(ProductName).append("\n");
        sb.append("Manufacturer:               ").append(Manufacturer).append("\n");
        sb.append("VendorId:                   ").append(String.format("%X", VendorId)).append("\n");
        sb.append("ProductId:                  ").append(String.format("%X", ProductId)).append("\n");
        sb.append("SerialNumber:               ").append(SerialNumber).append("\n");
        sb.append("FirmwareMajor:              ").append(String.format("%d", FirmwareMajor)).append("\n");
        sb.append("FirmwareMinor:              ").append(String.format("%d", FirmwareMinor)).append("\n");
        sb.append("CameraFrustumHFovInRadians: ").append(String.format("%.2f", CameraFrustumHFovInRadians)).append("\n");
        sb.append("CameraFrustumVFovInRadians: ").append(String.format("%.2f", CameraFrustumVFovInRadians)).append("\n");
        sb.append("CameraFrustumNearZInMeters: ").append(String.format("%.2f", CameraFrustumNearZInMeters)).append("\n");
        sb.append("CameraFrustumFarZInMeters:  ").append(String.format("%.2f", CameraFrustumFarZInMeters)).append("\n");
        sb.append("Hmd capability bits:\n").append(HmdParameters.HmdCapsToString(AvailableHmdCaps));
        sb.append("Hmd default capability bits:\n").append(HmdParameters.HmdCapsToString(DefaultHmdCaps));
        sb.append("Tracker capability bits:\n").append(HmdParameters.TrackingCapsToString(AvailableTrackingCaps));
        sb.append("Tracker default capability bits:\n").append(HmdParameters.TrackingCapsToString(DefaultTrackingCaps));
        sb.append("Default LeftEye FOV:\n").append(DefaultEyeFov[0].toString()).append("\n");
        sb.append("Default RightEye FOV:\n").append(DefaultEyeFov[1].toString()).append("\n");
        sb.append("Max LeftEye FOV:\n").append(MaxEyeFov[0].toString()).append("\n");
        sb.append("Max RightEye FOV:\n").append(MaxEyeFov[1].toString()).append("\n");
        sb.append("Resolution:                 ").append(Resolution.w).append("x").append(Resolution.h).append("\n");
        sb.append("DisplayRefreshRate:         ").append(String.format("%.2f", DisplayRefreshRate)).append("\n");

        return sb.toString();
    }

    // WARNING: Oculus seem to change these on a regular basis

    // HMD capability bits reported by device.
    // Read-only flags.
    public static int ovrHmdCap_DebugDevice            = 0x0010;   // (read only) Means HMD device is a virtual debug device.

    // Tracking capability bits reported by device.
    // Used with ovrHmd_ConfigureTracking.
    public static int ovrTrackingCap_Orientation       = 0x0010;   //  Supports orientation tracking (IMU).
    public static int ovrTrackingCap_MagYawCorrection  = 0x0020;   //  Supports yaw correction through magnetometer or other means.
    public static int ovrTrackingCap_Position          = 0x0040;   //  Supports positional tracking.


    public static String HmdCapsToString(int caps)
    {
        StringBuilder sb = new StringBuilder();

        if ((caps & ovrHmdCap_DebugDevice) != 0)
            sb.append(" ovrHmdCap_DebugDevice\n");

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
}
