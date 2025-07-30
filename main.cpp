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

const float sensingWall = 0.95;
const float sensingWallMisid = 0.05;
const float sensingPath = 0.85;
const float sensingPathMisid = 0.15;

bool inside(int x, int y, int max_x, int max_y) {
	return (x >= 0 && x < max_x && y >= 0 && y < max_y);
}

vector<bool> sensing(vector<bool>& sensed);
void execute(vector<vector<Cell>>& maze, Position goal);
float computePresence(float prev, vector<bool> reality, vector<bool> sensory);
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
	Position goal = locate(CellType::Goal, maze);
	cout << "Goal: " << goal.x << ",  " << goal.y << endl;

	execute(maze, goal);
	//cout << Direction::East.Left() << endl;
	return 0;
}
/*
    vector<bool> sensing(vector<bool>& sensed) {
	int size = (int)sensed.size();
	vector<bool> actuallySensed(size);
	int chance;

	for (int i = 0; i < size; ++i) {
		chance = (rand() % 100) + 1; // 1 to 100
		bool evaluated = sensed[i] ? chance <= (1 - sensingPath) * 100 : chance <= (1 - sensingWall) * 100;

		if (evaluated)
			actuallySensed[i] = !sensed[i];
		else
			actuallySensed[i] = sensed[i];
	}

	return actuallySensed;
    }
*/

vector<Position> moves = {
	{0,	-1},    // West
	{-1, 0},    // North
	{0,	1},     // East
	{1,	0},     // South
};

vector<bool> getReality(vector<vector<Cell>>& maze, int x, int y) {
	vector<bool> reality(4);
	int yBound = maze.size(), xBound = maze[0].size();
	for (int i = 0; i < (int)reality.size(); i++) {
		int new_x = x + moves[i].x, new_y = y + moves[i].y;
		if (!inside(new_x, new_y, xBound, yBound))
			reality[i] = true;
        else
            reality[i] = (maze[new_y][new_x].type == CellType::Wall);
	}
	return reality;
}

float computePresence(float cellProbability, vector<bool> reality, vector<bool> sensory) {
	int length = (int)reality.size();
	if (length != (int)sensory.size())
		return -1;

	for (int i = 0; i < length; ++i) {
		float probability;
		if (reality[i] == sensory[i])
            probability = reality[i] ? sensingWall : sensingWallMisid;
		else
			probability = reality[i] ? sensingPathMisid: sensingPath;
		cellProbability *= probability;
	}
	return cellProbability;
}

void execute(vector<vector<Cell>>& maze, Position goal) {
	//return;

	//[W,N,E,S]
	//false/0: path
	//true/1: wall

	Direction movedDirection;
	vector<bool> sensory(4);
	//0. Get Initial Location Probabilities
	float prob = 1.0 / (countType(CellType::Path, maze) + 1);
	for (int i = 0; i < (int)maze.size(); ++i)
		for (int j = 0; j < (int)maze[0].size(); ++j)
			if (maze[i][j].type != CellType::Wall)
				maze[i][j].label = prob;
	cout << "Initial Location Probabilities" << endl;
	print(maze);

	//1. Sensing: [0,0,0,1] //Filtering after Evidence
	sensory = {false, false, false, true};
	float sum = 0;
	for (int i = 0; i < (int)maze.size(); ++i)
		for (int j = 0; j < (int)maze[0].size(); ++j)
			if (maze[i][j].type != CellType::Wall) {
				float nonNormalized = computePresence(maze[i][j].label, getReality(maze, j, i), sensory);
				sum += nonNormalized;
				maze[i][j].label = nonNormalized;
			}

	for (int i = 0; i < (int)maze.size(); ++i)
		for (int j = 0; j < (int)maze[0].size(); ++j)
			if (maze[i][j].type != CellType::Wall) {
				float nonNormalized = maze[i][j].label;
				maze[i][j].label = nonNormalized / sum;
			}

	cout << "Filtering after Evidence [0, 0, 0, 1]" << endl;
	print(maze);
	return;
	/*
	    //2. Moving north-ward  //Windy Movement Probability
	    movedDirection = windyMove(Direction::North);

	    //3. Sensing: [1,0,0,0] //Filtering after Evidence
	    sensory = {false, true, true, true};

	    //4. Moving north-ward  //Windy Movement Probability
	    movedDirection = windyMove(Direction::North);

	    //5. Sensing: [1,1,0,0] //Filtering after Evidence
	    sensory = {false, false, true, true};

	    //6. Moving east-ward   //Windy Movement Probability
	    movedDirection = windyMove(Direction::East);

	    //7. Sensing: [0,1,1,0] //Filtering after Evidence
	    sensory = {true, false, false, true};
	*/
}

void print(const vector<vector<Cell>>& maze) {
	int rows = maze.size(), columns = maze[0].size();
	for (int i = 0; i < rows; i++) {
		cout << "  ";
		for (int j = 0; j < columns; j++) {
			const Cell& temp = maze[i][j];
			if (temp.type == CellType::Wall)
				cout << setw(6) << right << "#####";
			else
				// Print probability (label) formatted as "--.--"
				cout << setw(6) << right << fixed << setprecision(2) << temp.label * 100;
		}
		cout << endl;
	}
	cout << endl;
}
