#include "Unlit.hlsli"

//0�ԃ��W�X�^�ɃZ�b�g�����e�N�X�`��,�T���v���[��I��
Texture2D<float4> tex : register(t0);
SamplerState	  smp : register(s0);

float4 UnlitPS(VS_OUTPUT input) : SV_TARGET
{
    return tex.Sample(smp, input.uv);
}