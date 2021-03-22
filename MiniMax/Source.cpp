#include <Windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <limits> 

using namespace std;

struct BoardState {
	int board[3][3] = {	0,0,0,
						0,0,0,
						0,0,0,};
	int value = NULL;
};

struct Node {
	BoardState state;
	int level = NULL;
	vector<Node*> childs;

	~Node();
};

struct CGame {
	
}game;

Node* startNode = nullptr;

int maxDepth;

//vector<int>toMake = { -13, -13, -14, -9, -11, -15, -6, 8, 10, 7, -15, 8, -5, 19, 9, 2, 15, -20, -12, 3, 14, 4, -6, -8, 7, -8, 4};
vector<int>toMake = { 6, 5, 8, 7,2,1,3,4};

void makeNode(Node* _parent, int currentDepth) {
	_parent->level = currentDepth;
	if (currentDepth == maxDepth) {
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
	}
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
		cout << "val:" << _node->state.value << ", ";
		return _node->state.value;
	}

	if (isMaxi) {
		int value = INT_MIN;

		for (Node* _child : _node->childs) {
			int tempVal = ABPrune(_child, _depth - 1, alpha, beta, false);
			value = max(value, tempVal);
			alpha = max(alpha, value);
			if (alpha >= beta) {
				cout << "no bigger, ";
				break;
			}
		}
		cout << "choose:" << value << endl;
		return value;
	}
	else {
		int value = INT_MAX;

		for (Node* _child : _node->childs) {
			int tempVal = ABPrune(_child, _depth - 1, alpha, beta, true);
			value = min(value, tempVal);
			beta = min(beta, value);
			if (beta <= alpha) {
				cout << "no smaller, ";
				break;
			}
		}
		cout << "choose:" << value << endl;
		return value;
	}
}

void PrintNodes() {
	vector<Node*> toPrint;
	vector<Node*> toAdd;
	//toPrint.push_back(startNode);

	toAdd.push_back(startNode);
	while (!toAdd.empty()) {
		toPrint.push_back(toAdd[0]);
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
			for (int i = 0; i < (log(toPrint.size() + 1) / log(2) - _node->level + 1); i++) {
				cout << "  ";
			}
		}
		cout << _node->state.value;
		for (int i = 0; i < (log(toPrint.size() + 1)/log(2) - _node->level + 1); i++) {
			cout << " ";
		}
	}
}

int main() {
	srand(11);

	cout <<	"Minimax Program" << endl <<
			"Please Enter depth of tree: ";
	cin >> maxDepth;

	startNode = new Node();
	makeNode(startNode, 1);

	cout << "made nodes\n";
	PrintNodes();
	cout << endl;
	cout << endl;

	ABPrune(startNode, maxDepth + 1, INT_MIN, INT_MAX, true);
	system("pause");


	return 0;
}

Node::~Node()
{
	for (Node* _child : childs) {
		delete _child;
	}
	delete this;
}
