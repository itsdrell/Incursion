#include "Game/PlayerTank.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Map.hpp"

PlayerTank::~PlayerTank()
{
	
	
}

PlayerTank::PlayerTank()
{
	
	m_health = 10;
	m_isDead = false;
	m_radius = .5f;
	m_physicsRadius = 0.4f;
	m_faction = GOOD_GUY;
	m_position = Vector2(2.f,2.f);
	m_velocity = Vector2(0.f,0.f);
	m_fireCooldown = 0.f;
	m_type = PLAYER;

	m_currentOrientation = 0.f;
	m_turretOrientation = 90.f;
	m_angularVelocity = 100.f;

	m_texturePath = "Data/Images/PlayerTankBase.png";
	m_turretTexture = "Data/Images/PlayerTankTop.png";
	m_imageBounds = AABB2(Vector2(0.f,0.f), Vector2(1.f,1.f));
	m_speedAmount = 1.f;
}

void PlayerTank::Update(float deltaSeconds)
{
	PlayerInput(deltaSeconds);
	
	Entity::DetermineImageBounds();
	Entity::DetermineIfDead();
}

void PlayerTank::Render()
{
	// Tank Render
	
	g_theRenderer->PushMatrix();
	g_theRenderer->Translatef(m_position);
	g_theRenderer->Rotate2D(m_currentOrientation);
	
	Texture* ourTexture =  g_theRenderer->CreateOrGetTexture(m_texturePath);
	g_theRenderer->DrawTexturedAABB2(ourTexture,  /*m_imageBounds*/AABB2(Vector2(0.f,0.f), 0.5f, 0.5f));
	
	g_theRenderer->PopMatrix();
	
	// Turret Render
	g_theRenderer->PushMatrix();
	g_theRenderer->Translatef(m_position);
	g_theRenderer->Rotate2D(m_turretOrientation);

	ourTexture =  g_theRenderer->CreateOrGetTexture(m_turretTexture);
	g_theRenderer->DrawTexturedAABB2(ourTexture,AABB2(Vector2(0.f,0.f), 0.5f, 0.5f));
	
	g_theRenderer->PopMatrix();
	
	// Debug Render
	Entity::Render();
	
}

void PlayerTank::PlayerInput(float deltaSeconds)
{
	// These are inputs related to the specific game. Could also move some to specific instances
	XboxController currentController = g_theInput->GetController(0);
	
	if(currentController.m_triggers[0].m_fractionPressed > .5f) // Speed up for presentations
	{
		m_speedAmount = 4.f;
	}
	else
	{
		m_speedAmount = 1.f;
	}


	if(currentController.m_buttonStates[ XBOX_BUTTON_A].m_wasJustPressed) // GOD MODE ON AND OFF
	{
		if(m_health <= 10){m_health = 1000;}
		else{m_health = 10;}
	}

	if(currentController.m_buttonStates[XBOX_BUTTON_X].m_wasJustPressed) // Kill player
	{
		m_isDead = true;
	}

	if(currentController.m_joysticks[XBOX_JOYSTICK_LEFT].m_strength > 0.f) // Move Player (thrust)
	{
		UpdateDirection(deltaSeconds);

		if(currentController.m_joysticks[XBOX_JOYSTICK_LEFT].m_strength < 1.f)
			MovePlayer(currentController.m_joysticks[XBOX_JOYSTICK_LEFT].m_strength,deltaSeconds);
		else
		{
			// normal 100% throttle
			MovePlayer(m_speedAmount,deltaSeconds);
		}
		
	}


	if(currentController.m_triggers[1].m_fractionPressed > .5f)
	{
		if(m_fireCooldown <= 0)
		{
			Vector2 velocity = Vector2(CosDegrees(m_turretOrientation),SinDegrees(m_turretOrientation));
			g_theMap->SpawnNewEntity(BULLET,GOOD_GUY,m_position,velocity,m_turretOrientation);
			m_fireCooldown = .1f;
			m_vibrateAmount = 1.f;
			
			g_audio->PlaySound(g_theGame->m_PlayerShootNormalSound);
		}
		else
			m_fireCooldown -= deltaSeconds;
	}



	// Meh turn off vibrate
	/*if(currentController.m_triggers[1].m_fractionPressed <= 0){currentController.VibrateControllerByFloat(0.f);}*/

	if(currentController.m_joysticks[0].m_strength > 0.f)
		m_goalOrientation = currentController.m_joysticks[0].m_orientationDegrees;

	if(currentController.m_joysticks[1].m_strength > 0.f)
		UpdateTurretDirection(deltaSeconds, currentController.m_joysticks[1].m_orientationDegrees);


	// turns on and off vibration
	if(m_vibrateAmount >= 0.f)
	{

		currentController.VibrateControllerByFloat(m_vibrateAmount);
		m_vibrateAmount -= .1f;
	}
	else
	{
		currentController.VibrateControllerByFloat(0.f);
	}

}
	


void PlayerTank::MovePlayer(float strength, float deltaSeconds)
{
	/*float maxSpeed = 1.f;*/
	float speedMultiplier = strength;//RangeMapFloat(strength,0,1,0,1);

	m_velocity = Vector2(speedMultiplier * CosDegrees(m_currentOrientation), speedMultiplier * SinDegrees(m_currentOrientation)) * deltaSeconds;

	m_position += m_velocity;
}

void PlayerTank::UpdateDirection(float deltaSeconds)
{
	m_currentOrientation = TurnToward(m_currentOrientation,m_goalOrientation,m_angularVelocity * deltaSeconds);
	//m_currentOrientation += m_angularVelocity;
}

void PlayerTank::UpdateTurretDirection(float deltaSeconds,float goalDirection)
{
	m_turretOrientation = TurnToward(m_turretOrientation,goalDirection,m_angularVelocity * deltaSeconds);
}

void PlayerTank::Respawn()
{
	m_isDead = false;
	m_health = 10;
	g_theGame->m_GameOverSoundID = (unsigned int)-1;
}

