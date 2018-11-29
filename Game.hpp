#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include <string>

class Map;
class Texture;


enum gameStates
{
	LOADING,
	START_MENU,
	PLAYING,
	PAUSED,
	GAME_OVER,
	GAME_WON,
	NUMBER_OF_STATES
};

class Game
{
public:
	Game();
	void Update(float deltaSeconds);
	void Render();
	void CheckKeyBoardInputs();
	void SwitchPauseState();
	void SwitchLevel();
	void GameOver(float deltaSeconds);
	void LoadAssets();
	void ResetGame();

	// States
	bool m_slowMotionActive;
	bool m_gameIsPaused;
	bool m_debugMode;
	bool m_zoomedOut;

	float m_gameOverTimer;
	bool m_gameOver;
	float m_loadTimer;
	bool m_loaded;

	std::string m_joeyLevel;
	std::string m_theoLevel;
	std::string m_philLevel;
	std::string m_test;

	Map* m_levelOne;
	Map* m_levelTwo;
	Map* m_levelThree;
	Map* m_testLevel;


	gameStates m_currentState;

	Texture* m_loadTexture;
	Texture* m_startTexture;


	// Sounds
	SoundID m_titleScreenMusic;
	SoundPlaybackID m_titleScreenMusicID;

	SoundID m_gameBackgroundMusic;
	SoundPlaybackID m_gameBackgroundMusicID;

	SoundID m_GameOverSound;
	SoundPlaybackID m_GameOverSoundID;

	SoundID m_EnemyDiedSound;
	SoundID m_EnemyHitSound;
	SoundID m_EnemyShootSound;
	SoundID m_PlayerDiedSound;
	SoundID m_MapExitedSound;
	SoundID m_PauseSound;
	SoundID m_PlayerHitSound;
	SoundID m_PlayerShootNormalSound;
	SoundID m_QuitGameSound;
	SoundID m_StartGameSound;
	SoundID m_UpPauseSound;
	SoundID m_VictorySound;
	
};

extern Game* g_theGame;
