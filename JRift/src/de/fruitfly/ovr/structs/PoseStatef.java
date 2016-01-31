package de.fruitfly.ovr.structs;

public class PoseStatef
{
    public Posef     ThePose = new Posef();
    public Vector3f  AngularVelocity = new Vector3f();
    public Vector3f  LinearVelocity = new Vector3f();
    public Vector3f  AngularAcceleration = new Vector3f();
    public Vector3f  LinearAcceleration = new Vector3f();
    public double    TimeInSeconds = 0d;

    public String toString()
    {
        StringBuilder sb = new StringBuilder();
        sb.append(ThePose.toString());
        return sb.toString();
    }
}
