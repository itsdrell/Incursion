#include "Game/Map.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "PlayerTank.hpp"
#include "Game/Bullet.hpp"
#include "Game/EnemyStationaryTurret.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/Camera.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Image.hpp"
#include "Game/Explosion.hpp"
#include "Game/PickupToNextLevel.hpp"
#include "Engine/Math/Disc2.hpp"


const IntVector2 NORTH(0,1);
const IntVector2 SOUTH(0,-1);
const IntVector2 EAST(1,0);
const IntVector2 WEST(-1,0);


Map* g_theMap = nullptr;

Map::~Map()
{

}

Map::Map()
{
	
}

Map::Map(IntVector2 dimensions)
{
	// See how many tiles we are gonna create
	int numberOfTiles = dimensions.x * dimensions.y;

	// for tiled coordinates
	int xx = 1;
	int yy = 1;

	for(int i = 0; i < numberOfTiles-1; i++)
	{
		if(xx > dimensions.x) // move up a row
		{
			xx = 0;
			yy++;
		}

		// Spawn Tile
		if(yy == 1)
		{
			Tile* newTile = new Tile(IntVector2(xx,yy), STONE);
			m_tiles.push_back(*newTile);
			++xx;
			continue;
		}

		Tile* newTile = new Tile(IntVector2(xx,yy), GRASS);
		m_tiles.push_back(*newTile);
		++xx;
		
	}

	
}

Map::Map(int whatLevelToMake)
{
	// Creating spriteSheets here
	m_tileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png");
	m_tileSpriteSheet = new SpriteSheet(m_tileTexture,8,8);


	switch (whatLevelToMake)
	{
	case 1:
		TestLevelOne();
	default:
		break;
	}
	
	// Spawn player on a grass tile
	for(int playerSpawnIndex = 0; playerSpawnIndex < (int)m_tiles.size(); playerSpawnIndex++)
	{
		Tile currentTile = m_tiles.at(playerSpawnIndex);

		if(isSolid(&currentTile) == false)
		{
				m_player = new PlayerTank();
				m_player->m_position = Vector2((float)currentTile.m_tileCoordinates.x,(float)currentTile.m_tileCoordinates.y);
			 	AddEntityToMap(m_player);
			 	m_mainCamera = new Camera(m_player);
				break;
		}
	}

	SpawnNewEntity(ENEMY_TURRET,BAD_GUY,Vector2(10.f,10.f),Vector2(0.f,0.f),0.f);

	int amountOfTurrets = 0;
	int amountOfTanks = 0;

	for(int tileIndex = 0; tileIndex < (int)m_tiles.size(); tileIndex++)
	{
		Tile currentTile = m_tiles.at(tileIndex);

		if(isSolid(&currentTile) == false && CheckRandomChance(7.f) == true)
		{
			Vector2 position = Vector2((float)(currentTile.m_tileCoordinates.x),(float)(currentTile.m_tileCoordinates.y));
			
			bool roll = CheckRandomChance(50.f); // determine what to spawn

			if(roll == true)
			{
				if(amountOfTanks > 0){SpawnNewEntity(ENEMY_TANK,BAD_GUY,position,Vector2(0.f,0.f),0.f);}
				amountOfTanks-= 1;
			}
			else
			{
				if(amountOfTurrets > 0){SpawnNewEntity(ENEMY_TURRET,BAD_GUY,position,Vector2(0.f,0.f),0.f);}
				amountOfTurrets-=1;
			}
		}
	}

}

