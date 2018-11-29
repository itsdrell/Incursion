#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Image.hpp"
#include <vector>


// forward declare since classes are gonna include this to get g_theMap
class Camera;
class PlayerTank;
class Bullet;
class EnemyStationaryTurret;
class EnemyTank;
struct RayCastResult;
class SpriteSheet;
class PickupToNextLevel;


typedef std::vector<Entity*> EntityList;

class Map
{
public: 
	~Map();
	Map();
	Map(IntVector2 dimensions);
	Map(int whatLevelToMake); // If I wanted to do by Index and random generate
	Map(std::string imagePath, int levelOrder); // Generate based off an Image
	
	void Update(float deltaSeconds);
	void Render();
	void UpdateEntities(float deltaSeconds); // So I can seperate and do in order
	void RenderEntities();
	void DrawTileTexture(Tile* tileToDraw, Vector2 mins, Vector2 maxs);
	void RemoveTheDead();

	bool HasLineOfSight(const Vector2& startPosition, const Vector2& endPosition);
	RayCastResult RayCast(const Vector2& startPosition, const Vector2 direction, float maxDistance);
	
	void CollisionManager();
	void CheckCollisionEntityVsTile(Entity* currentEntity);
	void CollisionDiscVsDiscBothMovable(Entity*& firstEntity, Entity*& secondEntity);
	void CollisionDiscVsDiscOneMovable(Entity*& movable, Entity*& nonMovable)const;
	void CollisionBulletVsEntity(Entity*& bullet, Entity*& entity);

	bool isSolid(Tile* tileToCheck);
	AABB2 GetImageAABB2(Tile* tileToCheck);
	Tile* GetTile(IntVector2 directionToCheck);
	IntVector2 GetTileCoordinate(Vector2 position);

	void RemoveFromSpecificList(Entity*& entityToRemove, TypesOfEntities entityType); // all entity lists and specific
	void AddEntityToList(Entity* entityToAdd, EntityList& listToAddTo);
	void AddEntityToMap(Entity* entityToAdd);
	void SpawnNewEntity(TypesOfEntities type,TypesOfFactions faction, Vector2 position, Vector2 velocity, float orientation); // factory
	void CreateInstanceBasedOffColor(Rgba color,int tileIndex);

private:
	void TestLevelOne();

public:
	IntVector2 m_dimensions;
	std::vector<Tile> m_tiles; // by value
	
	EntityList m_entities; // by pointer
	EntityList m_entitiesByType[NUMBER_OF_ENTITY_TYPES];


	PlayerTank* m_player;
	Camera* m_mainCamera;
	PickupToNextLevel* m_toNextLevel;

	Texture* m_tileTexture;
	SpriteSheet* m_tileSpriteSheet;

	Image m_ImageMapBasedOff;

	int m_levelOrder;
	
};

extern Map* g_theMap;