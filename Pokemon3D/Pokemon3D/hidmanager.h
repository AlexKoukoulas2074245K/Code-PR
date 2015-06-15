#pragma once
#include "window.h"
#include <map>

#define KEY_LEFT   0x00000001
#define KEY_RIGHT  0x00000002
#define KEY_UP     0x00000004
#define KEY_DOWN   0x00000008
#define KEY_A      0x00000010
#define KEY_B      0x00000020
#define KEY_SELECT 0x00000040
#define KEY_START  0x00000080

class HIDManager
{
public:
	typedef unsigned int key_t;
	typedef std::map<key_t, key_t> key_dict;

	HIDManager();
	~HIDManager();
	
	void UpdateState(const MSG& msg);
	void CompleteFrame();

	bool KeyDown(const key_t key);
	bool KeyUp(const key_t key);
	bool KeyTapped(const key_t key);

private:
	key_dict mKeyDict;
	key_t mCurrState;
	key_t mPrevState;
};