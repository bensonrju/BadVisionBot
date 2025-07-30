#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time() to seed srand()

#include "celltype.h"
#include "direction.h"

using namespace std;



const float straightProb = 0.7;
const float LeftProb = 0.2;
const float RightProb = 0.1;

const float sensingPath = 0.85;
const float sensingWall = 0.95;


bool inside(int x, int y, int max_x, int max_y) {
	return (x >= 0 && x < max_x && y >= 0 && y < max_y);
}

void execute(vector<vector<Cell>>& maze, Position goal);
void print(const vector<vector<Cell>>& maze);

int main() {
	srand(time(0));
	vector<vector<Cell>> maze = {
		{Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path()},
		{Cell::Path(),	Cell::Wall(),	Cell::Wall(),	Cell::Wall(),	Cell::Wall(),	Cell::Path()},
		{Cell::Path(),	Cell::Wall(),	Cell::Path(),	Cell::Goal(),	Cell::Wall(),	Cell::Path()},
		{Cell::Path(),	Cell::Wall(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path()},
		{Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path()}
	};
	print(maze);
	//Position start = locate(CellType::Start, maze);
	Position goal = locate(CellType::Goal, maze);
	cout << "Goal: " << goal.x << ",  " << goal.y << endl;
	execute(maze, goal);
	cout << Direction::East.Left() << endl;
	return 0;
}

vector<bool> sensing(vector<bool>& sensed) {
	int size = (int)sensed.size();
	vector<bool> actuallySensed(size);
	for (int i = 0; i < size; ++i) {
		int chance = (rand() % 100) + 1; // 1 to 100
		bool evaluated = sensed[i] ? chance <= (1 - sensingPath) * 100 : chance <= (1 - sensingWall) * 100;
		if (evaluated)
			actuallySensed[i] = !sensed[i];
		else
			actuallySensed[i] = sensed[i];
	}
	return actuallySensed;
}

void execute(vector<vector<Cell>>& maze, Position goal) {
	return;

	//[W,N,E,S]
	//true: path
	//false: wall

	Direction movedDirection;
	vector<bool> sensed(4);
	//0. Get Initial Location Probabilities
	float prob = 1 / countType(CellType::Path, maze);
	for (int i = 0; i < (int)maze.size(); ++i)
		for (int j = 0; j < (int)maze[0].size(); ++j)
			if (maze[i][j].type == CellType::Path)
				maze[i][j].label = prob;

	/*
	//1. Sensing: [0,0,0,1] //Filtering after Evidence
	sensed = sensing(true, true, true, false);

	//2. Moving north-ward  //Windy Movement Probability
	movedDirection = windyMove(Direction::North);

	//3. Sensing: [1,0,0,0] //Filtering after Evidence
	sensed = sensing(false, true, true, true);

	//4. Moving north-ward  //Windy Movement Probability
	movedDirection = windyMove(Direction::North);

	//5. Sensing: [1,1,0,0] //Filtering after Evidence
	sensed = sensing(false, false, true, true);

	//6. Moving east-ward   //Windy Movement Probability
	movedDirection = windyMove(Direction::East);

	//7. Sensing: [0,1,1,0] //Filtering after Evidence
	sensed = sensing(true, false, false, true);
	*/
}

void print(const vector<vector<Cell>>& maze) {
	int rows = maze.size(), columns = maze[0].size();
	cout << endl;
	for (int i = 0; i < rows; i++) {
		cout << "  ";
		for (int j = 0; j < columns; j++) {
			const Cell& temp = maze[i][j];
			if (temp.type == CellType::Wall)
				cout << setw(6) << right << "#####";

			else {
				// Print probability (label) formatted as "--.--"
				cout << setw(6) << right << fixed << setprecision(2) << temp.label;
			}
		}
		cout << endl;
	}
}
