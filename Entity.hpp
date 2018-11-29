#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Game/Game.hpp"

enum TypesOfEntities
{
	PLAYER,
	ENEMY_TANK,
	BULLET,
	ENEMY_TURRET,
	EXPLOSION,
	NUMBER_OF_ENTITY_TYPES
};

enum TypesOfFactions
{
	GOOD_GUY,
	BAD_GUY,
	NUMBER_OF_FACTIONS
};


class Entity
{
public:
	virtual ~Entity();
	Entity();

	virtual void Update(float deltaSeconds);
	virtual void Render();

	virtual void DetermineImageBounds();

	virtual void DetermineIfDead();
	virtual void Respawn();

	void SoundOnDeath();

public:
	Vector2 m_position;
	Vector2 m_velocity;

	int m_health;

	float m_radius;
	float m_physicsRadius;
	float m_angularVelocity;
	float m_currentOrientation;
	float m_goalOrientation;
	float m_speed;

	TypesOfFactions m_faction;
	TypesOfEntities m_type;
	
	std::string m_texturePath;

	AABB2 m_imageBounds;

	bool m_isDead;
};