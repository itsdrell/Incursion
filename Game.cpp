#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerTank.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <vector>


Game* g_theGame = nullptr;


// Constructor so lets make what we need
Game::Game()
{
	TileDefinition::InitializeTileDefinitions();
	
	m_slowMotionActive = false;
	m_gameIsPaused = false;
	m_debugMode = false;
	m_zoomedOut = false;
	
	
	//m_test = "Data/Images/testMapSource.png";
	m_joeyLevel = "Data/Images/JoeyMap.png";
	m_philLevel = "Data/Images/PhilMap.png";
	m_theoLevel = "Data/Images/ThoeMap.png";

	m_levelOne = new Map(m_joeyLevel,1);
	m_levelTwo = new Map(m_theoLevel,2);
	m_levelThree = new Map(m_philLevel,3);
	
	//g_theMap = new Map(m_theoLevel,2);
	g_theMap = m_levelOne;

	m_gameOverTimer = 0.f;
	m_loadTimer = 0.f;
	m_loaded = false;
	m_gameOver = false;

	m_currentState = LOADING;
}

void Game::Update(float deltaSeconds)
{
	
	if(m_currentState == LOADING)
	{
		LoadAssets();
		
		m_loadTimer += deltaSeconds;
		
		if(m_loadTimer >= 3.f)
		{
			m_currentState = START_MENU;
		}
	}

	if(m_currentState == START_MENU)
	{
		
		if(m_titleScreenMusicID == -1){m_titleScreenMusicID = g_audio->PlaySound(m_titleScreenMusic,true);}
		
		XboxController currentController = g_theInput->GetController(0);
		
		if(currentController.m_buttonStates[XBOX_BUTTON_START].m_wasJustReleased)
		{
			g_audio->StopSound(m_titleScreenMusicID); // STOP MUSIC
			g_audio->PlaySound(m_StartGameSound);
			m_currentState = PLAYING;
		}

		if(currentController.m_buttonStates[XBOX_BUTTON_BACK].m_wasJustPressed || g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_ESCAPE))
		{
			g_theApp->m_isQuitting = true;
		}
		
		return; // So we don't do any code when we switch states
	}
	
	if(m_currentState == PLAYING)
	{
		
		if(m_gameBackgroundMusicID == -1){m_gameBackgroundMusicID = g_audio->PlaySound(m_gameBackgroundMusic,true);}

		CheckKeyBoardInputs();

		//State checks
		if(m_slowMotionActive == true) // Checks to see if we are in slow motion
			deltaSeconds *= .1f;
		if(m_gameIsPaused == true) // Checks to see if we are paused which overrides slowmotion
			deltaSeconds = 0;

		if(g_theMap != nullptr){g_theMap->Update(deltaSeconds);}

		if(g_theMap->m_entitiesByType[PLAYER].at(0)->m_isDead == true)
		{
			GameOver(deltaSeconds);
		}
	}
	
	if(m_currentState == GAME_WON)
	{	
		XboxController currentController = g_theInput->GetController(0);

		if(currentController.m_buttonStates[XBOX_BUTTON_START].m_wasJustReleased)
		{
			
			ResetGame();
			m_currentState = START_MENU;

		}
	}

}

