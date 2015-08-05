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
	virtual bool initialize() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

	bool isFinished() const { return m_isFinished; }
	void setRenderer(const sptr<Renderer> pRenderer){ m_pRenderer = pRenderer; }
	void setIOManager(const sptr<IOManager> pIoManager){ m_pIOManager = pIoManager; }
	void setHIDManager(const sptr<HIDManager> pHidManager){ m_pHIDManager = pHidManager; }
	void setFontEngine(const sptr<FontEngine> pFontEngine) { m_pFontEngine = pFontEngine; }

protected:
	bool m_isFinished;
	sptr<Renderer> m_pRenderer;
	sptr<IOManager> m_pIOManager;
	sptr<HIDManager> m_pHIDManager;
	sptr<FontEngine> m_pFontEngine;
};