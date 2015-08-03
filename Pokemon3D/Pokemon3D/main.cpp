#include <fstream>

#include "vld.h"
#include "gsqueue.h"
#include "renderer.h"
#include "config.h"

std::ofstream LOGFILE;

LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam);

int Shutdown(const int exitCode);

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	START_EXT_LOGGING();
	AppConfig winconfig(".\\appconfig.ini");

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
	wndClass.lpszClassName = AppConfig::APP_NAME;

	/* Window registraton */
	RegisterClassEx(&wndClass);

	util::pair<ulong> wndPos = {};
	if (AppConfig::FULL_SCREEN)
	{
		/* Display device info */
		DEVMODE	dmScreen = {};
		dmScreen.dmSize = sizeof(dmScreen);
		dmScreen.dmPelsWidth = (ulong) AppConfig::WIDTH;
		dmScreen.dmPelsHeight = (ulong) AppConfig::HEIGHT;
		dmScreen.dmBitsPerPel = 32;
		dmScreen.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		
		/* Apply changes */
		ChangeDisplaySettings(&dmScreen, CDS_FULLSCREEN);
	}
	else
	{
		wndPos.a = (GetSystemMetrics(SM_CXSCREEN) - AppConfig::WIDTH) / 2;
		wndPos.b = (GetSystemMetrics(SM_CYSCREEN) - AppConfig::HEIGHT) / 2;
	}

	/* Window creation */
	hWindow = CreateWindowEx(
		NULL,
		AppConfig::APP_NAME,
		AppConfig::APP_NAME,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		wndPos.a,
		wndPos.b,
		AppConfig::WIDTH,
		AppConfig::HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	
	/* Show window and cursor */
	ShowWindow(hWindow, SW_SHOW);
	ShowCursor(false);

	/* Game State queue initialization */
	GSQueue qstates;
	if (!qstates.Initialize(hWindow)) return Shutdown(EXIT_FAILURE);

	/* Game Loop */
	MSG message;
	while (1)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT) break;
			qstates.HandleInput(message);
		}
		else
		{
			qstates.Update();
			if (qstates.isEmpty()) break;
			qstates.Render();
		}
	}

	return Shutdown(EXIT_SUCCESS);
}

int Shutdown(const int exitCode)
{
	STOP_EXT_LOGGING();
	ShowCursor(true);
	return exitCode;
}

/* Custom window proc */
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
		} break;
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}