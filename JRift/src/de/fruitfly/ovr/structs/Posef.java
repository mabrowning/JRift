package de.fruitfly.ovr.structs;

public class Posef
{
    public Posef()
    {

    }

    public Posef(float quatx,
                 float quaty,
                 float quatz,
                 float quatw,
                 float posx,
                 float posy,
                 float posz
                 )
    {
        Orientation.x = quatx;
        Orientation.y = quaty;
        Orientation.z = quatz;
        Orientation.w = quatw;
        Position.x = posx;
        Position.y = posy;
        Position.z = posz;
    }

    public Quatf     Orientation = Quatf.IDENTITY;
    public Vector3f  Position = new Vector3f();

    public String getPositionString()
    {
        return String.format("x: %.5f, y: %.5f, z: %.5f", new Object[]{Float.valueOf(Position.x), Float.valueOf(Position.y), Float.valueOf(Position.z)});
    }

}
