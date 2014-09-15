package de.fruitfly.ovr.structs;

public class FovPort
{
    public float UpTan;
    public float DownTan;
    public float LeftTan;
    public float RightTan;

    public FovPort() {}

    public FovPort(float upTan, float downTan, float leftTan, float rightTan)
    {
        UpTan = upTan;
        DownTan = downTan;
        LeftTan = leftTan;
        RightTan = rightTan;
    }

    public FovPort(float sideTan)
    {
        UpTan = sideTan;
        DownTan = sideTan;
        LeftTan = sideTan;
        RightTan = sideTan;
    }

    public float GetMaxSideTan()
    {
        return Math.max(Math.max(UpTan, DownTan), Math.max(LeftTan, RightTan));
    }

    public static FovPort min(final FovPort a, final FovPort b)
    {
        FovPort fov = new FovPort( Math.min( a.UpTan   , b.UpTan    ),
                                   Math.min( a.DownTan , b.DownTan  ),
                                   Math.min( a.LeftTan , b.LeftTan  ),
                                   Math.min( a.RightTan, b.RightTan ) );
        return fov;
    }

    public static FovPort max(final FovPort a, final FovPort b)
    {
        FovPort fov = new FovPort(Math.max( a.UpTan   , b.UpTan    ),
                                  Math.max( a.DownTan , b.DownTan  ),
                                  Math.max( a.LeftTan , b.LeftTan  ),
                                  Math.max( a.RightTan, b.RightTan ) );
        return fov;
    }

    public FovPort enlargedFov(float additionalDegrees)
    {
        FovPort enlarged  = new FovPort();

        double UpFov      = Math.atan(UpTan) + Math.toRadians(additionalDegrees);
        double DownFov    = Math.atan(DownTan) + Math.toRadians(additionalDegrees);
        double LeftFov    = Math.atan(LeftTan) + Math.toRadians(additionalDegrees);
        double RightFov   = Math.atan(RightTan) + Math.toRadians(additionalDegrees);
        // TODO: Limit to max +-.5PI
        enlarged.UpTan    = (float)Math.tan(UpFov);
        enlarged.DownTan  = (float)Math.tan(DownFov);
        enlarged.LeftTan  = (float)Math.tan(LeftFov);
        enlarged.RightTan = (float)Math.tan(RightFov);

        return enlarged;
    }

    public FovPort enlargedFov(float additionalUpDegrees, float additionalDownDegrees, float additionalLeftDegrees, float additionalRightDegrees)
    {
        FovPort enlarged  = new FovPort();

        double UpFov      = Math.atan(UpTan) + Math.toRadians(additionalUpDegrees);
        double DownFov    = Math.atan(DownTan) + Math.toRadians(additionalDownDegrees);
        double LeftFov    = Math.atan(LeftTan) + Math.toRadians(additionalLeftDegrees);
        double RightFov   = Math.atan(RightTan) + Math.toRadians(additionalRightDegrees);
        // TODO: Limit to max +-.5PI
        enlarged.UpTan    = (float)Math.tan(UpFov);
        enlarged.DownTan  = (float)Math.tan(DownFov);
        enlarged.LeftTan  = (float)Math.tan(LeftFov);
        enlarged.RightTan = (float)Math.tan(RightFov);

        return enlarged;
    }

    public String toString()
    {
        double UpFov      = Math.toDegrees(Math.atan(UpTan));
        double DownFov    = Math.toDegrees(Math.atan(DownTan));
        double LeftFov    = Math.toDegrees(Math.atan(LeftTan));
        double RightFov   = Math.toDegrees(Math.atan(RightTan));

        return String.format("Up:%.2f, Down:%.2f, Left:%.2f, Right:%.2f", new Object[] {Float.valueOf((float)UpFov),Float.valueOf((float)DownFov),Float.valueOf((float)LeftFov),Float.valueOf((float)RightFov)});
    }
}
