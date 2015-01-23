package de.fruitfly.ovr.structs;

public class EulerOrient
{
    public EulerOrient(float x, float y, float z)
    {
        yaw = x;
        pitch = y;
        roll = z;
    }

    public EulerOrient() {}

    public float yaw = 0f;
    public float pitch = 0f;
    public float roll = 0f;
}
