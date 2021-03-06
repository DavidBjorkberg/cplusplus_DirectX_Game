#ifndef GAME_STATE_H
#define GAME_STATE_H

class GameState
{
public:
	GameState() {};
	virtual ~GameState() {};
	void clearDraw();
	virtual void update(float deltaTime) = 0;
	virtual void unloadTextures();
};

#endif // !GAME_STATE_H
