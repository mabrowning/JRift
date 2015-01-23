package de.fruitfly.ovr.structs;

import de.fruitfly.ovr.enums.EyeType;

public class EyeRenderDesc
{
    public EyeType  Eye;
    public FovPort  Fov = new FovPort();
    public Recti    EyeRenderViewport = new Recti();
    public Recti	DistortedViewport = new Recti(); 	           // Distortion viewport
    public Vector2f PixelsPerTanAngleAtCenter = new Vector2f();    // How many display pixels will fit in tan(angle) = 1.
    public Vector3f ViewAdjust = new Vector3f();  		           // Translation to be applied to view matrix.
}
