package de.fruitfly.ovr.structs;

import de.fruitfly.ovr.enums.EyeType;

public class EyeDesc
{
    public EyeType  Eye = EyeType.ovrEye_Left;
    public Sizei    TextureSize = new Sizei();     // Absolute size of render texture.
    public Recti    RenderViewport = new Recti();  // Viewport within texture where eye rendering takes place.
                                     // If specified as (0,0,0,0), it will be initialized to TextureSize.
    public FovPort  Fov = new FovPort();
}
