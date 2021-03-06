#pragma once
#include <windows.h>

class AppConfig
{
public:
	static bool FULL_SCREEN;
	static unsigned long WIDTH;
	static unsigned long HEIGHT;
	static bool VSYNC;
	static bool MULTISAMPLING;
	static unsigned int ANISOTROPIC;
	static float ASPECT;
	static char APP_NAME[16];

public:
	AppConfig(const char* filename);

private:
	static const unsigned long FS_WIDTH;
	static const unsigned long FS_HEIGHT;
	static const unsigned long WD_WIDTH;
	static const unsigned long WD_HEIGHT;

private:
	bool getBufferBoolean(const char* charBuffer) const;
	int getBufferInt(const char* charBuffer, const size_t bufferSize) const;
};