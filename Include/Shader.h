#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <wrl.h>
using namespace Microsoft::WRL;

/// <summary>
/// PipelineStateObjectとルートシグネチャをまとめたクラス
/// </summary>
class Shader
{
public:
	Shader(ID3D12Device* device, const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath,
		D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum,
		DXGI_FORMAT renderFormat,
		bool depthEnable, bool alphablendEnable, bool cullModeEnable,
		bool uvWrapEnable, bool sampleLinearEnable);
	~Shader();

	bool isEnabled() const { return m_enabled; }
	void SetShader(ID3D12GraphicsCommandList* cmdList);

private:
	bool ReadShaderObject(const wchar_t* filePath, ID3DBlob** shaderObj);
	bool CreateRootSignature(ID3D12Device* device, bool uvWrapEnable, bool sampleLinearEnable);
	bool CreatePSO(ID3D12Device* device, ID3DBlob* vertexShader, ID3DBlob* pixelShader,
		D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum,
		DXGI_FORMAT renderFormat,
		bool depthEnable, bool alphablendEnable, bool cullModeEnable);

	bool m_enabled;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pso;
};