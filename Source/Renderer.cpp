#include "Renderer.h"
#include "Game.h"
#include <vector>
#include <string>
#include <iostream>

Renderer::Renderer(Game* game, float r, float g, float b)
	: m_game(game)
	, m_featureLevel()
	, m_bufferIndex(0)
	, m_fenceValues()
	, m_rtvIncSize(0)
	, m_csuIncSize(0)
	, m_imageNum(0)
	, m_spriteNum(0)
	, m_spriteIndices(MaxSpriteNum)
	, m_sceneConstBufferMap{ nullptr }
	, m_constBufferMap{ nullptr }
	, m_spriteBufferMap{ nullptr }
	, m_camera(CamInParam(90.0f, 0.01f, 10000.0f), CamExtParam(ZeroVec3, UnitVec3X, UnitVec3Z))
	, m_shaderIndex(ShaderClassNone)
{
#ifdef _DEBUG
	EnableDebugLayer();
#endif
	SetBackColor(r, g, b);
	
	//ビュープロジェクション行列,その逆行列の計算
	m_cameraMatrix = m_camera.CalcViewProjMatrix((float)(m_game->GetWidth()), (float)(m_game->GetHeight()));
	m_cameraInverseMatrix = XMMatrixInverse(nullptr, m_cameraMatrix);

	for (auto i = m_spriteIndices.begin();i != m_spriteIndices.end();i++)
	{
		(*i) = 0;
	}

	ResetPointLightAll();
	ResetSpotLightAll();
}

Renderer::~Renderer()
{
	WaitForGPU();
	CoUninitialize(); //COM終了
}

void Renderer::SetBackColor(float r, float g, float b)
{
	m_backColor[0] = (r > 1.0f) ? 1.0f : ((r < 0.0f) ? 0.0f : r);
	m_backColor[1] = (g > 1.0f) ? 1.0f : ((g < 0.0f) ? 0.0f : g);
	m_backColor[2] = (b > 1.0f) ? 1.0f : ((b < 0.0f) ? 0.0f : b);
}

void Renderer::SetBackColor(float* color)
{
	SetBackColor(color[0], color[1], color[2]);
}

void Renderer::SetCameraExtParam(CamExtParam extP)
{
	m_camera.SetExtParam(extP);
	m_cameraMatrix = m_camera.CalcViewProjMatrix((float)(m_game->GetWidth()), 
		(float)(m_game->GetHeight()));
	m_cameraInverseMatrix = XMMatrixInverse(nullptr, m_cameraMatrix);
}

void Renderer::SetShader(int shaderIndex)
{
	//既に指定されたShaderクラスの場合は処理を行わない
	if (m_shaderIndex == shaderIndex) return;
	if (shaderIndex < 0 || shaderIndex >= ShaderNum) return;

	m_shaders[shaderIndex]->SetShader(m_cmdList.Get());
	m_shaderIndex = shaderIndex;
}

Ray Renderer::GetCameraRayOnScreenPos(XMFLOAT2 screenPos) const
{
	float halfWidth  = 0.5f * m_game->GetWidth();
	float halfHeight = 0.5f * m_game->GetHeight();

	//スクリーン上の座標を正規化デバイス座標系下で算出し
	//その後スクリーン座標系のものに変換
	XMFLOAT3 devPos{};
	devPos.x = (screenPos.x / halfWidth) - 1.0f;
	devPos.y = 1.0f - (screenPos.y / halfHeight); //上方向が正
	devPos.z = 0.0f;
	XMFLOAT3 scPos = CalcMatMul(devPos, m_cameraInverseMatrix);

	XMFLOAT3 camPos = m_camera.GetExtParam().eye;

	return Ray(camPos, Normalize(scPos - camPos));
}

