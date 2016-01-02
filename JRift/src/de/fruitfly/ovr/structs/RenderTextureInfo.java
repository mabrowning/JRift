package de.fruitfly.ovr.structs;

public class RenderTextureInfo
{
    public RenderTextureInfo() {}

    public RenderTextureInfo(int leftFovTextureResolutionWidth,
                             int leftFovTextureResolutionHeight,
                             int rightFovTextureResolutionWidth,
                             int rightFovTextureResolutionHeight,
                             int combinedTextureResolutionWidth,
                             int combinedTextureResolutionHeight,
                             int HmdNativeResolutionWidth,
                             int HmdNativeResolutionHeight,
                             float renderScale)
    {
        LeftFovTextureResolution.w = leftFovTextureResolutionWidth;
        LeftFovTextureResolution.h = leftFovTextureResolutionHeight;

        RightFovTextureResolution.w = rightFovTextureResolutionWidth;
        RightFovTextureResolution.h = rightFovTextureResolutionHeight;

        CombinedTextureResolution.w = combinedTextureResolutionWidth;
        CombinedTextureResolution.h = combinedTextureResolutionHeight;

        HmdNativeResolution.w = HmdNativeResolutionWidth;
        HmdNativeResolution.h = HmdNativeResolutionHeight;

        RenderScale = renderScale;
    }

    public Sizei LeftFovTextureResolution = new Sizei();
    public Sizei RightFovTextureResolution = new Sizei();
    public Sizei CombinedTextureResolution = new Sizei();
    public Sizei HmdNativeResolution = new Sizei();
    float RenderScale = 1.0f;
}
