#include "Game/PickupToNextLevel.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Rgba.hpp"

PickupToNextLevel::~PickupToNextLevel()
{

}

PickupToNextLevel::PickupToNextLevel(Vector2 position)
{
	m_position = position;
	m_radius = .1f;
	
	m_texture = g_theRenderer->CreateOrGetTexture("Data/Images/Extras_4x4.png");
	m_spriteSheet = new SpriteSheet(m_texture,4,4);
	m_spriteCoords = m_spriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,3));
}


void PickupToNextLevel::Render()
{
	g_theRenderer->PushMatrix();
	g_theRenderer->Translatef(m_position);

	g_theRenderer->DrawTexturedAABB2(AABB2(Vector2(0.f,0.f), 0.5f, 0.5f),*m_texture,m_spriteCoords.mins,m_spriteCoords.maxs,Rgba(255,255,255,255));
	if(g_theGame->m_debugMode == true)
	{
		g_theRenderer->Color4ub(Rgba(255,0,255,255));
		g_theRenderer->DrawDottedCircle(Vector2(0.f,0.f),m_radius);

	}
	g_theRenderer->PopMatrix();

	
}

