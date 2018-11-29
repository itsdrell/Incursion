#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Explosion.hpp"

Entity::~Entity()
{

}

Entity::Entity()
{

}

void Entity::Update(float deltaSeconds)
{
	m_position += (m_velocity * deltaSeconds * m_speed);
	
}

void Entity::Render()
{
	g_theRenderer->PushMatrix();

	g_theRenderer->Translatef(m_position);
	g_theRenderer->Rotate2D(m_currentOrientation);
	

	if(g_theGame->m_debugMode == true)
	{
		g_theRenderer->Color4ub(Rgba(255,0,255,255));
		g_theRenderer->DrawDottedCircle(Vector2(0.f,0.f),m_radius);

		g_theRenderer->Color4ub(Rgba(0,255,255,255));
		g_theRenderer->DrawDottedCircle(Vector2(0.f,0.f),m_physicsRadius);

	}
	g_theRenderer->PopMatrix();
}

void Entity::DetermineImageBounds()
{
	float minxx = (m_position.x - .5f); /** CosDegrees(m_currentOrientation);*/
	float minyy = (m_position.y - .5f); /** SinDegrees(m_currentOrientation);*/

	float maxxx = (m_position.x + .5f); /** CosDegrees(m_currentOrientation);*/
	float maxyy = (m_position.y + .5f); /** SinDegrees(m_currentOrientation);*/
	
	Vector2 MinBounds = Vector2(minxx, minyy);
	Vector2 MaxBounds = Vector2(maxxx, maxyy);

	m_imageBounds.mins = MinBounds;
	m_imageBounds.maxs = MaxBounds;
}

void Entity::DetermineIfDead()
{
	if(m_health <= 0)
	{
		m_isDead = true;
		
		ExplosionSize typeOfExplosion = Small;

		if(m_type == BULLET){typeOfExplosion = Small;}
		if(m_type == ENEMY_TANK || m_type == ENEMY_TURRET){typeOfExplosion = Large;}
		if(m_type == PLAYER){typeOfExplosion = ExtraLarge;}

		Entity* theExplosion = new Explosion(m_position,typeOfExplosion);

		g_theMap->AddEntityToMap(theExplosion);

		SoundOnDeath();
	}


}

void Entity::Respawn()
{

}

void Entity::SoundOnDeath()
{
	if(m_type == ENEMY_TANK || m_type == ENEMY_TURRET)
	{
		g_audio->PlaySound(g_theGame->m_EnemyDiedSound);
	}

	if(m_type == PLAYER)
	{
		g_audio->PlaySound(g_theGame->m_PlayerDiedSound);
	}
}

