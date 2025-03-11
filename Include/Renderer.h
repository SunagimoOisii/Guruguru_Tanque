#pragma once

#include <vector>
#include <memory>
#include <wrl.h> //ComPtr
#include <d3d12.h>
#include <string>
#include <dxgi1_4.h>
#include <unordered_map>
#include <DirectXTex.h>
#include <DirectXMath.h>

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "GameUtil.h"
#include "Shader.h"
#include "Camera.h"

using namespace DirectX;
using namespace Microsoft::WRL;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

struct RectF
{
	float width;
	float height;

	RectF():width(0.0f), height(0.0f) {}
	RectF(float w, float h):width(w), height(h) {}
};

class Renderer
{
public:
	Renderer(class Game* game, float r = 0.0f, float g = 0.0f, float b = 0.0f);
	~Renderer();

//�Z�b�^�[�Q�b�^�[

	void SetBackColor(float r, float g, float b);
	void SetBackColor(float* color);
	const float* GetBackColor() const { return m_backColor; } //�z��̐擪��������
	void SetCameraExtParam(CamExtParam extP);
	XMMATRIX GetCameraMatrix() const { return m_cameraMatrix; }
	void SetShader(int shaderIndex);
	Ray GetCameraRayOnScreenPos(XMFLOAT2 sPos) const;

//�֐�

	bool Initialize();

	void Update(float deltaTime);

	void WindowResize(const int width, const int height, const float aspectRatio);

	void Begin();

	void End();

	void Draw();

	/// <summary>
	/// �X�v���C�g�̕`��o�^(�O��Actor�̎g�p��z��)
	/// </summary>
	/// <param name="imgData">�`�悷��e�N�X�`���̏��</param>
	/// <param name="pos">�X�v���C�g�̍���̃X�N���[�����W</param>
	/// <param name="rect">�X�v���C�g�̕`��T�C�Y</param>
	/// <param name="imgPos">�e�N�X�`������̉摜���W</param>
	/// <param name="imgRect">�e�N�X�`���̃T�C�Y</param>
	bool DrawSprite(ImageData imgData, XMFLOAT2 pos, RectF* rect = nullptr, XMFLOAT2 imgPos = XMFLOAT2(0.0f, 0.0f),
		RectF* imgRect = nullptr);

	/// <summary>
	///�o�^���ꂽ�X�v���C�g�̕`�揈�����ꊇ�ōs��
	/// </summary>
	void DrawBatchSprite();

	void DrawMeshStrip(int vertexNum);

	void DrawMeshIndexed(int indicesNum, int startIndexPos, int instanceNum = 1);

	XMFLOAT2 ProjectToScreenPos(XMFLOAT3 worldPos) const;

/**********************************
���C�e�B���O�֌W
**********************************/

//���s����

	Light::Parallel GetParallelLight() const { return m_parallelLight; }

	void SetParallelLight(XMFLOAT3 dir, XMFLOAT3 color);

	void SetParallelLight(Light::Parallel light);

	void ResetParallelLight();

//�|�C���g���C�g
	/// <summary>
	/// �|�C���g���C�g�z�񂩂疢�g�p�̂��̂�T�����A����Έ������C�g���Z�b�g����
	/// </summary>
	/// <returns>
	/// ���p�\�̏ꍇ:�m�ۏꏊ�̃C���f�b�N�X
	/// <para></para>
	/// ���p�s�̏ꍇ:-1
	/// </returns>
	int AllocatePointLight(Light::Point light);

	void SetPointLight(int index, Light::Point light);

	void ResetPointLightAll();

	void ResetPointLight(int index);

//�X�|�b�g���C�g
	/// <summary>
	/// �X�|�b�g���C�g�z�񂩂疢�g�p�̂��̂�T�����A����Έ������C�g���Z�b�g����
	/// </summary>
	/// <returns>
	/// ���p�\�̏ꍇ:�m�ۏꏊ�̃C���f�b�N�X
	/// <para></para>
	/// ���p�s�̏ꍇ:-1
	/// </returns>
	int AllocateSpotLight(Light::Spot light);

	void SetSpotLight(int index, Light::Spot light);

	void ResetSpotLightAll();
	
	void ResetSpotLight(int index);

//����

	Light::Ambient GetAmbientLight() const { return m_ambientLight; }