bool Renderer::Initialize()
{
#ifdef _DEBUG
	EnableDebugLayer();
#endif
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED); //COM初期化

	if (!CreateFactory()		   || !CreateDevice(L"NVIDIA") ||
		!CreateCommandQueue()	   || !CreateSwapChain() ||
		!CreateCommandAllocators() || !CreateCommandList() ||
		!CreateRenderTargetView(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)  || !CreateFence()) return false;

	//深度バッファの生成
	if (!CreateDepthBuffer(m_depthBuffer.GetAddressOf()))				   return false;
	if (!CreateDepthHeap(m_depthHeap.GetAddressOf(), m_depthBuffer.Get())) return false;

	//入力レイアウト(入力アセンブラ)の設定
	//セマンティクスの情報を追加
	D3D12_INPUT_ELEMENT_DESC inputLayouts[5]{};
	inputLayouts[0] = { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	inputLayouts[1] = { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	inputLayouts[2] = { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	inputLayouts[3] = { "BONEID",0,DXGI_FORMAT_R16G16B16A16_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	inputLayouts[4] = { "WEIGHT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

	//各種シェーダに対応するShaderクラスの生成
	m_shaders[ShaderClassSprite] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\SpriteVS.cso", L"shader\\SpritePS.cso", inputLayouts, 2, m_renderTargetFormat,
		false, true, false, true, false);
	m_shaders[ShaderClassUnlitLoop] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\UnlitVS.cso", L"shader\\UnlitPS.cso", inputLayouts, 2, m_renderTargetFormat,
		true, false, false, true, true);
	m_shaders[ShaderClassUnlitClamp] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\UnlitVS.cso", L"shader\\UnlitPS.cso", inputLayouts, 2, m_renderTargetFormat,
		true, false, false, false, true);
	m_shaders[ShaderClassBasicLitLoop] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\BasicLitVS.cso", L"shader\\BasicLitPS.cso", inputLayouts, 3, m_renderTargetFormat,
		true, false, false, true, true);
	m_shaders[ShaderClassBasicLitClamp] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\BasicLitVS.cso", L"shader\\BasicLitPS.cso", inputLayouts, 3, m_renderTargetFormat,
		true, false, false, false, true);
	m_shaders[ShaderClassGrayScale] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\ImgProcGrayscaleVS.cso", L"shader\\ImgProcGrayscalePS.cso", inputLayouts, 2, m_renderTargetFormat,
		true, false, true, false, true);
	m_shaders[ShaderClassSepia] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\ImgProcSepiaVS.cso", L"shader\\ImgProcSepiaPS.cso", inputLayouts, 2, m_renderTargetFormat,
		true, false, true, false, true);
	m_shaders[ShaderClassInvertColor] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\ImgProcInvertColorVS.cso", L"shader\\ImgProcInvertColorPS.cso", inputLayouts, 2, m_renderTargetFormat,
		true, false, true, false, true);
	m_shaders[ShaderClassToon] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\BasicLitVS.cso", L"shader\\ToonPS.cso", inputLayouts, 3, m_renderTargetFormat,
		true, false, false, false, true);
	m_shaders[ShaderClassAnimLitClamp] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\AnimBasicLitVS.cso", L"shader\\AnimBasicLitPS.cso", inputLayouts, 5, m_renderTargetFormat,
		true, false, false, false, true);
	for (int i = 0;i < ShaderNum;i++)
	{
		if (!m_shaders[i]->isEnabled()) return false;
	}

	SetViewport(m_viewport);
	SetScissorRect(m_scissorRect);

	//ディスクリプタヒープと定数バッファの生成
	//メッシュ(モデル)の定数バッファについては各Shaderクラスで生成
	Transformat spriteMat{};
	SceneData sData{};
	spriteMat.posMat = XMMatrixIdentity();
	spriteMat.uvMat  = XMMatrixIdentity();
	for (int i = 0;i < FrameNum ;i++)
	{
		//ディスクリプタヒープ生成
		if (!CreateDescHeap(m_scDescHeap[i].GetAddressOf(), SCViewNum)) return false;

		//スプライト用定数バッファ生成と
		//そのビューのディスクリプタヒープへのセット
		for (int j = 0;j < MaxSpriteNum;j++)
		{
			if (!CreateConstBuffer(m_spriteBuffer[i][j].GetAddressOf(),
				(void*)&spriteMat, sizeof(Transformat),
				(void**)&m_spriteBufferMap[i][j])) return false;
			SetConstBufferView(m_spriteBuffer[i][j].Get(), m_scDescHeap[i].Get(),
				(UINT)(MaxImageNum + j));
		}

		if (!CreateConstBuffer(m_sceneConstBuffer[i].GetAddressOf(),
			(void*)&sData, sizeof(SceneData), &m_sceneConstBufferMap[i])) return false;
	}

	Vertex vertices[4]{};
	vertices[0] = { {-0.5f,-0.5f,0.0f},{0.0f,1.0f} };
	vertices[1] = { {-0.5f,0.5f,0.0f},{0.0f,0.0f} };
	vertices[2] = { {0.5f,-0.5f,0.0f},{1.0f,1.0f} };
	vertices[3] = { {0.5f,0.5f,0.0f},{1.0f,0.0f} };
	if (!CreateResourceBuffer(m_spriteVertexBuffer.GetAddressOf(), 4 * sizeof(Vertex)))		    return false;
	if (!UploadResourceBuffer(m_spriteVertexBuffer.Get(), (void*)vertices, 4 * sizeof(Vertex))) return false;
	SetVertexBufferView(m_spriteVertexBufferView, m_spriteVertexBuffer.Get(), 4 * sizeof(Vertex), sizeof(Vertex));

	m_cmdList->Close(); //新たなコマンドを呼び出せないかつコマンドが実行可能な状態にする

	//単色テクスチャの生成
	if (!CreateUnicolorImage(WHITE_TEX, ColorRGBA(255, 255, 255, 255))) return false;
	if (!CreateUnicolorImage(GRAY_TEX, ColorRGBA(127, 127, 127, 255)))  return false;
	if (!CreateUnicolorImage(BLACK_TEX, ColorRGBA(0, 0, 0, 255)))		return false;
	if (!CreateUnicolorImage(RED_TEX, ColorRGBA(255, 75, 0, 255)))		return false;
	if (!CreateUnicolorImage(GREEN_TEX, ColorRGBA(0, 255, 0, 255)))		return false;
	if (!CreateUnicolorImage(BLUE_TEX, ColorRGBA(0, 90, 255, 255)))		return false;
	if (!CreateUnicolorImage(YELLOW_TEX, ColorRGBA(255, 255, 0, 255)))	return false;
	if (!CreateUnicolorImage(CYAN_TEX, ColorRGBA(0, 255, 255, 255)))	return false;
	if (!CreateUnicolorImage(MAGENTA_TEX, ColorRGBA(255, 0, 255, 255))) return false;

	return true;
}

