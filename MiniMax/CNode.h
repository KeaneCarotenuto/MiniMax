// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// 
// (c) 2021 Media Design School
//
// File Name   : CNode.h
// Description : Node class for tree construction
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

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