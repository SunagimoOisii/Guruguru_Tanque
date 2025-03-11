#include "Shader.h"
#include "iostream"
#include <d3dcompiler.h> //D3DReadFileToBlob�֐�
#pragma comment(lib,"d3dcompiler.lib")

Shader::Shader(ID3D12Device* device, const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath,
	D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum,
	DXGI_FORMAT renderFormat,
	bool depthEnable, bool alphablendEnable, bool cullModeEnable,
	bool uvWrapEnable, bool sampleLinearEnable)
	: m_enabled(false)
{
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;

	if (!ReadShaderObject(vertexShaderPath, vsBlob.GetAddressOf()))		return;
	if (!ReadShaderObject(pixelShaderPath, psBlob.GetAddressOf()))		return;
	if (!CreateRootSignature(device, uvWrapEnable, sampleLinearEnable)) return;
	if (!CreatePSO(device, vsBlob.Get(), psBlob.Get(),
		inputLayouts, layoutNum, renderFormat,
		depthEnable, alphablendEnable, cullModeEnable)) return;
	std::cout << "A" << std::endl;
	m_enabled = true;
}

Shader::~Shader()
{
}

void Shader::SetShader(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(m_pso.Get());
	cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
}

//*************************************************************
//��public ��private
//*************************************************************

bool Shader::ReadShaderObject(const wchar_t* filePath, ID3DBlob** shaderObj)
{
	HRESULT hr = D3DReadFileToBlob(filePath, shaderObj);
	return SUCCEEDED(hr);
}

