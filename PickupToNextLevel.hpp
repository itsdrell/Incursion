#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"

class Texture;
class SpriteSheet;

class PickupToNextLevel
{

public:
	~PickupToNextLevel();
	PickupToNextLevel(Vector2 position);

	
	void Render();


public:
	Vector2 m_position;
	float m_radius;

	Texture* m_texture;
	SpriteSheet* m_spriteSheet;
	AABB2 m_spriteCoords;
};