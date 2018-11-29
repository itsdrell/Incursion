#pragma once

class App
{

public:
	App();
	~App();
	void RunFrame(); // this tells everyone else to do a frame
	void Render();
	void Update();

	bool m_isQuitting;
	
};

extern App* g_theApp;
