#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <limits> 

using namespace std;

struct BoardState {
	char board[3][3] = {{0,0,0},
						{0,0,0},
						{0,0,0} };
	int value = NULL;
};

struct Node {
	BoardState state;
	int level = NULL;
	float averageBelow;
	Node* parent = nullptr;
	vector<Node*> childs;

	~Node();
};

struct CGame {
	
}game;

Node* startNode = nullptr;
Node* bestNode = nullptr;

int maxDepth;

//vector<int>toMake = { -13, -13, -14, -9, -11, -15, -6, 8, 10, 7, -15, 8, -5, 19, 9, 2, 15, -20, -12, 3, 14, 4, -6, -8, 7, -8, 4};
vector<int>toMake = { 6, 5, 8, 7,2,1,3,4};

int CheckWin(Node* _parent);

void makeNodes(Node* _parent, int currentDepth, bool isX) {
	_parent->level = currentDepth;
	
	/*if (currentDepth == maxDepth) {
		bool makeRand = false;
		if (makeRand) {
			_parent->state.value = rand() % 10 + 1;
		}
		else {
			if (!toMake.empty()) {
				_parent->state.value = toMake[0];
				toMake.erase(toMake.begin());
			}
			else {
				cout << endl << "Failed to grab number from vector" << endl;
				_parent->state.value = rand() % 10 + 1;
			}
			
		}
	}
	else {
		for (int i = 2; i > 0; i--) {
			Node* child = new Node();
			_parent->childs.push_back(child);
			makeNode(child, currentDepth + 1);
		}
	}*/

	//-1 Y win, 0 unknown, 1 X win
	int winState = CheckWin(_parent);

	bool freeSpace = false;

	if (winState == 0) {

		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {

				if (_parent->state.board[y][x] == NULL) {
					freeSpace = true;

					Node* child = new Node();
					child->parent = _parent;
					_parent->childs.push_back(child);

					for (int _y = 0; _y < 3; _y++)
					{
						for (int _x = 0; _x < 3; _x++)
						{
							child->state.board[_y][_x] = _parent->state.board[_y][_x];
						}
					}

					child->state.board[y][x] = (isX ? 'X' : 'Y');

					makeNodes(child, currentDepth + 1, !isX);
				}

			}
		}
	}

	

	if (!freeSpace || winState != 0) {
		_parent->state.value = winState;
	}
}

/// <summary>
/// Checks if a node is in a winning state
/// </summary>
/// <param name="_parent">The node to check</param>
/// <returns>-1 = Y wins, 0 = No Wins, 1 = X wins</returns>
int CheckWin(Node* _parent)
{
	int winState = 0;

	for (int y = 0; y < 3; y++) {
		string line = "";
		for (int x = 0; x < 3; x++) {
			line += _parent->state.board[y][x];
		}

		if (line == "XXX") {
			winState = 1;
			return winState;
		}
		else if (line == "YYY") {
			winState = -1;
			return winState;
		}
		else {
			winState = 0;
		}
	}

	for (int x = 0; x < 3; x++) {
		string line = "";
		for (int y = 0; y < 3; y++) {
			line += _parent->state.board[y][x];
		}

		if (line == "XXX") {
			winState = 1;
			return winState;
		}
		else if (line == "YYY") {
			winState = -1;
			return winState;
		}
		else {
			winState = 0;
		}
	}

	//Just seperate
	if (true) {
		string line = "";

		for (int xy = 0; xy < 3; xy++) {
			line += _parent->state.board[xy][xy];
		}

		if (line == "XXX") {
			winState = 1;
			return winState;
		}
		else if (line == "YYY") {
			winState = -1;
			return winState;
		}
		else {
			winState = 0;
		}
	}

	for (int y = 0; y < 3; y++) {
		string line = "";
		for (int x = 2; x >= 0; x--) {
			line += _parent->state.board[y][x];
		}

		if (line == "XXX") {
			winState = 1;
			return winState;
		}
		else if (line == "YYY") {
			winState = -1;
			return winState;
		}
		else {
			winState = 0;
		}
	}

	return winState;
}

