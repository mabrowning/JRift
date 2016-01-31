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

    public static Quatf IDENTITY()
    {
        return new Quatf(0.0f, 0.0f, 0.0f, 1.0f);
    }

    public Quatf inverted()
    {
        return new Quatf(-x, -y, -z, w);
    }
}


/*
Some useful Quaternion values:

w	        x	        y	        z	        Description

1	        0	        0	        0	        Identity quaternion, no rotation
0	        1	        0	        0	        180° turn around X axis
0	        0	        1	        0	        180° turn around Y axis
0	        0	        0	        1	        180° turn around Z axis
sqrt(0.5)	sqrt(0.5)	0	        0	        90° rotation around X axis

sqrt(0.5)	0	        sqrt(0.5)	0	        90° rotation around Y axis

sqrt(0.5)	0	        0	        sqrt(0.5)	90° rotation around Z axis
sqrt(0.5)	-sqrt(0.5)	0	        0	       -90° rotation around X axis
sqrt(0.5)	0	        -sqrt(0.5)	0	       -90° rotation around Y axis
sqrt(0.5)	0	        0	        -sqrt(0.5) -90° rotation around Z axis

*/