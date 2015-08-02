#include "winconfig.h"

#include <string>

const unsigned long WindowConfig::FS_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const unsigned long WindowConfig::FS_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
const unsigned long WindowConfig::WD_WIDTH = 1000;
const unsigned long WindowConfig::WD_HEIGHT = (unsigned long) (WD_WIDTH / 
	  ((float)GetSystemMetrics(SM_CXSCREEN) / (float) GetSystemMetrics(SM_CYSCREEN)));
bool WindowConfig::FULL_SCREEN;
bool WindowConfig::VSYNC;
unsigned long WindowConfig::WIDTH;
unsigned long WindowConfig::HEIGHT;
float WindowConfig::ASPECT;
char WindowConfig::APP_NAME[16];

#include "common.h"
WindowConfig::WindowConfig(const char* filename)
{
	/* App name extraction */
	GetPrivateProfileString(
		"winconfig",
		"app_name",
		"",
		WindowConfig::APP_NAME,
		ARRAYSIZE(WindowConfig::APP_NAME),
		filename);

	/* Full screen switch extraction */
	char fsBuffer[8];
	GetPrivateProfileString(
		"winconfig",
		"fullscreen",
		"",
		fsBuffer,
		ARRAYSIZE(fsBuffer),
		filename);

	WindowConfig::FULL_SCREEN = (
		fsBuffer[0] == 't' &&
		fsBuffer[1] == 'r' &&
		fsBuffer[2] == 'u' &&
		fsBuffer[3] == 'e');

	if (WindowConfig::FULL_SCREEN)
	{
		WindowConfig::WIDTH = WindowConfig::FS_WIDTH;
		WindowConfig::HEIGHT = WindowConfig::FS_HEIGHT;
	}
	else
	{
		WindowConfig::WIDTH = WindowConfig::WD_WIDTH;
		WindowConfig::HEIGHT = WindowConfig::WD_HEIGHT;
	}

	WindowConfig::ASPECT = static_cast<float>(WindowConfig::WIDTH) /
		static_cast<float>(WindowConfig::HEIGHT);

	/* Vsync extraction */
	char vsBuffer[8];
	GetPrivateProfileString(
		"winconfig",
		"vsync",
		"",
		vsBuffer,
		ARRAYSIZE(vsBuffer),
		filename);

	WindowConfig::VSYNC = (
		vsBuffer[0] == 't' &&
		vsBuffer[1] == 'r' &&
		vsBuffer[2] == 'u' &&
		vsBuffer[3] == 'e');
}