Map::Map(std::string imagePath, int levelOrder)
{
	// Constructor things
	m_tileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png");
	m_tileSpriteSheet = new SpriteSheet(m_tileTexture,8,8);
	
	// Gonna make the player first and i'll just change it's position once I find its spawn tile
	m_player = new PlayerTank();
	AddEntityToMap(m_player);
	m_mainCamera = new Camera(m_player);
	m_toNextLevel = new PickupToNextLevel(Vector2(5.f,5.f));

	// Making the map
	m_ImageMapBasedOff = Image(imagePath);
	m_levelOrder = levelOrder;

	m_dimensions = m_ImageMapBasedOff.m_dimensions;
	//int amountOfTiles = m_dimensions.x * m_dimensions.y;
	//int tileLocation = 0;

	// Flipped horizontally
// 	for(int amountOfColors = 0; amountOfColors < amountOfTiles; amountOfColors++)
// 	{
// 		CreateInstanceBasedOffColor(m_ImageMapBasedOff.m_colors.at(amountOfColors),amountOfColors);
// 	}
	
	// Flipped vertically
	for(int y = 0; y < m_dimensions.y; y++)
	{
		for(int x = 0; x < m_dimensions.x; x++)
		{
			IntVector2 texelCorrds = IntVector2(x,(m_dimensions.y - y -1));
			CreateInstanceBasedOffColor(m_ImageMapBasedOff.m_colors.at(texelCorrds.x + m_dimensions.x * texelCorrds.y),x + m_dimensions.x * y);
		}
	}


// 	for(int amountOfColors = amountOfTiles-1; amountOfColors >= 0; amountOfColors--)
// 	{
// 		CreateInstanceBasedOffColor(m_ImageMapBasedOff.m_colors.at(amountOfColors),tileLocation);
// 		tileLocation++;
// 	}

	//SpawnNewEntity(EXPLOSION,BAD_GUY,Vector2(5.f,5.f),Vector2(0.f,0.f),0.f);
	
}

void Map::Update(float deltaSeconds)
{
	
	UpdateEntities(deltaSeconds);
	
	CollisionManager();

	m_mainCamera->Update();

	RemoveTheDead();
}

void Map::Render()
{
	
	// Create sprite sheet once since g_therender is not initialized when we create this map :l
// 	if(m_tileTexture == nullptr || m_tileSpriteSheet == nullptr)
// 	{
// 		m_tileTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png");
// 		m_tileSpriteSheet = new SpriteSheet(m_tileTexture,8,8);
// 
// 		// Set Tile coords since this bug is dumb
// 		TileDefinition::s_tileDefinition[GRASS].m_textureCoordinates = m_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(0,0));
// 		TileDefinition::s_tileDefinition[STONE].m_textureCoordinates = m_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(5,6));
// 	}
	
	
	// Do all the gl pops stuff
	g_theRenderer->PushMatrix(); // Saving old graph paper
	g_theRenderer->Translatef(Vector2(0.f,0.f)); // Move onto new paper
	m_mainCamera->Render();
	g_theRenderer->PopMatrix();
	// Draw tiles by using DrawAABB()

	for(int i = 0; i < (int)m_tiles.size(); i++)
	{
		Tile currentTile = m_tiles.at(i);
		Vector2 min = Vector2((float)(currentTile.m_tileCoordinates.x -1.f), (float)(currentTile.m_tileCoordinates.y -1.f));
		Vector2 max = Vector2((float)(currentTile.m_tileCoordinates.x), (float)(currentTile.m_tileCoordinates.y));
		
		DrawTileTexture(&currentTile,min,max);
		// Check what type of color we will need
// 		Rgba color;
// 		if(currentTile.m_type == GRASS)
// 		{
// 			color = Rgba(100,255,100,255);
// 		}
// 		else
// 		{
// 			color = Rgba(128,128,128,255);
// 		}
// 		
// 		g_theRenderer->DrawAABB2(AABB2(min,max),color);

	}
	m_toNextLevel->Render(); // I like the portal to be below everthing
	// Call each entities Render()
	//m_player->Render();
	RenderEntities();
	

	// Debug player tile
// 	Tile* playerTile = GetTile(IntVector2((int)m_player->m_position.x,(int)m_player->m_position.y));
// 	Vector2 min = Vector2((float)(playerTile->m_tileCoordinates.x -1.f), (float)(playerTile->m_tileCoordinates.y -1.f));
// 	Vector2 max = Vector2((float)(playerTile->m_tileCoordinates.x), (float)(playerTile->m_tileCoordinates.y));
// 	AABB2 textureBounds = GetImageAABB2(playerTile);
// 	Rgba tint = Rgba(0,0,0,255);
// 	g_theRenderer->DrawTexturedAABB2(AABB2(min,max),*m_tileSpriteSheet->m_spriteSheetTexture,textureBounds.mins,textureBounds.maxs,tint);
	
	//test
	//m_bullet->Render();
	//testTurret->Render();
	//testTank->Render();
// 	Texture* miniMapTex = g_theRenderer->CreateOrGetTexture(m_ImageMapBasedOff.m_path);
// 	g_theRenderer->DrawTexturedAABB2(miniMapTex,AABB2(Vector2(100.f,100.f), Vector2(200.f,200.f)));
	
}

