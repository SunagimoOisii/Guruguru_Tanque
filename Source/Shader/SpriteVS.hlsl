#include "Sprite.hlsli"

cbuffer cbuff : register(b0)
{
    matrix posMat;
    matrix uvMat;
}

VS_OUTPUT SpriteVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VS_OUTPUT output;
    
    output.pos = mul(posMat, pos);
    
    float4 t = mul(uvMat, float4(uv, 0, 1.0f));
    output.uv = t.xy;
    
    return output;
}