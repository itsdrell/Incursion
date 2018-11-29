#include "Game/GameCommon.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Tile.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_audio = nullptr;

RayCastResult::RayCastResult(bool contact, Vector2 contactPos, IntVector2 tileContact, float fractionDone, Vector2 collisionNormal)
{
	m_madeContact = contact;
	m_contactPosition = contactPos;
	m_tileCoorOfContact = tileContact;
	m_fractionComplete = fractionDone;
	m_impactNormal = collisionNormal;
}


// Creating the spritesheet for the tiles



// Create tile definitions
// TileDefinition* g_grassDefinition = new TileDefinition(GRASS,false,AABB2());
// TileDefinition* g_stoneDefinition = new TileDefinition(STONE, true, AABB2());

