#include "common.h"

LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam);

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	/* Window Handle */
	HWND hWindow;

	/* Window class specification */
	WNDCLASSEX wndClass = {};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wndClass.lpszClassName = window::APP_NAME;

	/* Window registraton */
	RegisterClassEx(&wndClass);

	util::pair<ulong> wndPos = {};
	if (window::FULLSCREEN)
	{
		/* Display device info */
		DEVMODE	dmScreen = {};
		dmScreen.dmSize = sizeof(dmScreen);
		dmScreen.dmPelsWidth = (ulong) window::WIDTH;
		dmScreen.dmPelsHeight = (ulong) window::HEIGHT;
		dmScreen.dmBitsPerPel = window::PEL_BIT;
		dmScreen.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		
		/* Apply changes */
		ChangeDisplaySettings(&dmScreen, CDS_FULLSCREEN);
	}
	else
	{
		//TODO handle multiple resolutions
	}

	/* Window creation */
	hWindow = CreateWindowEx(
		NULL,
		window::APP_NAME,
		window::APP_NAME,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		wndPos.a,
		wndPos.b,
		window::WIDTH,
		window::HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	
	/* Show window and cursor */
	ShowWindow(hWindow, SW_SHOW);
	ShowCursor(false);

	/* Game Loop */
	bool running = true;
	MSG message;
	while (running)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT) running = false;
			else;//TODO input here
		}
		else
		{
			
		}
	}

	/* Shutdown */
	ShowCursor(true);
	return 0;
}

LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_KEYDOWN:
		{
			if (wparam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return 0;
			}
		}break;
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}