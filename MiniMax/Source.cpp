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
	int value = NULL;
	int treeValue = NULL;
	BoardState state;
	bool wantsToMax = true;
	
	int level = NULL;

	//float averageBelow;

	Node* parent = nullptr;

	int a = NULL;
	int b = NULL;
	string reasonForBest = "";
	Node* bestChoice = nullptr;
	Node* targetEnd = nullptr;

	vector<Node*> childs;

	bool operator < (const Node& _other) const
	{
		return (value < _other.value);
	}
};

struct CGame {
	
}game;

Node* startNode = nullptr;
Node* bestNode = nullptr;

int maxDepth;

//vector<int>toMake = { -13, -13, -14, -9, -11, -15, -6, 8, 10, 7, -15, 8, -5, 19, 9, 2, 15, -20, -12, 3, 14, 4, -6, -8, 7, -8, 4};
vector<int>toMake = { 6, 5, 8, 7,2,1,3,4};

int CalcScore(Node* _parent);

void deleteTree(Node* _node)
{
	if (_node == nullptr) return;

	for (Node* _child : _node->childs) {
		deleteTree(_child);
	}

	delete _node;
}

void makeNodes(Node* _parent, int currentDepth, bool isX) {
	_parent->level = currentDepth;

	for (Node* _child : _parent->childs) {
		deleteTree(_child);
	}
	_parent->childs.clear();
	
	/*if (currentDepth == maxDepth) {
		bool makeRand = false;
		if (makeRand) {
			_parent->value = rand() % 10 + 1;
		}
		else {
			if (!toMake.empty()) {
				_parent->value = toMake[0];
				toMake.erase(toMake.begin());
			}
			else {
				cout << endl << "Failed to grab number from vector" << endl;
				_parent->value = rand() % 10 + 1;
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
	_parent->value = CalcScore(_parent);

	if (_parent->value > -100 && _parent->value < 80) {

		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {

				if (_parent->state.board[y][x] == NULL) {

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

					child->state.board[y][x] = (isX ? 'X' : 'O');

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
	int winState = 0;
	vector<string> lines;

	for (int y = 0; y < 3; y++) {
		string line = "";
		for (int x = 0; x < 3; x++) {
			if (_parent->state.board[y][x] == NULL) continue;
			line += _parent->state.board[y][x];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

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

		for (int xy = 0; xy < 3; xy++) {
			if (_parent->state.board[2-xy][xy] == NULL) continue;
			line += _parent->state.board[2-xy][xy];
		}

		sort(line.begin(), line.end());

		lines.push_back(line);
	}

	int x2 = 0;
	int x1 = 0;
	int o2 = 0;
	int o1 = 0;

	for (string _line : lines) {
		if (_line == "XXX") return 100 - _parent->level;
		if (_line == "OOO") return -100 - _parent->level;
		if (_line == "XX") x2++;
		else if (_line == "X") x1++;
		else if (_line == "OO") o2++;
		else if (_line == "O") o1++;
	}

	/*if (x2 == 0 && x1 == 0 && o2 == 0 && o1 == 0) {
		return INT_MIN;
	}*/

	return (3 * x2 + x1 - (3* o2 + o1));
}