void Renderer::WindowResize(const int width, const int height, const float aspectRatio)
{
	if (width == 0 || height == 0) return;

	//現在のレンダーターゲットビューを解放
	for (int i = 0; i < FrameNum; i++)
	{
		m_backBuffers[i]->Release();
	}

	//スワップチェーンのリサイズ
	m_swapChain->ResizeBuffers(FrameNum, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	//レンダーターゲットビューの再作成
	for (UINT i = 0; i < FrameNum; i++)
	{
		CreateRenderTargetView(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	}

	//ビューポートの更新
	m_viewport.Width	 = static_cast<float>(width);
	m_viewport.Height	 = static_cast<float>(height);
	float newAspectRatio = m_viewport.Width / m_viewport.Height;
	//ウィンドウの幅が大きければビューポートの高さをウィンドウの幅に
	//ウィンドウの高さが大きければビューポートの幅をウィンドウの高さに合わせる
	if (newAspectRatio > aspectRatio)
	{
		float viewportWidth  = height * aspectRatio;
		float viewportHeight = static_cast<float>(height);
		m_viewport.TopLeftX  = (width - viewportWidth) / 2.0f;
		m_viewport.TopLeftY  = 0.0f;
		m_viewport.Width	 = viewportWidth;
		m_viewport.Height	 = viewportHeight;
	}
	else
	{
		float viewportWidth  = static_cast<float>(width);
		float viewportHeight = width / aspectRatio;
		m_viewport.TopLeftX  = 0.0f;
		m_viewport.TopLeftY  = (height - viewportHeight) / 2.0f;
		m_viewport.Width	 = viewportWidth;
		m_viewport.Height	 = viewportHeight;
	}

	//シザー矩形の更新
	m_scissorRect.right  = width;
	m_scissorRect.bottom = height;
}

void Renderer::Update(float deltaTime)
{
//各バッファの遅延削除処理
	//メッシュ用定数バッファ
	auto it_meshB = m_constBufferReleaseList.begin();
	while (it_meshB != m_constBufferReleaseList.end())
	{
		(*it_meshB).second -= 1;
		if ((*it_meshB).second <= 0)
		{
			int index = (*it_meshB).first;
			for (int i = 0;i < FrameNum;i++)
			{
				m_constBuffer[i][index]->Unmap(0, nullptr);
				m_constBuffer[i][index].Reset();
			}
			it_meshB = m_constBufferReleaseList.erase(it_meshB);
		}
		else it_meshB++;
	}

	//頂点バッファ
	auto it_verB = m_vertexBufferReleaseList.begin();
	while (it_verB != m_vertexBufferReleaseList.end())
	{
		(*it_verB).second -= 1;
		if ((*it_verB).second <= 0)
		{
			int index = (*it_verB).first;
			m_vertexBuffer[index].Reset();
			it_verB = m_vertexBufferReleaseList.erase(it_verB);
		}
		else it_verB++;
	}

	//インデックスバッファ
	auto it_indB = m_indexBufferReleaseList.begin();
	while (it_indB != m_indexBufferReleaseList.end())
	{
		(*it_indB).second -= 1;
		if ((*it_indB).second <= 0)
		{
			int index = (*it_indB).first;
			m_indexBuffer[index].Reset();
			it_indB = m_indexBufferReleaseList.erase(it_indB);
		}
		else it_indB++;
	}

	//シェーダリソース
	auto it_shaR = m_imageBufferReleaseList.begin();
	while (it_shaR != m_imageBufferReleaseList.end())
	{
		(*it_shaR).second--;
		if ((*it_shaR).second <= 0)
		{
			int index = (*it_shaR).first;
			m_imageBuffer[index].Reset();
			it_shaR = m_imageBufferReleaseList.erase(it_shaR);
		}
		else it_shaR++;
	}
}

void Renderer::Begin()
{
	//コマンドアロケータとコマンドリストをリセットし描画コマンド受付開始
	m_cmdAllocators[m_bufferIndex]->Reset();
	m_cmdList->Reset(m_cmdAllocators[m_bufferIndex].Get(), nullptr);

	//バックバッファ(リソース)を表示状態から描画状態にする
	SetResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//ディスクリプタヒープのハンドルを取得し
	//カレントバックバッファと深度バッファをレンダーターゲットに設定
	auto rtvH = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	auto dsvH = m_depthHeap->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += m_bufferIndex * m_rtvIncSize;
	m_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

	m_cmdList->ClearRenderTargetView(rtvH, m_backColor, 0, nullptr);
	m_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

//ビューポートとシザー矩形の設定
	//トポロジー,頂点バッファビューの指定,ディスクリプタ,PipelineStateObject,
	// ルートシグネチャの指定等はend()で行う
	m_cmdList->RSSetViewports(1, &m_viewport);
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);

	//シェーダ処理の準備と
	//シェーダリソース,定数バッファ(スプライト,メッシュ用の両方)のディスクリプタヒープへのセット
	m_shaderIndex = ShaderClassNone;
	m_cmdList->SetDescriptorHeaps(1, m_scDescHeap[m_bufferIndex].GetAddressOf());

	//シーン共通のデータを転送
	SceneData sData{};
	sData.viewProj = m_cameraMatrix;
	sData.eye = m_camera.GetExtParam().eye;
	sData.parallelLight = m_parallelLight;
	memcpy((void*)sData.pointLights, m_pointLights, MaxPointLightNum * sizeof(Light::Point));
	memcpy((void*)sData.spotLights, m_spotLights, MaxSpotLightNum * sizeof(Light::Spot));
	sData.ambientLight = m_ambientLight;
	memcpy(m_sceneConstBufferMap[m_bufferIndex], (void*)&sData, sizeof(SceneData));
}

void Renderer::End()
{
	//スプライトのバッチ描画
	DrawBatchSprite();

	//バックバッファ(リソース)を描画状態から表示状態にする
	SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	m_cmdList->Close(); //描画コマンドを実行可能にする

	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);

	m_swapChain->Present(1, 0); //バッファをフリップ

	MoveToNextFrame(); //カレントバックバッファのインデックス取得
}

void Renderer::Draw()
{
//各スプライトの描画
	for (UINT i = 0;i < MaxImageNum;i++)
	{
	//テクスチャのルートパラメータ番号と描画に使用するテクスチャのディスクリプタを接続
		auto handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += (MaxConstBufferNum + i) * m_csuIncSize;
		m_cmdList->SetGraphicsRootDescriptorTable(1, handle);

		m_cmdList->DrawInstanced(4, 1, 4 * i, 0);
	}
}

bool Renderer::DrawSprite(ImageData imgData, XMFLOAT2 pos, RectF* rect, XMFLOAT2 imgPos, RectF* imgRect)
{
	if (m_spriteNum >= MaxSpriteNum) return false; //スプライト描画個数の上限チェック

	m_spriteIndices[m_spriteNum] = imgData.imgIndex;

	//描画位置とUVの座標を計算
	RectF scRect = (rect != nullptr) ? *rect
		: RectF(static_cast<float>(imgData.width), static_cast<float>(imgData.height));
	XMFLOAT2 p1 = { pos.x,pos.y };
	RectF uvRect = (imgRect != nullptr) ? *imgRect
		: RectF(static_cast<float>(imgData.width), static_cast<float>(imgData.height));
	XMFLOAT2 u1 = { imgPos.x,imgPos.y };

	//座標変換行列を生成
	Transformat spriteMat{};
	spriteMat.posMat = CalcSpriteMatrix(p1, scRect.width, scRect.height,
		(float)m_game->GetWidth(), (float)m_game->GetHeight());
	spriteMat.uvMat = CalcSpriteUVMatrix(u1, uvRect.width, uvRect.height,
		(float)imgData.width, (float)imgData.height);

	//バッファへのコピー
	memcpy(m_spriteBufferMap[m_bufferIndex][m_spriteNum],
		(void*)&spriteMat, sizeof(Transformat));
	m_spriteIndices[m_spriteNum] = imgData.imgIndex;
	m_spriteNum++;

	return true;
}

