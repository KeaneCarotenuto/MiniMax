#pragma warning( disable : 26812)

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <limits> 
#include <algorithm>

#include "CButton.h"


using namespace std;

struct BoardState {
	char board[3][3] = {{0,0,0},
						{0,0,0},
						{0,0,0} };
};

struct Node {
	int value = NULL; //Value of current board state
	BoardState state; //Current board state
	
	int level = NULL; //How far along the tree it is

	Node* parent = nullptr; 

	vector<Node*> childs; //all of the possible steps
};

class CGame {
public:
	sf::RenderWindow* wind = nullptr;
	std::vector<sf::Drawable*> toDraw;

	enum class GameStates {
		Menu = 0,
		ChooseMode,
		ChooseDifficulty,
		ChooseFirstAI,
		ChooseFirst,
		AITurn,
		Player1Turn,
		Player2Turn,
		EndScreen,
		Quit,
	};

	bool pvp = false;
	bool hardAI = false;
	bool oFirst = false;

	//Store the current node of the game
	Node* startNode = nullptr;

	//How far the program should search (set way higher than needed to be safe)
	int maxDepth = 100;

	bool isMade = false;
	bool pFirst = true;

	bool frozenClick = false;

	sf::Font font;

	vector<CButton*> buttons;

	map<string, CButton*> buttomMap;

	

	GameStates state = GameStates::Menu;
}game;

void makeNodes(Node* _parent, int currentDepth, bool isX);

int CalcScore(Node* _parent);

int ABPrune(Node* _node, int _depth, int alpha, int beta, bool isMaxi);

bool tryPlace(int x, int y, bool isX);

void UpdateBoard();

void AITurn();

void InitGame();

int FixedUpdate();

void EndGame(string winner);

void EnableGridButtons();

void Draw();

void deleteTree(Node* _node);

void deleteBranch(Node* _node);

void DisableAllButtons();

void CheckButtonsPressed();

void CreateTextButton(void(*function)(), std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float _x, float _y, sf::Color _bgColour, float _padding, bool _isE = true);

void CreateTile(int col, int row, std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float tl_x, float tl_y, float br_x, float br_y, sf::Color _bgColour, bool _isE);


void StartGame() {
	game.state = CGame::GameStates::ChooseMode;

	DisableAllButtons();

	InitGame();
}

void Quit() {
	game.state = CGame::GameStates::Quit;

	DisableAllButtons();
}

void ChoosePVP() {
	DisableAllButtons();
	EnableGridButtons();
	game.pvp = true;
	game.state = CGame::GameStates::Player1Turn;
}

void ChoosePVAI() {
	game.state = CGame::GameStates::ChooseDifficulty;
}

void ChooseEasy() {
	game.hardAI = false;
	game.state = CGame::GameStates::ChooseFirstAI;
}

void ChooseHard() {
	game.hardAI = true;
	game.state = CGame::GameStates::ChooseFirstAI;
}

void ChooseAI() {
	DisableAllButtons();
	EnableGridButtons();
	game.pFirst = false;
	game.state = CGame::GameStates::AITurn;
}

void ChooseYou() {
	DisableAllButtons();
	EnableGridButtons();
	game.pFirst = true;
	game.state = CGame::GameStates::Player1Turn;
}

void Menu() {
	game.state = CGame::GameStates::Menu;
}

