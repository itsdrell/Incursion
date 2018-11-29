#include "Game/Entity.hpp"
#include "Game/Bullet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"

Bullet::~Bullet()
{

}

Bullet::Bullet(TypesOfFactions typeOfBullet, Vector2 bulletSpawnPosition, Vector2 bulletVecocity, float orientation)
{
	m_faction = typeOfBullet;
	m_type = BULLET;
	m_health = DetermineHealth();
	m_isDead = false;
	m_radius = .09f;
	m_physicsRadius = .06f;
	m_currentOrientation = orientation; // We might need this later for shaped bullets
	m_speed = 5.f;
	m_velocity = bulletVecocity;
	m_position = bulletSpawnPosition;

	m_texturePath = "Data/Images/Bullet.png";

	m_imageBounds = AABB2(Vector2(0.f,0.f), Vector2(1.f,1.f));

	//I'm gonna set the values I don't need from entity
	m_goalOrientation = 0.f;
	m_angularVelocity = 0.f;
	

}

void Bullet::Update(float deltaSeconds)
{
	if(IsPredictedTileSolid(deltaSeconds) == false)
	{
		m_position += (m_velocity * deltaSeconds * m_speed);
	}
	else
	{
		BounceOfCollision(deltaSeconds);
		m_health -= 1;
		
	}

	Entity::DetermineIfDead();
	UpdateOrientation();
}

void Bullet::Render()
{
	g_theRenderer->PushMatrix();
	g_theRenderer->Translatef(m_position);
	g_theRenderer->Scalef(.1f);
	g_theRenderer->Rotate2D(m_currentOrientation);

	Texture* ourTexture =  g_theRenderer->CreateOrGetTexture(m_texturePath);
	g_theRenderer->DrawTexturedAABB2(ourTexture,  /*m_imageBounds*/AABB2(Vector2(0.f,0.f), 0.5f, 0.5f));

	g_theRenderer->PopMatrix();
	
	
	Entity::Render(); // debug stuff is here
}

bool Bullet::IsPredictedTileSolid(float deltaSeconds) const
{
	// Get the next frame position
	Vector2 nextPosition = m_position + (m_velocity * deltaSeconds * m_speed);
	
	IntVector2 nextTileCoord = g_theMap->GetTileCoordinate(nextPosition);
	Tile* nextTile = g_theMap->GetTile(nextTileCoord);

	if(nextTile == nullptr){return false;} // out of map
	if(nextTile->m_type == STONE){return true;}

	return false;
}

void Bullet::BounceOfCollision(float deltaSeconds)
{


	
	IntVector2 currentTile = g_theMap->GetTileCoordinate(m_position);
	Vector2 nextPosition = m_position + (m_velocity * m_speed *deltaSeconds);
	IntVector2 nextTile = g_theMap->GetTileCoordinate(nextPosition);

	if(currentTile.x != nextTile.x)
	{
		m_velocity.x *= -1.f;
	}
	else
	{
		m_velocity.y *= -1.f;
	}
	return;

// 	IntVector2 tileNormal = currentTile - nextTile;
// 
// 	float xx = ClampFloat((float)tileNormal.x,-1,1);
// 	float yy =  ClampFloat((float)tileNormal.y,-1,1);
// 
// 
//  	Vector2 normalOfTile = Vector2(xx,yy);
	// Game me a meh bounce
// 	float dotProductOfVectors = DotProduct(m_velocity,normalOfTile);
// 	Vector2 NVector = dotProductOfVectors * normalOfTile;
// 	m_velocity = m_velocity - (2.f * NVector);

	// Hacky kinda off bounce
// 	if(normalOfTile == Vector2(1.f,0.f) || normalOfTile == Vector2(-1.f,0.f))
// 	{
// 		m_velocity.x *= -1;
// 	}
// 	if(normalOfTile == Vector2(0.f,1.f)|| normalOfTile == Vector2(0.f,-1.f))
// 	{
// 		m_velocity.y *= -1;
// 	}
// 
// 	if(normalOfTile == Vector2(-1.f,-1.f) || normalOfTile == Vector2(1.f,1.f) || normalOfTile == Vector2(-1.f,1.f) || normalOfTile == Vector2(1.f,-1.f))
// 	{
// 		m_velocity.x *= -1;
// 		m_velocity.y *= -1;
// 	}
// 
// 	return;
}

int Bullet::DetermineHealth()
{
	if(m_faction == GOOD_GUY)
	{
		return 3;
	}
	else
		return 1;
}

void Bullet::UpdateOrientation()
{
	m_currentOrientation = m_velocity.GetOrientationDegrees();
}

