#pragma once
#include "Game/Entity.hpp"

class IntVector2;
class Vector2;

class EnemyStationaryTurret: public Entity
{
public:
	virtual ~EnemyStationaryTurret();
	EnemyStationaryTurret(IntVector2 tileLocation);

	virtual void Update(float deltaSeconds);
	virtual void Render();
	void DrawLaserLine();

	void Shoot(float deltaSeconds);
	void RotateTowards(Entity* target,float deltaSeconds);


private:
	Vector2 GetPositionFromTileCoor(IntVector2 tileCoord);

public:
	std::string m_cannonTexture;
	float m_shootCooldown;
	bool m_eyesOnTarget;
	float m_turretVisionRange;
};