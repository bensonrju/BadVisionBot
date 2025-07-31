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

bool inside(int row, int col, int maxRow, int maxCol) {
	return (row >= 0 && row < maxRow && col >= 0 && col < maxCol);
}

vector<Position> moves = {
	{ 0, -1},   // West
	{-1,  0},   // North
	{ 0,  1},   // East
	{ 1,  0}    // South
};

void execute(vector<vector<Cell>>& maze, Position goal);

void sensing(vector<vector<Cell>>& maze, int rows, int columns, vector<bool> sensory);
vector<bool> getReality(vector<vector<Cell>>& maze, int row, int col, int maxRow, int maxCol);
float computePresence(float prev, vector<bool> reality, vector<bool> sensory);
inline void printSensory(vector<bool> sensory);

void movingProb(vector<vector<Cell>>& maze, int rows, int columns, Direction direction, int directions);
vector<float> getTransitionProb(Direction dir);
inline void printMovement(Direction dir);

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
	execute(maze, goal);
	return 0;
}

void execute(vector<vector<Cell>>& maze, Position goal) {
	int rows = maze.size(), columns = maze[0].size(), directions = moves.size();
	//[W,N,E,S]
	//false/0: path
	//true/1: wall

	vector<bool> sensory(4);
	vector<vector<Cell>> predictionMaze;
	//0. Get Initial Location Probabilities
	// S1 Prior Probability
	float prob = 1.0 / (countType(CellType::Path, maze) + 1);
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j)
			if (maze[i][j].type != CellType::Wall)
				maze[i][j].label = prob;
	cout << "Initial Location Probabilities" << endl;
	print(maze);

	//1. Sensing: [0, 0, 0, 1] //Filtering after Evidence
	// S1 Posterior Probability
	sensory = {false, false, false, true};
	sensing(maze, rows, columns, sensory);

	//2. Moving north-ward  //Windy Movement Probability Prediction
	// S2 Prior Probability
	movingProb(maze, rows, columns, Direction::North, directions);

	//3. Sensing: [1, 0, 0, 0] //Filtering after Evidence
	sensory = {true, false, false, false};
	sensing(maze, rows, columns, sensory);

	//4. Moving north-ward  //Windy Movement Probability Prediction
	movingProb(maze, rows, columns, Direction::North, directions);

	//5. Sensing: [1, 1, 0, 0] //Filtering after Evidence
	sensory = {true, true, false, false};
	sensing(maze, rows, columns, sensory);

	//6. Moving east-ward   //Windy Movement Probability Prediction
	movingProb(maze, rows, columns, Direction::East, directions);

	//7. Sensing: [0, 1, 1, 0] //Filtering after Evidence
	sensory = {false, true, true, false};
	sensing(maze, rows, columns, sensory);
}

void sensing(vector<vector<Cell>>& maze, int rows, int columns, vector<bool> sensory) {
	float sum = 0;
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j)
			if (maze[i][j].type != CellType::Wall) {
				float nonNormalized = computePresence(maze[i][j].label,
													  getReality(maze, i, j, rows, columns),
													  sensory);
				sum += nonNormalized;
				maze[i][j].label = nonNormalized;
			}
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j)
			if (maze[i][j].type != CellType::Wall) {
				float nonNormalized = maze[i][j].label;
				maze[i][j].label = nonNormalized / sum;
			}
	printSensory(sensory);
	print(maze);
}

vector<bool> getReality(vector<vector<Cell>>& maze, int row, int col, int maxRow, int maxCol) {
	vector<bool> reality(4);
	for (int i = 0; i < (int)reality.size(); i++) {
		int new_row = row + moves[i].row, new_col = col + moves[i].col;
		if (!inside(new_row, new_col, maxRow, maxCol))
			reality[i] = true;
		else
			reality[i] = (maze[new_row][new_col].type == CellType::Wall);
	}
	return reality;
}

float computePresence(float cellProbability, vector<bool> reality, vector<bool> sensory) {
	int length = (int)reality.size();
	if (length != (int)sensory.size())
		return -1;

	for (int i = 0; i < length; ++i) {
		float probability;
		/* =====+===== Posterior Calculation =====+===== */
		if (reality[i]) // Assuming S=(x, y)
			probability = sensory[i]
						  ? /* P(Zi = w | R = w) = 0.95 */ sensingWall
						  : /* P(Zi = ~w | R = w) = 0.05 */ sensingWallMisid;
		else
			probability = sensory[i]
						  ? /* P(Zi = ~w | R = ~w) = 0.85 */ sensingPathMisid
						  : /* P(Zi = w | R = ~w) = 0.15 */ sensingPath;

		cellProbability *= probability;
	}
	return cellProbability;
}

inline void printSensory(vector<bool> sensory) {
	cout << "Filtering after Evidence ["
		 << sensory[0] << ", " << sensory[1] << ", "
		 << sensory[2] << ", " << sensory[3] << "]" << endl;
}

void movingProb(vector<vector<Cell>>& maze, int rows, int columns, Direction direction, int directions) {
	vector<vector<Cell>> predictionMaze = maze;
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j)
			predictionMaze[i][j].label = 0.0;

	vector<float> transitionProb = getTransitionProb(direction);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			if (maze[i][j].type == CellType::Wall)
				continue;

			for (int k = 0; k < directions; ++k) {
				int newi = i + moves[k].row, newj = j + moves[k].col;
				float prob = transitionProb[k] * maze[i][j].label;

				if (!inside(newi, newj, rows, columns) || maze[newi][newj].type == CellType::Wall)
                    // Probability of bouncing is added to the initial/current cell
					predictionMaze[i][j].label += prob;
				else
                    // Probability of moving to neighboring cell is added to the neighboring cell
					predictionMaze[newi][newj].label += prob;
			}
		}
	}
	printMovement(direction);
	print(predictionMaze);
	maze = predictionMaze;
}

vector<float> getTransitionProb(Direction dir) {
	vector<float> directionProb(4);
	directionProb[dir.Value()] = 0.7;
	directionProb[dir.Left().Value()] = 0.2;
	directionProb[dir.Right().Value()] = 0.1;
	directionProb[dir.Opposite().Value()] = 0.0;
	return directionProb;
}

inline void printMovement(Direction dir) {
	cout << "Prediction after attempting to move "
		 << dir << endl;
}

void print(const vector<vector<Cell>>& maze) {
	int rows = maze.size(), columns = maze[0].size();
	for (int i = 0; i < rows; i++) {
		cout << "  ";
		for (int j = 0; j < columns; j++)
			if (maze[i][j].type == CellType::Wall)
				cout << setw(6) << right << "#####";
			else
				cout << setw(6) << right << fixed << setprecision(2) << maze[i][j].label * 100;
		cout << endl;
	}
	cout << endl;
}
