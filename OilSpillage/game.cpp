#include "game.h"
#include "Input.h"
#include "Sound.h"
#include "UI/UserInterface.h"
#include "Inventory/Container.h"
#include "Inventory/Item.h"
#include "States/MenuGameState.h"
#include "States/PlayingGameState.h"
#include "States/UpgradingGameState.h"
#include "States/HighscoreGameState.h"

std::unique_ptr<Game> Game::instance;

void Game::start(Window* window)
{
	instance = std::make_unique<Game>();
	instance->window = window;
	instance->graphics.init(window);

	Input::init(instance->window);
	Sound::init();
	UserInterface::initStaticVariables();
	Container::playerInventory = std::make_unique<Container>();
	Item::init();

	Sound::load("./data/sound/CarCrash.wav");
	Sound::load("./data/sound/CarGlass1.wav");
	Sound::load("./data/sound/CarGlass2.wav");
	Sound::load("./data/sound/CarGlass3.wav");
	Sound::load("./data/sound/CarGlass4.wav");
	Sound::load("./data/sound/CarImpact1.wav");
	Sound::load("./data/sound/CarImpact2.wav");
	Sound::load("./data/sound/CarImpact3.wav");
	Sound::load("./data/sound/CarImpact4.wav");
	Sound::load("./data/sound/CarImpactSoft.wav");
	Sound::load("./data/sound/FlameLoop1.wav");
	Sound::load("./data/sound/FlameLoop2.wav");
	Sound::load("./data/sound/HitSound.wav");
	Sound::load("./data/sound/MachineGunSound1.wav");
	Sound::load("./data/sound/MachineGunSound2.wav");
	Sound::load("./data/sound/MachineGunSound3.wav");
	Sound::load("./data/sound/MachineGunSound4.wav");
	Sound::load("./data/sound/MachineGunSound5.wav");
	Sound::load("./data/sound/MachineGunSound6.wav");
	Sound::load("./data/sound/MetalImpact1.wav");
	Sound::load("./data/sound/MetalImpact2.wav");
	Sound::load("./data/sound/MetalImpact3.wav");
	Sound::load("./data/sound/MetalImpactPitched1.wav");
	Sound::load("./data/sound/MetalImpactPitched2.wav");
	Sound::load("./data/sound/MetalImpactPitched3.wav");
	
	Sound::load("./data/sound/Drift2.mp3");
	Sound::load("./data/sound/Drift1.mp3");
	Sound::load("./data/sound/CarEngine1.mp3");
	Sound::load("./data/sound/CarEngine2.mp3");
	Sound::load("./data/sound/CarExhaust1.mp3");
	Sound::load("./data/sound/CarExhaust2.mp3");
	Sound::load("./data/sound/CarExhaust3.mp3");
	Sound::load("./data/sound/CarExhaust4.mp3");
	Sound::load("./data/sound/RobotBullet1.mp3");
	Sound::load("./data/sound/RobotBullet2.mp3");
	Sound::load("./data/sound/RobotBullet3.mp3");
	Sound::load("./data/sound/RobotBullet4.mp3");
	Sound::load("./data/sound/RobotBullet5.mp3");
	Sound::load("./data/sound/RobotBullet6.mp3");
	Sound::load("./data/sound/Lazer1.mp3");
	Sound::load("./data/sound/Lazer2.mp3");
	Sound::load("./data/sound/Lazer3.mp3");
	Sound::load("./data/sound/Lazer4.mp3");
	Sound::load("./data/sound/LazerImpact.mp3");


	Sound::playSoundtrack("./data/sound/OilSpillageSoundtrack1_Calm.mp3", "./data/sound/OilSpillageSoundtrack1_Aggressive.mp3");

	//Input::setKeyboardPlayerID(0);
	instance->running = true;
	instance->run();
}

void Game::quit()
{
	instance->running = false;
}

GameState* Game::getCurrentState()
{
	return instance->state.get();
}

int Game::getCurrentStateIndex()
{
	return instance->currentState;
}

void Game::setState(State state)
{
	int newState = static_cast<int>(state);

	if (instance->oldState == -1 && instance->currentState != newState)
	{
		instance->oldState = instance->currentState;
		instance->currentState = newState;
	}
}

Graphics& Game::getGraphics() noexcept
{
	return instance->graphics;
}

float Game::lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

float Game::getDeltaTime()
{
	return instance->deltaTime;
}

