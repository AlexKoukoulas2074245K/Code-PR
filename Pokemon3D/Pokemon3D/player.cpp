#include "player.h"

#include "level.h"
#include "camera.h"
#include "hidmanager.h"

Player::Player(const vec3f& pos, const sptr<Level>& level):
m_pCamera(new Camera(pos)),
m_pLevel(level)
{}

Player::~Player(){}

void Player::computePosition()
{
	m_levelCoords = m_pLevel->getCoords(m_pCamera->getPosition3());
	m_levelPos = m_pCamera->getPosition3();
}

void Player::update(const sptr<HIDManager> HIDManager)
{
	if (HIDManager->keyTapped(KEY_LEFT)) m_pCamera->turn(Camera::LEFT);
	else if (HIDManager->keyTapped(KEY_RIGHT)) m_pCamera->turn(Camera::RIGHT);
	else if (HIDManager->keyDown(KEY_UP))
	{
		if (
			m_pCamera->getOrientation() == Camera::NORTH &&
			m_pLevel->validCoords(m_levelCoords.x, m_levelCoords.y - 1))
				m_pCamera->moveTo(m_pLevel->getPosition(uint2{m_levelCoords.x, m_levelCoords.y - 1}, 0.0f).z);
	
		else if (
			m_pCamera->getOrientation() == Camera::SOUTH &&
			m_pLevel->validCoords(m_levelCoords.x, m_levelCoords.y + 1))
				m_pCamera->moveTo(m_pLevel->getPosition(uint2{m_levelCoords.x, m_levelCoords.y + 1}, 0.0f).z);
		
		else if (
			m_pCamera->getOrientation() == Camera::WEST &&
		    m_pLevel->validCoords(m_levelCoords.x - 1, m_levelCoords.y))
				m_pCamera->moveTo(-m_pLevel->getPosition(uint2{m_levelCoords.x - 1, m_levelCoords.y}, 0.0f).x);

		else if (
			m_pCamera->getOrientation() == Camera::EAST &&
			m_pLevel->validCoords(m_levelCoords.x + 1, m_levelCoords.y))
				m_pCamera->moveTo(-m_pLevel->getPosition(uint2{m_levelCoords.x + 1, m_levelCoords.y}, 0.0f).x);	
	}
	m_pCamera->update();
	computePosition();
}

uptr<Camera>& Player::modCameraPointer() { return m_pCamera; }