bool Shader::CreateRootSignature(ID3D12Device* device, bool uvWrapEnable, bool sampleLinearEnable)
{
//���[�g�V�O�l�`���̐ݒ�(�f�B�X�N���v�^�����W���f�B�X�N���v�^�e�[�u�������[�g�p�����[�^�ŊK�w����)
	//���̓A�Z���u��
	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

//�f�B�X�N���v�^�����W
	D3D12_DESCRIPTOR_RANGE dRange[2]{};
	//�萔�̐ݒ�
	dRange[0].NumDescriptors = 1;
	dRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	dRange[0].BaseShaderRegister = 0; //���W�X�^�ԍ��w�� ��ʂ��ƂɃ��W�X�^��p�ӂ��Ă������Ă����Ȃ�
	dRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//�e�N�X�`���̐ݒ�
	dRange[1].NumDescriptors = 2;
	dRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dRange[1].BaseShaderRegister = 0;
	dRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

//�f�B�X�N���v�^�e�[�u��
	//�V�F�[�_���\�[�X(�e�N�X�`��)��PS,�萔�o�b�t�@(�ϊ��s��)��VS�Ŏg�p
	D3D12_ROOT_PARAMETER rParam[2] = {};
	//���_�V�F�[�_(�萔�̐ݒ�)
	rParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //�s�N�Z���V�F�[�_����A�N�Z�X�\
	rParam[0].DescriptorTable.pDescriptorRanges = &dRange[0];
	rParam[0].DescriptorTable.NumDescriptorRanges = 1;

	//�s�N�Z���V�F�[�_(�e�N�X�`���̐ݒ�)
	rParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rParam[1].DescriptorTable.pDescriptorRanges = &dRange[1];
	rParam[1].DescriptorTable.NumDescriptorRanges = 1;

	//���[�g�p�����[�^
	desc.pParameters = rParam;
	desc.NumParameters = 2;

//�T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC  samplerDesc{};
	D3D12_TEXTURE_ADDRESS_MODE uvAddressMode{};
	D3D12_FILTER filter{};
	if (uvWrapEnable)
	{
		uvAddressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
	else
	{
		uvAddressMode = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	}
	if (sampleLinearEnable)
	{
		filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	}
	else
	{
		filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	}

	//uv���W���͈͊O���̓���ݒ�
	samplerDesc.AddressU = uvAddressMode;
	samplerDesc.AddressV = uvAddressMode;
	samplerDesc.AddressW = uvAddressMode;

	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = filter;

	//LOD
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	desc.pStaticSamplers = &samplerDesc;
	desc.NumStaticSamplers = 1;

	ComPtr<ID3DBlob> blob;
	HRESULT hr = D3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		blob.GetAddressOf(), nullptr);
	if (FAILED(hr)) return false;

	hr = device->CreateRootSignature(
		0, blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Shader::CreatePSO(ID3D12Device* device, ID3DBlob* vertexShader, ID3DBlob* pixelShader,
	D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum,
	DXGI_FORMAT renderFormat,
	bool depthEnable, bool alphablendEnable, bool cullModeEnable)
{
//�O���t�B�b�N�p�C�v���C���ƃ����_�[�^�[�Q�b�g�̐ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pDesc{};
	//�e�V�F�[�_�I�u�W�F�N�g�̃|�C���^�ƃT�C�Y�̐ݒ�
	pDesc.VS.pShaderBytecode = vertexShader->GetBufferPointer();
	pDesc.VS.BytecodeLength  = vertexShader->GetBufferSize();
	pDesc.PS.pShaderBytecode = pixelShader->GetBufferPointer();
	pDesc.PS.BytecodeLength  = pixelShader->GetBufferSize();

	pDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//�A���t�@�u�����f�B���O
	D3D12_RENDER_TARGET_BLEND_DESC rtDesc{};
	rtDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	rtDesc.LogicOpEnable = FALSE;
	if (alphablendEnable)
	{
		rtDesc.BlendEnable    = TRUE;
		rtDesc.BlendOp		  = D3D12_BLEND_OP_ADD;
		rtDesc.SrcBlend		  = D3D12_BLEND_SRC_ALPHA;
		rtDesc.DestBlend	  = D3D12_BLEND_INV_SRC_ALPHA;
		rtDesc.SrcBlendAlpha  = D3D12_BLEND_ONE;
		rtDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		rtDesc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;
	}
	else
	{
		rtDesc.BlendEnable = FALSE;
	}
	pDesc.BlendState.AlphaToCoverageEnable  = FALSE;
	pDesc.BlendState.IndependentBlendEnable = FALSE;
	pDesc.BlendState.RenderTarget[0]		= rtDesc;

	//���X�^���C�Y
	pDesc.RasterizerState.MultisampleEnable		= FALSE;
	pDesc.RasterizerState.FillMode				= D3D12_FILL_MODE_SOLID;
	pDesc.RasterizerState.DepthClipEnable		= TRUE;
	pDesc.RasterizerState.FrontCounterClockwise = FALSE;
	pDesc.RasterizerState.DepthBias				= D3D12_DEFAULT_DEPTH_BIAS;
	pDesc.RasterizerState.DepthBiasClamp	    = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	pDesc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	pDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	pDesc.RasterizerState.ForcedSampleCount		= 0;
	pDesc.RasterizerState.ConservativeRaster	= D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	if (cullModeEnable)
	{
		pDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	}
	else
	{
		pDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}

	if (depthEnable)
	{
		pDesc.DepthStencilState.DepthEnable	   = TRUE;
		pDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pDesc.DepthStencilState.DepthFunc	   = D3D12_COMPARISON_FUNC_LESS;
	}
	else
	{
		pDesc.DepthStencilState.StencilEnable = FALSE;
	}
	pDesc.DepthStencilState.StencilEnable = FALSE;

	//���_���C�A�E�g
	pDesc.InputLayout.pInputElementDescs = inputLayouts; //���_���C�A�E�g�z��̐擪�|�C���^
	pDesc.InputLayout.NumElements = layoutNum; //�z��̗v�f��

	pDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pDesc.NumRenderTargets = 1;
	pDesc.RTVFormats[0] = renderFormat; //RTV�̃t�H�[�}�b�g�Ɠ������̂ɂ���

	pDesc.SampleDesc.Count = 1;
	pDesc.SampleDesc.Quality = 0;

	pDesc.pRootSignature = m_rootSignature.Get();

	HRESULT hr = device->CreateGraphicsPipelineState(&pDesc, IID_PPV_ARGS(m_pso.GetAddressOf()));

	return SUCCEEDED(hr);
}