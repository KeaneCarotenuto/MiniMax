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
	sf::RenderWindow* wind;
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

void DisplayBoard();

void PlayerTurn();

void AITurn();

void InitGame();

int FixedUpdate();
void Draw();

void deleteTree(Node* _node);

void deleteBranch(Node* _node);

void CheckButtonsPressed();

void CreateTextButton(void(*function)(), std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float _x, float _y, sf::Color _bgColour, float _padding, bool _isE = true);

void CreateClickable(void(*function)(), std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float tl_x, float tl_y, float br_x, float br_y, sf::Color _bgColour, bool _isE);


void StartGame() {
	cout << "Yep\n";
	game.state = CGame::GameStates::ChooseFirstAI;
	InitGame();
}

void ChooseAI() {
	game.pFirst = false;
	game.state = CGame::GameStates::AITurn;
}

void ChooseYou() {
	game.pFirst = true;
	game.state = CGame::GameStates::AITurn;
}

void TestButton() {
	cout << "\nWorks\n";
}

int main() {

	//Creating Different Windows
	sf::RenderWindow window(sf::VideoMode(800, 600), "ProgramName - By Keane Carotenuto");
	game.wind = &window;

	//Manages the FixedUpdate() timing
	float stepTime = 0;
	bool drawn = false;
	sf::Clock clock;

	//FixedUpdate() call rate
	float step = (1.0f / 60.0f);

	if (!game.font.loadFromFile("Roboto.ttf")) std::cout << "Failed to load Roboto\n\n";

	/*InitGame();*/

	CreateTextButton(&StartGame, "Play", 25, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 150, 0), 5);
	CreateTextButton(&ChooseAI, "AI", 25, sf::Color::White, sf::Text::Style::Bold, 0, 0, sf::Color::Color(0, 150, 0), 5, false);
	CreateTextButton(&ChooseYou, "You", 25, sf::Color::White, sf::Text::Style::Bold, 0, 30, sf::Color::Color(0, 150, 0), 5, false);

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			CreateClickable(&TestButton, to_string(10*y + x), 25, sf::Color::White, sf::Text::Style::Bold, x * 50 + 200, y * 50 + 200, (x + 1) * 50 + 200, (y + 1) * 50 + 200, sf::Color::Color(0, 150, 0), true);
		}
	}
	


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


		game.buttomMap.find("Play")->second->isEnabled = true;
		game.buttomMap.find("AI")->second->isEnabled = false;
		game.buttomMap.find("You")->second->isEnabled = false;
		break;

	case CGame::GameStates::ChooseMode:

		break;

	case CGame::GameStates::ChooseDifficulty:
		break;

	case CGame::GameStates::ChooseFirstAI:
		game.buttomMap.find("Play")->second->isEnabled = false;
		game.buttomMap.find("AI")->second->isEnabled = true;
		game.buttomMap.find("You")->second->isEnabled = true;
		break;

	case CGame::GameStates::ChooseFirst:
		
		break;

	case CGame::GameStates::AITurn:
		system("CLS");

		if (!game.startNode->childs.empty() || !game.isMade) {
			if (game.pFirst) {
				PlayerTurn();
			}

			AITurn();

			if (!game.pFirst) {
				PlayerTurn();
			}
		}
		else {
			system("CLS");
			std::cout << (game.startNode->value > 50 ? "X" : (game.startNode->value < -50 ? "Y" : "No one")) << " won this round.";

			system("pause");

			game.state = CGame::GameStates::Menu;
		}
		break;

	case CGame::GameStates::Player1Turn:
		break;

	case CGame::GameStates::Player2Turn:

		break;

	default:
		break;
	}


	/*system("CLS");

	if (!game.startNode->childs.empty() || !game.isMade){
		if (game.pFirst) {
			PlayerTurn();
		}

		AITurn();

		if (!game.pFirst) {
			PlayerTurn();
		}
	}
	else {
		system("CLS");
		std::cout << (game.startNode->value > 50 ? "X" : (game.startNode->value < -50 ? "Y" : "No one")) << " won this round.";

		system("pause");

		InitGame();
	}*/

	

	return 1;
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
}

void InitGame()
{
	deleteTree(game.startNode);
	game.startNode = nullptr;

	game.isMade = false;

	/*cout << "Minimax Program" << endl <<
		"Player First? 1=yes 0=no";
	cin >> game.pFirst;

	if (game.pFirst != 0 && game.pFirst != 1) {
		game.pFirst = 0;
		cout << endl << "Incorrect answer, AI goes first..." << endl;
	}*/

	game.startNode = new Node();

	cout << endl;
	cout << endl;
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

		makeNodes(game.startNode, 1, false);
		game.isMade = true;
		cout << "made nodes\n";

		return;
	}

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
	if (!hasChosen) std::cout << "\nFailed\n";
}

void PlayerTurn()
{
	system("CLS");

	DisplayBoard();

	//Ask for user input
	int x = NULL;
	int y = NULL;
	bool isGood = false;

	while (!isGood) {
		cout << "X and Y? ";
		cin >> x;
		cin >> y;
		cout << endl;
		isGood = tryPlace(x, y, false);
	}

	cout << endl;
}

void DisplayBoard()
{
	game.wind->clear();

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (game.startNode->state.board[y][x] == NULL) {
				cout << ".";
				continue;
			}
			cout << game.startNode->state.board[y][x];

			if (game.startNode->state.board[y][x] == 'X') {
				sf::RectangleShape shape;
				shape.setPosition(x * 100, y * 100);
				shape.setFillColor(sf::Color::White);
				shape.setSize(sf::Vector2f(80.0f, 80.0f));
				game.wind->draw(shape);
			}
			else if (game.startNode->state.board[y][x] == 'O') {
				sf::CircleShape shape;
				shape.setPosition(x * 100, y * 100);
				shape.setFillColor(sf::Color::White);
				shape.setRadius(80.0f / 2);
				game.wind->draw(shape);
			}
			

			
		}
		cout << endl;
	}

	//Update main window
	game.wind->display();
}

void makeNodes(Node* _parent, int currentDepth, bool isX) {
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
		cout << endl << "Choose between 0 and 2 for both" << endl;
		return false;
	}

	Node* tempNode = nullptr;

	//Check if no children (player may be starting)
	if (game.startNode->childs.empty()) {
		game.startNode->state.board[y][x] = (isX ? 'X' : 'O');
		cout << endl << "(No Children) Placed at " << x << ", " << y << endl;
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
			cout << endl << "Placed at " << x << ", " << y << endl;
		}
		else {
			cout << endl << "Failed at " << x << ", " << y << endl;
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
						if (_button->function != nullptr) _button->function();
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
	tempText->setPosition(100 - (tempText->getGlobalBounds().width) / 2, _y);

	//Button rect
	sf::RectangleShape* buttonRect = new sf::RectangleShape;
	buttonRect->setPosition(0, tempText->getGlobalBounds().top - _padding);
	buttonRect->setSize(sf::Vector2f(200, 30));
	buttonRect->setFillColor(_bgColour);

	//create
	CButton* button = new CButton(buttonRect, tempText, function);
	game.buttons.push_back(button);
	game.buttomMap[_string] = button;
}

void CreateClickable(void(*function)(), std::string _string, int _fontSize, sf::Color _tColour, sf::Text::Style _style, float tl_x, float tl_y, float br_x, float br_y, sf::Color _bgColour, bool _isE)
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
	CButton* button = new CButton(buttonRect, tempText, function);
	game.buttons.push_back(button);
	game.buttomMap[_string] = button;
}