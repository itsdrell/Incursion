#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>



class TileDefinition
{
	


public:
	~TileDefinition();
	TileDefinition();
//	TileDefinition(TileType type, bool solid, AABB2 spriteCoordinates);
	bool IsSolid(TileType typeToCheck);

public:
	bool m_solid;
	TileType m_tileType;
	AABB2 m_textureCoordinates;
	
	// Class global...?
	static TileDefinition s_tileDefinition[NUMBER_OF_TILE_TYPES];
	static void InitializeTileDefinitions();
	// If im creating a sprite sheet in game global, i dont really need to store the texture?
	
};