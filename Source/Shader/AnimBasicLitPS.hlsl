#include "AnimBasicLit.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState	  smp : register(s0);

float4 AnimBasicLitPS(VS_OUTPUT input) : SV_TARGET
{
    float3 n = normalize(input.normal_world);
    
//以下のライティングの影響度計算でmax()を用いて0と比較することが度々あるが
//それは影響度が負の値になることを防ぐため
    
//平行光源の拡散反射光
    float3 lambert = diff * max(0.0, -dot(n, paraLightDir));
    
//並行光源の鏡面反射光
    float3 r = reflect(paraLightDir, n);
    float3 e = normalize(eye - input.pos_world);
    float3 specular = spec * pow(max(0.0, dot(r, e)), specPow);
    
//ポイントライト処理
    for (int i = 0; i < PointLightNum; i++)
    {
        //拡散反射光
        //物体表面からライトへのベクトル,距離を計算
        float3 v = pointLights[i].pos - input.pos_world;
        float len = length(v);
        //影響度の計算のmax()にて0.0と比較することで負の値を防ぐ
        float affect = pow(max(0.0, 1.0 - len / pointLights[i].radius), 2.0);
    
        //鏡面反射光
        float3 ray = normalize(v);
        lambert += affect * pointLights[i].color * max(0.0, dot(n, ray));
        r = reflect(-ray, n);
        specular += affect * pointLights[i].color * pow(max(0.0, dot(r, e)), specPow);
    }
    
//スポットライト処理(角度に関する影響度(affectR)以外はポイントライトと同じ)
    for (i = 0; i < SpotLightNum; i++)
    {
        //拡散反射光
        float3 v = spotLights[i].pos - input.pos_world;
        float len = length(v);
        float3 ray = normalize(v);
        float rAngle = radians(spotLights[i].angle);
        float affectD = pow(max(0.0, 1.0 - len / spotLights[i].radius), 2.0);
        float affectR = pow(max(0.0, 1.0 - acos(-dot(ray, spotLights[i].dir)) / rAngle), 0.5);
        float affect = affectD * affectR;
        lambert += affect * spotLights[i].color * max(0.0, dot(n, ray));
    
        //鏡面反射光
        r = reflect(-ray, n);
        specular += affect * spotLights[i].color * pow(max(0.0, dot(r, e)), specPow);
    }
    
//最終的な拡散反射光,鏡面反射光
    lambert *= diff;
    specular *= spec;
    
//反射強度
    float3 brightness = paraLightCol * max(lambert + specular, amb * ambientLight);
    
//最終的な出力内容
    float4 color = tex.Sample(smp, input.uv);
    return float4(brightness * color.rgb, 1.0);
}