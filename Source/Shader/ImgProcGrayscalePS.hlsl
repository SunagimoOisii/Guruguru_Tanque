#include "ImgProcGrayscale.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState	  smp : register(s0);

float4 ImgProcGrayscalePS(VS_OUTPUT input) : SV_TARGET
{
    //NTSC加重平均法でグレースケールの輝度を求める
    static float3 ntsc = { 0.298912, 0.58661, 0.114478 };
    
    float4 texel = tex.Sample(smp, input.uv);
    float gray   = dot(ntsc, texel.rgb);
    
    return float4(gray, gray, gray, 1.0f);
}