int main() {
	srand(time(0));

	//Creating Different Windows
	sf::RenderWindow window(sf::VideoMode(180, 180), "Tic-Tac-Toe - By Keane Carotenuto");
	game.wind = &window;

	ShowWindow(GetConsoleWindow(), SW_MINIMIZE);

	//Manages the FixedUpdate() timing
	float stepTime = 0;
	bool drawn = false;
	sf::Clock clock;

	//FixedUpdate() call rate
	float step = (1.0f / 60.0f);

	if (!game.font.loadFromFile("Roboto.ttf")) std::cout << "Failed to load Roboto\n\n";

	CreateTextButton(nullptr, "Tic-Tac-Toe", 25, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5);
	CreateTextButton(&StartGame, "Play", 25, sf::Color::White, sf::Text::Style::Bold, 0, 35, sf::Color::Color(0, 150, 0), 5);
	CreateTextButton(nullptr, "KeaneCarotenuto", 20, sf::Color::White, sf::Text::Style::Bold, 0, 80, sf::Color::Color(0, 0, 0, 0), 5);
	CreateTextButton(nullptr, "@gmail.com", 20, sf::Color::White, sf::Text::Style::Bold, 0, 100, sf::Color::Color(0, 0, 0, 0), 5);
	CreateTextButton(&Quit, "Quit", 25, sf::Color::White, sf::Text::Style::Bold, 0, 150, sf::Color::Color(150, 0, 0), 5);

	CreateTextButton(nullptr, "Choose Mode", 20, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5, false);
	CreateTextButton(&ChoosePVP, "PVP", 25, sf::Color::White, sf::Text::Style::Bold, 0, 35, sf::Color::Color(150, 150, 0), 5, false);
	CreateTextButton(&ChoosePVAI, "PVAI", 25, sf::Color::White, sf::Text::Style::Bold, 0, 70, sf::Color::Color(0, 150, 150), 5, false);

	CreateTextButton(nullptr, "Choose Starting", 20, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5, false);
	CreateTextButton(&ChooseAI, "AI", 25, sf::Color::White, sf::Text::Style::Bold, 0, 35, sf::Color::Color(150, 150, 0), 5, false);
	CreateTextButton(&ChooseYou, "You", 25, sf::Color::White, sf::Text::Style::Bold, 0, 70, sf::Color::Color(0, 150, 0), 5, false);

	CreateTextButton(nullptr, "Choose Difficulty", 20, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5, false);
	CreateTextButton(&ChooseEasy, "Easy", 25, sf::Color::White, sf::Text::Style::Bold, 0, 35, sf::Color::Color(0, 150, 0), 5, false);
	CreateTextButton(&ChooseHard, "Hard", 25, sf::Color::White, sf::Text::Style::Bold, 0, 70, sf::Color::Color(150, 0, 0), 5, false);

	CreateTextButton(nullptr, "Winner", 25, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5, false);
	CreateTextButton(&Menu, "Menu", 25, sf::Color::White, sf::Text::Style::Bold, 0, 35, sf::Color::Color(0, 150, 0), 5, false);
	

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			
			CreateTile(x, y, to_string(x) + to_string(y), 25, sf::Color::White, sf::Text::Style::Bold, x * 60, y * 60, x * 60 + 55, y * 60 + 55, sf::Color::White, true);
			game.buttomMap.find(to_string(x) + to_string(y))->second->text->setString("");
		}
	}

	CreateTextButton(nullptr, "Wait", 25, sf::Color::White, sf::Text::Style::Bold, 0, 75, sf::Color::Color(150, 0, 0), 5, false);

	while (window.isOpen() == true)
	{
		stepTime += clock.getElapsedTime().asSeconds();
		clock.restart();

		while (stepTime >= step)
		{
			//Main Loop of Game
			if (FixedUpdate() == 0) return 0;

			stepTime -= step;
			drawn = false;
		}

		//Draws After Updates
		if (drawn)
		{
			//sf::sleep(sf::seconds(0.01f));
		}
		else
		{
			Draw();

			drawn = true;
		}

		CheckButtonsPressed();

		sf::Event newEvent;

		//Quit
		while (window.pollEvent(newEvent))
		{
			if (newEvent.type == sf::Event::Closed)
			{
				window.close();
			}
		}
	}

	return 0;

	

	while (true) {

		
	}
	system("pause");

	return 0;
}

