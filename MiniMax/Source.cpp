#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <limits> 
#include <algorithm>


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
}game;

void makeNodes(Node* _parent, int currentDepth, bool isX);

int CalcScore(Node* _parent);

int ABPrune(Node* _node, int _depth, int alpha, int beta, bool isMaxi);

void tryPlace(int x, int y, bool isX);

void DisplayBoard();

void PlayerTurn();

void AITurn();

void InitGame();

int FixedUpdate();
void Draw();

//Store the current node of the game
Node* startNode = nullptr;

//How far the program should search (set way higher than needed to be safe)
int maxDepth = 100;

bool isMade = false;
bool pFirst = true;

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

	InitGame();

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
	system("CLS");

	if (pFirst) {
		PlayerTurn();
	}

	AITurn();

	if (!pFirst) {
		PlayerTurn();
	}

	return 1;
}

void Draw() {
	game.wind->clear();

	for (sf::Drawable* item : game.toDraw)
	{
		game.wind->draw((*item));
	}

	//Update main window
	game.wind->display();
}

void InitGame()
{
	cout << "Minimax Program" << endl <<
		"Player First? 1=yes 0=no";
	cin >> pFirst;

	startNode = new Node();

	cout << endl;
	cout << endl;
}

void AITurn()
{
	//If not already made, create the tree (doing this after use goes makes it smaller)
	if (!isMade) {
		makeNodes(startNode, 1, true);
		isMade = true;
		cout << "made nodes\n";
	}

	//Minimax + Prune
	ABPrune(startNode, maxDepth + 1, -10000, 10000, true);

	bool hasChosen = false;
	for (Node* _child : startNode->childs) {
		if (_child->value == startNode->value) {
			startNode = _child;
			hasChosen = true;
			break;
		}
	}
	if (!hasChosen) std::cout << "\nFailed\n";
}

void PlayerTurn()
{
	DisplayBoard();

	//Ask for user input
	int x;
	int y;
	cout << "X and Y? ";
	cin >> x;
	cin >> y;

	//try to place user input
	tryPlace(x, y, false);

	cout << endl;
}

void DisplayBoard()
{
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (startNode->state.board[y][x] == NULL) {
				cout << ".";
				continue;
			}
			cout << startNode->state.board[y][x];
		}
		cout << endl;
	}
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
void tryPlace(int x, int y, bool isX) {
	Node* tempNode = nullptr;

	//Check if no children (player may be starting)
	if (startNode->childs.empty()) {
		startNode->state.board[y][x] = (isX ? 'X' : 'O');
		cout << endl << "(No Children) Placed at " << x << ", " << y << endl;
		return;
	}

	//If the chosen spot is free, search for it in children
	if (startNode->state.board[y][x] == NULL) {
		for (Node* _node : startNode->childs) {
			if (_node->state.board[y][x] == (isX ? 'X' : 'O')) {
				tempNode = _node;
				break;
			}
		}

		//If the child exists, set that as the new board state
		if (tempNode != nullptr) {
			startNode = tempNode;
			cout << endl << "Placed at " << x << ", " << y << endl;
		}
		else {
			cout << endl << "Failed at " << x << ", " << y << endl;
		}
	}
}