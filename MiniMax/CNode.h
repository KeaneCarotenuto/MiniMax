#pragma once
#include <vector>

class CNode
{
public:
	//Value of current board state
	int value = NULL; 

	//Current board state
	char board[3][3] = { {0,0,0},
						{0,0,0},
						{0,0,0} };

	//How deep in the tree it is
	int level = NULL; 

	CNode* parent = nullptr;

	std::vector<CNode*> childs; //all of the possible steps
};