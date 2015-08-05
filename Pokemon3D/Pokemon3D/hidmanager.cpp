#include "hidmanager.h"

#define VK_Z 0x5A
#define VK_X 0x58

HIDManager::HIDManager():
m_currState(0), m_prevState(0)
{
	m_keyDict[VK_LEFT] = KEY_LEFT;
	m_keyDict[VK_RIGHT] = KEY_RIGHT;
	m_keyDict[VK_UP] = KEY_UP;
	m_keyDict[VK_DOWN] = KEY_DOWN;
	m_keyDict[VK_Z] = KEY_A;
	m_keyDict[VK_X] = KEY_B;
	m_keyDict[VK_RETURN] = KEY_START;
	m_keyDict[VK_BACK] = KEY_SELECT;
}
HIDManager::~HIDManager(){}

void HIDManager::updateState(const MSG& msg)
{
	if (!m_keyDict.count(msg.wParam)) return;
	if (msg.message == WM_KEYDOWN) m_currState |= m_keyDict[msg.wParam];
	else if (msg.message == WM_KEYUP) m_currState &= ~m_keyDict[msg.wParam];
}

void HIDManager::completeFrame(){ m_prevState = m_currState; }
bool HIDManager::keyDown(const key_t key){ return (m_currState & key) != 0; }
bool HIDManager::keyUp(const key_t key){ return (!(m_currState & key)) == 0; }
bool HIDManager::keyTapped(const key_t key){ return (m_currState & key) != 0 && (m_prevState & key) == 0; }