int ABPrune(Node* _node, int _depth, int alpha, int beta, bool isMaxi) {
	bool isTerm = true;
	for (Node* _child : _node->childs) {
		if (_child != nullptr) {
			isTerm = false;
			break;
		}
	}

	if (_depth == 0 || isTerm) {
		//cout << "val:" << _node->state.value << ", ";
		return _node->state.value;
	}

	if (isMaxi) {
		int value = INT_MIN;

		for (Node* _child : _node->childs) {
			if (_child != nullptr) {
				bestNode = _child;
			}
			int tempVal = ABPrune(_child, _depth - 1, alpha, beta, false);
			value = max(value, tempVal);
			alpha = max(alpha, value);
			if (alpha >= beta) {
				//cout << "no bigger, ";
				break;
			}
		}
		//cout << "choose:" << value << endl;
		return value;
	}
	else {
		int value = INT_MAX;

		for (Node* _child : _node->childs) {
			if (_child != nullptr) {
				bestNode = _child;
			}
			int tempVal = ABPrune(_child, _depth - 1, alpha, beta, true);
			value = min(value, tempVal);
			beta = min(beta, value);
			if (beta <= alpha) {
				//cout << "no smaller, ";
				break;
			}
		}
		//cout << "choose:" << value << endl;
		return value;
	}
}

void PrintNodes() {
	vector<Node*> toPrint;
	vector<Node*> toAdd;
	//toPrint.push_back(startNode);

	toAdd.push_back(startNode);
	while (!toAdd.empty()) {
		if (toAdd[0]->state.value != 0) {
			toPrint.push_back(toAdd[0]);
		}
		for (Node* _child : toAdd[0]->childs) {
			toAdd.push_back(_child);
		}

		toAdd.erase(toAdd.begin());
	}

	int level = 0;
	for (Node* _node : toPrint) {
		if (_node->level != level) {
			cout << endl;
			level = _node->level;
			/*for (int i = 0; i < (log(toPrint.size() + 1) / log(2) - _node->level + 1); i++) {
				cout << "  ";
			}*/
		}
		cout << _node->state.value << " ";
		/*for (int i = 0; i < (log(toPrint.size() + 1)/log(2) - _node->level + 1); i++) {
			cout << " ";
		}*/
	}
}

void tryPlace(int x, int y, bool isX) {
	Node* tempNode = nullptr;
	if (startNode->state.board[y][x] == NULL) {
		for (Node* _node : startNode->childs) {
			if (_node->state.board[y][x] == (isX ? 'X' : 'Y')) {
				tempNode = _node;
				break;
			}
		}

		if (tempNode != nullptr) {
			for (Node* _node : startNode->childs) {
				//if (_node != tempNode) delete _node;
			}
			startNode = tempNode;
			cout << endl << "Placed at " << x << ", " << y << endl;
		}
		else {
			cout << endl << "Failed at " << x << ", " << y << endl;
		}
	}
}

int main() {
	srand(11);

	cout <<	"Minimax Program" << endl <<
			"Please Enter depth of tree: ";
	cin >> maxDepth;

	startNode = new Node();
	//startNode->state.board[0][0] = 'X';
	//makeNodes(startNode, 1, true);

	cout << "made nodes\n";
	//PrintNodes();
	cout << endl;
	cout << endl;

	while (true) {
		//system("CLS");

		makeNodes(startNode, 1, true);
		ABPrune(startNode, maxDepth + 1, INT_MIN, INT_MAX, true);

		if (bestNode == nullptr || bestNode == startNode) {
			cout << endl << "cannot continue...";
			break;
		}

		Node* currentNode = bestNode;
		while (currentNode->parent != startNode) {
			currentNode = currentNode->parent;
		}
		if (currentNode->parent == startNode) {
			Node* tempNode = startNode;
			startNode = currentNode;
			//delete tempNode;
		}

		cout << endl;
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				if (startNode->state.board[y][x] == NULL) cout << ".";
				cout << startNode->state.board[y][x];
			}
			cout << endl;
		}

		int x;
		int y;
		cout << "X and Y? ";
		cin >> x;
		cin >> y;

		tryPlace(x, y, false);
		
	}

	//ABPrune(startNode, maxDepth + 1, INT_MIN, INT_MAX, false);
	system("pause");


	return 0;
}

Node::~Node()
{
	for (Node* _child : childs) {
		if (_child != nullptr) {
			delete _child;
		}
	}
	delete this;
}
