#pragma once
#include "Game/Entity.hpp"

class Bullet: public Entity
{
public:
	virtual ~Bullet();
	Bullet(TypesOfFactions typeOfBullet, Vector2 bulletSpawnPosition, Vector2 bulletVecocity, float orientation);
	virtual void Update(float deltaSeconds);
	virtual void Render();
	bool IsPredictedTileSolid(float deltaSeconds) const;
	void BounceOfCollision(float deltaSeconds);

private:
	int DetermineHealth();
	void UpdateOrientation();
};