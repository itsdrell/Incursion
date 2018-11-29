#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Game/PlayerTank.hpp"

class Camera
{
public:
	Camera();
	~Camera();
	Camera(PlayerTank *thePlayer);

	void Update();
	void Render();


public:
	Vector2 m_worldPos;
	float m_orientationDegrees;
	float m_numTilesInViewVertically;

	PlayerTank* m_player;
};