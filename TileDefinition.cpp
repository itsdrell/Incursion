#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


TileDefinition TileDefinition::s_tileDefinition[NUMBER_OF_TILE_TYPES];


void TileDefinition::InitializeTileDefinitions()
{
	Texture* m_tileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png");
	SpriteSheet* m_tileSpriteSheet = new SpriteSheet(m_tileTexture,8,8);
	
	TileDefinition& grass = s_tileDefinition[ GRASS ];
	grass.m_tileType = GRASS;
	grass.m_solid = false;
	grass.m_textureCoordinates = m_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(0,0));

	TileDefinition& stone = s_tileDefinition[ STONE ];
	stone.m_tileType = STONE;
	stone.m_solid = true;
	stone.m_textureCoordinates = m_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(5,6));
	
	TileDefinition& water = s_tileDefinition[ WATER ];
	water.m_tileType = WATER;
	water.m_solid = true;
	water.m_textureCoordinates = m_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(4,7));

	TileDefinition& lava = s_tileDefinition[ LAVA ];
	lava.m_tileType = LAVA;
	lava.m_solid = true;
	lava.m_textureCoordinates = m_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(7,7));

	TileDefinition& diamond = s_tileDefinition[ DIAMOND ];
	diamond.m_tileType = LAVA;
	diamond.m_solid = true;
	diamond.m_textureCoordinates = m_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(6,6));
}


TileDefinition::~TileDefinition()
{

}

// TileDefinition::TileDefinition(TileType type, bool solid, AABB2 spriteCoordinates)
// {
// 	m_solid = solid;
// 	m_tileType = type;
// 	m_textureCoordinates = spriteCoordinates;
// 
// 
// 	// Add it to the list
// 	s_tileDefinition[type] = this;
// }

TileDefinition::TileDefinition()
{
}

bool TileDefinition::IsSolid(TileType typeToCheck)
{
	return s_tileDefinition[typeToCheck].m_solid;
}
