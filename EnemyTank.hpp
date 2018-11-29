#pragma once
#include "Game/Entity.hpp"

class IntVector2;
class Vector2;

class EnemyTank: public Entity
{
public:
	virtual ~EnemyTank();
	//EnemyTank();
	EnemyTank(IntVector2 tileCoord);

	virtual void Update(float deltaSeconds);
	virtual void Render();

	void MoveTank(float deltaSeconds);
	void Shoot(float deltaSeconds);
	void RotateTowards(Entity* target,float deltaSeconds);
	void Wander(float deltaSeconds);

private:
	Vector2 GetPositionFromTileCoor(IntVector2 tileCoord);

public:
	bool m_eyesOnTarget;
	float m_shootCooldown;
	float m_goalDirection; // random direction picked
	float m_wanderCooldown;
	float m_speed;

	Vector2 m_lastSeenPosition;
	bool m_lostPlayer;
};