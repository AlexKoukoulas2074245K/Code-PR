#include "hidmanager.h"

#define VK_Z 0x5A
#define VK_X 0x58

HIDManager::HIDManager():
mCurrState(0), mPrevState(0)
{
	mKeyDict[VK_LEFT] = KEY_LEFT;
	mKeyDict[VK_RIGHT] = KEY_RIGHT;
	mKeyDict[VK_UP] = KEY_UP;
	mKeyDict[VK_DOWN] = KEY_DOWN;
	mKeyDict[VK_Z] = KEY_A;
	mKeyDict[VK_X] = KEY_B;
	mKeyDict[VK_RETURN] = KEY_START;
	mKeyDict[VK_BACK] = KEY_SELECT;
}
HIDManager::~HIDManager(){}

void HIDManager::UpdateState(const MSG& msg)
{
	if (!mKeyDict.count(msg.wParam)) return;
	if (msg.message == WM_KEYDOWN) mCurrState |= mKeyDict[msg.wParam];
	else if (msg.message == WM_KEYUP) mCurrState &= ~mKeyDict[msg.wParam];
}

void HIDManager::CompleteFrame(){ mPrevState = mCurrState; }
bool HIDManager::KeyDown(const key_t key){ return (mCurrState & key) != 0; }
bool HIDManager::KeyUp(const key_t key){ return (!(mCurrState & key)) == 0; }
bool HIDManager::KeyTapped(const key_t key){ return (mCurrState & key) != 0 && (!(mPrevState & key)) == 0; }