void Map::UpdateEntities(float deltaSeconds)
{
	for(int i = 0; i < NUMBER_OF_ENTITY_TYPES; i++)
	{
		
		for(Entity* currentEntity: m_entitiesByType[i])
		{
			if(currentEntity->m_isDead == false)
			{
				currentEntity->Update(deltaSeconds);
			}
			
		}

	}
}

void Map::RenderEntities()
{
// 	for(int i = 0; i < NUMBER_OF_ENTITY_TYPES; i++)
// 		{
// 			for(Entity* currentEntity: m_entitiesByType[i])
// 			{
// 				if(currentEntity->m_isDead == false)
// 				{
// 					currentEntity->Render();
// 				}
// 			}
// 		}
	for(int typeIndex = 0; typeIndex < NUMBER_OF_ENTITY_TYPES; typeIndex++)
	{
		for(int EntityIndex = 0; EntityIndex < (int)m_entitiesByType[typeIndex].size(); EntityIndex++)
		{
			Entity*& currentEntity = m_entitiesByType[typeIndex].at(EntityIndex);
			if(currentEntity->m_isDead == false){currentEntity->Render();}
		}
	}

}

void Map::DrawTileTexture(Tile* tileToDraw, Vector2 mins, Vector2 maxs)
{
	AABB2 textureBounds = GetImageAABB2(tileToDraw);
	Rgba tint = Rgba(255,255,255,255);
	g_theRenderer->DrawTexturedAABB2(AABB2(mins,maxs),*m_tileSpriteSheet->m_spriteSheetTexture,textureBounds.mins,textureBounds.maxs,tint);
	return;
}

void Map::RemoveTheDead()
{
	for(int entityIndex = 0; entityIndex < (int)m_entities.size(); entityIndex++)
	{
		Entity*& currentEntity = m_entities.at(entityIndex);

		if(currentEntity->m_isDead == true && currentEntity->m_type != PLAYER)
		{
			// Remove from specific list
			RemoveFromSpecificList(currentEntity,currentEntity->m_type);
			
			// Remove from entity list
			m_entities.erase(m_entities.begin() + (entityIndex)); // by using erase i don't have to worry about nullptr in lists
			
			// Free memory isn't need cause the erase function does that...?
			//delete currentEntity;

		}
	}
}

bool Map::HasLineOfSight(const Vector2& startPosition, const Vector2& endPosition)
{
	float xx = endPosition.x - startPosition.x;
	float yy = endPosition.y - startPosition.y;
	Vector2 directionVector = Vector2(xx,yy); // non normalized doing that v here
	
	RayCastResult checkSight = RayCast(startPosition,directionVector.GetNormalized(),10.f);

	if(checkSight.m_madeContact == true)
	{
		float distanceToContact = GetDistance(startPosition,checkSight.m_contactPosition);
		float distanceToTarget = directionVector.GetLength();

		if(distanceToTarget <= distanceToContact)
		{
			return true;
		}
	}
	


	return false;
}

