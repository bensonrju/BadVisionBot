#pragma once

#include <vector>
#include <string.h>

#define NMOVES 4


#ifndef PROBLEN
#define PROBLEN 6
#endif

using namespace std;

struct Position {
	int row;
	int col;
};

Position moves[] = {
	{ 0, -1},   // West
	{-1,  0},   // North
	{ 0,  1},   // East
	{ 1,  0}    // South
};

enum class CellType { Path, Wall, Start, Goal };
struct Cell {
	CellType type;
	float label;

	Cell(CellType type, int label = 0) : type(type), label(label) {}
	Cell(int label) {
		this->type = CellType::Path;
		this->label = label;
	}
	static Cell Path() {
		return Cell(CellType::Path);
	}
	static Cell Wall() {
		return Cell(CellType::Wall);
	}
	static Cell Start() {
		return Cell(CellType::Start);
	}
	static Cell Goal() {
		return Cell(CellType::Goal);
	}
};

int countType(CellType type, const vector<vector<Cell>>& arr) {
	int counter = 0;
	for (int i = 0; i < (int)arr.size(); ++i)
		for (int j = 0; j < (int)arr[0].size(); ++j)
			if (arr[i][j].type == type)
				counter++;
	return counter;
}

int countValidBlocks(const vector<vector<Cell>>& arr) {
	return countType(CellType::Path, arr) 
		 + countType(CellType::Goal, arr);
}

Position locate(CellType type, const vector<vector<Cell>>& arr) {
	for (int i = 0; i < (int)arr.size(); ++i)
		for (int j = 0; j < (int)arr[0].size(); ++j)
			if (arr[i][j].type == type)
				return Position{i, j};
	// Could throw here if not found
	return Position{-1, -1};
}

string genWall() {
	int wallLen = PROBLEN - 1;
	string wall = "";
	for (int i = 0; i < wallLen; i++)
		wall += "#";

	return wall;
}

void print(const vector<vector<Cell>>& maze) {
	int rows = maze.size(), columns = maze[0].size();
	int DECPREC = PROBLEN - 4;
	for (int i = 0; i < rows; i++) {
		cout << "  ";
		for (int j = 0; j < columns; j++)
			if (maze[i][j].type == CellType::Wall)
				cout << setw(PROBLEN) << right << genWall();
			else
				cout << setw(PROBLEN) << right 
					 << fixed << setprecision(DECPREC) 
					 << maze[i][j].label * 100;
		cout << endl;
	}
	cout << endl;
}

