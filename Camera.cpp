#include "Game/Camera.hpp"
#include "Game/GameCommon.hpp"

Camera::Camera(PlayerTank *thePlayer)
{
	m_player = thePlayer;
	m_worldPos = m_player->m_position;
	m_orientationDegrees = 0.f;
	m_numTilesInViewVertically = 7.f;
}

Camera::Camera()
{

}

Camera::~Camera(){}

void Camera::Update()
{
	m_worldPos = m_player->m_position;

}

void Camera::Render()
{
	Vector2 mins = Vector2(m_worldPos.x - m_numTilesInViewVertically, m_worldPos.y - m_numTilesInViewVertically);
	Vector2 maxs = Vector2(m_worldPos.x + m_numTilesInViewVertically, m_worldPos.y + m_numTilesInViewVertically);
	
	if(g_theGame->m_zoomedOut == true)
	{
		g_theRenderer->LoadIdentity();
		//g_theRenderer->Translatef(m_worldPos);
		g_theRenderer->SetOrtho(Vector2(0.f,0.f),Vector2(50.f,50.f));
	}
	else
	{
		g_theRenderer->LoadIdentity();
		//g_theRenderer->Translatef(m_worldPos);
		g_theRenderer->SetOrtho(mins,maxs);
	}
	
	
	//g_theRenderer->DrawAABB2(AABB2(mins,maxs), Rgba(100.f,100.f,100.f,200.f));
	
	
	
}