RayCastResult Map::RayCast(const Vector2& startPosition, const Vector2 direction, float maxDistance) 
{
	int StepsPerTile = 10;
	
	int numberOfSteps = ((int)(maxDistance)) * StepsPerTile; // RAYCAST_STEPS_PER_TILE
	Vector2 singleStep = direction / ((float)(StepsPerTile)); // displacement vector
	IntVector2 previousTileCoord = GetTileCoordinate(startPosition);

	// Check to see if we even need to loop
	Tile* checkTile = GetTile(previousTileCoord);
	
	if(checkTile == nullptr){return RayCastResult(false,Vector2(0.f,0.f),IntVector2(0,0),1.f,Vector2(0.f,0.f));} // out of bounds bug

	if(checkTile->m_type == STONE){return RayCastResult(true,startPosition,previousTileCoord,0.f,direction);}

	for(int stepNumber = 0; stepNumber <= numberOfSteps; stepNumber++)
	{
		Vector2 currentPosition = startPosition + (singleStep * (float)stepNumber);
		IntVector2 currentTilePosition = GetTileCoordinate(currentPosition);

		checkTile = GetTile(currentTilePosition);
		if(checkTile->m_type == STONE)
			return RayCastResult(true,currentPosition,currentTilePosition,(float)(numberOfSteps/stepNumber),direction); // change this direction

		previousTileCoord = currentTilePosition;
	}

	// No collision
	Vector2 endPos = (direction * maxDistance) + startPosition;
	IntVector2 endTileCoords = GetTileCoordinate(endPos);
	return RayCastResult(false,endPos,endTileCoords,1.f,Vector2(0.f,0.f));
}

void Map::CollisionManager()
{
	if(m_player->m_isDead == false)
	{
		if(DoDiscsOverlap(Disc2(m_player->m_position,m_player->m_radius),Disc2(m_toNextLevel->m_position,m_toNextLevel->m_radius)))
		{
			if(m_levelOrder != 3){
				g_audio->PlaySound(g_theGame->m_MapExitedSound);
				g_theGame->SwitchLevel();
			}
			else
			{
				// Victory
				g_audio->PlaySound(g_theGame->m_VictorySound);
				g_theGame->m_currentState = GAME_WON;
			}
		}
	}
	
	// Tile vs Player
	if(m_player->m_isDead == false){CheckCollisionEntityVsTile(m_player);}

	// Tile vs Tank
	for(int i = 0; i < (int)m_entitiesByType[ENEMY_TANK].size(); i++)
	{
		Entity*& currentEntity = m_entitiesByType[ENEMY_TANK].at(i);

		CheckCollisionEntityVsTile(currentEntity);
	}

	// I could make these into seperate functions...?

	// Tank vs Tank
		// Player tank vs enemy tanks
		for(int playerIndex = 0; playerIndex < (int)m_entitiesByType[PLAYER].size(); playerIndex++)
		{
			Entity*& currentPlayer = m_entitiesByType[PLAYER].at(playerIndex);
			
			if(currentPlayer->m_isDead == true){continue;} // dont collide with dead player

			for(int enemyTankIndex = 0; enemyTankIndex < (int)m_entitiesByType[ENEMY_TANK].size(); enemyTankIndex++)
			{
				Entity*& currentTank = m_entitiesByType[ENEMY_TANK].at(enemyTankIndex);

				CollisionDiscVsDiscBothMovable(currentPlayer,currentTank);
			}
		}
		// Enemy Tanks vs enemy tanks
		for(int firstTankIndex = 0; firstTankIndex < (int)m_entitiesByType[ENEMY_TANK].size(); firstTankIndex++)
		{
			Entity*& firstTank = m_entitiesByType[ENEMY_TANK].at(firstTankIndex);

			for(int secondTankIndex = 1; secondTankIndex < (int)m_entitiesByType[ENEMY_TANK].size(); secondTankIndex++)
			{
				Entity*& secondTank = m_entitiesByType[ENEMY_TANK].at(secondTankIndex);

				CollisionDiscVsDiscBothMovable(firstTank,secondTank);
			}
		}


	// Turret vs Tank
		for(int playerIndex = 0; playerIndex < (int)m_entitiesByType[PLAYER].size(); playerIndex++)
		{
			Entity*& currentPlayer = m_entitiesByType[PLAYER].at(playerIndex);

			if(currentPlayer->m_isDead == true){continue;} // dont collide with dead player

			for(int enemyTurretIndex = 0; enemyTurretIndex < (int)m_entitiesByType[ENEMY_TURRET].size(); enemyTurretIndex++)
			{
				Entity*& currentTurret = m_entitiesByType[ENEMY_TURRET].at(enemyTurretIndex);

				CollisionDiscVsDiscOneMovable(currentPlayer,currentTurret);
			}
		}
		// Enemy Tanks vs enemy Turrets
		for(int firstTankIndex = 0; firstTankIndex < (int)m_entitiesByType[ENEMY_TANK].size(); firstTankIndex++)
		{
			Entity*& firstTank = m_entitiesByType[ENEMY_TANK].at(firstTankIndex);

			for(int turretIndex = 1; turretIndex < (int)m_entitiesByType[ENEMY_TURRET].size(); turretIndex++)
			{
				Entity*& currentTurret = m_entitiesByType[ENEMY_TURRET].at(turretIndex);
				
				CollisionDiscVsDiscOneMovable(firstTank,currentTurret);
			}
		}

	// Bullet vs Entity
		for(int bulletIndex = 0; bulletIndex < (int)m_entitiesByType[BULLET].size(); bulletIndex++)
		{
			Entity*& currentBullet = m_entitiesByType[BULLET].at(bulletIndex);

			if(currentBullet->m_faction == BAD_GUY)
			{
				// Bullet vs player
				for(int playerIndex = 0; playerIndex < (int)m_entitiesByType[PLAYER].size(); playerIndex++)
				{
					Entity*& currentPlayer = m_entitiesByType[PLAYER].at(playerIndex);

					CollisionBulletVsEntity(currentBullet,currentPlayer);
				}
			}
			else // its a good guy bullet
			{
				// Bullet vs tanks
				for(int EnemyTankIndex = 0;EnemyTankIndex < (int)m_entitiesByType[ENEMY_TANK].size(); EnemyTankIndex++)
				{
					Entity*& currentEnemyTank = m_entitiesByType[ENEMY_TANK].at(EnemyTankIndex);

					CollisionBulletVsEntity(currentBullet,currentEnemyTank);
				}

				// Bullet vs Turrets
				for(int EnemyTurretIndex = 0; EnemyTurretIndex < (int)m_entitiesByType[ENEMY_TURRET].size(); EnemyTurretIndex++)
				{
					Entity*& currentTurret = m_entitiesByType[ENEMY_TURRET].at(EnemyTurretIndex);

					CollisionBulletVsEntity(currentBullet,currentTurret);
				}
			}
		}

	
}