void Renderer::DrawBatchSprite()
{
	SetShader(ShaderClassSprite);

	//直前のメッシュの描画処理がStripモードを使う保証はないのでここで設定
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_cmdList->IASetVertexBuffers(0, 1, &m_spriteVertexBufferView);
	for (UINT i = 0;i < m_spriteNum;i++)
	{
		auto handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += m_spriteIndices[i] * m_csuIncSize;
		m_cmdList->SetGraphicsRootDescriptorTable(0, handle);

		handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += (MaxImageNum + i) * m_csuIncSize;
		m_cmdList->SetGraphicsRootDescriptorTable(1, handle);

		m_cmdList->DrawInstanced(4, 1, 0, 0);
	}
	m_spriteNum = 0;
}

void Renderer::DrawMeshStrip(int vertexNum)
{
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_cmdList->DrawInstanced(vertexNum, 1, 0, 0);
}

void Renderer::DrawMeshIndexed(int indicesNum, int startIndexPos, int instanceNum)
{
	m_cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_cmdList->DrawIndexedInstanced(indicesNum, instanceNum, startIndexPos, 0, 0);
}

XMFLOAT2 Renderer::ProjectToScreenPos(XMFLOAT3 worldPos) const
{
	XMFLOAT3 devPos  = CalcMatMul(worldPos, m_cameraMatrix);
	float halfWidth  = 0.5f * m_game->GetWidth();
	float halfHeight = 0.5f * m_game->GetHeight();
	return XMFLOAT2(halfWidth * (devPos.x + 1.0f), halfHeight * (1.0f - devPos.y));
}

/**********************************
ライティング関係
**********************************/

//平行光源

void Renderer::SetParallelLight(XMFLOAT3 dir, XMFLOAT3 color)
{
	m_parallelLight.direction = Normalize(dir);
	m_parallelLight.color	  = color;
}

void Renderer::SetParallelLight(Light::Parallel light)
{
	m_parallelLight = light;
}

void Renderer::ResetParallelLight()
{
	Light::Parallel light(ZeroVec3, ZeroVec3);
	m_parallelLight = light;
}

//ポイントライト

int Renderer::AllocatePointLight(Light::Point light)
{
	if (m_usingPointLightsNum >= MaxPointLightNum) return -1;

	int index = 0;
	for (;index < MaxPointLightNum;index++)
	{
		if (m_availablePointLights[index]) break;
	}
	if (index >= MaxPointLightNum) return -1;

	m_pointLights[index] = light;
	m_availablePointLights[index] = false;
	m_usingPointLightsNum++;
	return index;
}

void Renderer::SetPointLight(int index, Light::Point light)
{
	if (index < 0 || index >= MaxPointLightNum) return;

	//AllocatePointLight()を経由しない等、使用中のライトを上書きする操作を
	//行った場合に向け、使用状況の調整を行う
	if (m_availablePointLights[index])
	{
		m_availablePointLights[index] = false;
		m_usingPointLightsNum++;
	}

	m_pointLights[index] = light;
}

void Renderer::ResetPointLightAll()
{
	Light::Point light(ZeroVec3, ZeroVec3, 1.0f);
	for (int i = 0;i < MaxPointLightNum;i++)
	{
		m_pointLights[i] = light;
		m_availablePointLights[i] = true;
	}
	m_usingPointLightsNum = 0;
}

void Renderer::ResetPointLight(int index)
{
	if (index < 0 || index >= MaxPointLightNum) return;
	if (m_availablePointLights[index])			return;

	m_pointLights[index] = Light::Point(ZeroVec3, ZeroVec3, 1.0f);
	m_availablePointLights[index] = true;
	m_usingPointLightsNum--;
}

//スポットライト

int Renderer::AllocateSpotLight(Light::Spot light)
{
	if (m_usingSpotLightsNum >= MaxSpotLightNum) return -1;

	int index = 0;
	for (;index < MaxSpotLightNum;index++)
	{
		if (m_availableSpotLights[index]) break;
	}
	if (index >= MaxSpotLightNum) return -1;

	m_spotLights[index] = light;
	m_availableSpotLights[index] = false;
	m_usingSpotLightsNum++;
	return index;
}

void Renderer::SetSpotLight(int index, Light::Spot light)
{
	if (index < 0 || index >= MaxSpotLightNum) return;

	//AllocateSpotLight()を経由しない等、使用中のライトを上書きする操作を
	//行った場合に向け、使用状況の調整を行う
	if (m_availableSpotLights[index])
	{
		m_availableSpotLights[index] = false;
		m_usingSpotLightsNum++;
	}

	m_spotLights[index] = light;
}

void Renderer::ResetSpotLightAll()
{
	Light::Spot light(ZeroVec3, 1.0f, -1.0f * UnitVec3Z, 30.0f, OneVec3);
	for (int i = 0;i < MaxSpotLightNum;i++)
	{
		m_spotLights[i] = light;
		m_availableSpotLights[i] = true;
	}
	m_usingSpotLightsNum = 0;
}

void Renderer::ResetSpotLight(int index)
{
	if (index < 0 || index >= MaxSpotLightNum) return;
	if (m_availableSpotLights[index])		   return;

	m_spotLights[index] = Light::Spot(ZeroVec3, 1.0, -1.0f * UnitVec3Z, 30.0f, OneVec3);
	m_availableSpotLights[index] = true;
	m_usingSpotLightsNum--;
}

//環境光

