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

//セッターゲッター

	void SetBackColor(float r, float g, float b);
	void SetBackColor(float* color);
	const float* GetBackColor() const { return m_backColor; } //配列の先頭がかえる
	void SetCameraExtParam(CamExtParam extP);
	XMMATRIX GetCameraMatrix() const { return m_cameraMatrix; }
	void SetShader(int shaderIndex);
	Ray GetCameraRayOnScreenPos(XMFLOAT2 sPos) const;

//関数

	bool Initialize();

	void Update(float deltaTime);

	void WindowResize(const int width, const int height, const float aspectRatio);

	void Begin();

	void End();

	void Draw();

	/// <summary>
	/// スプライトの描画登録(外部Actorの使用を想定)
	/// </summary>
	/// <param name="imgData">描画するテクスチャの情報</param>
	/// <param name="pos">スプライトの左上のスクリーン座標</param>
	/// <param name="rect">スプライトの描画サイズ</param>
	/// <param name="imgPos">テクスチャ左上の画像座標</param>
	/// <param name="imgRect">テクスチャのサイズ</param>
	bool DrawSprite(ImageData imgData, XMFLOAT2 pos, RectF* rect = nullptr, XMFLOAT2 imgPos = XMFLOAT2(0.0f, 0.0f),
		RectF* imgRect = nullptr);

	/// <summary>
	///登録されたスプライトの描画処理を一括で行う
	/// </summary>
	void DrawBatchSprite();

	void DrawMeshStrip(int vertexNum);

	void DrawMeshIndexed(int indicesNum, int startIndexPos, int instanceNum = 1);

	XMFLOAT2 ProjectToScreenPos(XMFLOAT3 worldPos) const;

/**********************************
ライティング関係
**********************************/

//平行光源

	Light::Parallel GetParallelLight() const { return m_parallelLight; }

	void SetParallelLight(XMFLOAT3 dir, XMFLOAT3 color);

	void SetParallelLight(Light::Parallel light);

	void ResetParallelLight();

//ポイントライト
	/// <summary>
	/// ポイントライト配列から未使用のものを探索し、あれば引数ライトをセットして
	/// </summary>
	/// <returns>
	/// 利用可能の場合:確保場所のインデックス
	/// <para></para>
	/// 利用不可の場合:-1
	/// </returns>
	int AllocatePointLight(Light::Point light);

	void SetPointLight(int index, Light::Point light);

	void ResetPointLightAll();

	void ResetPointLight(int index);

//スポットライト
	/// <summary>
	/// スポットライト配列から未使用のものを探索し、あれば引数ライトをセットして
	/// </summary>
	/// <returns>
	/// 利用可能の場合:確保場所のインデックス
	/// <para></para>
	/// 利用不可の場合:-1
	/// </returns>
	int AllocateSpotLight(Light::Spot light);

	void SetSpotLight(int index, Light::Spot light);

	void ResetSpotLightAll();
	
	void ResetSpotLight(int index);

//環境光

	Light::Ambient GetAmbientLight() const { return m_ambientLight; }

	void SetAmbientLight(XMFLOAT3 light);

	void SetAmbientLight(Light::Ambient light);

/**********************************
各種バッファとシェーダリソースの操作メソッド
**********************************/

//頂点バッファ
	/// <summary>
	/// 頂点バッファでnullptrのものを探索し新しいバッファを生成
	/// </summary>
	/// <returns>
	/// 生成成功の場合:生成場所のインデックス
	/// <para></para>
	/// 生成不可の場合:-1
	/// </returns>
	int AllocateVertexBuffer(void* pData, size_t dSize, size_t stride);

	void SetVertexBufferViewToCommandList(int index);

	/// <summary>
	/// 指定の頂点バッファをm_vertexBufferReleaseListに加える
	/// <para></para>
	/// 関数内で直ちに削除するとエラーの可能性があるのでUpdate関数で削除を遅延実行する
	/// </summary>
	void ReleaseVertexBuffer(int index);

