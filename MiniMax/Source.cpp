//Disable 
#pragma warning( disable : 26812)

//Includes
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
#include "CNode.h"
#include "CGame.h"

using namespace std;

//Function Declarations
#pragma region Declarations
void makeNodes(CNode* _parent, int currentDepth, bool isX);

int CalcScore(CNode* _parent);

int ABPrune(CNode* _node, int _depth, int alpha, int beta, bool isMaxi);

bool tryPlace(int x, int y, bool isX);

void UpdateBoard();

void AITurn();

void InitGame();

int FixedUpdate();

void CreateButtons();

void EndGame(string winner);

void EnableGridButtons();

void Draw();

void deleteTree(CNode* _node);

void deleteBranch(CNode* _node);

void DisableAllButtons();

void CheckButtonsPressed();

void CreateTextButton(void(*function)(), std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float _x, float _y, sf::Color _bgColour, float _padding, bool _isE = true);

void CreateTile(int col, int row, std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float tl_x, float tl_y, float br_x, float br_y, sf::Color _bgColour, bool _isE);
#pragma endregion

//Global game var
CGame game;

/// <summary>
/// Starts the game
/// </summary>
void StartGame() {
	game.state = CGame::GameStates::ChooseMode;

	DisableAllButtons();

	InitGame();
}

/// <summary>
/// Quits...
/// </summary>
void Quit() {
	game.state = CGame::GameStates::Quit;

	DisableAllButtons();
}

/// <summary>
/// If user chose PVP
/// </summary>
void ChoosePVP() {
	DisableAllButtons();
	EnableGridButtons();
	game.pvp = true;
	game.state = CGame::GameStates::Player1Turn;
}

/// <summary>
/// If user chose to vs AI
/// </summary>
void ChoosePVAI() {
	game.state = CGame::GameStates::ChooseDifficulty;
}

/// <summary>
/// If user chose easy AI
/// </summary>
void ChooseEasy() {
	game.hardAI = false;
	game.state = CGame::GameStates::ChooseFirstAI;
}

/// <summary>
/// If user chose Hard AI
/// </summary>
void ChooseHard() {
	game.hardAI = true;
	game.state = CGame::GameStates::ChooseFirstAI;
}

/// <summary>
/// If user chose AI to go first
/// </summary>
void ChooseAI() {
	DisableAllButtons();
	EnableGridButtons();
	game.pFirst = false;
	game.state = CGame::GameStates::AITurn;
}

/// <summary>
/// If user chose to go first
/// </summary>
void ChooseYou() {
	DisableAllButtons();
	EnableGridButtons();
	game.pFirst = true;
	game.state = CGame::GameStates::Player1Turn;
}

/// <summary>
/// Returns to menu
/// </summary>
void Menu() {
	deleteTree(game.startNode);
	game.startNode = nullptr;

	game.isMade = false;
	game.startNode = new CNode();

	UpdateBoard();

	game.state = CGame::GameStates::Menu;
}

int main() {
	//Set seed
	srand(time(0));

	//Creating Different Windows
	sf::RenderWindow window(sf::VideoMode(350, 350), "Tic-Tac-Toe - By Keane Carotenuto");
	game.wind = &window;

	//Minimise console
	ShowWindow(GetConsoleWindow(), SW_MINIMIZE);

	//Manages the FixedUpdate() timing
	float stepTime = 0;
	bool drawn = false;
	sf::Clock clock;

	//FixedUpdate() call rate
	float step = (1.0f / 60.0f);

	//Load font
	if (!game.font.loadFromFile("Roboto.ttf")) std::cout << "Failed to load Roboto\n\n";

	//Make all buttons
	CreateButtons();

	//Run loop
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

		//Checks for buttons being clicked
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
}

