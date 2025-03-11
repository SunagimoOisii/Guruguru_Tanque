#include "ImgProcInvertColor.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState	  smp : register(s0);

float4 ImgProcInvertColorPS(VS_OUTPUT input) : SV_TARGET
{
    float4 texel = tex.Sample(smp, input.uv);
    return float4(1.0 - texel.rgb, 1.0);

}