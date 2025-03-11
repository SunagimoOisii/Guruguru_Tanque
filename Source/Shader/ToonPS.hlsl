#include "BasicLit.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState      smp : register(s0);

float4 ToonPS(VS_OUTPUT input) : SV_TARGET
{
    float3 n = normalize(input.normal_world);

    //平行光源の拡散反射光
    float3 lambert = diff * max(0.0, -dot(n, paraLightDir));

    //ステップ関数のしきい値を段階ごとに調整して、ステップ間隔を徐々に大きくする
    float3 threshold1 = float3(0.01, 0.01, 0.01);
    float3 threshold2 = float3(0.2, 0.2, 0.2);
    float3 threshold3 = float3(0.99, 0.99, 0.99);

    lambert = step(threshold1, lambert) * 0.25 + step(threshold2, lambert) * 0.25 + step(threshold3, lambert) * 0.5;

    //ポイントライト処理
    for (int i = 0; i < PointLightNum; i++)
    {
        float3 v = pointLights[i].pos - input.pos_world;
        float len = length(v);
        float affect = pow(max(0.0, 1.0 - len / pointLights[i].radius), 2.0);

        float3 ray = normalize(v);
        lambert += affect * pointLights[i].color * max(0.0, dot(n, ray));
    }

    //スポットライト処理
    for (i = 0; i < SpotLightNum; i++)
    {
        float3 v = spotLights[i].pos - input.pos_world;
        float len = length(v);
        float3 ray = normalize(v);
        float rAngle = radians(spotLights[i].angle);
        float affectD = pow(max(0.0, 1.0 - len / spotLights[i].radius), 2.0);
        float affectR = pow(max(0.0, 1.0 - acos(-dot(ray, spotLights[i].dir)) / rAngle), 0.5);
        float affect = affectD * affectR;
        lambert += affect * spotLights[i].color * max(0.0, dot(n, ray));
    }

    //反射強度
    float3 brightness = paraLightCol * max(lambert, amb * ambientLight);

    //最終的な出力内容
    float4 color = tex.Sample(smp, input.uv);
    return float4(brightness * color.rgb, 1.0);
}