void Renderer::SetAmbientLight(XMFLOAT3 light)
{
	m_ambientLight = Light::Ambient(light);
}

void Renderer::SetAmbientLight(Light::Ambient light)
{
	m_ambientLight = light;
}

/**********************************
各種バッファとシェーダリソースの操作メソッド
**********************************/

//頂点バッファ

int Renderer::AllocateVertexBuffer(void* pData, size_t dSize, size_t stride)
{
	int index = 0;
	for (;index < MaxVertexBufferNum;index++)
	{
		if (m_vertexBuffer[index].Get() == nullptr) break;
	}
	if (index >= MaxVertexBufferNum) return -1;

	if (!CreateResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
		(UINT64)dSize))return -1;

	if (!UploadResourceBuffer(m_vertexBuffer[index].Get(),
		pData, (UINT64)dSize))
	{
		m_vertexBuffer[index].Reset();
		return -1;
	}

	SetVertexBufferView(m_vertexBufferView[index],
		m_vertexBuffer[index].Get(), (UINT)dSize, (UINT)stride);

	return index;
}

void Renderer::SetVertexBufferViewToCommandList(int index)
{
	m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView[index]);
}

void Renderer::ReleaseVertexBuffer(int index)
{
//無効または削除済みのバッファの場合は何もしない
	if (index < 0 || index >= MaxVertexBufferNum) return;
	if (m_vertexBuffer[index].Get() == nullptr)   return;
	
	m_vertexBufferReleaseList.push_back(std::make_pair(index, ReleaseWaitFrame));
}

//インデックスバッファ

int Renderer::AllocateIndexBuffer(void* pData, size_t dSize)
{
	int index = 0;
	for (;index < MaxIndexBufferNum;index++)
	{
		if (m_indexBuffer[index].Get() == nullptr) break;
	}
	if (index >= MaxIndexBufferNum) return -1;

	if (!CreateResourceBuffer(m_indexBuffer[index].GetAddressOf(),
		(UINT64)dSize)) return -1;

	if (!UploadResourceBuffer(m_indexBuffer[index].Get(),
		pData, (UINT64)dSize))
	{
		m_indexBuffer[index].Reset();
		return -1;
	}

	SetIndexBufferView(m_indexBufferView[index],
		m_indexBuffer[index].Get(), (UINT)dSize);

	return index;
}

void Renderer::SetIndexBufferViewToCommandList(int index)
{
	m_cmdList->IASetIndexBuffer(&m_indexBufferView[index]);
}

void Renderer::ReleaseIndexBuffer(int index)
{
//無効または削除済みのバッファの場合は何もしない
	if (index < 0 || index >= MaxIndexBufferNum) return;
	if (m_indexBuffer[index].Get() == nullptr)   return;

	m_indexBufferReleaseList.push_back(std::make_pair(index, ReleaseWaitFrame));
}

//定数バッファ

int  Renderer::AllocateConstBuffer(const void* pData, size_t dSize)
{
	int index = 0;
	for (;index < MaxConstBufferNum;index++)
	{
		if (m_constBuffer[0][index].Get() == nullptr) break;
	}
	if (index >= MaxConstBufferNum) return -1;

	for (int i = 0;i < FrameNum;i++)
	{
		if (!CreateConstBuffer(m_constBuffer[i][index].GetAddressOf(), pData, dSize,
			&m_constBufferMap[i][index]))
		{
			if (i > 0)
			{
				for (int j = i - 1;j >= 0;j--)
				{
					m_constBuffer[j][index].Reset();
				}
			}
			return -1;
		}

		//モデル固有,シーン共通の各々の定数バッファビューを
		//2個続けてディスクリプタヒープに配置
		SetConstBufferView(m_constBuffer[i][index].Get(), m_scDescHeap[i].Get(),
			(UINT)(MaxImageNum + MaxSpriteNum + 2 * index));
		SetConstBufferView(m_sceneConstBuffer[i].Get(), m_scDescHeap[i].Get(),
			(UINT)(MaxImageNum + MaxSpriteNum + 2 * index + 1));
	}

	return index;
}

void Renderer::SetConstBufferViewToCommandList(UINT tableIndex, UINT bufferIndex)
{
	//シーン共通定数バッファビューを直接バインドしないが
	//モデル固有のものと共に2個連続で配置していることをディスクリプタレンジで指定
	auto handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += (MaxImageNum + MaxSpriteNum + 2 * bufferIndex) * m_csuIncSize;
	m_cmdList->SetGraphicsRootDescriptorTable(tableIndex, handle);
}

void Renderer::UploadConstBuffer(int index, const void* pData, size_t dSize) const
{
	//不正なインデックスの場合は何もしない
	if (index < 0 || index >= MaxConstBufferNum) return;

	memcpy(m_constBufferMap[m_bufferIndex][index], pData, dSize);
}

void Renderer::ReleaseConstBuffer(int index)
{
	//無効または削除済みのバッファの場合は何もしない
	if (index < 0 || index >= MaxConstBufferNum)  return;
	if (m_constBuffer[0][index].Get() == nullptr) return;

	m_constBufferReleaseList.push_back(std::make_pair(index, ReleaseWaitFrame));
}

//シェーダリソース

ImageData Renderer::AllocateShaderResource(const std::wstring& filePath, bool ddsFlag)
{
	auto it = m_images.find(filePath);
	if (it != m_images.end())
	{
		(*it).second.counter++;
		return (*it).second;
	}

	ImageData imgData = { -1,0,0,0,L"" };
	if (m_imageNum >= MaxImageNum) return imgData;

	UINT index = 0;
	while (index < MaxImageNum)
	{
		if (m_imageBuffer[index].Get() == nullptr) break;
		index++;
	}
	if (index >= MaxImageNum) return imgData;

	TexMetadata metaData{};
	if (!ReadImageFile(m_imageBuffer[index].GetAddressOf(),
		metaData, filePath.c_str(), ddsFlag)) return imgData;

	for (int i = 0;i < FrameNum;i++)
	{
		SetShaderResourceView(m_imageBuffer[index].Get(),
			metaData.format, m_scDescHeap[i].Get(), index);
	}

	imgData.imgIndex = index;
	imgData.width    = (UINT)metaData.width;
	imgData.height   = (UINT)metaData.height;
	imgData.counter  = 1;
	imgData.filePath = filePath;
	m_imageNum++;
	m_images[filePath] = imgData;

	return imgData;
}

