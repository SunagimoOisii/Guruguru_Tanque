#include "ImgProcSepia.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState	  smp : register(s0);

float4 ImgProcSepiaPS(VS_OUTPUT input) : SV_TARGET
{
    //セピア変換行列
    static matrix<float, 3, 3> sepia =
    {
        { 0.393, 0.769, 0.189 },
        { 0.349, 0.686, 0.168 },
        { 0.272, 0.534, 0.131 }
    };
    
    float4 texel = tex.Sample(smp, input.uv);
    float3 color = mul(sepia, texel.rgb);
	
    return float4(color, 1.0);
}