int FixedUpdate() {
	switch (game.state)
	{
	case CGame::GameStates::Menu:
		deleteTree(game.startNode);
		game.startNode = nullptr;

		game.isMade = false;
		game.startNode = new Node();

		UpdateBoard();

		DisableAllButtons();
		game.buttomMap.find("Tic-Tac-Toe")->second->isEnabled = true;
		game.buttomMap.find("KeaneCarotenuto")->second->isEnabled = true;
		game.buttomMap.find("@gmail.com")->second->isEnabled = true;
		game.buttomMap.find("Play")->second->isEnabled = true;
		game.buttomMap.find("Quit")->second->isEnabled = true;
		break;

	case CGame::GameStates::ChooseMode:
		DisableAllButtons();

		game.buttomMap.find("Choose Mode")->second->isEnabled = true;
		game.buttomMap.find("PVP")->second->isEnabled = true;
		game.buttomMap.find("PVAI")->second->isEnabled = true;

		break;

	case CGame::GameStates::ChooseDifficulty:
		DisableAllButtons();

		game.buttomMap.find("Choose Difficulty")->second->isEnabled = true;
		game.buttomMap.find("Easy")->second->isEnabled = true;
		game.buttomMap.find("Hard")->second->isEnabled = true;
		break;

	case CGame::GameStates::ChooseFirstAI:
		DisableAllButtons();

		game.buttomMap.find("Choose Starting")->second->isEnabled = true;
		game.buttomMap.find("AI")->second->isEnabled = true;
		game.buttomMap.find("You")->second->isEnabled = true;
		break;

	case CGame::GameStates::ChooseFirst:
		
		break;

	case CGame::GameStates::AITurn:

		if (!game.startNode->childs.empty() || !game.isMade) {
			AITurn();

			game.state = CGame::GameStates::Player1Turn;
		}
		else {

			EndGame((game.startNode->value > 50 ? "X" : (game.startNode->value < -50 ? "O" : " ")));
		}
		break;

	case CGame::GameStates::Player1Turn:

		if (!game.startNode->childs.empty() || !game.isMade) {
			UpdateBoard();
		}
		else {

			EndGame((game.startNode->value > 50 ? "X" : (game.startNode->value < -50 ? "O" : " ")));
		}
		break;

	case CGame::GameStates::Player2Turn:
		if (!game.startNode->childs.empty() || !game.isMade) {
			UpdateBoard();
		}
		else {

			EndGame((game.startNode->value > 50 ? "X" : (game.startNode->value < -50 ? "O" : " ")));
		}
		break;

	case CGame::GameStates::Quit:
		return 0;
		break;

	default:
		break;
	}

	return 1;
}

void EndGame(string winner)
{
	game.state = CGame::GameStates::EndScreen;
	DisableAllButtons();
	game.buttomMap.find("Winner")->second->isEnabled = true;

	string toWrite = "e";
	if (game.pvp) {
		if (winner == " ") toWrite = "Tie!";
		if (winner == "X") toWrite = "X Won!";
		if (winner == "O") toWrite = "O Won!";
	}
	else {
		if (winner == " ") toWrite = "Tie!";
		if (winner == "X") toWrite = "CPU Won!";
		if (winner == "O") toWrite = "You Won!";
	}

	game.buttomMap.find("Winner")->second->text->setString(toWrite);
	game.buttomMap.find("Winner")->second->rect->setFillColor(winner == "X" ? sf::Color::Color(150,0,0) : winner == "O" ? sf::Color::Color(0, 0, 150) : sf::Color::Transparent);
	game.buttomMap.find("Winner")->second->text->setOrigin((game.buttomMap.find("Winner")->second->text->getGlobalBounds().width) / 2, 0);
	game.buttomMap.find("Winner")->second->text->setPosition(game.wind->getSize().x / 2, game.buttomMap.find("Winner")->second->text->getPosition().y);

	game.buttomMap.find("Menu")->second->isEnabled = true;
}

void Draw() {
	game.wind->clear();

	for (sf::Drawable* item : game.toDraw)
	{
		game.wind->draw((*item));
	}

	for (CButton* button : game.buttons) {
		if (button->isEnabled) {
			game.wind->draw(*button->rect);
			button->text->setFont(game.font);
			game.wind->draw(*button->text);
		}
	}

	//Update main window
	game.wind->display();

	game.toDraw.clear();

	if (game.wind->getSize() != sf::Vector2u(180, 180)) game.wind->setSize(sf::Vector2u(180, 180));
}