void Map::CheckCollisionEntityVsTile(Entity* currentEntity)
{
	/*Entity* currentEntity = m_player;*/

	// Make int vector
	IntVector2 playerTilePosition = IntVector2((int)(currentEntity->m_position.x + 1.f), (int)(currentEntity->m_position.y + 1.f));
	// Then find each direction tile corresponding to that one
	
	Tile* northTile = GetTile(playerTilePosition + NORTH); 
	Tile* southTile = GetTile(playerTilePosition + SOUTH); 
	Tile* eastTile = GetTile(playerTilePosition + EAST); 
	Tile* westTile = GetTile(playerTilePosition + WEST); 
	
	
	
	// DO collision checks for each of those tiles
	if(northTile != nullptr && isSolid(northTile) == true){currentEntity->m_position 
		= CorrectiveDiscVsBox(Disc2(currentEntity->m_position, currentEntity->m_physicsRadius),northTile->GetAABB2());}
	
	if(southTile != nullptr && isSolid(southTile) == true){currentEntity->m_position 
		= CorrectiveDiscVsBox(Disc2(currentEntity->m_position, currentEntity->m_physicsRadius),southTile->GetAABB2());}
	
	if(eastTile != nullptr && isSolid(eastTile) == true){currentEntity->m_position 
		= CorrectiveDiscVsBox(Disc2(currentEntity->m_position, currentEntity->m_physicsRadius),eastTile->GetAABB2());}
	
	if(westTile != nullptr && isSolid(westTile) == true){currentEntity->m_position 
		= CorrectiveDiscVsBox(Disc2(currentEntity->m_position, currentEntity->m_physicsRadius),westTile->GetAABB2());}

	Tile* northEastTile = GetTile(playerTilePosition + NORTH + EAST); 
	Tile* southEastTile = GetTile(playerTilePosition + SOUTH + EAST); 
	Tile* northWestTile = GetTile(playerTilePosition + NORTH + WEST); 
	Tile* southWestTile = GetTile(playerTilePosition + SOUTH + WEST);

	if(northEastTile != nullptr && isSolid(northEastTile) == true){
		Disc2 disc = Disc2(currentEntity->m_position, currentEntity->m_physicsRadius);
		Vector2 point = northEastTile->GetAABB2().mins;
		if(disc.IsPointInside(point)) 
			currentEntity->m_position = CorrectiveDiscVsDot(disc, point);
	}

	if(southEastTile != nullptr && isSolid(southEastTile) == true){
		Disc2 disc = Disc2(currentEntity->m_position, currentEntity->m_physicsRadius);
		Vector2 point = Vector2(southEastTile->GetAABB2().mins.x,southEastTile->GetAABB2().maxs.y);
		if(disc.IsPointInside(point)) 
			currentEntity->m_position = CorrectiveDiscVsDot(disc, point);
	}

	if(northWestTile != nullptr && isSolid(northWestTile) == true){
		Disc2 disc = Disc2(currentEntity->m_position, currentEntity->m_physicsRadius);
		Vector2 point = Vector2(northWestTile->GetAABB2().maxs.x,northWestTile->GetAABB2().mins.y);
		if(disc.IsPointInside(point)) 
			currentEntity->m_position = CorrectiveDiscVsDot(disc, point);
	}

	if(southWestTile != nullptr && isSolid(southWestTile) == true){
		Disc2 disc = Disc2(currentEntity->m_position, currentEntity->m_physicsRadius);
		Vector2 point = southWestTile->GetAABB2().maxs;
		if(disc.IsPointInside(point)) 
			currentEntity->m_position = CorrectiveDiscVsDot(disc, point);
	}

}

