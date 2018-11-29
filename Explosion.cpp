#include "Game/Explosion.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"



Explosion::~Explosion()
{

}

Explosion::Explosion(Vector2 position, ExplosionSize sizeOfExplosion)
{
	
	m_position = position;
	m_type = EXPLOSION;
	
	switch (sizeOfExplosion)
	{
	case Small:
		m_scale = .2f;
		m_size = AABB2(Vector2(m_position.x - m_scale, m_position.y - m_scale), Vector2(m_position.x + m_scale, m_position.y + m_scale));
		m_explosionAnimLength = 1.5f;
		break;
	case Medium:
		m_scale = .4f;
		m_size = AABB2(Vector2(m_position.x - m_scale, m_position.y - m_scale), Vector2(m_position.x + m_scale, m_position.y + m_scale));
		m_explosionAnimLength = 2.f;
		break;
	case Large:
		m_scale = 1.f;
		m_size = AABB2(Vector2(m_position.x - m_scale, m_position.y - m_scale), Vector2(m_position.x + m_scale, m_position.y + m_scale));
		m_explosionAnimLength = 3.f;
		break;
	case ExtraLarge:
		m_scale = 2.f;
		m_size = AABB2(Vector2(m_position.x - m_scale, m_position.y - m_scale), Vector2(m_position.x + m_scale, m_position.y + m_scale));
		m_explosionAnimLength = 3.f;
		break;
	default:
		break;
	}

	

	Texture* explosionTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Explosion_5x5.png");
	SpriteSheet* explosionSheet = new SpriteSheet(explosionTexture,5,5);
	m_explosionAnimation = new SpriteAnimation(*explosionSheet,m_explosionAnimLength,SPRITE_ANIM_MODE_PLAY_TO_END,0,25);
	
}

void Explosion::Update(float deltaSeconds)
{
	m_explosionAnimation->Update(deltaSeconds);

	if(m_explosionAnimation->IsFinished() == true)
	{
		m_isDead = true;
	}
}

void Explosion::Render()
{
	Vector2 texMins = m_explosionAnimation->GetCurrentTexCoords().mins;
	Vector2 textMaxs = m_explosionAnimation->GetCurrentTexCoords().maxs;

	g_theRenderer->AdditiveBlend();
	g_theRenderer->DrawTexturedAABB2(m_size,*m_explosionAnimation->GetTexture(),texMins,textMaxs,Rgba(255,255,255,255));
	g_theRenderer->RestoreAdditiveBlend();
}
