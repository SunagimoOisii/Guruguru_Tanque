#include <Windows.h>
#include <memory>

#include "Game.h"

std::unique_ptr<Game> game;

//�E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace //�E�B���h�E�̐ݒ�ɂ���
{
	const wchar_t* WinClassName = L"SampleGame";
	const wchar_t* WinGameName  = L"���邮��^���N�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I�I";
	UINT WinWidth  = 1920;
	UINT WinHeight = 1080;
	//������Ԃŉ�ʂ����ς��ɕ`�悷�邽�߂ɏ���Width�̊����𑝂₵�Ă���
	const float AspectRatio = static_cast<float>(WinWidth + 69) / WinHeight;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
//�E�B���h�E�\���̐ݒ�
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

	if (!RegisterClassExW(&wc)) return 1; //�E�B���h�E�N���X�o�^

//�E�B���h�E����
	//��ʑS�̂̃T�C�Y���擾
	int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	//screenWidth = 1920/5;
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	//screenHeight = 1080/5;

	//rc,style��p���ăN���C�A���g�̈�̎w��
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

//�Q�[���̐����Ə�����
	game = std::make_unique<Game>();
	game->Initialize(hwnd, WinWidth, WinHeight);

	ShowWindow(hwnd, SW_SHOWNORMAL);

//���b�Z�[�W���[�v
	MSG msg{};
	while (true)
	{
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		if (msg.message == WM_QUIT) break;

		//�Q�[�����[�v
		if (!game->Loop())
		{
			DestroyWindow(hwnd);
		}
	}

	UnregisterClassW(wc.lpszClassName, wc.hInstance); //�E�B���h�E�o�^������

	return 0;
}

//�E�B���h�E�v���V�[�W����`
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//�E�B���h�E�j�������s
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