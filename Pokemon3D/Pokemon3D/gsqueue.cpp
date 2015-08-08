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
	if (!Renderer::get().initialize(hWindow)) return false;
	FontEngine::get().prepareFontEngine("stdfont");

	/* Add and initialize game states */
	m_states.push_back(value_t(new GSPlay));
	for (cnst_iter iter = m_states.begin();
		 iter != m_states.end();
		 ++iter)
	{
		if (!iter->get()->initialize()) return false;
	}

	return true;
}

void GSQueue::handleInput(const MSG& msg)
{
	HIDManager::get().updateState(msg);
}

void GSQueue::update()
{
	m_states.front()->update();
	if (m_states.front()->isFinished())
	{
		m_states.pop_front();
		m_isEmpty = m_states.empty();
	}
}

void GSQueue::render()
{
	m_states.front()->render();
	Renderer::get().completeFrame();
	HIDManager::get().completeFrame();
}

bool GSQueue::isEmpty() const { return m_isEmpty; }
