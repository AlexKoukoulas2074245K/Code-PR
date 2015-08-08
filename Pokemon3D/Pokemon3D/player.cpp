#include "player.h"

#include "level.h"
#include "camera.h"
#include "hidmanager.h"

Player::Player(const vec3f& pos, const sptr<Level>& level):
m_pCamera(new Camera(pos)),
m_pLevel(level),
m_canCollide(true)
{}

Player::~Player(){}

void Player::computePosition()
{
	m_levelCoords = m_pLevel->getCoords(m_pCamera->getPosition3());
	m_levelPos = m_pCamera->getPosition3();
}

void Player::update()
{
	if (HIDManager::get().keyTapped(KEY_LEFT)) m_pCamera->turn(Camera::LEFT);
	else if (HIDManager::get().keyTapped(KEY_RIGHT)) m_pCamera->turn(Camera::RIGHT);
	else if (HIDManager::get().keyDown(KEY_UP))
	{
		if (
			m_pCamera->getOrientation() == Camera::NORTH &&
			(m_pLevel->validCoords(m_levelCoords.x, m_levelCoords.y - 1) || !m_canCollide))
				m_pCamera->moveTo(m_pLevel->getPosition(uint2{m_levelCoords.x, m_levelCoords.y - 1}, 0.0f).z);
	
		else if (
			m_pCamera->getOrientation() == Camera::SOUTH &&
			(m_pLevel->validCoords(m_levelCoords.x, m_levelCoords.y + 1) || !m_canCollide))
				m_pCamera->moveTo(m_pLevel->getPosition(uint2{m_levelCoords.x, m_levelCoords.y + 1}, 0.0f).z);
		
		else if (
			m_pCamera->getOrientation() == Camera::WEST &&
		    (m_pLevel->validCoords(m_levelCoords.x - 1, m_levelCoords.y) || !m_canCollide))
				m_pCamera->moveTo(-m_pLevel->getPosition(uint2{m_levelCoords.x - 1, m_levelCoords.y}, 0.0f).x);

		else if (
			m_pCamera->getOrientation() == Camera::EAST &&
			(m_pLevel->validCoords(m_levelCoords.x + 1, m_levelCoords.y) || !m_canCollide))
				m_pCamera->moveTo(-m_pLevel->getPosition(uint2{m_levelCoords.x + 1, m_levelCoords.y}, 0.0f).x);	
	}

	/* Collisions on-off switch */
	if (HIDManager::get().keyTapped(KEY_A) && HIDManager::get().keyTapped(KEY_B)) m_canCollide = !m_canCollide;
	m_pCamera->update();
	computePosition();
}

uptr<Camera>& Player::modCameraPointer() { return m_pCamera; }
bool Player::canCollide() const { return m_canCollide; }