void Game::Render()
{
	
	if(m_currentState == LOADING)
	{
		m_loadTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Loading.png");
		g_theRenderer->DrawTexturedAABB2(m_loadTexture,AABB2(Vector2(0.f,0.f),Vector2(1000.f,1000.f)));
	}

	if(m_currentState == START_MENU)
	{
		g_theRenderer->LoadIdentity();
		//g_theRenderer->Translatef(m_worldPos);
		g_theRenderer->SetOrtho(Vector2(0.f,0.f),Vector2(1000.f,1000.f));
		
		m_startTexture = g_theRenderer->CreateOrGetTexture("Data/Images/titleScreenIncursion.png");
		g_theRenderer->DrawTexturedAABB2(m_startTexture,AABB2(Vector2(0.f,0.f),Vector2(1000.f,1000.f)));
	}


	if(m_currentState == PLAYING)
	{
		if(g_theMap!= nullptr){g_theMap->Render();}

		if(m_gameIsPaused == false && m_gameOver == false && m_zoomedOut == false )
		{
			g_theRenderer->LoadIdentity();
			g_theRenderer->Scalef(.02f);
			
			// hard code translate based off map size
		// Top right
// 			if(g_theMap->m_levelOrder ==1){g_theRenderer->Translatef(Vector2(25.f,25.f));}
// 			if(g_theMap->m_levelOrder == 2){g_theRenderer->Translatef(Vector2(31.f,31.f));}
// 			if(g_theMap->m_levelOrder == 3){g_theRenderer->Translatef(Vector2(18.f,11.f));}
			
		// bottom right
// 			if(g_theMap->m_levelOrder ==1){g_theRenderer->Translatef(Vector2(25.f,-50.f));}
// 			if(g_theMap->m_levelOrder == 2){g_theRenderer->Translatef(Vector2(31.f,-50.f));}
// 			if(g_theMap->m_levelOrder == 3){g_theRenderer->Translatef(Vector2(18.f,-50.f));}
			
		// bottom left
			if(g_theMap->m_levelOrder ==1){g_theRenderer->Translatef(Vector2(-50.f,-50.f));}
			if(g_theMap->m_levelOrder == 2){g_theRenderer->Translatef(Vector2(-49.f,-49.f));}
			if(g_theMap->m_levelOrder == 3){g_theRenderer->Translatef(Vector2(-49.f,-49.f));}
			g_theMap->Render();
			g_theRenderer->PopMatrix();
		}
		


		// pause menu
		if(m_gameIsPaused == true && m_gameOver == false)
		{
			// Center it around the player position
			Entity* player = g_theMap->m_entitiesByType[PLAYER].at(0);
			Vector2 mins = player->m_position - Vector2(2.f,2.f);
			Vector2 maxs = player->m_position + Vector2(2.f,2.f);
			AABB2 drawSpace = AABB2(mins,maxs);

			Texture* pauseMenu = g_theRenderer->CreateOrGetTexture("Data/Images/IncursionPause.png");

			// Draw the background really big so we dont have to worry about position
			Rgba color = Rgba(0,0,0,170);
			AABB2 box = AABB2(Vector2(-100.f,-100.f), Vector2(100.f,100.f));
			g_theRenderer->DrawAABB2(box,color);
			g_theRenderer->DrawTexturedAABB2(pauseMenu,drawSpace);
		}

		if(m_gameOver == true)
		{
			// Center it around the player position
			Entity* player = g_theMap->m_entitiesByType[PLAYER].at(0);
			Vector2 mins = player->m_position - Vector2(2.f,2.f);
			Vector2 maxs = player->m_position + Vector2(2.f,2.f);
			AABB2 drawSpace = AABB2(mins,maxs);

			Texture* pauseMenu = g_theRenderer->CreateOrGetTexture("Data/Images/GameOver.png");

			// Draw the background really big so we dont have to worry about position
			Rgba color = Rgba(0,0,0,170);
			AABB2 box = AABB2(Vector2(-100.f,-100.f), Vector2(100.f,100.f));
			g_theRenderer->DrawAABB2(box,color);
			g_theRenderer->DrawTexturedAABB2(pauseMenu,drawSpace);
		}
	}
	
	if(m_currentState == GAME_WON)
	{
		g_theRenderer->LoadIdentity();
		//g_theRenderer->Translatef(m_worldPos);
		g_theRenderer->SetOrtho(Vector2(0.f,0.f),Vector2(1000.f,1000.f));

		m_startTexture = g_theRenderer->CreateOrGetTexture("Data/Images/VictoryScreen.png");
		g_theRenderer->DrawTexturedAABB2(m_startTexture,AABB2(Vector2(0.f,0.f),Vector2(1000.f,1000.f)));
	}
}


