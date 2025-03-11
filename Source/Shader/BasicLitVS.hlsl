#include "BasicLit.hlsli"

VS_OUTPUT BasicLitVS(float4 pos : POSITION, float2 uv : TEXCOORD,
    float4 normal : NORMAL)
{
    VS_OUTPUT output;
    
    output.pos_world    = mul(world, pos).xyz;
    output.pos          = mul(viewProj, float4(output.pos_world, 1.0));
    output.uv           = uv;
    //ワールド行列のスケールにより法線ベクトルの長さが変化するため正規化を行う
    output.normal_world = normalize(mul(world, float4(normal.xyz, 0.0)).xyz);
    
    return output;
}