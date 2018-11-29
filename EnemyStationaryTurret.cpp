#include "EnemyStationaryTurret.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vector2.hpp"

EnemyStationaryTurret::~EnemyStationaryTurret()
{
	
}

EnemyStationaryTurret::EnemyStationaryTurret(IntVector2 tileLocation)
{
	m_health = 10;
	m_isDead = false;
	m_radius = .5f;
	m_physicsRadius = 0.4f;
	m_faction = BAD_GUY;
	m_position = GetPositionFromTileCoor(tileLocation);
	m_velocity = Vector2(0.f,0.f);
	m_type = ENEMY_TURRET;
	m_shootCooldown = 0.f;
	m_currentOrientation = GetRandomFloat(0.f,360.f);
	m_angularVelocity = 1.f;
	m_goalOrientation = GetRandomFloat(0.f,360.f);
	m_eyesOnTarget = false;
	m_turretVisionRange = 10.f;

	m_texturePath = "Data/Images/EnemyTurretBase.png";
	m_cannonTexture = "Data/Images/EnemyTurretTop.png";
	m_imageBounds = AABB2(Vector2(0.f,0.f), Vector2(1.f,1.f)); // may need a seperate bounds for cannon?
}

void EnemyStationaryTurret::Update(float deltaSeconds)
{
	
	if(g_theGame->m_gameIsPaused == true){return;}

	// AI stuff :O
	Entity* player = g_theMap->m_entitiesByType[PLAYER].at(0); // WE CAN FOR LOOP THIS FOR MULTIPLAYER LATER
	
	if(GetDistance(m_position, player->m_position) <= m_turretVisionRange && player->m_isDead == false) // in distance
	{
		if(g_theMap->HasLineOfSight(m_position, player->m_position)) // clear LOS
		{
			/*RotateTowards(player,deltaSeconds);*/
			m_eyesOnTarget = true;
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


	// Determine what to do based on vision
	if(m_eyesOnTarget == true)
	{
		RotateTowards(player,deltaSeconds);
	}
	else
	{
		m_currentOrientation = TurnToward(m_currentOrientation,m_goalOrientation,m_angularVelocity);
	}

	// pick a new "wander" direction
	if(m_currentOrientation == m_goalOrientation)
	{
		m_goalOrientation -= 181.f;
		
	}


	Entity::DetermineImageBounds();
	Entity::DetermineIfDead();
}

void EnemyStationaryTurret::Render()
{
	Entity::Render();
	
	Texture* ourTexture =  g_theRenderer->CreateOrGetTexture(m_texturePath);
	g_theRenderer->DrawTexturedAABB2(ourTexture, m_imageBounds);

	g_theRenderer->PushMatrix();
	g_theRenderer->Translatef(m_position);
	g_theRenderer->Rotate2D(m_currentOrientation);

	ourTexture =  g_theRenderer->CreateOrGetTexture(m_cannonTexture);
	g_theRenderer->DrawTexturedAABB2(ourTexture,AABB2(Vector2(0.f,0.f), 0.5f, 0.5f));

	g_theRenderer->PopMatrix();
	
	DrawLaserLine();
	
}

void EnemyStationaryTurret::DrawLaserLine()
{
	// Line
	RayCastResult length = g_theMap->RayCast(m_position,Vector2::MakeDirectionAtDegrees(m_currentOrientation),m_turretVisionRange);
	Vector2 endOfLine = length.m_contactPosition;
	// 	float distanceOfLine = GetDistance(m_position,endOfLine);
	// 	float dis = 
	// 	for(int i = 1; i <= 11; i++)
	// 	{
	// 		float currentDistance = i* (distanceOfLine / distanceOfLine);
	// 		g_theRenderer->Color4ub(Rgba(255.f,0.f,0.f,currentDistance));
	// 		g_theRenderer->DrawLine2D(m_position,currentDistance);
	// 	}
	g_theRenderer->Color4ub(Rgba(255,0,0,255));
	g_theRenderer->DrawLine2D(m_position,endOfLine);
	g_theRenderer->Color4ub(Rgba(0,0,0,255));
}

void EnemyStationaryTurret::Shoot(float deltaSeconds)
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

void EnemyStationaryTurret::RotateTowards(Entity* target, float deltaSeconds)
{
	// Rotate towards player
	float xx = target->m_position.x - m_position.x;
	float yy = target->m_position.y - m_position.y;
	Vector2 direction = Vector2(xx,yy);

	
	m_currentOrientation = TurnToward(m_currentOrientation,direction.GetOrientationDegrees(),m_angularVelocity);
	

	// Check to see if we are within angular range to fire
	if(GetAngularDisplacement(m_currentOrientation,direction.GetOrientationDegrees()) < 5.f)
	{
		Shoot(deltaSeconds);
	}

// 	Vector2 direction = target->m_position - m_position;
// 
// 	m_currentOrientation = TurnToward(m_currentOrientation,direction.GetOrientationDegrees(),m_angularVelocity);
// 
// 	if(DotProduct(direction,m_velocity.GetNormalized()) > .8f)
// 	{
// 		Shoot(deltaSeconds);
// 	}
}

Vector2 EnemyStationaryTurret::GetPositionFromTileCoor(IntVector2 tileCoord)
{
	float xx = ((float)(tileCoord.x)) - .5f;
	float yy = ((float)(tileCoord.y)) - .5f;

	return Vector2(xx,yy);
}

