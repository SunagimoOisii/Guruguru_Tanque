#include "Sprite.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState      smp : register(s0);

float4 SpritePS(VS_OUTPUT input) : SV_TARGET
{
    return tex.Sample(smp, input.uv);
}