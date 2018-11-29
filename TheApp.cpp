#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"

App* g_theApp = nullptr;



App::App()
{
	g_theRenderer = new Renderer();
	g_theInput = new InputSystem();
	g_audio = new AudioSystem();
	g_theGame = new Game();

	m_isQuitting = false;
}

App::~App()
{
	delete g_theGame;
	g_theGame = nullptr;

	delete g_audio;
	g_audio = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;

	delete g_theInput;
	g_theInput = nullptr;
}

void App::RunFrame()
{
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_audio->BeginFrame();

	Update();
	Render();

	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
	g_audio->EndFrame();

}

void App::Render()
{

	g_theGame->Render();
}

void App::Update()
{
	//float deltaSeconds = (1.f / 60.f); // temp time

	// Get ds
	static double timeLastUpdate = GetCurrentTimeSeconds();
	double timeNow = GetCurrentTimeSeconds();

	float deltaSeconds = (float)(timeNow - timeLastUpdate);
	timeLastUpdate = timeNow;

	g_theGame->Update(deltaSeconds);


}
