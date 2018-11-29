#pragma once
#include "Game/Entity.hpp"

class AABB2;
class SpriteAnimation;
class Vector2;

enum ExplosionSize
{
	Small,
	Medium,
	Large,
	ExtraLarge,
	AMOUNT_OF_SIZES
};

class Explosion: public Entity
{
public:	
	~Explosion();
	Explosion(Vector2 position, ExplosionSize sizeOfExplosion);
	
	virtual void Update(float deltaSeconds);
	virtual void Render();

public:
	SpriteAnimation* m_explosionAnimation;
	AABB2 m_size;
	float m_scale;
	float m_explosionAnimLength;
};