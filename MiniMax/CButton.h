//Class for clickable button
#pragma once
#include <SFML/Graphics.hpp>

class CButton
{
public:
	CButton(sf::RectangleShape* _rect, sf::Text* _text, void(*_function)());
	~CButton();
	sf::RectangleShape* rect;
	sf::Text* text;
	void(*function)();

	//Can be clicked, and should be drawn
	bool isEnabled = true;

	//used for tic tac toe tiles to distinguish
	bool isTicTacTile = false;
	int tictacX = 0;
	int tictacY = 0;
};

