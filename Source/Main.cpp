#include <Windows.h>
#include <memory>

#include "Game.h"

std::unique_ptr<Game> game;

//ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace //ウィンドウの設定について
{
	const wchar_t* WinClassName = L"SampleGame";
	const wchar_t* WinGameName  = L"ぐるぐるタンク！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！";
	UINT WinWidth  = 1920;
	UINT WinHeight = 1080;
	//初期状態で画面いっぱいに描画するために少しWidthの割合を増やしている
	const float AspectRatio = static_cast<float>(WinWidth + 69) / WinHeight;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
//ウィンドウ構造体設定
	WNDCLASSEXW wc{};
	wc.cbSize		 = sizeof(WNDCLASSEXW);
	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.hInstance	 = hInstance;
	wc.hIcon	 	 = LoadIconW(nullptr, L"IDI_ICON");
	wc.hCursor		 = LoadCursorW(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszClassName = WinClassName;
	wc.hIconSm		 = LoadIconW(nullptr, L"IDI_ICON");

	if (!RegisterClassExW(&wc)) return 1; //ウィンドウクラス登録

//ウィンドウ生成
	//画面全体のサイズを取得
	int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	//screenWidth = 1920/5;
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	//screenHeight = 1080/5;

	//rc,styleを用いてクライアント領域の指定
	RECT rc{};
	rc.right	    = static_cast<LONG>(WinWidth);
	rc.bottom		= static_cast<LONG>(WinHeight);
	DWORD dwstyle   = WS_OVERLAPPEDWINDOW;
	DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_APPWINDOW;
	AdjustWindowRect(&rc, dwstyle, FALSE);

	HWND hwnd;
	hwnd = CreateWindowExW(dwExStyle, WinClassName, WinGameName, dwstyle, CW_USEDEFAULT, CW_USEDEFAULT,
		screenWidth, screenHeight,
		nullptr, nullptr, hInstance, nullptr);
	if (!hwnd)
	{
		return 1;
	}

//ゲームの生成と初期化
	game = std::make_unique<Game>();
	game->Initialize(hwnd, WinWidth, WinHeight);

	ShowWindow(hwnd, SW_SHOWNORMAL);

//メッセージループ
	MSG msg{};
	while (true)
	{
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		if (msg.message == WM_QUIT) break;

		//ゲームループ
		if (!game->Loop())
		{
			DestroyWindow(hwnd);
		}
	}

	UnregisterClassW(wc.lpszClassName, wc.hInstance); //ウィンドウ登録を解除

	return 0;
}

//ウィンドウプロシージャ定義
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//ウィンドウ破棄時発行
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			return 0;
		}
		break;

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			int width  = LOWORD(lParam);
			int height = HIWORD(lParam);
			game->GetRenderer()->WindowResize(width, height, AspectRatio);
		}
		return 0;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}