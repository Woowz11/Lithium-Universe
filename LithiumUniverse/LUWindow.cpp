#include <Windows.h>
#include "Console.h";

const LPCWSTR GameWindowClass = L"GameWindow";

LRESULT CALLBACK GameWindowLife(HWND id, UINT msg, WPARAM param, LPARAM lparam) {
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(id);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(id, msg, param, lparam);
	}
	return 0;
}

int WINAPI GameWindow(HINSTANCE inst, HINSTANCE prev, LPSTR cmdline, int cmdshow) {
	WNDCLASSEX win;
	HWND id;
	MSG msg;

	win.cbSize        = sizeof(WNDCLASSEX);
	win.style         = 0;
	win.lpfnWndProc   = GameWindowLife;
	win.cbClsExtra    = 0;
	win.cbWndExtra    = 0;
	win.hInstance     = inst;
	win.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	win.hCursor       = LoadCursor(NULL, IDC_ARROW);
	win.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	win.lpszMenuName  = NULL;
	win.lpszClassName = GameWindowClass;
	win.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&win)) {
		MessageBox(NULL, L"GOVNO POTOM SDELAY!", L"GOVNO!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	id = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		GameWindowClass,
		L"LithiumUniverse",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, inst, NULL
	);

	if (id == NULL) {
		MessageBox(NULL, L"GOVNO POTOM SDELAY! 2", L"GOVNO! 2",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(id, cmdshow);
	UpdateWindow(id);

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

int GameWindowUpdate() {
	return GameWindow(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOW);
}