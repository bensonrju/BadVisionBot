#pragma once

#include <vector>

using namespace std;

struct Position {
	int row;
	int col;
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

Position locate(CellType type, const vector<vector<Cell>>& arr) {
	for (int i = 0; i < (int)arr.size(); ++i)
		for (int j = 0; j < (int)arr[0].size(); ++j)
			if (arr[i][j].type == type)
				return Position{i, j};
	// Could throw here if not found
	return Position{-1, -1};
}
