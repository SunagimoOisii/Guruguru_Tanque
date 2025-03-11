#include "ImgProcInvertColor.hlsli"

cbuffer cbuff : register(b0)
{
    matrix world;
}

cbuffer cbuff : register(b1)
{
    matrix viewProj;
}

VS_OUTPUT ImgProcInvertColorVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VS_OUTPUT output;
    output.pos = mul(mul(viewProj, world), pos);
    output.uv  = uv;
    
    return output;
}