//インデックスバッファ
	/// <summary>
	/// インデックスバッファでnullptrのものを探索し新しいバッファを生成
	/// </summary>
	/// <returns>
	/// 生成成功の場合:生成場所のインデックス
	/// <para></para>
	/// 生成不可の場合:-1
	/// </returns>
	int AllocateIndexBuffer(void* pData, size_t dSize);

	void SetIndexBufferViewToCommandList(int index);

	/// <summary>
	/// 指定のインデックスバッファをm_indexBufferReleaseListに加える
	/// <para></para>
	/// 関数内で直ちに削除するとエラーの可能性があるのでUpdate関数で削除を遅延実行する
	/// </summary>
	void ReleaseIndexBuffer(int index);

//定数バッファ
	/// <summary>
	/// 定数バッファでnullptrのものを探索し新しいバッファを生成
	/// </summary>
	/// <returns>
	/// 生成成功の場合:生成場所のインデックス
	/// <para></para>
	/// 生成不可の場合:-1
	/// </returns>
	int  AllocateConstBuffer(const void* pData, size_t dSize);

	void SetConstBufferViewToCommandList(UINT tableIndex, UINT bufferIndex);

	void UploadConstBuffer(int index, const void* pData, size_t dSize) const;

	/// <summary>
	/// 指定の定数バッファをm_constBufferReleaseListに加える
	/// <para></para>
	/// 関数内で直ちに削除するとエラーの可能性があるのでUpdate関数で削除を遅延実行する
	/// </summary>
	void ReleaseConstBuffer(int index);

//シェーダリソース
	/// <summary>
	/// imageBufferでnullptrのものを探索し新しいバッファを生成
	/// </summary>
	/// <returns>
	/// 生成成功の場合:指定パスのImageData
	/// <para></para>
	/// 生成不可の場合:imgIndexを-1にしたImageData
	/// </returns>
	ImageData AllocateShaderResource(const std::wstring& filePath, bool ddsFlag = false);

	/// <summary>
	/// 指定のシェーダリソースをm_imageBufferReleaseListに加える
	/// <para></para>
	/// 関数内で直ちに削除するとエラーの可能性があるのでUpdate関数で削除を遅延実行する
	/// </summary>
	void ReleaseShaderResource(const ImageData& imgData);

	void SetShaderResourceViewToCommandList(UINT tableIndex, UINT bufferIndex);

/***************************************************************************
↑関数　↓変数(public)
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

	bool CreateFactory(); //DXGIFactoryの初期化でデバッグ情報出力
	bool CreateDevice(const wchar_t* adapterName);

/**********************************
描画
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
シェーダリソース
**********************************/

	bool CreateShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
		DXGI_FORMAT format, UINT16 mipLevels = 1, UINT16 depthOrArraySize = 1,
		D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D);

	bool UploadShaderResource(ID3D12Resource* buffer, void* src, UINT lineSize, UINT allSize);

	bool CreateDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum);

	void SetShaderResourceView(ID3D12Resource* buffer, DXGI_FORMAT format,
		ID3D12DescriptorHeap* heap, UINT index);

/**********************************
フェンス
**********************************/

	bool CreateFence();

	void MoveToNextFrame();

	void WaitForGPU();

	void SetResourceBarrier(D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter);

	void EnableDebugLayer(); //DirectXのデバッグ情報(DXGIについては出力しない)

/**********************************
バッファ
**********************************/

	bool CreateResourceBuffer(ID3D12Resource** buffer, UINT64 bSize);

	bool CreateDepthBuffer(ID3D12Resource** buffer);

	bool UploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize,void** map = nullptr);

	void SetVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vBV, ID3D12Resource* buffer,
		UINT dSize, UINT stride);

	void SetIndexBufferView(D3D12_INDEX_BUFFER_VIEW& iBV, ID3D12Resource* buffer, UINT dSize);

/**********************************
定数バッファ
**********************************/

	//シーン共通の定数バッファを生成する
	//生成後にビューをディスクリプタヒープへは載せない
	bool CreateConstBuffer(ID3D12Resource** buffer, const void* pData, size_t dSize,
		void** pMap = nullptr);

	void SetConstBufferView(ID3D12Resource* buffer, ID3D12DescriptorHeap* dHeap, UINT index);

	/// <summary>256倍数のバイトサイズを計算する<para></para>
	/// 定数バッファはこの倍数のバイトサイズである必要がある
	///</summary>
	UINT CalcAlignment256(size_t size);

