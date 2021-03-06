#ifndef UI_MAIN_MENU_H
#define UI_MAIN_MENU_H

#include "../UserInterface.h"
#include "../Elements/Button.h"
#include "../Elements/CheckBox.h"
#include "../Elements/Slider.h"
#include "../Elements/ButtonPromtBar.h"

class UIMainMenu : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<Button> buttonPlay;
	std::unique_ptr<Button> buttonOptions;
	std::unique_ptr<Button> buttonCredits;
	std::unique_ptr<Button> buttonHighscore;
	std::unique_ptr<Button> buttonQuit;
	std::unique_ptr<ButtonPromptBar> promptBar;
	Element* selected;

public:
	UIMainMenu();
	virtual ~UIMainMenu();

	void init();
	virtual void unloadTextures();

	
};

#endif // !UI_MAIN_MENU_H
