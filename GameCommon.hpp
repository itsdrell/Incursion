#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Map.hpp"

class TileDefinition;
class SpriteSheet;
class Texture;

extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_audio; 



struct RayCastResult
{
	bool m_madeContact;
	Vector2 m_contactPosition;
	IntVector2 m_tileCoorOfContact;
	float m_fractionComplete;
	Vector2 m_impactNormal;

	RayCastResult(bool contact, Vector2 contactPos, IntVector2 tileContact, float fractionDone, Vector2 collisionNormal);
};
