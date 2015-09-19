#pragma once

#include <map>

#include "common.h"
#include "pokedef.h"
#include "chatbox.h"


class Level;
class Camera;
class Npc;
class OWObject;
class Player
{
public:
	enum PlayerState { IDLE, TALKING, MENU };

public:
	Player(const uint2& levelCoords, sptr<Level>& level);
	~Player();

	void initialize(const std::string gameInfo[GAME_INFO_SIZE]);
	void computePosition();
	void update();
	void render();

	uptr<Camera>& modCameraPointer();
	bool canCollide() const;
	const vec3f& getLevelPos() const;
	const vec3f& getCameraPos() const;
	const vec3f& getCameraForward() const;
	const uint2& getLevelCoords() const;
	const float getCameraRotDy() const;

private:
	void determineMovement();
	void determineAction();

private:
	uptr<Camera> m_pCamera;
	const sptr<Level>& m_pLevel;
	Npc* m_pNpcTalking;
	OWObject* m_pObjTalking;
	ChatBox m_chatBox;

	uint2 m_levelCoords;
	std::map<Orientation, uint2> m_surrCoords;
	vec3f m_levelPos;
	vec3f m_cameraPos;
	
	PlayerState m_currState;
	bool m_canCollide;
	float m_rotdy;
	std::string m_gameInfo[GAME_INFO_SIZE];
};