void InitGame()
{
	deleteTree(game.startNode);
	game.startNode = nullptr;

	game.isMade = false;

	game.pvp = false;

	game.hardAI = false;

	game.startNode = new Node();

	for (CButton* _button : game.buttons) {
		if (_button->isTicTacTile) {
			_button->rect->setFillColor(sf::Color::White);
		}
	}
}

void AITurn()
{
	//If not already made, create the tree (doing this after use goes makes it smaller)
	if (!game.isMade) {
		if (!tryPlace(0, 0, true)) {
			int x = 1;
			int y = 1;
			while (!tryPlace(x, y, true)) {
				x = std::rand() % 3;
				y = std::rand() % 3;
			}
		}

		game.buttomMap.find("Wait")->second->isEnabled = true;

		UpdateBoard();
		Draw();

		makeNodes(game.startNode, 1, false);

		game.buttomMap.find("Wait")->second->isEnabled = false;

		return;
	}

	if (game.hardAI) {
		//Minimax + Prune
		ABPrune(game.startNode, game.maxDepth + 1, -10000, 10000, true);

		bool hasChosen = false;
		for (Node* _child : game.startNode->childs) {
			if (_child->value == game.startNode->value) {
				game.startNode = _child;
				hasChosen = true;
				break;
			}
		}
	}
	else {
		while (!tryPlace(rand() % 3, rand() % 3, true));
	}
	

	
}

void UpdateBoard()
{
	//game.wind->clear();

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (game.startNode->state.board[y][x] == NULL) {
				game.buttomMap.find(to_string(x) + to_string(y))->second->text->setString("");
				continue;
			}

			if (game.startNode->state.board[y][x] == 'X') {
				game.buttomMap.find(to_string(x) + to_string(y))->second->text->setString("X");
				game.buttomMap.find(to_string(x) + to_string(y))->second->rect->setFillColor(sf::Color::Color(150, 0, 0));
			}
			else if (game.startNode->state.board[y][x] == 'O') {
				game.buttomMap.find(to_string(x) + to_string(y))->second->text->setString("O");
				game.buttomMap.find(to_string(x) + to_string(y))->second->rect->setFillColor(sf::Color::Color(0, 0, 150));
			}
			

			
		}
	}
}

void makeNodes(Node* _parent, int currentDepth, bool isX) {
	game.isMade = true;
	_parent->level = currentDepth;

	//Calculate value of current node
	_parent->value = CalcScore(_parent);

	//If it isnt a winning board state, make children
	if (_parent->value > -100 && _parent->value < 80) {

		//Check for empty spaces, and make child with that space 
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {

				if (_parent->state.board[y][x] == NULL) {
					//Make the child
					Node* child = new Node();
					child->parent = _parent;
					_parent->childs.push_back(child);

					//Copy board
					for (int _y = 0; _y < 3; _y++)
					{
						for (int _x = 0; _x < 3; _x++)
						{
							child->state.board[_y][_x] = _parent->state.board[_y][_x];
						}
					}

					//Set new position
					child->state.board[y][x] = (isX ? 'X' : 'O');

					//Try make more nodes for this state
					makeNodes(child, currentDepth + 1, !isX);
				}

			}
		}
	}
}

