package de.fruitfly.ovr;

import de.fruitfly.ovr.enums.EyeType;
import de.fruitfly.ovr.structs.EyeRenderDesc;

public class EyeRenderParams
{
    public EyeRenderDesc[] Eyes = new EyeRenderDesc[2];
    public boolean valid = false;
    public float worldScale = 1f;

    public EyeRenderParams()
    {
        Eyes[0] = new EyeRenderDesc();
        Eyes[1] = new EyeRenderDesc();
    }
    
    public EyeRenderParams(
            int eye1DescInt,
            int eye1EyeRenderViewportPosX,
            int eye1EyeRenderViewportPosY,
            int eye1EyeRenderViewportSizeW,
            int eye1EyeRenderViewportSizeH,
            float eye1DescFovUpTan,
            float eye1DescFovDownTan,
            float eye1DescFovLeftTan,
            float eye1DescFovRightTan,
            int eye1DistortedViewportPosx,
            int eye1DistortedViewportPosy,
            int eye1DistortedViewportSizew,
            int eye1DistortedViewportSizeh,
            float eye1PixelsPerTanAngleAtCenterx,
            float eye1PixelsPerTanAngleAtCentery,
            float eye1ViewAdjustx,
            float eye1ViewAdjusty,
            float eye1ViewAdjustz,
            int eye2DescInt,
            int eye2EyeRenderViewportPosX,
            int eye2EyeRenderViewportPosY,
            int eye2EyeRenderViewportSizeW,
            int eye2EyeRenderViewportSizeH,
            float eye2DescFovUpTan,
            float eye2DescFovDownTan,
            float eye2DescFovLeftTan,
            float eye2DescFovRightTan,
            int eye2DistortedViewportPosx,
            int eye2DistortedViewportPosy,
            int eye2DistortedViewportSizew,
            int eye2DistortedViewportSizeh,
            float eye2PixelsPerTanAngleAtCenterx,
            float eye2PixelsPerTanAngleAtCentery,
            float eye2ViewAdjustx,
            float eye2ViewAdjusty,
            float eye2ViewAdjustz,
            float worldScale
        )
    {
        Eyes[0] = new EyeRenderDesc();
        Eyes[0].Eye = EyeType.fromInteger(eye1DescInt);
        Eyes[0].EyeRenderViewport.Pos.x = eye1EyeRenderViewportPosX;
        Eyes[0].EyeRenderViewport.Pos.y = eye1EyeRenderViewportPosY;
        Eyes[0].EyeRenderViewport.Size.w = eye1EyeRenderViewportSizeW;
        Eyes[0].EyeRenderViewport.Size.h = eye1EyeRenderViewportSizeH;
        Eyes[0].Fov.UpTan = eye1DescFovUpTan;
        Eyes[0].Fov.DownTan = eye1DescFovDownTan;
        Eyes[0].Fov.LeftTan = eye1DescFovLeftTan;
        Eyes[0].Fov.RightTan = eye1DescFovRightTan;
        Eyes[0].DistortedViewport.Pos.x = eye1DistortedViewportPosx;
        Eyes[0].DistortedViewport.Pos.y = eye1DistortedViewportPosy;
        Eyes[0].DistortedViewport.Size.w = eye1DistortedViewportSizew;
        Eyes[0].DistortedViewport.Size.h = eye1DistortedViewportSizeh;
        Eyes[0].PixelsPerTanAngleAtCenter.x = eye1PixelsPerTanAngleAtCenterx;
        Eyes[0].PixelsPerTanAngleAtCenter.y = eye1PixelsPerTanAngleAtCentery;
        Eyes[0].ViewAdjust.x = eye1ViewAdjustx;
        Eyes[0].ViewAdjust.y = eye1ViewAdjusty;
        Eyes[0].ViewAdjust.z = eye1ViewAdjustz;
        Eyes[1] = new EyeRenderDesc();
        Eyes[1].Eye = EyeType.fromInteger(eye2DescInt);
        Eyes[1].EyeRenderViewport.Pos.x = eye2EyeRenderViewportPosX;
        Eyes[1].EyeRenderViewport.Pos.y = eye2EyeRenderViewportPosY;
        Eyes[1].EyeRenderViewport.Size.w = eye2EyeRenderViewportSizeW;
        Eyes[1].EyeRenderViewport.Size.h = eye2EyeRenderViewportSizeH;
        Eyes[1].Fov.UpTan = eye2DescFovUpTan;
        Eyes[1].Fov.DownTan = eye2DescFovDownTan;
        Eyes[1].Fov.LeftTan = eye2DescFovLeftTan;
        Eyes[1].Fov.RightTan = eye2DescFovRightTan;
        Eyes[1].DistortedViewport.Pos.x = eye2DistortedViewportPosx;
        Eyes[1].DistortedViewport.Pos.y = eye2DistortedViewportPosy;
        Eyes[1].DistortedViewport.Size.w = eye2DistortedViewportSizew;
        Eyes[1].DistortedViewport.Size.h = eye2DistortedViewportSizeh;
        Eyes[1].PixelsPerTanAngleAtCenter.x = eye2PixelsPerTanAngleAtCenterx;
        Eyes[1].PixelsPerTanAngleAtCenter.y = eye2PixelsPerTanAngleAtCentery;
        Eyes[1].ViewAdjust.x = eye2ViewAdjustx;
        Eyes[1].ViewAdjust.y = eye2ViewAdjusty;
        Eyes[1].ViewAdjust.z = eye2ViewAdjustz;       
        this.worldScale = worldScale;
        valid = true;
    }
}
