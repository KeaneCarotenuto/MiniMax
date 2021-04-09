#pragma once
#include <vector>

#include "CNode.h"
#include "CButton.h"

class CGame {
public:
	//Current Window
	sf::RenderWindow* wind = nullptr;

	//Things to draw
	std::vector<sf::Drawable*> toDraw;

	//List of game states
	enum class GameStates {
		Menu = 0,
		ChooseMode,
		ChooseDifficulty,
		ChooseFirstAI,
		AITurn,
		Player1Turn,
		Player2Turn,
		EndScreen,
		Quit,
	};

	//Is PVP or Player VS AI 
	bool pvp = false;

	//Is Hard or easy AI
	bool hardAI = false;

	//is player going first
	bool pFirst = true;

	//Store the current node of the game
	CNode* startNode = nullptr;

	//How far the program should search (set way higher than needed to be safe)
	int maxDepth = 100;

	//has tree been made
	bool isMade = false;

	//Stop holding click from multiple clicks
	bool frozenClick = false;

	sf::Font font;

	//List of all buttons made
	std::vector<CButton*> buttons;

	//Map of buttons linked to ID strings
	std::map<std::string, CButton*> buttomMap;

	//Current state
	GameStates state = GameStates::Menu;
};