/// <summary>
/// Checks if a node is in a winning state
/// </summary>
/// <param name="_parent">The node to check</param>
/// <returns>-1 = Y wins, 0 = No Wins, 1 = X wins</returns>
int CalcScore(Node* _parent)
{
	//Storing lines
	vector<string> lines;

	//Get all horiz lines
	for (int y = 0; y < 3; y++) {
		string line = "";
		for (int x = 0; x < 3; x++) {
			if (_parent->state.board[y][x] == NULL) continue;
			line += _parent->state.board[y][x];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

	//Get all vert lines
	for (int x = 0; x < 3; x++) {
		string line = "";
		for (int y = 0; y < 3; y++) {
			if (_parent->state.board[y][x] == NULL) continue;
			line += _parent->state.board[y][x];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

	//Just seperate
	if (true) {
		string line = "";

		//get one diag line
		for (int xy = 0; xy < 3; xy++) {
			if (_parent->state.board[xy][xy] == NULL) continue;
			line += _parent->state.board[xy][xy];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

	//Just seperate
	if (true) {
		string line = "";

		//get other diag line
		for (int xy = 0; xy < 3; xy++) {
			if (_parent->state.board[2 - xy][xy] == NULL) continue;
			line += _parent->state.board[2 - xy][xy];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

	//Calculating score of state
	int x2 = 0;
	int x1 = 0;
	int o2 = 0;
	int o1 = 0;

	//Check if winning, else add up score then return
	for (string _line : lines) {
		if (_line == "XXX") return 100 - _parent->level;
		if (_line == "OOO") return -100 - _parent->level;
		if (_line == "XX") x2++;
		else if (_line == "X") x1++;
		else if (_line == "OO") o2++;
		else if (_line == "O") o1++;
	}

	return (3 * x2 + x1 - (3 * o2 + o1));
}

int ABPrune(Node* _node, int _depth, int alpha, int beta, bool isMaxi) {
	//If end node, return value
	if (_depth == 0 || _node->childs.empty()) {
		return _node->value;
	}

	//If is maximiser
	if (isMaxi) {
		//Set really low
		int value = -10000;
		Node* _best = nullptr;

		//For each child
		for (Node* _child : _node->childs) {
			int tempVal = ABPrune(_child, _depth - 1, alpha, beta, false);
			value = max(value, tempVal);
			alpha = max(alpha, value);

			//If the value is the same as the temp value, then that node is the current best node
			if (value == tempVal) {
				_best = _child;
			}

			if (beta <= alpha) {
				break;
			}
		}
		//Return value and best choice is set
		_node->value = value;
		return value;
	}
	else {
		//set really high
		int value = 10000;
		Node* _best = nullptr;

		for (Node* _child : _node->childs) {
			int tempVal = ABPrune(_child, _depth - 1, alpha, beta, true);
			value = min(value, tempVal);
			beta = min(beta, value);

			//If the value is the same as the temp value, then that node is the current best node
			if (value == tempVal) {
				_best = _child;
			}

			if (beta <= alpha) {
				break;
			}
		}
		//Return value and best choice is set
		_node->value = value;
		return value;
	}
}

/// <summary>
/// Try place user mark
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="isX"></param>
bool tryPlace(int x, int y, bool isX) {
	if (game.startNode->state.board[y][x] != NULL || x == NAN || y == NAN || x<0 || x>2 || y<0 || y>2) {
		return false;
	}

	Node* tempNode = nullptr;

	//Check if no children (player may be starting)
	if (game.startNode->childs.empty()) {
		game.startNode->state.board[y][x] = (isX ? 'X' : 'O');
		return true;
	}

	//If the chosen spot is free, search for it in children
	if (game.startNode->state.board[y][x] == NULL) {
		for (Node* _node : game.startNode->childs) {
			if (_node->state.board[y][x] == (isX ? 'X' : 'O')) {
				tempNode = _node;
				break;
			}
		}

		//If the child exists, set that as the new board state
		if (tempNode != nullptr) {
			game.startNode = tempNode;
		}
		else {
			return false;
		}
	}

	return true;
}

void deleteTree(Node* _node) {

	if (_node == nullptr) return;

	Node* currentNode = _node;
	while (currentNode->parent != nullptr) {
		currentNode = currentNode->parent;
	}

	deleteBranch(currentNode);
}

void deleteBranch(Node* _node)
{
	if (_node == nullptr) return;

	for (Node* _child : _node->childs) {
		deleteBranch(_child);
	}

	delete _node;
	_node = nullptr;
}


void DisableAllButtons()
{
	for (CButton* _button : game.buttons)
	{
		_button->isEnabled = false;
	}
}

void EnableGridButtons()
{
	game.buttomMap.find("00")->second->isEnabled = true;
	game.buttomMap.find("01")->second->isEnabled = true;
	game.buttomMap.find("02")->second->isEnabled = true;
	game.buttomMap.find("10")->second->isEnabled = true;
	game.buttomMap.find("11")->second->isEnabled = true;
	game.buttomMap.find("12")->second->isEnabled = true;
	game.buttomMap.find("20")->second->isEnabled = true;
	game.buttomMap.find("21")->second->isEnabled = true;
	game.buttomMap.find("22")->second->isEnabled = true;
}

void CheckButtonsPressed()
{
	//Check Mouse lick
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

		//Prevents multiple clicks if holding down
		if (!game.frozenClick) {

			//Loops through all buttons
			for (CButton* _button : game.buttons)
			{
				if (_button->isEnabled) {
					//If click, do func
					if (_button->rect->getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition(*game.wind))) {
						if (_button->function != nullptr) {
							_button->function();
							break;
						}
						if (_button->isTicTacTile) {
							if (game.pvp) {
								if (game.state == CGame::GameStates::Player1Turn) {
									tryPlace(_button->tictacX, _button->tictacY, false);
									if (!game.isMade) makeNodes(game.startNode, 1, true);
									game.state = CGame::GameStates::Player2Turn;
								}
								else {
									tryPlace(_button->tictacX, _button->tictacY, true);
									if (!game.isMade) makeNodes(game.startNode, 1, true);
									game.state = CGame::GameStates::Player1Turn;
								}
							}
							else {
								tryPlace(_button->tictacX, _button->tictacY, false);
								game.state = CGame::GameStates::AITurn;
							}
							
							break;
						}
					}
				}
			}
		}
		game.frozenClick = true;
	}
	else {
		game.frozenClick = false;
	}
}

/// <summary>
/// Creates A button with given parameters
/// </summary>
void CreateTextButton(void(*function)(), std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float _x, float _y, sf::Color _bgColour, float _padding, bool _isE)
{
	//Text
	sf::Text* tempText = new sf::Text;
	tempText->setString(_string);
	tempText->setCharacterSize(_fontSize);
	tempText->setFillColor(_tColour);
	tempText->setStyle(_style);
	tempText->setFont(game.font);

	//Middle of button
	tempText->setOrigin((tempText->getGlobalBounds().width) / 2, 0);
	tempText->setPosition(game.wind->getSize().x / 2, _y);

	//Button rect
	sf::RectangleShape* buttonRect = new sf::RectangleShape;
	buttonRect->setPosition(0, tempText->getGlobalBounds().top - _padding);
	buttonRect->setSize(sf::Vector2f(180, 30));
	buttonRect->setFillColor(_bgColour);

	//create
	CButton* button = new CButton(buttonRect, tempText, function);
	game.buttons.push_back(button);
	game.buttomMap[_string] = button;
}

void CreateTile(int col, int row, std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float tl_x, float tl_y, float br_x, float br_y, sf::Color _bgColour, bool _isE)
{
	//Text
	sf::Text* tempText = new sf::Text;
	tempText->setString(_string);
	tempText->setCharacterSize(_fontSize);
	tempText->setFillColor(_tColour);
	tempText->setStyle(_style);
	tempText->setFont(game.font);

	//Middle of button
	tempText->setOrigin((tempText->getGlobalBounds().width) / 2, (tempText->getGlobalBounds().height) / 2);
	tempText->setPosition(tl_x + (br_x - tl_x) / 2, tl_y + (br_y - tl_y) / 2);

	//Button rect
	sf::RectangleShape* buttonRect = new sf::RectangleShape;
	buttonRect->setPosition(tl_x, tl_y);
	buttonRect->setSize(sf::Vector2f(br_x - tl_x, br_y - tl_y));
	buttonRect->setFillColor(_bgColour);

	//create
	CButton* button = new CButton(buttonRect, tempText, nullptr);
	button->isTicTacTile = true;
	button->tictacX = col;
	button->tictacY = row;
	game.buttons.push_back(button);
	game.buttomMap[_string] = button;
}