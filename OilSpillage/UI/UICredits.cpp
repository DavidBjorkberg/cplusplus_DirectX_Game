#include "UICredits.h"
#include "..//Input.h"
#include "..//game.h"
#include "..//States/MenuGameState.h"
#include <fstream>

void UICredits::updateUI(float deltaTime)
{
	if (Input::CheckButton(CONFIRM, PRESSED, 0))
	{
		static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(Menu::MENU_MAIN);
	}
}

void UICredits::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	
	for (size_t i = 0; i < this->creditsText.size(); i++)
	{
		Vector2 textSize(UserInterface::getFontArial()->MeasureString(this->creditsText[i].c_str()) * 0.4f);
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->creditsText[i].c_str(), Vector2(SCREEN_WIDTH / 2 - textSize.x / 2, 5 + 80 * 0.4f * i), Colors::White, 0.0f, Vector2(), 0.4f);
	}

	this->buttonBack->draw(true);
	UserInterface::getSpriteBatch()->End();
}

UICredits::UICredits()
{
}

UICredits::~UICredits()
{
}

void UICredits::init()
{
	std::string temp;
	std::ifstream creditsFile("credits.txt");

	while (!creditsFile.eof())
	{
		std::getline(creditsFile, temp);
		this->creditsText.push_back(temp);
	}

	creditsFile.close();

	this->buttonBack = std::make_unique<Button>(Text("Back", Color(Colors::Black), Center), Vector2(SCREEN_WIDTH - Button::size.x - 10, SCREEN_HEIGHT - Button::size.y - 10));
	this->buttonBack->setNeighbours(nullptr, nullptr, nullptr, nullptr);
}