#include "Game/EnemyTank.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"


EnemyTank::~EnemyTank()
{

}

EnemyTank::EnemyTank(IntVector2 tileCoord)
{
	m_health = 10;
	m_isDead = false;
	m_radius = .5f;
	m_physicsRadius = 0.4f;
	m_faction = BAD_GUY;
	/*m_position = GetPositionFromTileCoor(tileCoord);*/
	m_position.x = ((float)(tileCoord.x));
	m_position.y = ((float)(tileCoord.y));
	m_velocity = Vector2(0.f,0.f);
	m_type = ENEMY_TANK;
	m_shootCooldown = 0.f;

	m_speed = 1.f;
	m_currentOrientation = GetRandomFloat(0.f,360.f);
	m_angularVelocity = 3.f;
	m_goalDirection = GetRandomFloat(0.f,360.f);

	m_texturePath = "Data/Images/EnemyTank1.png";
	m_imageBounds = AABB2(Vector2(0.f,0.f), Vector2(1.f,1.f)); 

	m_wanderCooldown = 3.f;
	m_eyesOnTarget = false;
	m_lostPlayer = true;

	m_lastSeenPosition = Vector2(0.f,0.f);
}

void EnemyTank::Update(float deltaSeconds)
{

	Entity* player = g_theMap->m_entitiesByType[PLAYER].at(0); // WE CAN FOR LOOP THIS FOR MULTIPLAYER LATER
	//Entity* player = g_theMap->m_entitiesByType[ENEMY_TURRET].at(GetRandomIntLessThan(g_theMap->m_entitiesByType[ENEMY_TURRET].size()));
	
	// Can we see the player
	if(GetDistance(m_position, player->m_position) <= 20.f && player->m_isDead == false) // in distance
	{
		if(g_theMap->HasLineOfSight(m_position, player->m_position)) // clear LOS
		{
			/*RotateTowards(player,deltaSeconds);*/
			m_eyesOnTarget = true; // We can see them
			m_lostPlayer = false; // have not lost them
			m_lastSeenPosition = player->m_position; // set the last seen pos
		}
		else
		{
			m_eyesOnTarget = false;
		}
	}
	else
	{
		m_eyesOnTarget = false;
	}

	//If we can see
	if(m_eyesOnTarget == true)
	{
		RotateTowards(player,deltaSeconds); // move towards player

	}
	else // if we cant see
	{
		if(m_lostPlayer == true) // and know that we checked the last location
		{
			Wander(deltaSeconds); // wander
		}
		else // move towards last seen
		{
			TurnToward(m_currentOrientation, m_lastSeenPosition.GetOrientationDegrees(),m_angularVelocity);
		}
		
	}


	// If we get to the last seen position and we cant see the player then reset
	float distanceFromLastSeen = GetDistance(m_position,m_lastSeenPosition);
	if(distanceFromLastSeen < 1.f) // modify this to set when they give up. Making it bigger makes it easier
	{
		m_lostPlayer = true;
	}



	Entity::DetermineImageBounds();

	MoveTank(deltaSeconds);

	Entity::DetermineIfDead();
	
	


}

void EnemyTank::Render()
{
	// We may be able to just use a general Entity render since this one is 
	// Gonna be like the asteroid (hella simple)
	g_theRenderer->PushMatrix();
	g_theRenderer->Translatef(m_position);
	g_theRenderer->Rotate2D(m_currentOrientation);

	Texture* ourTexture =  g_theRenderer->CreateOrGetTexture(m_texturePath);
	g_theRenderer->DrawTexturedAABB2(ourTexture, AABB2(Vector2(0.f,0.f), 0.5f, 0.5f));

	g_theRenderer->PopMatrix();


	if(g_theGame->m_debugMode == true)
		g_theRenderer->DrawCircle(m_lastSeenPosition,.5f);


	Entity::Render();
}

void EnemyTank::MoveTank(float deltaSeconds)
{
	m_velocity = Vector2(m_speed * CosDegrees(m_currentOrientation), m_speed * SinDegrees(m_currentOrientation)) * deltaSeconds;

	m_position += m_velocity;
}

void EnemyTank::Shoot(float deltaSeconds)
{
	if(m_shootCooldown <= 0)
	{
		Vector2 bulletPosition = Vector2(m_physicsRadius * CosDegrees(m_currentOrientation), m_physicsRadius * SinDegrees(m_currentOrientation));
		bulletPosition+= m_position;
		Vector2 bulletVelocity = Vector2(CosDegrees(m_currentOrientation),SinDegrees(m_currentOrientation));

		g_theMap->SpawnNewEntity(BULLET,BAD_GUY,bulletPosition,bulletVelocity,m_currentOrientation);

		m_shootCooldown = 1.f;

		g_audio->PlaySound(g_theGame->m_EnemyShootSound);
	}
	else
		m_shootCooldown -= deltaSeconds;
}

void EnemyTank::RotateTowards(Entity* target, float deltaSeconds)
{
// 	float xx = target->m_position.x - m_position.x;
// 	float yy = target->m_position.y - m_position.y;
	//Vector2 direction = Vector2(xx,yy);
	Vector2 direction = target->m_position - m_position;

	m_currentOrientation = TurnToward(m_currentOrientation,direction.GetOrientationDegrees(),m_angularVelocity);

	// Check to see if we are within angular range to fire
// 	if(GetAngularDisplacement(m_currentOrientation,direction.GetOrientationDegrees()+180) < 5.f)
// 	{
// 		Shoot(deltaSeconds);
// 	}

	if(DotProduct(direction,m_velocity.GetNormalized()) > .8f)
	{
		Shoot(deltaSeconds);
	}

}

void EnemyTank::Wander(float deltaSeconds)
{
	// Check to see if we need to update goal direction
	if(m_wanderCooldown <= 0)
	{
		m_goalDirection = GetRandomFloat(0.f,360.f);
		m_wanderCooldown = 4.f;
	}
	else
	{
		m_wanderCooldown -= deltaSeconds;
	}

	m_currentOrientation = TurnToward(m_currentOrientation,m_goalDirection,m_angularVelocity);
}

Vector2 EnemyTank::GetPositionFromTileCoor(IntVector2 tileCoord)
{
	float xx = ((float)(tileCoord.x)) - .5f;
	float yy = ((float)(tileCoord.y)) - .5f;

	return Vector2(xx,yy);
}