void Map::CollisionDiscVsDiscBothMovable(Entity*& firstEntity, Entity*& secondEntity)
{
	// Check to see if they even overlap
	if(DoDiscsOverlap(Disc2(firstEntity->m_position, firstEntity->m_physicsRadius),
		Disc2(secondEntity->m_position,secondEntity->m_physicsRadius)) == false){return;}

	// Finding the vector from one to the other
	Vector2 displacementFirstToSecond = (secondEntity->m_position - firstEntity->m_position);
	Vector2 displacementSecondToFirst = (firstEntity->m_position - secondEntity->m_position);

	// Finding the direction from one to the other
	Vector2 directionFirstToSecond = displacementFirstToSecond.GetNormalized();
	Vector2 directionSecondToFirst = displacementSecondToFirst.GetNormalized();

	// Getting the distance once since its the same
	float distance = displacementFirstToSecond.GetLength();
	
	// Get the overlap
	float overlap = (firstEntity->m_physicsRadius + secondEntity->m_physicsRadius) - distance;

	// Create each push back
	Vector2 deltaPositionForFirst = (directionSecondToFirst * overlap) * .5f;
	Vector2 deltaPositionForSecond = (directionFirstToSecond * overlap) * .5f;;

	// Apply push back :)
	firstEntity->m_position += deltaPositionForFirst;
	secondEntity->m_position += deltaPositionForSecond;

	Vector2 temp1 = firstEntity->m_position;
	Vector2 temp2 = secondEntity->m_position;
	
}

void Map::CollisionDiscVsDiscOneMovable(Entity*& movable, Entity*& nonMovable) const
{
	
	if(DoDiscsOverlap(Disc2(movable->m_position, movable->m_physicsRadius),
		Disc2(nonMovable->m_position,nonMovable->m_physicsRadius)) == false){return;}

	// Finding the vector from one to the other
	Vector2 displacementFirstToSecond = (movable->m_position - nonMovable->m_position);

	// Finding the direction from one to the other
	Vector2 directionFirstToSecond = displacementFirstToSecond.GetNormalized();


	// Getting the distance once since its the same
	float distance = displacementFirstToSecond.GetLength();

	// Get the overlap
	float overlap = (movable->m_physicsRadius + nonMovable->m_physicsRadius) - distance;

	// Create each push back
	Vector2 deltaPosition = (directionFirstToSecond * overlap);

	// Apply push back :)
	movable->m_position += deltaPosition;
	
}

