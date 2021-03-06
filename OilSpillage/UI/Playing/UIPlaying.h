#ifndef UI_PLAYING_H
#define UI_PLAYING_H

#include "../UserInterface.h"
#include "../Elements/Slider.h"
#include "../Elements/Minimap.h"
#include "../Elements/ObjectiveBox.h"
#include "../Elements/AnimatedText.h"

class UIPlaying : public UserInterface
{
private:
	std::string getFormattedTime();
	void updateUI(float deltaTime);
	void drawUI();

	bool shouldInit;
	float respawnTimer;
	float timerScale;
	float objectiveTimer;

	int lastMinute;
	bool scaleUp;
	float scaleTimer;
	float timeChangeTimer;
	std::unique_ptr<AnimatedText> timeChangeText;
	std::unique_ptr<AnimatedText> timerText;
	std::unique_ptr<AnimatedText> timer;
	std::unique_ptr<AnimatedText> score;
	std::unique_ptr<Slider> healthBar;
	std::unique_ptr<Slider> bossHealthBar;
	std::unique_ptr<Slider> bossShieldBar1;
	std::unique_ptr<Slider> bossShieldBar2;

	std::unique_ptr<Minimap> minimap;
	std::unique_ptr<ObjectiveBox> objectiveBox;
public:
	UIPlaying();
	virtual ~UIPlaying();

	void init();
	void resetMinimapFog();
	bool hasExploredOnMinimap(Vector3 worldPosition) const;
	void addTimeChangeText(float amount);
};

#endif // !UI_PLAYING_H
