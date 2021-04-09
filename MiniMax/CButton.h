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

	bool isEnabled = true;

	bool isTicTacTile = false;
	int tictacX = 0;
	int tictacY = 0;
};