void Game::CheckKeyBoardInputs()
{
	// These are inputs related to the specific game. Could also move some to specific instances
	XboxController currentController = g_theInput->GetController(0);
	

	if(currentController.m_buttonStates[XBOX_BUTTON_BACK].m_wasJustPressed || g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_ESCAPE))
	{
		if(m_gameIsPaused == true)
		{
			ResetGame();
			g_audio->PlaySound(m_QuitGameSound);
			m_currentState = START_MENU;
		}
	}

	if(g_theInput->WasKeyJustReleased(g_theInput->G_THE_LETTER_P) || currentController.m_buttonStates[XBOX_BUTTON_BACK].m_wasJustReleased 
		|| g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_ESCAPE || currentController.m_buttonStates[XBOX_BUTTON_START].m_wasJustReleased)) // pause
	{
		Entity*& player = g_theMap->m_entitiesByType[PLAYER].at(0);


		if(m_gameIsPaused == true && player->m_isDead == true)
		{
			player->Respawn(); // "respawn"
							   //m_gameIsPaused = false;
		}

		if(player->m_isDead == false)
		{
			m_gameIsPaused = !m_gameIsPaused;

			if(m_gameIsPaused == false){g_audio->PlaySound(m_PauseSound);}
			if(m_gameIsPaused == true){g_audio->PlaySound(m_UpPauseSound);}
		}

		if(m_gameOver == true)
		{
			player->Respawn(); // "respawn"
			m_gameIsPaused = false;
			m_gameOver = false;
		}
	}

	if(currentController.m_buttonStates[XBOX_BUTTON_START].m_wasJustReleased) // Respawn after dying
	{
		Entity*& player = g_theMap->m_entitiesByType[PLAYER].at(0);
		
		if(m_gameOver == true)
		{
			player->Respawn(); // "respawn"
			m_gameIsPaused = false;
			m_gameOver = false;
		}

		if(player->m_isDead == false) // START BUTTON NEEDS THIS IDK WHY
		{

			if(m_gameIsPaused == false){g_audio->PlaySound(m_PauseSound);}
			if(m_gameIsPaused == true){g_audio->PlaySound(m_UpPauseSound);}

			//m_gameIsPaused = !m_gameIsPaused;
		}
	}

	if(g_theInput->IsKeyPressed(g_theInput->G_THE_LETTER_T)) // Slow mo on
	{
		m_slowMotionActive = true;
	}

	if(g_theInput->WasKeyJustReleased(g_theInput->G_THE_LETTER_T)) // Slow mo off
	{
		m_slowMotionActive = false;
	}

// 	if(currentController.m_buttonStates[XBOX_BUTTON_BACK].m_isDown)
// 	{
// 		g_theApp->m_isQuitting = true;
// 	}

	if(currentController.m_buttonStates[XBOX_BUTTON_START].m_wasJustPressed)
	{
		Entity*& player = g_theMap->m_entitiesByType[PLAYER].at(0);
		

		if(m_gameIsPaused == true && player->m_isDead == true)
		{
			player->Respawn(); // "respawn"
			//m_gameIsPaused = false;
		}
		
		if(player->m_isDead == false)
		{
			m_gameIsPaused = !m_gameIsPaused;
		}
	}

	if(g_theInput->WasKeyJustReleased(g_theInput->KEYBOARD_F1)) // Slow mo on
	{
		m_debugMode = !m_debugMode;
	}

	if(g_theInput->WasKeyJustReleased(g_theInput->G_THE_LETTER_Z) || currentController.m_buttonStates[XBOX_BUTTON_Y].m_wasJustPressed) // Slow mo on
	{
		m_zoomedOut = !m_zoomedOut;
	}
	
	if(g_theInput->WasKeyJustPressed(g_theInput->G_THE_LETTER_N)|| currentController.m_buttonStates[XBOX_BUTTON_DPAD_RIGHT].m_wasJustPressed)
	{
		SwitchLevel();
	}

}


