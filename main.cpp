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
	//cout << "Goal: " << goal.x << ",  " << goal.y << endl;

	execute(maze, goal);
	//cout << Direction::East.Left() << endl;
	return 0;
}

vector<Position> moves = {
	//  col, row
	//   x,  y
	{-1,  0},   // West
	{ 0, -1},   // North
	{ 1,  0},   // East
	{ 0,  1}    // South
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

	/* =====+===== Posterior Calculation =====+===== */

	for (int i = 0; i < length; ++i) {
		float probability;
		if (reality[i]) {	// Assuming S=(x, y)
			if(sensory[i]) 	// P( Zi=  w | R=  w ) = 0.95
				probability = sensingWall;
			else			// P( Zi= ~w | R=  w ) = 0.05
				probability = sensingPathMisid;
		}
		else {
			if(!sensory[i]) // P( Zi= ~w | R= ~w ) = 0.85
				probability = sensingPath;
			else			// P( Zi=  w | R= ~w ) = 0.15
				probability = sensingPathMisid;
		}

		cellProbability *= probability;
	}
	return cellProbability;
}

vector<float> transitionProb(Direction dir) {
	vector<vector<float>> directionProb = {
		{0.7, 0.1, 0, 0.2},	// West
		{0.2, 0.7, 0.1, 0},	// North
		{0, 0.2, 0.7, 0.1},	// East
		{0.0, 0, 0.2, 0.7}	// South
	};
	return directionProb[dir];
}


void execute(vector<vector<Cell>>& maze, Position goal) {
	int yBound = maze.size(), xBound = maze[0].size();
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

	//2. Moving north-ward  //Windy Movement Probability
	for (int i = 0; i < (int)maze.size(); ++i)
		for (int j = 0; j < (int)maze[0].size(); ++j) {
			float prob;
			for (int k = 0; k < 4; ++k) {
				int new_x = j + moves[i].x, new_y = i + moves[i].y;
				if (!inside(new_x, new_y, xBound, yBound))
					prob += maze[i][j].label /* * transition_prob(to neighbor) */;
				else
					prob += maze[new_y][new_x].label /* * transition_prob(to neighbor) */;
			}
			maze[i][j].label *= prob;
		}
	/*
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