void Map::CollisionBulletVsEntity(Entity*& bullet, Entity*& entity)
{
	if(DoDiscsOverlap(Disc2(bullet->m_position, bullet->m_physicsRadius),
		Disc2(entity->m_position,entity->m_physicsRadius)) == false){return;}
	
	// If it doenst return you have collision
	// Do the damage
	bullet->m_isDead = true; // bullet is now marked for destroy
	entity->m_health -= 1;

	if(entity->m_faction == BAD_GUY) // bad guy sound on hit
	{
		g_audio->PlaySound(g_theGame->m_EnemyHitSound);
	}

	if(entity->m_faction == GOOD_GUY) // bad guy sound on hit
	{
		g_audio->PlaySound(g_theGame->m_PlayerHitSound);
	}
}

bool Map::isSolid(Tile* tileToCheck)
{
	return TileDefinition::s_tileDefinition[tileToCheck->m_type].m_solid;
}

AABB2 Map::GetImageAABB2(Tile* tileToCheck)
{
	return TileDefinition::s_tileDefinition[tileToCheck->m_type].m_textureCoordinates;
}

Tile* Map::GetTile(IntVector2 directionToCheck)
{
// 	for(int i = 0; i < (int)m_tiles.size()-1 ; i++)
// 	{
// 		if(m_tiles.at(i).m_tileCoordinates == directionToCheck)
// 		{
// 			return &m_tiles.at(i);
// 		}
// 	}
// 
// 	return nullptr;

	return &m_tiles.at(directionToCheck.x + m_dimensions.x * directionToCheck.y);
}

IntVector2 Map::GetTileCoordinate(Vector2 position)
{
	return IntVector2((int)position.x + 1,(int)position.y + 1);
	//return IntVector2((int)position.x,(int)position.y);
}

void Map::RemoveFromSpecificList(Entity*& entityToRemove, TypesOfEntities entityType)
{
	for(int i = 0; i < (int)m_entitiesByType[entityType].size(); i++)
	{
		Entity*& currentEntity = m_entitiesByType[entityType].at(i);

		if(currentEntity == entityToRemove)
		{
			delete currentEntity; // idk if this actually deletes the entity or just the pointer
			// Doing it here tho because we have to be able to find this entity in the other list before deleting it
			m_entitiesByType[entityType].erase(m_entitiesByType[entityType].begin() + i);
		}
	}
}

void Map::AddEntityToList(Entity* entityToAdd,EntityList& listToAddTo)
{
	// Checks for gaps
// 	for(int i = 0; i < listToAddTo.size(); i++)
// 	{
// 		if(listToAddTo.at(i) == nullptr)
// 		{
// 			listToAddTo.at(i) = entityToAdd;
// 			return;
// 		}
// 
// 		listToAddTo.at(i) = entityToAdd;
// 		return;
// 	}
	
	listToAddTo.push_back(entityToAdd);
	
	return;
}

void Map::AddEntityToMap(Entity* entityToAdd)
{
	AddEntityToList(entityToAdd,m_entities);
	AddEntityToList(entityToAdd, m_entitiesByType[entityToAdd->m_type]);
	
}

void Map::SpawnNewEntity(TypesOfEntities type, TypesOfFactions faction, Vector2 position, Vector2 velocity, float orientation)
{
	Entity* newEntity = nullptr;
	
	// In case we want tile coor
	IntVector2 pos = IntVector2((int)(position.x),(int)(position.y));
	
	switch (type)
	{
	case PLAYER: // not using this for one player atm but maybe for multiplayer
		newEntity = new PlayerTank();
		break;
	case ENEMY_TANK:
		newEntity = new EnemyTank(pos);
		break;
	case BULLET:
		newEntity = new Bullet(faction,position,velocity,orientation);
		break;
	case ENEMY_TURRET:
		newEntity = new EnemyStationaryTurret(pos);
		break;
	case EXPLOSION:
		newEntity = new Explosion(position,ExtraLarge); // this wont work for factory stuff but im using it here for testing
	default:
		break;
	}


	AddEntityToMap(newEntity);
}

