#include "Game/Tile.hpp"
#include "Engine/Math/AABB2.hpp"

Tile::~Tile()
{

}

Tile::Tile()
{

}

Tile::Tile(IntVector2 tileCoordinates, TileType type)
{
	m_tileCoordinates = tileCoordinates;
	m_type = type;
}

AABB2 Tile::GetAABB2()
{
	Vector2 min = Vector2((float)(m_tileCoordinates.x -1.f),(float)(m_tileCoordinates.y -1.f));
	Vector2 max = Vector2((float)(m_tileCoordinates.x),(float)(m_tileCoordinates.y));

	return AABB2(min,max);
}
