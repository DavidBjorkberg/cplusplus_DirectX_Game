#pragma once
#include "window.h"
#include "graphics.h"
#include"Keyboard.h"
#include"Mouse.h"
#include"ImGui/imgui.h"
//#include"ImGui/imgui_impl_win32.h"
//#include"ImGui/imgui_impl_dx11.h"
#include"vehicle.h"
#include "AI/Actor.h"
class Game {
	Window* window = nullptr;

	std::unique_ptr<DirectX::Mouse> mouse;
	static Graphics graphics;
	GameObject* testObject = nullptr;
	GameObject* testObject2 = nullptr;
	//GameObject* AiTestObject = nullptr;
	AIPlayer * aiObject = nullptr;

	void addQuad(int x);

	float deltaTime = 0.0f;
	__int64 curTime = 0;
	__int64 prevTime = 0;
	__int64 countsPerSec = 0;
	float secPerCount = 1.0f / countsPerSec;
	float timerForParticle;
	Vehicle player;
	Camera camera;
	//TestAI AI;

	//Particles
	float colors[4] = { 1.0f,1.0f,1.0f,1.0f };
	float colors2[4] = { 1.0f,1.0f,1.0f,1.0f };
	float colors3[4] = { 1.0f,1.0f,1.0f,1.0f };
	float colors4[4] = { 1.0f,1.0f,1.0f,1.0f };
	float size1 = 0.02f;
	float size2 = 0.03f;
	int addNrOfParticles = 1.0f;
	int lifeTime = 1.0f;
public:
	Game();
	~Game();
	void init(Window* window);
	void run();
	static Graphics& getGraphics()
	{
		return graphics;
	};
	
};
