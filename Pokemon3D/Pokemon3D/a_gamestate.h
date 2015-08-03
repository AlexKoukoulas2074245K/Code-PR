#pragma once
#include "common.h"
#include "pokedef.h"

class Renderer;
class IOManager;
class HIDManager;
class FontEngine;
class GameState
{
public:
	virtual ~GameState(){};
	virtual bool Initialize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	bool isFinished() const { return mFinished; }
	void setRenderer(const sptr<Renderer> renderer){ mRenderer = renderer; }
	void setIOManager(const sptr<IOManager> ioManager){ mIOManager = ioManager; }
	void setHIDManager(const sptr<HIDManager> hidManager){ mHIDManager = hidManager; }
	void setFontEngine(const sptr<FontEngine> fontEngine) { mFontEngine = fontEngine; }

protected:
	bool mFinished;
	sptr<Renderer> mRenderer;
	sptr<IOManager> mIOManager;
	sptr<HIDManager> mHIDManager;
	sptr<FontEngine> mFontEngine;
};