// This is just a trigger for the pause button
void Game::SwitchPauseState()
{
	m_gameIsPaused = !m_gameIsPaused;
}

void Game::SwitchLevel()
{
	
	if(g_theMap->m_levelOrder == m_levelOne->m_levelOrder)
	{
		g_theMap = m_levelTwo;
		m_levelTwo->m_player->m_health = m_levelOne->m_player->m_health; // Make sure player keeps thier health
		return;
	}

	if(g_theMap->m_levelOrder == m_levelTwo->m_levelOrder)
	{
		g_theMap = m_levelThree;
		m_levelThree->m_player->m_health = m_levelTwo->m_player->m_health;
		return;
	}

	if(g_theMap->m_levelOrder == m_levelThree->m_levelOrder)
	{
		g_theMap = m_levelOne;
		m_levelOne->m_player->m_health = m_levelThree->m_player->m_health;
		return;
	}
}

void Game::GameOver(float deltaSeconds)
{
	if(m_gameOverTimer <= 2.f && m_gameIsPaused == false)
	{
		m_gameOverTimer += deltaSeconds;
	}
	else
	{
		m_gameIsPaused = true;
		m_gameOver = true;
		if(m_GameOverSoundID == -1){m_GameOverSoundID = g_audio->PlaySound(m_GameOverSound);}
		m_gameOverTimer = 0.f;
	}
}

void Game::LoadAssets()
{
	if(m_loaded == false)
	{
		
		m_titleScreenMusic = g_audio->CreateOrGetSound("Data/Audio/zac_attract.wav");
		m_titleScreenMusicID = (unsigned int)-1;

		m_gameBackgroundMusic = g_audio->CreateOrGetSound("Data/Audio/zac_background1.wav");
		m_gameBackgroundMusicID = (unsigned int)-1;

		m_PlayerDiedSound = g_audio->CreateOrGetSound("Data/Audio/PlayerDied.wav");
		m_GameOverSoundID = (unsigned int)-1;

		m_EnemyDiedSound = g_audio->CreateOrGetSound("Data/Audio/EnemyDied.wav");
		m_EnemyHitSound = g_audio->CreateOrGetSound("Data/Audio/EnemyHit.wav");
		m_EnemyShootSound = g_audio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
		m_GameOverSound = g_audio->CreateOrGetSound("Data/Audio/GameOver.mp3");
		m_MapExitedSound = g_audio->CreateOrGetSound("Data/Audio/MapExited.mp3");;
		m_PauseSound = g_audio->CreateOrGetSound("Data/Audio/Pause.mp3");
		m_PlayerHitSound = g_audio->CreateOrGetSound("Data/Audio/PlayerHit.wav");
		m_PlayerShootNormalSound = g_audio->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg");
		m_QuitGameSound = g_audio->CreateOrGetSound("Data/Audio/QuitGame.mp3");
		m_StartGameSound = g_audio->CreateOrGetSound("Data/Audio/StartGame.mp3");
		m_UpPauseSound =  g_audio->CreateOrGetSound("Data/Audio/Unpause.mp3");
		m_VictorySound = g_audio->CreateOrGetSound("Data/Audio/Victory.mp3");

		m_loaded = true;
	}
}

void Game::ResetGame()
{
	delete m_levelOne;
	delete m_levelTwo;
	delete m_levelThree;

	m_levelOne = new Map(m_joeyLevel,1);
	m_levelTwo = new Map(m_theoLevel,2);
	m_levelThree = new Map(m_philLevel,3);

	//g_theMap = new Map(m_theoLevel,2);
	g_theMap = m_levelOne;

	m_gameOverTimer = 0.f;

	m_gameIsPaused = false;
	m_gameOver = false;

	g_audio->StopSound(m_gameBackgroundMusicID);
	m_gameBackgroundMusicID = (unsigned int)-1;
	m_titleScreenMusicID = (unsigned int)-1;
}

