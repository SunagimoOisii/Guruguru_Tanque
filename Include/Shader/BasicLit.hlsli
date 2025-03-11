//�s�N�Z���V�F�[�_�ɏo�͂���f�[�^���܂Ƃ߂��\����
//�@�������P�x�Ƃ��ēn��
struct VS_OUTPUT
{
    float4 pos          : SV_POSITION;
    float2 uv           : TEXCOORD;
    float3 pos_world    : POSITION_WORLD;
    float3 normal_world : NORMAL_WORLD;
};

cbuffer cbuff : register(b0)
{
    matrix world;
    float3 diff;
    float3 spec;
    float  specPow;
    float3 amb;
}

struct PointLight
{
    float3 pos;
    float3 color;
    float  radius;
};
static const int PointLightNum = 10;

struct SpotLight
{
    float3 pos;
    float  radius;
    float3 dir;
    float  angle;
    float3 color;
};
static const int SpotLightNum = 3;

//�V�[�����ʒ萔�o�b�t�@�ɑΉ�
cbuffer cbuff : register(b1)
{
    matrix viewProj;
    float3 eye;
    float3 paraLightDir;
    float3 paraLightCol;
    PointLight pointLights[PointLightNum];
    SpotLight  spotLights[SpotLightNum];
    float3 ambientLight;
}