/**********************************
テクスチャ
**********************************/

	bool ReadImageFile(ID3D12Resource** buffer, TexMetadata& metaData,
		const wchar_t* filePath, bool ddsFlag = FALSE);

	/// <summary>
	/// 単色テクスチャを生成する
	/// </summary>
	/// <param name="imgName">拡張子は付けずに指定</param>
	/// <returns></returns>
	bool CreateUnicolorImage(const wchar_t* imgName, ColorRGBA unicolor);

/***************************************************************************
↑関数　↓変数(private)
***************************************************************************/

	class Game* m_game;
	static const UINT FrameNum = 2;
	static const int ReleaseWaitFrame = 5;

//デバイス情報

	ComPtr<IDXGIFactory4> m_dxgiFactory;
	D3D_FEATURE_LEVEL	  m_featureLevel;
	ComPtr<ID3D12Device>  m_device;

//描画処理

	ComPtr<ID3D12CommandQueue> m_cmdQueue;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12CommandAllocator> m_cmdAllocators[FrameNum];
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	UINT m_bufferIndex;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_depthHeap;
	ComPtr<ID3D12Resource> m_backBuffers[FrameNum];
	ComPtr<ID3D12Resource> m_depthBuffer; //深度バッファは映像生成の過程のみ使用するのでバッファリング変数は不要
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

//シェーダリソース

	static const UINT MaxImageNum  = 10000;
	static const UINT MaxSpriteNum = 10000;
	UINT m_imageNum;
	UINT m_spriteNum; //現在のスプライト番号(フレーム毎に0にリセット)
	std::vector<UINT> m_spriteIndices; //各スプライトに描画する画像番号の配列(フレーム毎に0にリセット)
	std::unordered_map <std::wstring, ImageData> m_images;
	ComPtr<ID3D12Resource>			 m_imageBuffer[MaxImageNum];
	std::vector<std::pair<int, int>> m_imageBufferReleaseList;
	ComPtr<ID3D12DescriptorHeap> m_scDescHeap[FrameNum]; //シェーダリソース,リソースバッファ用descHeap

	//ディスクリプタヒープでのインクリメントサイズ

	UINT m_rtvIncSize;
	UINT m_csuIncSize;

//シェーダ

	static const UINT ShaderNum = 10;
	DXGI_FORMAT				m_renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	std::unique_ptr<Shader> m_shaders[ShaderNum];
	int						m_shaderIndex;

//頂点バッファ

	static const UINT MaxVertexBufferNum = 50000;
	ComPtr<ID3D12Resource>			 m_vertexBuffer[MaxVertexBufferNum];
	ComPtr<ID3D12Resource>			 m_spriteVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW		 m_vertexBufferView[MaxVertexBufferNum];
	D3D12_VERTEX_BUFFER_VIEW		 m_spriteVertexBufferView;
	std::vector<std::pair<int, int>> m_vertexBufferReleaseList;

//インデックスバッファ

	static const UINT MaxIndexBufferNum = 50000;
	ComPtr<ID3D12Resource>			 m_indexBuffer[MaxIndexBufferNum];
	D3D12_INDEX_BUFFER_VIEW			 m_indexBufferView[MaxIndexBufferNum];
	std::vector<std::pair<int, int>> m_indexBufferReleaseList;

//フェンス

	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValues[FrameNum];

	float m_backColor[3];

//シーン共通の定数バッファ

	ComPtr<ID3D12Resource> m_sceneConstBuffer[FrameNum];
	void*				   m_sceneConstBufferMap[FrameNum];

//モデル(メッシュ)用定数バッファ

	static const UINT MaxConstBufferNum = 50000;
	ComPtr<ID3D12Resource> m_constBuffer[FrameNum][MaxConstBufferNum];
	void*				   m_constBufferMap[FrameNum][MaxConstBufferNum];

//スプライト用定数バッファ

	ComPtr<ID3D12Resource> m_spriteBuffer[FrameNum][MaxSpriteNum];
	void*				   m_spriteBufferMap[FrameNum][MaxSpriteNum];

	Camera	 m_camera;
	XMMATRIX m_cameraMatrix;
	XMMATRIX m_cameraInverseMatrix; //正規デバイス座標から同次座標への変換に使用

	Light::Parallel m_parallelLight;

	static const UINT SCViewNum = MaxImageNum + MaxSpriteNum + 2 * MaxConstBufferNum;
};