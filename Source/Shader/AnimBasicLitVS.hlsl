#include "AnimBasicLit.hlsli"

VS_OUTPUT AnimBasicLitVS(float4 pos : POSITION, float2 uv : TEXCOORD,
float4 normal : NORMAL, min16uint4 boneID : BONEID, float4 weight : WEIGHT,
uint instanceID : SV_InstanceID)
{
    VS_OUTPUT output;
    
    //元々の頂点座標からスケルタルアニメーションでスキンの座標を計算
    float4 p = mul(pose[boneID[0]], mul(igPose[boneID[0]], pos)) * weight[0];
    p += mul(pose[boneID[1]], mul(igPose[boneID[1]], pos)) * weight[1];
    p += mul(pose[boneID[2]], mul(igPose[boneID[2]], pos)) * weight[2];
    p += mul(pose[boneID[3]], mul(igPose[boneID[3]], pos)) * weight[3];

    //元々の法線ベクトルからスケルタルアニメーションで方向を計算
    float4 normalU = float4(normal.xyz, 0.0);
    float4 n = mul(pose[boneID[0]], mul(igPose[boneID[0]], normalU)) * weight[0];
    n += mul(pose[boneID[1]], mul(igPose[boneID[1]], normalU)) * weight[1];
    n += mul(pose[boneID[2]], mul(igPose[boneID[2]], normalU)) * weight[2];
    n += mul(pose[boneID[3]], mul(igPose[boneID[3]], normalU)) * weight[3];
       
    output.pos_world    = mul(world, p).xyz;
    if(instanceID == 1)
    {
        output.pos_world += float3(1.5, 0.0, 0.0);
    }
    else if (instanceID == 2)
    {
        output.pos_world += float3(-1.5, 0.0, 0.0);
    }
    output.pos          = mul(viewProj, float4(output.pos_world, 1.0));
    output.uv           = uv;
    output.normal_world = normalize(mul(world, float4(normalize(n.xyz), 0.0)).xyz);

    return output;
}