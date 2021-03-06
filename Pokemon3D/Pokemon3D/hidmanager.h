#pragma once
#include <map>

#include "config.h"

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
	static HIDManager& get();
public:
	typedef unsigned int key_t;
	typedef std::map<key_t, key_t> key_dict;

	void updateState(const MSG& msg);
	void completeFrame();

	bool keyDown(const key_t key);
	bool keyUp(const key_t key);
	bool keyTapped(const key_t key);

private:
	HIDManager();
	HIDManager(const HIDManager&) = delete;
	void operator=(const HIDManager&) = delete;

private:
	key_dict m_keyDict;
	key_t m_currState;
	key_t m_prevState;
};