int ABPrune(Node* _node, int _depth, int alpha, int beta, bool isMaxi) {
	if (isMaxi) _node->wantsToMax = true;
	else _node->wantsToMax = false;
	bool isTerm = true;
	for (Node* _child : _node->childs) {
		if (_child != nullptr) {
			isTerm = false;
			break;
		}
	}

	sort(_node->childs.begin(), _node->childs.end());

	if (isMaxi) {
		reverse(_node->childs.begin(), _node->childs.end());
	}

	if (_depth == 0 || isTerm) {
		//cout << "val:" << _node->value << ", ";
		_node->targetEnd = _node;
		_node->treeValue = _node->value;
		return _node->value;
	}


	if (isMaxi) {
		int value = -10000;
		Node* _best = nullptr;

		for (Node* _child : _node->childs) {
			int tempVal = ABPrune(_child, _depth - 1, alpha, beta, false);
			value = max(value, tempVal);
			alpha = max(alpha, value);

			_node->a = alpha;
			_node->b = beta;


			if (value == tempVal) {
				if (_child->targetEnd != nullptr) _node->targetEnd = _child->targetEnd;
				_best = _child;
			}

			if (alpha >= beta) {
				_node->reasonForBest = "Alpha >= Beta, so prune after " + to_string(value) + (string)": ";
				//cout << "no bigger, ";
				break;
			}
		}
		_node->reasonForBest += "choose " + to_string(value);
		//cout << "choose:" << value << endl;
		_node->bestChoice = _best;
		_node->treeValue = value;
		return value;
	}
	else {
		int value = 10000;
		Node* _best = nullptr;

		for (Node* _child : _node->childs) {
			int tempVal = ABPrune(_child, _depth - 1, alpha, beta, true);
			value = min(value, tempVal);
			beta = min(beta, value);

			_node->a = alpha;
			_node->b = beta;

			if (value == tempVal) {
				if (_child->targetEnd != nullptr) _node->targetEnd = _child->targetEnd;
				_best = _child;
			}

			if (beta <= alpha) {
				_node->reasonForBest = "Beta <= Alpha, so prune after " + to_string(value) + ": ";
				//cout << "no smaller, ";
				break;
			}
		}
		_node->reasonForBest += "choose " +  to_string(value);
		//cout << "choose:" << value << endl;
		_node->bestChoice = _best;
		return value;
	}
}

void PrintNodes() {
	vector<Node*> toPrint;
	vector<Node*> toAdd;
	//toPrint.push_back(startNode);

	toAdd.push_back(startNode);
	while (!toAdd.empty()) {
		if (toAdd[0]->value != 0) {
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
		cout << _node->value << " ";
		/*for (int i = 0; i < (log(toPrint.size() + 1)/log(2) - _node->level + 1); i++) {
			cout << " ";
		}*/
	}
}

void tryPlace(int x, int y, bool isX) {
	Node* tempNode = nullptr;
	if (startNode->childs.empty()) {
		startNode->state.board[y][x] = (isX ? 'X' : 'O');
		cout << endl << "Placed at " << x << ", " << y << endl;
		return;
	}

	if (startNode->state.board[y][x] == NULL) {
		for (Node* _node : startNode->childs) {
			if (_node->state.board[y][x] == (isX ? 'X' : 'O')) {
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

	
	//PrintNodes();
	cout << endl;
	cout << endl;

	while (true) {
		system("CLS");
		cout << endl;
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

		/*if (startNode->bestChoice != nullptr) {
			cout << endl << "Next:" << endl;
			for (int y = 0; y < 3; y++) {
				for (int x = 0; x < 3; x++) {
					if (startNode->bestChoice->state.board[y][x] == NULL) cout << ".";
					cout << startNode->bestChoice->state.board[y][x];
				}
				cout << endl;
			}
		}*/
		

		int x;
		int y;
		cout << "X and Y? ";
		cin >> x;
		cin >> y;

		tryPlace(x, y, true);

		makeNodes(startNode, 1, false);
		cout << "made nodes\n";
		ABPrune(startNode, maxDepth + 1, -10000, 10000, false);
		//ABPrune(startNode, maxDepth + 1, INT_MIN, INT_MAX, true);

		/*if (bestNode == nullptr || bestNode == startNode) {
			cout << endl << "cannot continue...";
			break;
		}*/



		Node* currentNode = startNode->bestChoice;
		while (currentNode->parent != startNode) {
			currentNode = currentNode->parent;
		}
		if (currentNode->parent == startNode) {
			Node* tempNode = startNode;
			startNode = currentNode;
			//delete tempNode;
		}
	}

	//ABPrune(startNode, maxDepth + 1, INT_MIN, INT_MAX, false);
	system("pause");


	return 0;
}