/// <summary>
/// Create all buttons needed for game
/// </summary>
void CreateButtons()
{
	//Tictactoe "buttons"
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {

			CreateTile(x, y, to_string(x) + to_string(y), 50, sf::Color::White, sf::Text::Style::Bold, x * 120, y * 120, x * 120 + 110, y * 120 + 110, sf::Color::White, true);
			game.buttomMap.find(to_string(x) + to_string(y))->second->text->setString("");
		}
	}

	//Menu buttons
	CreateTextButton(nullptr, "Tic-Tac-Toe", 50, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5);
	CreateTextButton(&StartGame, "Play", 50, sf::Color::White, sf::Text::Style::Bold, 0, 70, sf::Color::Color(0, 150, 0), 5);
	CreateTextButton(nullptr, "KeaneCarotenuto", 40, sf::Color::White, sf::Text::Style::Bold, 0, 160, sf::Color::Color(0, 0, 0, 0), 5);
	CreateTextButton(nullptr, "@gmail.com", 40, sf::Color::White, sf::Text::Style::Bold, 0, 200, sf::Color::Color(0, 0, 0, 0), 5);
	CreateTextButton(&Quit, "Quit", 50, sf::Color::White, sf::Text::Style::Bold, 0, 290, sf::Color::Color(150, 0, 0), 5);

	//Mode buttons
	CreateTextButton(nullptr, "Choose Mode", 40, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5, false);
	CreateTextButton(&ChoosePVP, "PVP", 50, sf::Color::White, sf::Text::Style::Bold, 0, 70, sf::Color::Color(150, 150, 0), 5, false);
	CreateTextButton(&ChoosePVAI, "PVAI", 50, sf::Color::White, sf::Text::Style::Bold, 0, 140, sf::Color::Color(0, 150, 150), 5, false);

	//Choose starting buttons
	CreateTextButton(nullptr, "Choose Starting", 40, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5, false);
	CreateTextButton(&ChooseAI, "AI", 50, sf::Color::White, sf::Text::Style::Bold, 0, 70, sf::Color::Color(150, 150, 0), 5, false);
	CreateTextButton(&ChooseYou, "You", 50, sf::Color::White, sf::Text::Style::Bold, 0, 140, sf::Color::Color(0, 150, 0), 5, false);

	//Difficulty buttons
	CreateTextButton(nullptr, "Choose Difficulty", 40, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 0, 0), 5, false);
	CreateTextButton(&ChooseEasy, "Easy", 50, sf::Color::White, sf::Text::Style::Bold, 0, 70, sf::Color::Color(0, 150, 0), 5, false);
	CreateTextButton(&ChooseHard, "Hard", 50, sf::Color::White, sf::Text::Style::Bold, 0, 140, sf::Color::Color(150, 0, 0), 5, false);

	//End Screen buttons
	CreateTextButton(nullptr, "Winner", 50, sf::Color::White, sf::Text::Style::Bold, 0, 100, sf::Color::Color(0, 0, 0), 5, false);
	CreateTextButton(&Menu, "Menu", 50, sf::Color::White, sf::Text::Style::Bold, 0, 180, sf::Color::Color(0, 150, 0, 200), 5, false);

	//Wait Button
	CreateTextButton(nullptr, "Wait", 50, sf::Color::White, sf::Text::Style::Bold, 0, 75, sf::Color::Color(150, 0, 0), 5, false);
}

/// <summary>
/// Fixed update runs at fixed rate
/// </summary>
/// <returns>1 is keep running, 0 is exit</returns>
int FixedUpdate() {
	//Game states
	switch (game.state)
	{
	case CGame::GameStates::Menu:
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

	case CGame::GameStates::AITurn:

		if (!game.startNode->childs.empty() || !game.isMade) {
			AITurn();

			game.state = CGame::GameStates::Player1Turn;
		}
		else {
			UpdateBoard();
			EndGame((game.startNode->value > 50 ? "X" : (game.startNode->value < -50 ? "O" : " ")));
		}
		break;

	case CGame::GameStates::Player1Turn:
		UpdateBoard();
		if (game.startNode->childs.empty() && game.isMade) {
			EndGame((game.startNode->value > 50 ? "X" : (game.startNode->value < -50 ? "O" : " ")));
		}
		break;

	case CGame::GameStates::Player2Turn:
		UpdateBoard();
		if (game.startNode->childs.empty() && game.isMade) {
			EndGame((game.startNode->value > 50 ? "X" : (game.startNode->value < -50 ? "O" : " ")));
		}
		break;

	case CGame::GameStates::EndScreen:
		break;

	case CGame::GameStates::Quit:
		return 0;
		break;

	default:
		break;
	}

	return 1;
}

/// <summary>
/// Displays end game message
/// </summary>
/// <param name="winner">pass in winner (could just be char or int... oh well too lazy)</param>
void EndGame(string winner)
{
	game.state = CGame::GameStates::EndScreen;
	game.buttomMap.find("Winner")->second->isEnabled = true;

	//End message setting
	string toWrite = "";
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

	//Writing buttons and changing display of them based on text
	game.buttomMap.find("Winner")->second->text->setString(toWrite);
	game.buttomMap.find("Winner")->second->rect->setFillColor(winner == "X" ? sf::Color::Color(100,0,0,200) : winner == "O" ? sf::Color::Color(0, 0, 100, 200) : sf::Color::Color(0,0,0,200));
	game.buttomMap.find("Winner")->second->rect->setOutlineColor(sf::Color::Black);
	game.buttomMap.find("Winner")->second->rect->setOutlineThickness(2.0f);
	game.buttomMap.find("Winner")->second->text->setOrigin((game.buttomMap.find("Winner")->second->text->getGlobalBounds().width) / 2, 0);
	game.buttomMap.find("Winner")->second->text->setPosition(game.wind->getSize().x / 2, game.buttomMap.find("Winner")->second->text->getPosition().y);

	game.buttomMap.find("Menu")->second->isEnabled = true;
	game.buttomMap.find("Menu")->second->rect->setOutlineColor(sf::Color::Black);
	game.buttomMap.find("Menu")->second->rect->setOutlineThickness(2.0f);
}

