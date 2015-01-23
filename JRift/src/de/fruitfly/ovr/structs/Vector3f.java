package de.fruitfly.ovr.structs;

public class Vector3f
{
    public Vector3f() {}

    public Vector3f(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    public Vector3f negated()
    {
        return new Vector3f(-x,-y,-z);
    }

    public Vector3f add(final Vector3f b)
    {
        return new Vector3f(x + b.x, y + b.y, z + b.z);
    }

    public Vector3f subtract(final Vector3f b)
    {
        return new Vector3f(x - b.x, y - b.y, z - b.z);
    }

    public float dot(final Vector3f b)
    {
        return x*b.x + y*b.y + z*b.z;
    }

    public Vector3f cross(final Vector3f b)
    {
        return new Vector3f(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
    }

    public Vector3f normalised()
    {
        float l = length();
        if (l != 0)
            return divide(l);

        return null;
    }

    public float length()
    {
        return (float)Math.sqrt(lengthSq());
    }

    public float lengthSq()
    {
        return (x * x + y * y + z * z);
    }

    public Vector3f divide(float s)
    {
        float rcp = 1f / s;
        return new Vector3f(x * rcp, y * rcp, z * rcp);
    }

    public float x;
    public float y;
    public float z;
}