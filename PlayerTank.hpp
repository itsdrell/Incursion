#pragma once
#include "Game/Entity.hpp"

class PlayerTank: public Entity
{
public:
	virtual ~PlayerTank();
	PlayerTank();
	
	
	virtual void Update(float deltaSeconds);
	virtual void Render();

	void PlayerInput(float deltaSeconds);
	void MovePlayer(float strength,float time);
	void UpdateDirection(float deltaSeconds);
	void UpdateTurretDirection(float deltaSeconds, float goalDirection);
	virtual void Respawn();
public:
	std::string m_turretTexture;
	float m_turretOrientation;
	float m_turretGoalOrientation;
	float m_fireCooldown;
	float m_vibrateAmount = 0.f;

	float m_speedAmount;
};