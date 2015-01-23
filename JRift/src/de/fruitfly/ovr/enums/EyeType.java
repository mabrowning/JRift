package de.fruitfly.ovr.enums;

public enum EyeType
{
    ovrEye_Center(-1),
    ovrEye_Left(0),
    ovrEye_Right(1),
    ovrEye_Count(2);

    private final int eyeTypeEnum;

    private EyeType(int value)
    {
        this.eyeTypeEnum = value;
    }

    public int value()
    {
        return this.eyeTypeEnum;
    }

    public static EyeType fromInteger(int x) {
        switch(x) {
            case -1:
                return ovrEye_Center;
            case 0:
                return ovrEye_Left;
            case 1:
                return ovrEye_Right;
        }
        return ovrEye_Count;
    }

    public static String toString(EyeType type)
    {
        switch(type)
        {
            case ovrEye_Center:
                return "ovrEye_Center";
            case ovrEye_Left:
                return "ovrEye_Left";
            case ovrEye_Right:
                return "ovrEye_Right";
        }

        return "ovrEye_Count";
    }
}
