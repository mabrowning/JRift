package de.fruitfly.ovr.structs;

public class Quatf
{
    public Quatf()
    {
        w = 1.0f;
    }

    public Quatf(float X, float Y, float Z, float W)
    {
        x = X;
        y = Y;
        z = Z;
        w = W;
    }

    public float x;
    public float y;
    public float z;
    public float w;

    public static Quatf IDENTITY = new Quatf(0.0f, 0.0f, 0.0f, 1.0f);

    public Quatf inverted()
    {
        return new Quatf(-x, -y, -z, w);
    }
}