void Map::CreateInstanceBasedOffColor(Rgba color, int tileIndex)
{
	int xx = tileIndex % m_dimensions.x;
	int yy = (int)(tileIndex / m_dimensions.x); // So we build down

	if(color == Rgba(0,0,0,255)) // OUT OF BOUNDS FILL
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), DIAMOND);
		m_tiles.push_back(*newTile);
	}

	if(color == Rgba(131,118,156,255)) // WATER
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), WATER);
		m_tiles.push_back(*newTile);
	}

	if(color == Rgba(126,37,83,255)) // LAVA
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), LAVA);
		m_tiles.push_back(*newTile);
	}

	if(color == Rgba(29,43,83,255)) // Solid
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), STONE);
		m_tiles.push_back(*newTile);
	}

	if(color == Rgba(0,135,81,255)) // Non solid
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), GRASS);
		m_tiles.push_back(*newTile);
	}

	if(color == Rgba(255,0,77,255)) // turret
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), GRASS);
		m_tiles.push_back(*newTile);

		SpawnNewEntity(ENEMY_TURRET,BAD_GUY,Vector2((float)xx,(float)yy),Vector2(0.f,0.f),0.f);
	}

	if(color == Rgba(255,119,168,255)) // tank
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), GRASS);
		m_tiles.push_back(*newTile);

		SpawnNewEntity(ENEMY_TANK,BAD_GUY,Vector2((float)xx,(float)yy),Vector2(0.f,0.f),0.f);
	}

	if(color == Rgba(41,173,255,255)) // player
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), GRASS);
		m_tiles.push_back(*newTile);

		m_player->m_position = Vector2((float)xx-.5f,(float)yy-.5f);
	}

	if(color == Rgba(255,241,232,255)) // End Goal
	{
		Tile* newTile = new Tile(IntVector2(xx,yy), GRASS);
		m_tiles.push_back(*newTile);

		// Set portal location
		m_toNextLevel->m_position = Vector2((float)xx-.5f,(float)yy-.5f);
	}
}

void Map::TestLevelOne()
{
	// See how many tiles we are gonna create
	m_dimensions = IntVector2(20,20);
	int numberOfTiles = m_dimensions.x * m_dimensions.y;

	// for tiled coordinates
	int xx = 1;
	int yy = 1;

	for(int i = 0; i < numberOfTiles; i++)
	{
		if(xx >= m_dimensions.x) // move up a row
		{
			xx = 1;
			yy++;
		}

		// Spawn Tiles
		// This is to create a border
		if(yy == 1 || xx == 1 || xx == m_dimensions.x-1 || yy == m_dimensions.y+1)
		{
			Tile* newTile = new Tile(IntVector2(xx,yy), STONE);
			m_tiles.push_back(*newTile);
			++xx;
			continue;
		}

		// The inside part
		bool createWall = CheckRandomChance(10.f); // Check to see if we randomly get a stone tile
		//bool createWall = false; // Check to see if we randomly get a stone tile
												   
		if(createWall == true)
		{
			Tile* newTile = new Tile(IntVector2(xx,yy), STONE);
			m_tiles.push_back(*newTile);
			++xx;
		}
		else
		{
			Tile* newTile = new Tile(IntVector2(xx,yy), GRASS);
			m_tiles.push_back(*newTile);
			++xx;
		}
		
// 		for(int i = 0; i < 10; i++)
// 		{
// 			float ranX = GetRandomFloat(1.f,m_dimensions.x);
// 			float ranY = GetRandomFloat(1.f,m_dimensions.y);
// 			SpawnNewEntity(ENEMY_TURRET,BAD_GUY,Vector2(GetRandomFloat(1.f,m_dimensions.x),GetRandomFloat(1.f,m_dimensions.y)),Vector2(0.f,0.f),0.f);
// 		}
		//SpawnNewEntity(ENEMY_TANK,BAD_GUY,Vector2(GetRandomFloat(1.f,m_dimensions.x),GetRandomFloat(1.f,m_dimensions.y)),Vector2(0.f,0.f),0.f);
	}
}