void Renderer::SetShaderResourceViewToCommandList(UINT tableIndex, UINT bufferIndex)
{
	auto handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += bufferIndex * m_csuIncSize;
	m_cmdList->SetGraphicsRootDescriptorTable(tableIndex, handle);
}

void Renderer::ReleaseShaderResource(const ImageData& imgData)
{
	//無効または削除済みのバッファの場合は何もしない
	if (imgData.imgIndex < 0 || imgData.imgIndex >= MaxImageNum) return;
	if (m_imageBuffer == nullptr) return;
	if (m_imageBuffer[imgData.imgIndex] == nullptr)				 return;

	auto it = m_images.find(imgData.filePath);
	if (it == m_images.end()) return;

	(*it).second.counter--;
	if ((*it).second.counter <= 0)
	{
		m_images.erase(it);
		m_imageBufferReleaseList.push_back(std::make_pair(imgData.imgIndex, ReleaseWaitFrame));
	}
}

//*************************************************************
//↑public ↓private
//*************************************************************

bool Renderer::CreateFactory()
{
	UINT debugFlag;
#ifdef _DEBUG
	debugFlag = DXGI_CREATE_FACTORY_DEBUG;
#else
	debugFlag = 0;
#endif

	HRESULT hr = CreateDXGIFactory2(debugFlag, IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::CreateDevice(const wchar_t* adapterName)
{
//PCのアダプタをvectorに収容
	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* sa = nullptr;
	int i = 0;
	while (m_dxgiFactory->EnumAdapters(i, &sa) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.emplace_back(sa);
		++i;
	}
	sa = nullptr;

//vector内で引数指定のアダプタ名を含むものを探索
	for (auto adp : adapters)
	{
		DXGI_ADAPTER_DESC adpDesc = {};
		adp->GetDesc(&adpDesc);
		std::wstring str = adpDesc.Description;
		//発見したものをD3D12CreateDevice関数の第一引数にする
		if (str.find(adapterName) != std::wstring::npos) //nposはfindで見つからなかったときの値を指す
		{
			sa = adp;
			break;
		}
	}

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

//アダプタが見つからなければ第一引数nullptrでアダプタは自動選択
	for (auto level : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(sa, level, IID_PPV_ARGS(m_device.GetAddressOf()))))
		{
			m_featureLevel = level;
			break;
		}
		return (m_device.Get() != nullptr);
	}

//メモリ解放
	for (auto adp : adapters)
	{
		adp->Release();
	}

	//各種データサイズ(インクリメントサイズ)をm_rtvIncSizeに保存
	m_rtvIncSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_csuIncSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

/**********************************
描画
**********************************/

bool Renderer::CreateCommandQueue()
{
	//コマンドキューの設定を表す構造体の作成
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags	  = D3D12_COMMAND_QUEUE_FLAG_NONE; //デフォルトのコマンドキュー
	desc.NodeMask = 0;							   //複数GPUがある際の設定
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; //通常の優先度
	desc.Type	  = D3D12_COMMAND_LIST_TYPE_DIRECT;		 //GPUが実行可能な設定

	HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_cmdQueue.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width			  = m_game->GetWidth();
	scDesc.Height			  = m_game->GetHeight();
	scDesc.Format			  = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo			  = FALSE;
	scDesc.BufferUsage		  = DXGI_USAGE_BACK_BUFFER;
	scDesc.BufferCount		  = FrameNum;
	scDesc.SampleDesc.Count	  = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Scaling			  = DXGI_SCALING_NONE;
	scDesc.SwapEffect		  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.AlphaMode		  = DXGI_ALPHA_MODE_UNSPECIFIED;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
	fsDesc.Windowed = TRUE;

	HWND hwnd = m_game->GetHwnd();
	ComPtr<IDXGISwapChain1> swapChain;
	HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd( //このメソッドでIDXGISwapChain1を作成
		m_cmdQueue.Get(), hwnd, &scDesc, &fsDesc,
		nullptr, swapChain.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = swapChain.As(&m_swapChain);
	if (FAILED(hr)) return false;

	//Alt+Enterによるフルスクリーンモード封印
	hr = m_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	//次に表示されるCurrentBackBufferのindex取得
	hr = swapChain.As(&m_swapChain);
	if (FAILED(hr)) return false;
	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	return SUCCEEDED(hr);
}

bool Renderer::CreateCommandAllocators()
{
	//バッファの数だけアロケータ確保
	for (UINT i = 0; i < FrameNum; ++i)
	{
		HRESULT hr = m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_cmdAllocators[i].GetAddressOf()));
		if (FAILED(hr)) return false;
	}
	return true;
}

bool Renderer::CreateCommandList()
{
	HRESULT hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_cmdAllocators[m_bufferIndex].Get(), nullptr, IID_PPV_ARGS(m_cmdList.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::CreateRenderTargetView(DXGI_FORMAT format)
{
//rtvのディスクリプタをバッファの数指定
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.NumDescriptors = FrameNum;
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask		= 0;

//ディスクリプタヒープ作成
	if (m_rtvHeap == nullptr)
	{
		HRESULT hr = m_device->CreateDescriptorHeap(
			&heapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf()));
		if (FAILED(hr)) return false;
	}

	//ディスクリプタヒープにはアドレス操作でアクセスする
	auto handle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

//バッファを取り出しRTV設定
	for (UINT i = 0;i < FrameNum;++i)
	{
		HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf()));
		if (FAILED(hr)) return false;

		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format				  = format; //grapicPipelineでのformatもこれに同じにする
		viewDesc.ViewDimension		  = D3D12_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice	  = 0;
		viewDesc.Texture2D.PlaneSlice = 0;

		m_device->CreateRenderTargetView(m_backBuffers[i].Get(), &viewDesc, handle);
		handle.ptr += m_rtvIncSize;
	}
	return true;
}

