#include "gsqueue.h"

#include "renderer.h"
#include "iomanager.h"
#include "hidmanager.h"
#include "fpscounter.h"
#include "fontengine.h"
#include "gsplay.h"
#include "a_gamestate.h"
#include "colors.h"

GSQueue::GSQueue():
m_pRenderer(new Renderer),
m_pIOManager(new IOManager),
m_pHIDManager(new HIDManager),
m_pFontEngine(new FontEngine("stdfont")),
m_pFpsc(new FpsCounter),
m_isEmpty(false)
{
	LOGLN("GSQueue constructor called"); 
}

GSQueue::~GSQueue()
{
	LOGLN("GSQueue destructor called.");
}

bool GSQueue::initialize(const HWND& hWindow)
{
	/* Initialize the renderer */
	if (!m_pRenderer->initialize(hWindow)) return false;
	m_pIOManager->setRenderer(m_pRenderer);
	m_pFontEngine->prepareFontEngine(m_pIOManager, m_pRenderer);

	/* Add and initialize game states */
	m_states.push_back(value_t(new GSPlay));
	for (cnst_iter iter = m_states.begin();
		 iter != m_states.end();
		 ++iter)
	{
		iter->get()->setRenderer(m_pRenderer);
		iter->get()->setIOManager(m_pIOManager);
		iter->get()->setHIDManager(m_pHIDManager);
		iter->get()->setFontEngine(m_pFontEngine);
		if (!iter->get()->initialize()) return false;
	}

	return true;
}

void GSQueue::handleInput(const MSG& msg)
{
	m_pHIDManager->updateState(msg);
}

void GSQueue::update()
{
	m_states.front()->update();
	if (m_states.front()->isFinished()) 
	{
		m_states.pop_front();
		m_isEmpty = m_states.empty();
	}
	m_pHIDManager->completeFrame();
	m_pFpsc->update();
}

void GSQueue::render()
{
	m_states.front()->render();
	m_pRenderer->renderText("Fps: " + std::to_string(m_pFpsc->getFPS()), {-0.95f, 0.9f}, m_pFontEngine.get(), COLOR_BLACK);
	m_pRenderer->completeFrame();
}

bool GSQueue::isEmpty() const { return m_isEmpty; }
