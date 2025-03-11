#include "AnimBasicLit.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState	  smp : register(s0);

float4 AnimBasicLitPS(VS_OUTPUT input) : SV_TARGET
{
    float3 n = normalize(input.normal_world);
    
//�ȉ��̃��C�e�B���O�̉e���x�v�Z��max()��p����0�Ɣ�r���邱�Ƃ��x�X���邪
//����͉e���x�����̒l�ɂȂ邱�Ƃ�h������
    
//���s�����̊g�U���ˌ�
    float3 lambert = diff * max(0.0, -dot(n, paraLightDir));
    
//���s�����̋��ʔ��ˌ�
    float3 r = reflect(paraLightDir, n);
    float3 e = normalize(eye - input.pos_world);
    float3 specular = spec * pow(max(0.0, dot(r, e)), specPow);
    
//�|�C���g���C�g����
    for (int i = 0; i < PointLightNum; i++)
    {
        //�g�U���ˌ�
        //���̕\�ʂ��烉�C�g�ւ̃x�N�g��,�������v�Z
        float3 v = pointLights[i].pos - input.pos_world;
        float len = length(v);
        //�e���x�̌v�Z��max()�ɂ�0.0�Ɣ�r���邱�Ƃŕ��̒l��h��
        float affect = pow(max(0.0, 1.0 - len / pointLights[i].radius), 2.0);
    
        //���ʔ��ˌ�
        float3 ray = normalize(v);
        lambert += affect * pointLights[i].color * max(0.0, dot(n, ray));
        r = reflect(-ray, n);
        specular += affect * pointLights[i].color * pow(max(0.0, dot(r, e)), specPow);
    }
    
//�X�|�b�g���C�g����(�p�x�Ɋւ���e���x(affectR)�ȊO�̓|�C���g���C�g�Ɠ���)
    for (i = 0; i < SpotLightNum; i++)
    {
        //�g�U���ˌ�
        float3 v = spotLights[i].pos - input.pos_world;
        float len = length(v);
        float3 ray = normalize(v);
        float rAngle = radians(spotLights[i].angle);
        float affectD = pow(max(0.0, 1.0 - len / spotLights[i].radius), 2.0);
        float affectR = pow(max(0.0, 1.0 - acos(-dot(ray, spotLights[i].dir)) / rAngle), 0.5);
        float affect = affectD * affectR;
        lambert += affect * spotLights[i].color * max(0.0, dot(n, ray));
    
        //���ʔ��ˌ�
        r = reflect(-ray, n);
        specular += affect * spotLights[i].color * pow(max(0.0, dot(r, e)), specPow);
    }
    
//�ŏI�I�Ȋg�U���ˌ�,���ʔ��ˌ�
    lambert *= diff;
    specular *= spec;
    
//���ˋ��x
    float3 brightness = paraLightCol * max(lambert + specular, amb * ambientLight);
    
//�ŏI�I�ȏo�͓��e
    float4 color = tex.Sample(smp, input.uv);
    return float4(brightness * color.rgb, 1.0);
}