bool Renderer::CreateDepthHeap(ID3D12DescriptorHeap** heap, ID3D12Resource* depth)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NumDescriptors = 1;
	desc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	HRESULT hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap));
	if (FAILED(hr)) return false;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format		  = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags		  = D3D12_DSV_FLAG_NONE;

	m_device->CreateDepthStencilView(depth, &dsvDesc,
		(*heap)->GetCPUDescriptorHandleForHeapStart());

	return true;
}

void Renderer::SetViewport(D3D12_VIEWPORT& vp)
{
	int w = m_game->GetWidth();
	int h = m_game->GetHeight();

	vp.Width	= static_cast<float>(w);
	vp.Height	= static_cast<float>(h);
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
}

void Renderer::SetScissorRect(D3D12_RECT& scissor)
{
	int w = m_game->GetWidth();
	int h = m_game->GetHeight();

	scissor.left   = 0;
	scissor.top	   = 0;
	scissor.right  = w;
	scissor.bottom = h;
}

/**********************************
テクスチャ(シェーダリソース)
**********************************/

bool Renderer::CreateShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
	DXGI_FORMAT format, UINT16 mipLevels, UINT16 depthOrArraySize,D3D12_RESOURCE_DIMENSION dimension)
{
//転送設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type				  = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

//テクスチャデータ(シェーダリソース)の設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension			= dimension;
	desc.Width				= width; //画像幅
	desc.Height				= height; //画像高さ
	desc.DepthOrArraySize	= depthOrArraySize;
	desc.MipLevels			= mipLevels;
	desc.Format				= format;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags				= D3D12_RESOURCE_FLAG_NONE;
	desc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;

//後々テクスチャをピクセルシェーダで使用することを考慮した設定にする
	HRESULT hr = m_device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr, IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr);
}

bool Renderer::UploadShaderResource(ID3D12Resource* buffer, void* src, UINT lineSize, UINT allSize)
{
	HRESULT hr = buffer->WriteToSubresource(0, nullptr, src, lineSize, allSize);
	return SUCCEEDED(hr);
}

bool Renderer::CreateDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum)
{
//sRV用のディスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask		= 0;
	desc.NumDescriptors = dNum;
	desc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(dHeap));

	return SUCCEEDED(hr);
}

void Renderer::SetShaderResourceView(ID3D12Resource* buffer, DXGI_FORMAT format,
	ID3D12DescriptorHeap* dHeap, UINT index)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format					 = format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension			 = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels	 = 1;

	//dHeapの先頭ハンドルを得てそこからindex番目の場所にsRVを生成し保存する
	auto handle = dHeap->GetCPUDescriptorHandleForHeapStart();
	//handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * index;
	handle.ptr += m_csuIncSize * index;
	m_device->CreateShaderResourceView(buffer, &desc, handle);
}

/**********************************
フェンス
**********************************/

bool Renderer::CreateFence()
{
	for (UINT i = 0;i < FrameNum;++i)
	{
		m_fenceValues[i] = 0;
	}
	HRESULT hr = m_device->CreateFence(
		m_fenceValues[m_bufferIndex], D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_fence.GetAddressOf()));
	if (FAILED(hr)) return false;

	m_fenceValues[m_bufferIndex]++;

	return true;
}

void Renderer::MoveToNextFrame()
{
	//コマンドキューにシグナル状態セット 
	//GPU処理完了でそのm_fenceの値を第二引数に更新
	auto currentValue = m_fenceValues[m_bufferIndex];
	m_cmdQueue->Signal(m_fence.Get(), currentValue);

	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	if (m_fence->GetCompletedValue() < m_fenceValues[m_bufferIndex])
	{
		//次のバックバッファのGPU処理完了の確認 未終了なら待機処理
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		if (event)
		{
			m_fence->SetEventOnCompletion(m_fenceValues[m_bufferIndex], event);
			WaitForSingleObjectEx(event, INFINITE, FALSE);
			CloseHandle(event);
		}
	}

	m_fenceValues[m_bufferIndex] = currentValue + 1;
}

void Renderer::WaitForGPU()
{
	m_cmdQueue->Signal(m_fence.Get(), m_fenceValues[m_bufferIndex]);
	HANDLE event = CreateEvent(nullptr, false, false, nullptr);
	if (event)
	{
		m_fence->SetEventOnCompletion(m_fenceValues[m_bufferIndex], event);
		WaitForSingleObjectEx(event, INFINITE, FALSE);
		CloseHandle(event);
	}
	m_fenceValues[m_bufferIndex]++;
	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::SetResourceBarrier(D3D12_RESOURCE_STATES stateBefore,
	D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER desc = {};
	desc.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.Transition.pResource   = m_backBuffers[m_bufferIndex].Get();
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	desc.Transition.StateBefore = stateBefore;
	desc.Transition.StateAfter  = stateAfter;

	m_cmdList->ResourceBarrier(1, &desc);
}

void Renderer::EnableDebugLayer()
{
	ComPtr<ID3D12Debug> debugLayer = nullptr;
	D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.GetAddressOf()));

	debugLayer->EnableDebugLayer();
}

/**********************************
バッファ
**********************************/

bool Renderer::CreateResourceBuffer(ID3D12Resource** buffer, UINT64 bSize)
{
//ヒープ
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type				  = D3D12_HEAP_TYPE_UPLOAD; //CPUのMap,Unmapでデータ転送を行う
	heapProp.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

//リソース
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width				= bSize;
	desc.Height				= 1;
	desc.DepthOrArraySize   = 1;
	desc.MipLevels			= 1;
	desc.Format				= DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags				= D3D12_RESOURCE_FLAG_NONE;
	desc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

//バッファ生成
	HRESULT hr = m_device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr);
}

