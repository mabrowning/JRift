package de.fruitfly.ovr.enums;

public enum HmdType
{
    ovrHmd_None(0),
    ovrHmd_DK1(3),
    ovrHmd_DKHD(4),
    ovrHmd_CrystalCoveProto(5),
    ovrHmd_DK2(6),
    ovrHmd_Other(999);

    private final int hmdTypeEnum;

    private HmdType(int value)
        {
            this.hmdTypeEnum = value;
        }

    public static HmdType fromInteger(int x) {
        switch(x) {
            case 0:
                return ovrHmd_None;
            case 3:
                return ovrHmd_DK1;
            case 4:
                return ovrHmd_DKHD;
            case 5:
                return ovrHmd_CrystalCoveProto;
            case 6:
                return ovrHmd_DK2;
        }
        return ovrHmd_Other;
    }

    public static String toString(HmdType type)
    {
        if (type == null)
            return "null";

        switch(type)
        {
            case ovrHmd_None:
                return "None";
            case ovrHmd_DK1:
                return "DK1";
            case ovrHmd_DKHD:
                return "DKHD";
            case ovrHmd_CrystalCoveProto:
                return "CrystalCoveProto";
            case ovrHmd_DK2:
                return "DK2";
        }

        return "Unknown";
    }
}
//// Definitions of axes for coordinate and rotation conversions.
//enum Axis
//{
//    Axis_X = 0, Axis_Y = 1, Axis_Z = 2
//};
//
//// RotateDirection describes the rotation direction around an axis, interpreted as follows:
////  CW  - Clockwise while looking "down" from positive axis towards the origin.
////  CCW - Counter-clockwise while looking from the positive axis towards the origin,
////        which is in the negative axis direction.
////  CCW is the default for the RHS coordinate system. Oculus standard RHS coordinate
////  system defines Y up, X right, and Z back (pointing out from the screen). In this
////  system Rotate_CCW around Z will specifies counter-clockwise rotation in XY plane.
//enum RotateDirection
//{
//    Rotate_CCW = 1,
//    Rotate_CW  = -1
//};
//
//// Constants for right handed and left handed coordinate systems
//enum HandedSystem
//{
//    Handed_R = 1, Handed_L = -1
//};