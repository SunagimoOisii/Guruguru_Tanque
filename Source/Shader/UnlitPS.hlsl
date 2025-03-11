#include "Unlit.hlsli"

//0番レジスタにセットしたテクスチャ,サンプラーを選択
Texture2D<float4> tex : register(t0);
SamplerState	  smp : register(s0);

float4 UnlitPS(VS_OUTPUT input) : SV_TARGET
{
    return tex.Sample(smp, input.uv);
}