bool Renderer::CreateDepthBuffer(ID3D12Resource** buffer)
{
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension		  = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width			  = (UINT64)m_game->GetWidth();
	desc.Height			  = (UINT64)m_game->GetHeight();
	desc.DepthOrArraySize = 1;
	desc.Format			  = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Flags			  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type				  = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty	  = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format			   = DXGI_FORMAT_D32_FLOAT;

	HRESULT hr = m_device->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue, IID_PPV_ARGS(buffer));

	return (SUCCEEDED(hr));
}

bool Renderer::UploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize, void** map)
{
//map関数を用いてGPU上リソースをCPUに対応させリソース読み書きを行う
	if (map != nullptr)
	{
		*map = nullptr;
		HRESULT hr = buffer->Map(0, nullptr, map); //転送先バッファの仮想アドレスを第三引数で取得
		if (FAILED(hr) || *map == nullptr) return false;
		memcpy(*map, src, bSize);
	}
	else
	{
		void* pmap = nullptr;
		HRESULT hr = buffer->Map(0, nullptr, &pmap);
		if (FAILED(hr) || pmap == nullptr) return false;
		memcpy(pmap, src, bSize);
		buffer->Unmap(0, nullptr);
	}
	return true;
}

void Renderer::SetVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vBV, ID3D12Resource* buffer, UINT dSize, UINT stride)
{
//頂点情報の詳細をセット
	vBV.BufferLocation = buffer->GetGPUVirtualAddress(); //仮想アドレス
	vBV.SizeInBytes	   = dSize; //データの総バイト数
	vBV.StrideInBytes  = stride; //1頂点あたりのバイト数
}

void Renderer::SetIndexBufferView(D3D12_INDEX_BUFFER_VIEW& iBV, ID3D12Resource* buffer, UINT dSize)
{
	iBV.BufferLocation = buffer->GetGPUVirtualAddress();
	iBV.Format		   = DXGI_FORMAT_R16_UINT;
	iBV.SizeInBytes	   = dSize;
}

/**********************************
定数バッファ
**********************************/

bool Renderer::CreateConstBuffer(ID3D12Resource** buffer, const void* pData, size_t dSize,
	 void** pMap)
{
	if (!CreateResourceBuffer(buffer, CalcAlignment256(dSize)))
	{
		return false;
	}

	if (pMap == nullptr)
	{
		void* map = nullptr;
		HRESULT hr = (*buffer)->Map(0, nullptr, &map);
		if (FAILED(hr) || (map) == nullptr)
		{
			return false;
		}
		memcpy(map, pData, dSize);
		(*buffer)->Unmap(0, nullptr);
	}
	else
	{
		*pMap = nullptr;
		HRESULT hr = (*buffer)->Map(0, nullptr, pMap);
		if (FAILED(hr) || (*pMap) == nullptr)
		{
			return false;
		}
		memcpy(*pMap, pData, dSize);
	}

	return true;
}

void Renderer::SetConstBufferView(ID3D12Resource* buffer, ID3D12DescriptorHeap* dHeap, UINT index)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
	desc.BufferLocation = buffer->GetGPUVirtualAddress();
	desc.SizeInBytes = static_cast<UINT>(buffer->GetDesc().Width);

	auto handle = dHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * m_csuIncSize;

	m_device->CreateConstantBufferView(&desc, handle);
}

UINT Renderer::CalcAlignment256(size_t size)
{
	UINT64 s = static_cast<UINT64>(size);
	return (size + 0xff) & ~0xff;
}

/**********************************
テクスチャ
**********************************/

bool Renderer::ReadImageFile(ID3D12Resource** buffer, TexMetadata& metaData,
	const wchar_t* filePath, bool ddsFlag)
{
	ScratchImage img = {};

//ファイル読込
	HRESULT hr;
	if (ddsFlag) hr = LoadFromDDSFile(filePath, DDS_FLAGS_NONE, &metaData, img);
	else		 hr = LoadFromWICFile(filePath, WIC_FLAGS_NONE, &metaData, img);

	if (FAILED(hr))	return false;

//シェーダリソース生成
	if (!CreateShaderResource(buffer, static_cast<UINT>(metaData.width),
		static_cast<UINT>(metaData.height), metaData.format,
		static_cast<UINT16>(metaData.mipLevels),
		static_cast<UINT16>(metaData.arraySize),
		static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension))) return false;

	auto image = img.GetImage(0, 0, 0);
	if (!UploadShaderResource(*buffer, image->pixels, static_cast<UINT>(image->rowPitch),
		static_cast<UINT>(image->slicePitch))) return false;

	return true;
}

bool Renderer::CreateUnicolorImage(const wchar_t* imgName, ColorRGBA unicolor)
{
	//シェーダリソースの数が最大
	//または同名のものが存在していた場合はfalseを返す
	if (m_imageNum >= MaxImageNum) return false;
	auto it = m_images.find(imgName);
	if (it != m_images.end()) return false;

	//生成テクスチャの保存場所にするために未使用imageBufferを探索
	UINT index = 0;
	while (index < MaxImageNum)
	{
		if (m_imageBuffer[index].Get() == nullptr) break;
		index++;
	}
	if (index >= MaxImageNum) return false;

	//4*4でテクスチャ生成
	std::vector<ColorRGBA> colors(4 * 4);
	for (ColorRGBA& color : colors)
	{
		color = unicolor;
	}
	if (!CreateShaderResource(m_imageBuffer[index].GetAddressOf(),
		4, 4, DXGI_FORMAT_R8G8B8A8_UNORM)) return false;
	if (!UploadShaderResource(m_imageBuffer[index].Get(), colors.data(),
		(UINT)(sizeof(ColorRGBA) * 4), (UINT)(sizeof(ColorRGBA) * 16))) return false;
	for (int i = 0;i < FrameNum;i++)
	{
		SetShaderResourceView(m_imageBuffer[index].Get(),
			DXGI_FORMAT_R8G8B8A8_UNORM, m_scDescHeap[i].Get(), index);
	}

	//生成テクスチャを記録
	ImageData imgData = { (int)index,4,4,1,imgName };
	m_images[imgName] = imgData;

	return true;
}