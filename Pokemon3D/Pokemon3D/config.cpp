#include "config.h"

#include <string>

const unsigned long AppConfig::FS_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const unsigned long AppConfig::FS_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
const unsigned long AppConfig::WD_WIDTH = 1000;
const unsigned long AppConfig::WD_HEIGHT = (unsigned long) (WD_WIDTH / 
	  ((float)GetSystemMetrics(SM_CXSCREEN) / (float) GetSystemMetrics(SM_CYSCREEN)));

bool AppConfig::FULL_SCREEN;
bool AppConfig::VSYNC;
bool AppConfig::MULTISAMPLING;
unsigned int AppConfig::ANISOTROPIC;
unsigned long AppConfig::WIDTH;
unsigned long AppConfig::HEIGHT;
float AppConfig::ASPECT;
char AppConfig::APP_NAME[16];

AppConfig::AppConfig(const char* filename)
{
	/* App name extraction */
	GetPrivateProfileString(
		"winconfig",
		"app_name",
		"",
		AppConfig::APP_NAME,
		ARRAYSIZE(AppConfig::APP_NAME),
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

	AppConfig::FULL_SCREEN = getBufferBoolean(fsBuffer);

	if (AppConfig::FULL_SCREEN)
	{
		AppConfig::WIDTH = AppConfig::FS_WIDTH;
		AppConfig::HEIGHT = AppConfig::FS_HEIGHT;
	}
	else
	{
		AppConfig::WIDTH = AppConfig::WD_WIDTH;
		AppConfig::HEIGHT = AppConfig::WD_HEIGHT;
	}

	AppConfig::ASPECT = (float)(AppConfig::WIDTH) /
		(float)(AppConfig::HEIGHT);

	/* Vsync boolean extraction */
	char vsBuffer[8];
	GetPrivateProfileString(
		"winconfig",
		"vsync",
		"",
		vsBuffer,
		ARRAYSIZE(vsBuffer),
		filename);

	AppConfig::VSYNC = getBufferBoolean(vsBuffer);
	
	/* Multi-sampling boolean extraction */
	char msBuffer[8];
	GetPrivateProfileString(
		"rendconfig",
		"multisampling",
		"",
		msBuffer,
		ARRAYSIZE(msBuffer),
		filename);

	AppConfig::MULTISAMPLING = getBufferBoolean(msBuffer);

	/* Anisotropic filter value extraction */
	char atBuffer[4];
	GetPrivateProfileString(
		"rendconfig",
		"anisotropic",
		"",
		atBuffer,
		ARRAYSIZE(atBuffer),
		filename);

	AppConfig::ANISOTROPIC = (unsigned int)getBufferInt(atBuffer, ARRAYSIZE(atBuffer));
}

bool AppConfig::getBufferBoolean(const char* charBuffer) const
{
	return charBuffer[0] == 't' &&
		   charBuffer[1] == 'r' &&
	   	   charBuffer[2] == 'u' &&
		   charBuffer[3] == 'e';
}

int AppConfig::getBufferInt(const char* charBuffer, const size_t bufferSize) const
{
	std::string s;
	for (size_t i = 0; i < bufferSize; ++i) s.push_back(charBuffer[i]);
	return std::stoi(s);
}