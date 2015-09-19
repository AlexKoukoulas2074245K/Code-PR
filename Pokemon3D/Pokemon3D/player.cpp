#include "player.h"

#include "level.h"
#include "camera.h"
#include "npc.h"
#include "hidmanager.h"
#include "owobject.h"

Player::Player(
	const uint2& levelCoords,
	sptr<Level>& level):

m_pLevel(level),
m_canCollide(true),
m_currState(IDLE),
m_levelCoords(levelCoords),
m_levelPos(level->getPosition(m_levelCoords)),
m_pCamera(new Camera())
{
	m_pCamera->setPosition(-m_levelPos.x, m_levelPos.y, m_levelPos.z);
	computePosition();
}
Player::~Player(){}

void Player::initialize(const std::string gameInfo[GAME_INFO_SIZE])
{
	m_gameInfo[0] = gameInfo[0];
	m_gameInfo[1] = gameInfo[1];
	m_chatBox.initialize(m_gameInfo);
}

void Player::computePosition()
{
	m_cameraPos = m_pCamera->getPosition();
	m_levelCoords = m_pLevel->getCoords(m_cameraPos);
	m_levelPos = m_pLevel->getPosition(m_levelCoords);
	m_surrCoords[NORTH] = uint2{m_levelCoords.x, m_levelCoords.y - 1};
	m_surrCoords[EAST] = uint2{m_levelCoords.x + 1, m_levelCoords.y};
	m_surrCoords[SOUTH] = uint2{m_levelCoords.x, m_levelCoords.y + 1};
	m_surrCoords[WEST] = uint2{m_levelCoords.x - 1, m_levelCoords.y };
}

void Player::update()
{
	if (m_currState == IDLE)
	{
		if (HIDManager::get().keyTapped(KEY_LEFT)) m_pCamera->turn(Camera::LEFT);
		else if (HIDManager::get().keyTapped(KEY_RIGHT)) m_pCamera->turn(Camera::RIGHT);
		else if (HIDManager::get().keyDown(KEY_UP)) determineMovement();
		else if (HIDManager::get().keyTapped(KEY_A)) determineAction();
	}
	
	else if(m_currState == TALKING)
	{
		m_chatBox.update();
		if (!m_chatBox.isActive())
		{
			m_currState = IDLE;
			if(m_pNpcTalking) m_pNpcTalking->setTalking(false);
		}
	}

	m_pCamera->update(m_rotdy);
	computePosition();

	/* CHEAT --- Collisions on-off switch */
	if (HIDManager::get().keyTapped(KEY_A) &&
		HIDManager::get().keyTapped(KEY_B)) m_canCollide = !m_canCollide;
	/* ---------------------------------- */
}

void Player::render()
{
	m_chatBox.render();
}

void Player::determineMovement()
{
	Orientation camOri = m_pCamera->getOrientation();

	if (camOri == NORTH || camOri == SOUTH)
	{
		if (m_pLevel->validCoords(m_surrCoords[camOri]) || !m_canCollide)
			m_pCamera->moveTo(m_pLevel->getPosition(m_surrCoords[camOri]).z);
	}
	else
	{
		if (m_pLevel->validCoords(m_surrCoords[camOri]) || !m_canCollide)
			m_pCamera->moveTo(-m_pLevel->getPosition(m_surrCoords[camOri]).x);
	}
}

void Player::determineAction()
{
	Orientation ori = m_pCamera->getOrientation();
	m_pNpcTalking = m_pLevel->getNpcAt(m_surrCoords[ori]);
	m_pObjTalking = m_pLevel->getObjectAt(m_surrCoords[ori]);

	if (m_pNpcTalking)
	{
		if (ori == NORTH) m_pNpcTalking->setOrientation(SOUTH);
		else if (ori == EAST) m_pNpcTalking->setOrientation(WEST);
		else if (ori == SOUTH) m_pNpcTalking->setOrientation(NORTH);
		else m_pNpcTalking->setOrientation(EAST);
		m_pNpcTalking->setTalking(true);
		m_currState = TALKING;
		m_chatBox.startDialog(m_pNpcTalking->getMessage());
	}
	else if (m_pObjTalking)
	{
		m_currState = TALKING;
		m_chatBox.startDialog(m_pObjTalking->getMessage());
	}
}
	
uptr<Camera>& Player::modCameraPointer() { return m_pCamera; }
bool Player::canCollide() const { return m_canCollide; }
const vec3f& Player::getLevelPos() const { return m_levelPos; }
const vec3f& Player::getCameraPos() const { return m_cameraPos; }
const vec3f& Player::getCameraForward() const { return m_pCamera->getForward(); }
const uint2& Player::getLevelCoords() const { return m_levelCoords; }
const float Player::getCameraRotDy() const { return m_rotdy; }