float Game::getLocalScale()
{
	return instance->localScale;
}
GameInfo& Game::getGameInfo() noexcept
{
	return instance->gameInfo;
}

bool Game::getDrivingMode()
{
	return instance->drivingMode;
}

void Game::setDrivingMode(bool realistic)
{
	instance->drivingMode = realistic;
}


int Game::getNrOfStagesDone()
{
	return instance->gameInfo.nrOfClearedStages;
}



void Game::createCurrentState()
{
	VehicleSlots* transfer = nullptr;
	VehicleSlots* newSlots = nullptr;

	if (oldState == STATE_PLAYING)
	{
		this->gameInfo.time = static_cast<PlayingGameState*>(state.get())->getTime();
	}
	if (currentState == STATE_MENU)
	{
		Container::playerInventory = std::make_unique<Container>();
		this->gameInfo = GameInfo();
		localScale = 1.0f;
		state->clearDraw();
		state = std::make_unique<MenuGameState>();
		Sound::stopAllLoops();
	}
	else if (currentState == STATE_PLAYING)
	{
		gameInfo.highScoreStage = 0;

		if (oldState == STATE_UPGRADING)
		{
			transfer = static_cast<UpgradingGameState*>(state.get())->getPlayer()->getSlots();
			newSlots = new VehicleSlots(*transfer);

			static_cast<UpgradingGameState*>(state.get())->getPlayer()->stopEngineSound();
			Sound::stopAllLoops();
		}
		
		if (gameInfo.nrOfClearedStages <= 0)
		{
			// TODO: generate seed
			state->clearDraw();

			state = std::make_unique<PlayingGameState>(-1,7.0f*60.0f);
		}
		else
		{
			// TODO: use RNG with seed from above to generate subsequent map seeds
			state->clearDraw();

			state = std::make_unique<PlayingGameState>(-1,gameInfo.time);
		}

		gameInfo.nrOfClearedStages++;
		static_cast<PlayingGameState*>(state.get())->setCurrentMenu(PlayingGameState::MENU_BEFORE_PLAYING);
		static_cast<PlayingGameState*>(state.get())->generateObjectives();

		if (oldState == STATE_UPGRADING)
		{	
			Sound::stopAllLoops();
			static_cast<PlayingGameState*>(state.get())->getPlayer()->startEngineSound();
			static_cast<PlayingGameState*>(state.get())->getPlayer()->setVehicleSlots(newSlots);
		}
	}
	else if (currentState == STATE_UPGRADING)
	{
		graphics.removeAllUIDraw();
		if (oldState == STATE_PLAYING)
		{
			localScale += 0.05f;
			transfer = static_cast<PlayingGameState*>(state.get())->getPlayer()->getSlots();
			newSlots = new VehicleSlots(*transfer);

			static_cast<PlayingGameState*>(state.get())->getPlayer()->stopEngineSound();
			Sound::stopAllLoops();
		}
		else if (oldState == STATE_MENU)
		{
			transfer = static_cast<MenuGameState*>(state.get())->getSlots();
			newSlots = new VehicleSlots(*transfer);
		}
		state->clearDraw();

		state = std::make_unique<UpgradingGameState>();

		if (oldState == STATE_PLAYING)
		{
			Sound::stopAllLoops();
			static_cast<UpgradingGameState*>(state.get())->getPlayer()->setVehicleSlots(newSlots);
		}
		else if (oldState == STATE_MENU)
		{
			static_cast<UpgradingGameState*>(state.get())->getPlayer()->setVehicleSlots(newSlots);
		}
	}
	else if (currentState == STATE_HIGHSCORE)
	{
		state->clearDraw();

		state = std::make_unique<HighscoreGameState>();
	}
}

void Game::run()
{
	//Init the state
	createCurrentState();

	//Store counts per second
	countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	secPerCount = 1.0f / countsPerSec; //store seconds per count

	//Initial previous time	
	prevTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTime);

	while (running && window->update()) {
		curTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
		deltaTime = (curTime - prevTime) * secPerCount;

		Input::update(deltaTime);
		Sound::update(deltaTime);
		state->update(deltaTime);

		if (oldState != -1) {
			graphics.clearDraw();
			
			createCurrentState(); //Init the new state
			curTime = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)& curTime);
			oldState = -1;
		}
		

		//deltaTime reset
		prevTime = curTime;
	}
	Sound::stopAll();
	Sound::deinit();
}

Game::Game() : currentState(STATE_MENU), oldState(-1), running(false) {}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}