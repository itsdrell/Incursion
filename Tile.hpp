#pragma once
#include "Engine/Math/IntVector2.hpp"

class AABB2;

enum TileType
{
	GRASS			,
	STONE			,
	WATER			,
	LAVA			,
	DIAMOND			,
	NUMBER_OF_TILE_TYPES	,
};

class Tile
{
public:
	~Tile();
	Tile();
	Tile(IntVector2 tileCoordinates, TileType type);
	AABB2 GetAABB2();

public:
	IntVector2 m_tileCoordinates;
	TileType m_type;
};