	void SetAmbientLight(XMFLOAT3 light);

	void SetAmbientLight(Light::Ambient light);

/**********************************
�e��o�b�t�@�ƃV�F�[�_���\�[�X�̑��상�\�b�h
**********************************/

//���_�o�b�t�@
	/// <summary>
	/// ���_�o�b�t�@��nullptr�̂��̂�T�����V�����o�b�t�@�𐶐�
	/// </summary>
	/// <returns>
	/// ���������̏ꍇ:�����ꏊ�̃C���f�b�N�X
	/// <para></para>
	/// �����s�̏ꍇ:-1
	/// </returns>
	int AllocateVertexBuffer(void* pData, size_t dSize, size_t stride);

	void SetVertexBufferViewToCommandList(int index);

	/// <summary>
	/// �w��̒��_�o�b�t�@��m_vertexBufferReleaseList�ɉ�����
	/// <para></para>
	/// �֐����Œ����ɍ폜����ƃG���[�̉\��������̂�Update�֐��ō폜��x�����s����
	/// </summary>
	void ReleaseVertexBuffer(int index);

//�C���f�b�N�X�o�b�t�@
	/// <summary>
	/// �C���f�b�N�X�o�b�t�@��nullptr�̂��̂�T�����V�����o�b�t�@�𐶐�
	/// </summary>
	/// <returns>
	/// ���������̏ꍇ:�����ꏊ�̃C���f�b�N�X
	/// <para></para>
	/// �����s�̏ꍇ:-1
	/// </returns>
	int AllocateIndexBuffer(void* pData, size_t dSize);

	void SetIndexBufferViewToCommandList(int index);

	/// <summary>
	/// �w��̃C���f�b�N�X�o�b�t�@��m_indexBufferReleaseList�ɉ�����
	/// <para></para>
	/// �֐����Œ����ɍ폜����ƃG���[�̉\��������̂�Update�֐��ō폜��x�����s����
	/// </summary>
	void ReleaseIndexBuffer(int index);

//�萔�o�b�t�@
	/// <summary>
	/// �萔�o�b�t�@��nullptr�̂��̂�T�����V�����o�b�t�@�𐶐�
	/// </summary>
	/// <returns>
	/// ���������̏ꍇ:�����ꏊ�̃C���f�b�N�X
	/// <para></para>
	/// �����s�̏ꍇ:-1
	/// </returns>
	int  AllocateConstBuffer(const void* pData, size_t dSize);

	void SetConstBufferViewToCommandList(UINT tableIndex, UINT bufferIndex);

	void UploadConstBuffer(int index, const void* pData, size_t dSize) const;

	/// <summary>
	/// �w��̒萔�o�b�t�@��m_constBufferReleaseList�ɉ�����
	/// <para></para>
	/// �֐����Œ����ɍ폜����ƃG���[�̉\��������̂�Update�֐��ō폜��x�����s����
	/// </summary>
	void ReleaseConstBuffer(int index);

//�V�F�[�_���\�[�X
	/// <summary>
	/// imageBuffer��nullptr�̂��̂�T�����V�����o�b�t�@�𐶐�
	/// </summary>
	/// <returns>
	/// ���������̏ꍇ:�w��p�X��ImageData
	/// <para></para>
	/// �����s�̏ꍇ:imgIndex��-1�ɂ���ImageData
	/// </returns>
	ImageData AllocateShaderResource(const std::wstring& filePath, bool ddsFlag = false);

	/// <summary>
	/// �w��̃V�F�[�_���\�[�X��m_imageBufferReleaseList�ɉ�����
	/// <para></para>
	/// �֐����Œ����ɍ폜����ƃG���[�̉\��������̂�Update�֐��ō폜��x�����s����
	/// </summary>
	void ReleaseShaderResource(const ImageData& imgData);

	void SetShaderResourceViewToCommandList(UINT tableIndex, UINT bufferIndex);

/***************************************************************************
���֐��@���ϐ�(public)
***************************************************************************/

	static const int MaxPointLightNum = 10;
	Light::Point m_pointLights[MaxPointLightNum];
	bool m_availablePointLights[MaxPointLightNum];
	int m_usingPointLightsNum;