/// <summary>
/// Draws all objects onto screen
/// </summary>
void Draw() {
	game.wind->clear();

	//All objects in todraw
	for (sf::Drawable* item : game.toDraw)
	{
		game.wind->draw((*item));
	}

	//All Buttons
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

	//Makes sure window size does not change
	if (game.wind->getSize() != sf::Vector2u(350, 350)) game.wind->setSize(sf::Vector2u(350, 350));
}

/// <summary>
/// Initialises the game and resets things (clearing memory etc)
/// </summary>
void InitGame()
{
	deleteTree(game.startNode);
	game.startNode = nullptr;

	game.isMade = false;

	game.pvp = false;

	game.hardAI = false;

	game.startNode = new CNode();

	//Reset tictac buttons
	for (CButton* _button : game.buttons) {
		if (_button->isTicTacTile) {
			_button->rect->setFillColor(sf::Color::White);
		}
	}
}

/// <summary>
/// Lets the ai go
/// </summary>
void AITurn()
{
	//If not already made, create the tree (doing so after placing first makes it 8x smaller)
	if (!game.isMade) {
		if (!tryPlace(0, 0, true)) {
			int x = 1;
			int y = 1;
			while (!tryPlace(x, y, true)) {
				x = std::rand() % 3;
				y = std::rand() % 3;
			}
		}

		//Display wait message
		game.buttomMap.find("Wait")->second->isEnabled = true;

		//Update and draw board
		UpdateBoard();
		Draw();

		//Make tree
		makeNodes(game.startNode, 1, false);

		game.buttomMap.find("Wait")->second->isEnabled = false;

		return;
	}

	//Choose best if hard, or random if easy
	if (game.hardAI) {
		//Minimax + Prune
		ABPrune(game.startNode, game.maxDepth + 1, -10000, 10000, true);

		bool hasChosen = false;
		for (CNode* _child : game.startNode->childs) {
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

/// <summary>
/// Updates colours and text of board tiles
/// </summary>
void UpdateBoard()
{
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (game.startNode->board[y][x] == NULL) {
				game.buttomMap.find(to_string(x) + to_string(y))->second->text->setString("");
				continue;
			}

			if (game.startNode->board[y][x] == 'X') {
				game.buttomMap.find(to_string(x) + to_string(y))->second->text->setString("X");
				game.buttomMap.find(to_string(x) + to_string(y))->second->rect->setFillColor(sf::Color::Color(150, 0, 0));
			}
			else if (game.startNode->board[y][x] == 'O') {
				game.buttomMap.find(to_string(x) + to_string(y))->second->text->setString("O");
				game.buttomMap.find(to_string(x) + to_string(y))->second->rect->setFillColor(sf::Color::Color(0, 0, 150));
			}
			

			
		}
	}
}

/// <summary>
/// Makes the tree of nodes from parent
/// </summary>
/// <param name="_parent">Starting Node</param>
/// <param name="currentDepth">Set to 1 for start</param>
/// <param name="isX">If next move is X, set to true</param>
void makeNodes(CNode* _parent, int currentDepth, bool isX) {
	game.isMade = true;
	_parent->level = currentDepth;

	//Calculate value of current node
	_parent->value = CalcScore(_parent);

	//If it isnt a winning board state, make children
	if (_parent->value > -100 && _parent->value < 80) {

		//Check for empty spaces, and make child with that space 
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {

				if (_parent->board[y][x] == NULL) {
					//Make the child
					CNode* child = new CNode();
					child->parent = _parent;
					_parent->childs.push_back(child);

					//Copy board
					for (int _y = 0; _y < 3; _y++)
					{
						for (int _x = 0; _x < 3; _x++)
						{
							child->board[_y][_x] = _parent->board[_y][_x];
						}
					}

					//Set new position
					child->board[y][x] = (isX ? 'X' : 'O');

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
int CalcScore(CNode* _parent)
{
	//Storing lines
	vector<string> lines;

	//Get all horiz lines
	for (int y = 0; y < 3; y++) {
		string line = "";
		for (int x = 0; x < 3; x++) {
			if (_parent->board[y][x] == NULL) continue;
			line += _parent->board[y][x];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

	//Get all vert lines
	for (int x = 0; x < 3; x++) {
		string line = "";
		for (int y = 0; y < 3; y++) {
			if (_parent->board[y][x] == NULL) continue;
			line += _parent->board[y][x];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

	//Just seperate
	if (true) {
		string line = "";

		//get one diag line
		for (int xy = 0; xy < 3; xy++) {
			if (_parent->board[xy][xy] == NULL) continue;
			line += _parent->board[xy][xy];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

	//Just seperate
	if (true) {
		string line = "";

		//get other diag line
		for (int xy = 0; xy < 3; xy++) {
			if (_parent->board[2 - xy][xy] == NULL) continue;
			line += _parent->board[2 - xy][xy];
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

int ABPrune(CNode* _node, int _depth, int alpha, int beta, bool isMaxi) {
	//If end node, return value
	if (_depth == 0 || _node->childs.empty()) {
		return _node->value;
	}

	//If is maximiser
	if (isMaxi) {
		//Set really low
		int value = -10000;
		CNode* _best = nullptr;

		//For each child
		for (CNode* _child : _node->childs) {
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
		CNode* _best = nullptr;

		for (CNode* _child : _node->childs) {
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
/// <param name="x">X coord</param>
/// <param name="y">Y coord</param>
/// <param name="isX">true to place X, false for O</param>
bool tryPlace(int x, int y, bool isX) {
	if (game.startNode->board[y][x] != NULL || x == NAN || y == NAN || x<0 || x>2 || y<0 || y>2) {
		return false;
	}

	CNode* tempNode = nullptr;

	//Check if no children (player may be starting)
	if (game.startNode->childs.empty()) {
		game.startNode->board[y][x] = (isX ? 'X' : 'O');
		return true;
	}

	//If the chosen spot is free, search for it in children
	if (game.startNode->board[y][x] == NULL) {
		for (CNode* _node : game.startNode->childs) {
			if (_node->board[y][x] == (isX ? 'X' : 'O')) {
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

/// <summary>
/// Delete entire tree
/// </summary>
/// <param name="_node">Pass any node in tree</param>
void deleteTree(CNode* _node) {

	if (_node == nullptr) return;

	CNode* currentNode = _node;
	while (currentNode->parent != nullptr) {
		currentNode = currentNode->parent;
	}

	deleteBranch(currentNode);
}

/// <summary>
/// Delete only tree below current node
/// </summary>
/// <param name="_node">Node's children will be deleted</param>
void deleteBranch(CNode* _node)
{
	if (_node == nullptr) return;

	for (CNode* _child : _node->childs) {
		deleteBranch(_child);
	}

	delete _node;
	_node = nullptr;
}

/// <summary>
/// Disables all buttons...
/// </summary>
void DisableAllButtons()
{
	for (CButton* _button : game.buttons)
	{
		_button->isEnabled = false;
	}
}

/// <summary>
/// Enables tic tac toe buttons
/// </summary>
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

/// <summary>
/// Checks for buttons being clicked
/// </summary>
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

						//If tictac button do special task
						if (_button->isTicTacTile && game.state != CGame::GameStates::EndScreen) {

							//If player vs player
							if (game.pvp) {

								//if player 1 turn or p2 turn
								if (game.state == CGame::GameStates::Player1Turn) {
									//try place at location
									bool isValid = tryPlace(_button->tictacX, _button->tictacY, false);

									//If valid, make nodes if needed, otherwise go to other player
									if (isValid) {
										game.buttomMap.find("Wait")->second->isEnabled = true;

										if (!game.isMade) makeNodes(game.startNode, 1, true);

										game.buttomMap.find("Wait")->second->isEnabled = false;

										game.state = CGame::GameStates::Player2Turn;
									}
								}
								else {
									bool isValid = tryPlace(_button->tictacX, _button->tictacY, true);
									if (isValid) {
										game.state = CGame::GameStates::Player1Turn;
									}
								}
							}
							//VSing AI
							else {
								if (tryPlace(_button->tictacX, _button->tictacY, false)) {
									game.state = CGame::GameStates::AITurn;
								}
							}
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
	buttonRect->setPosition(0, tempText->getGlobalBounds().top - _padding - 5);
	buttonRect->setSize(sf::Vector2f(350, 60));
	buttonRect->setFillColor(_bgColour);

	//create
	CButton* button = new CButton(buttonRect, tempText, function);
	game.buttons.push_back(button);
	game.buttomMap[_string] = button;
}

/// <summary>
/// Creates Tic Tac Toe Tile
/// </summary>
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