	static const int MaxSpotLightNum = 3;
	Light::Spot m_spotLights[MaxSpotLightNum];
	bool m_availableSpotLights[MaxSpotLightNum];
	int m_usingSpotLightsNum;

	Light::Ambient m_ambientLight;

	static const int ShaderClassNone		  = -1;
	static const int ShaderClassSprite 	      = 0;
	static const int ShaderClassUnlitLoop     = 1;
	static const int ShaderClassUnlitClamp    = 2;
	static const int ShaderClassBasicLitLoop  = 3;
	static const int ShaderClassBasicLitClamp = 4;
	static const int ShaderClassGrayScale	  = 5;
	static const int ShaderClassSepia		  = 6;
	static const int ShaderClassInvertColor	  = 7;
	static const int ShaderClassToon		  = 8;
	static const int ShaderClassAnimLitClamp  = 9;

	std::vector<std::pair<int, int>> m_constBufferReleaseList;

private:

	bool CreateFactory(); //DXGIFactory�̏������Ńf�o�b�O���o��
	bool CreateDevice(const wchar_t* adapterName);

/**********************************
�`��
**********************************/

	bool CreateCommandQueue();

	bool CreateSwapChain();

	bool CreateCommandAllocators();

	bool CreateCommandList();

	bool CreateRenderTargetView(DXGI_FORMAT format);

	bool CreateDepthHeap(ID3D12DescriptorHeap** heap, ID3D12Resource* depth);

	void SetViewport(D3D12_VIEWPORT& vp);

	void SetScissorRect(D3D12_RECT& scissor);

/**********************************
�V�F�[�_���\�[�X
**********************************/

	bool CreateShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
		DXGI_FORMAT format, UINT16 mipLevels = 1, UINT16 depthOrArraySize = 1,
		D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D);

	bool UploadShaderResource(ID3D12Resource* buffer, void* src, UINT lineSize, UINT allSize);

	bool CreateDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum);

	void SetShaderResourceView(ID3D12Resource* buffer, DXGI_FORMAT format,
		ID3D12DescriptorHeap* heap, UINT index);

/**********************************
�t�F���X
**********************************/

	bool CreateFence();

	void MoveToNextFrame();

	void WaitForGPU();

	void SetResourceBarrier(D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter);

	void EnableDebugLayer(); //DirectX�̃f�o�b�O���(DXGI�ɂ��Ă͏o�͂��Ȃ�)

/**********************************
�o�b�t�@
**********************************/

	bool CreateResourceBuffer(ID3D12Resource** buffer, UINT64 bSize);

	bool CreateDepthBuffer(ID3D12Resource** buffer);

	bool UploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize,void** map = nullptr);

	void SetVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vBV, ID3D12Resource* buffer,
		UINT dSize, UINT stride);

	void SetIndexBufferView(D3D12_INDEX_BUFFER_VIEW& iBV, ID3D12Resource* buffer, UINT dSize);

/**********************************
�萔�o�b�t�@
**********************************/

	//�V�[�����ʂ̒萔�o�b�t�@�𐶐�����
	//������Ƀr���[���f�B�X�N���v�^�q�[�v�ւ͍ڂ��Ȃ�
	bool CreateConstBuffer(ID3D12Resource** buffer, const void* pData, size_t dSize,
		void** pMap = nullptr);

	void SetConstBufferView(ID3D12Resource* buffer, ID3D12DescriptorHeap* dHeap, UINT index);

	/// <summary>256�{���̃o�C�g�T�C�Y���v�Z����<para></para>
	/// �萔�o�b�t�@�͂��̔{���̃o�C�g�T�C�Y�ł���K�v������
	///</summary>
	UINT CalcAlignment256(size_t size);

/**********************************
�e�N�X�`��
**********************************/

	bool ReadImageFile(ID3D12Resource** buffer, TexMetadata& metaData,
		const wchar_t* filePath, bool ddsFlag = FALSE);

	/// <summary>
	/// �P�F�e�N�X�`���𐶐�����
	/// </summary>
	/// <param name="imgName">�g���q�͕t�����Ɏw��</param>
	/// <returns></returns>
	bool CreateUnicolorImage(const wchar_t* imgName, ColorRGBA unicolor);

/***************************************************************************
���֐��@���ϐ�(private)
***************************************************************************/

	class Game* m_game;
	static const UINT FrameNum = 2;
	static const int ReleaseWaitFrame = 5;

//�f�o�C�X���

	ComPtr<IDXGIFactory4> m_dxgiFactory;
	D3D_FEATURE_LEVEL	  m_featureLevel;
	ComPtr<ID3D12Device>  m_device;

//�`�揈��

	ComPtr<ID3D12CommandQueue> m_cmdQueue;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12CommandAllocator> m_cmdAllocators[FrameNum];
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	UINT m_bufferIndex;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_depthHeap;
	ComPtr<ID3D12Resource> m_backBuffers[FrameNum];
	ComPtr<ID3D12Resource> m_depthBuffer; //�[�x�o�b�t�@�͉f�������̉ߒ��̂ݎg�p����̂Ńo�b�t�@�����O�ϐ��͕s�v
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

//�V�F�[�_���\�[�X

	static const UINT MaxImageNum  = 10000;
	static const UINT MaxSpriteNum = 10000;
	UINT m_imageNum;
	UINT m_spriteNum; //���݂̃X�v���C�g�ԍ�(�t���[������0�Ƀ��Z�b�g)
	std::vector<UINT> m_spriteIndices; //�e�X�v���C�g�ɕ`�悷��摜�ԍ��̔z��(�t���[������0�Ƀ��Z�b�g)
	std::unordered_map <std::wstring, ImageData> m_images;
	ComPtr<ID3D12Resource>			 m_imageBuffer[MaxImageNum];
	std::vector<std::pair<int, int>> m_imageBufferReleaseList;
	ComPtr<ID3D12DescriptorHeap> m_scDescHeap[FrameNum]; //�V�F�[�_���\�[�X,���\�[�X�o�b�t�@�pdescHeap

	//�f�B�X�N���v�^�q�[�v�ł̃C���N�������g�T�C�Y

	UINT m_rtvIncSize;
	UINT m_csuIncSize;

//�V�F�[�_

	static const UINT ShaderNum = 10;
	DXGI_FORMAT				m_renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	std::unique_ptr<Shader> m_shaders[ShaderNum];
	int						m_shaderIndex;

//���_�o�b�t�@

	static const UINT MaxVertexBufferNum = 50000;
	ComPtr<ID3D12Resource>			 m_vertexBuffer[MaxVertexBufferNum];
	ComPtr<ID3D12Resource>			 m_spriteVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW		 m_vertexBufferView[MaxVertexBufferNum];
	D3D12_VERTEX_BUFFER_VIEW		 m_spriteVertexBufferView;
	std::vector<std::pair<int, int>> m_vertexBufferReleaseList;

//�C���f�b�N�X�o�b�t�@

	static const UINT MaxIndexBufferNum = 50000;
	ComPtr<ID3D12Resource>			 m_indexBuffer[MaxIndexBufferNum];
	D3D12_INDEX_BUFFER_VIEW			 m_indexBufferView[MaxIndexBufferNum];
	std::vector<std::pair<int, int>> m_indexBufferReleaseList;

//�t�F���X

	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValues[FrameNum];

	float m_backColor[3];

//�V�[�����ʂ̒萔�o�b�t�@

	ComPtr<ID3D12Resource> m_sceneConstBuffer[FrameNum];
	void*				   m_sceneConstBufferMap[FrameNum];

//���f��(���b�V��)�p�萔�o�b�t�@

	static const UINT MaxConstBufferNum = 50000;
	ComPtr<ID3D12Resource> m_constBuffer[FrameNum][MaxConstBufferNum];
	void*				   m_constBufferMap[FrameNum][MaxConstBufferNum];

//�X�v���C�g�p�萔�o�b�t�@

	ComPtr<ID3D12Resource> m_spriteBuffer[FrameNum][MaxSpriteNum];
	void*				   m_spriteBufferMap[FrameNum][MaxSpriteNum];

	Camera	 m_camera;
	XMMATRIX m_cameraMatrix;
	XMMATRIX m_cameraInverseMatrix; //���K�f�o�C�X���W���瓯�����W�ւ̕ϊ��Ɏg�p

	Light::Parallel m_parallelLight;

	static const UINT SCViewNum = MaxImageNum + MaxSpriteNum + 2 * MaxConstBufferNum;
};