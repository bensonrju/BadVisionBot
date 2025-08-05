#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time() to seed srand()

//#define PROBLEN 8

#include "celltype.h"
#include "direction.h"
#include "localization.h"


using namespace std;

const float straightProb = 0.7;
const float LeftProb = 0.2;
const float RightProb = 0.1;

const float sensingWall = 0.95;
const float sensingWallMisid = 0.05;
const float sensingPath = 0.85;
const float sensingPathMisid = 0.15;

RCBounds bounds = {0, 5, 0, 6};

void initProb(vector<vector<Cell>>& maze);

void sensing(vector<vector<Cell>>& maze, int rows, int columns, vector<bool> sensory);
vector<bool> getReality(vector<vector<Cell>>& maze, int row, int col, int maxRow, int maxCol);
float computePresence(float prev, vector<bool> reality, vector<bool> sensory);

void movingProb(vector<vector<Cell>>& maze, int rows, int columns, Direction direction);

int main() {
	vector<vector<Cell>> maze = {
		{Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path()},
		{Cell::Path(),	Cell::Wall(),	Cell::Wall(),	Cell::Wall(),	Cell::Wall(),	Cell::Path()},
		{Cell::Path(),	Cell::Wall(),	Cell::Path(),	Cell::Goal(),	Cell::Wall(),	Cell::Path()},
		{Cell::Path(),	Cell::Wall(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path()},
		{Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path(),	Cell::Path()}
	};

	print(maze);

	int rows = maze.size(), columns = maze[0].size();
	vector<bool> sensory(4);
	vector<vector<Cell>> predictionMaze;

	if(boundsCheck(rows, columns, bounds)) {
		bounds.row_max = rows; bounds.col_max = columns;
	}

	// 0.	S1 Prior Probability
	initProb(maze);

	// 1.	S1 Posterior Probability
	sensory = {false, false, false, true};
	sensing(maze, rows, columns, sensory);

	// 2.	S2 Prior Probability
	movingProb(maze, rows, columns, Direction::North);

	// 3.	S2 Posterior Probability
	sensory = {true, false, false, false};
	sensing(maze, rows, columns, sensory);

	// 4.	S3 Prior Probability
	movingProb(maze, rows, columns, Direction::North);

	// 5.	S3 Posterior Probability
	sensory = {true, true, false, false};
	sensing(maze, rows, columns, sensory);

	// 6.	S4 Prior Probability
	movingProb(maze, rows, columns, Direction::East);

	// 7.	S4 Posterior Probability
	sensory = {false, true, true, false};
	sensing(maze, rows, columns, sensory);

	return 0;
}

void initProb(vector<vector<Cell>>& maze) {
	int rows = maze.size(), columns = maze[0].size();

	float prob = 1.0 / (countType(CellType::Path, maze) + 1);
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j)
			if (maze[i][j].type != CellType::Wall)
				maze[i][j].label = prob;
	
	cout << "Initial Location Probabilities" << endl;
	print(maze);
}

void sensing(vector<vector<Cell>>& maze, int rows, int columns, vector<bool> sensory) {
	float sum = 0;
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j)
			if (maze[i][j].type != CellType::Wall) {
				float nonNormalized = computePresence(
								maze[i][j].label,
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
		
		if(!within(new_row, new_col, bounds))
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
		float prob;
		/* =====+===== Posterior Calculation =====+===== */
		if (reality[i]) // Assuming S=(x, y)
			prob = sensory[i]
				  ? /* P(Zi = w | R = w) = 0.95 */ sensingWall
				  : /* P(Zi = ~w | R = w) = 0.05 */ sensingWallMisid;
		else
			prob = sensory[i]
				  ? /* P(Zi = ~w | R = ~w) = 0.85 */ sensingPathMisid
				  : /* P(Zi = w | R = ~w) = 0.15 */ sensingPath;

		cellProbability *= prob;
	}
	return cellProbability;
}

void movingProb(vector<vector<Cell>>& maze, int rows, int columns, Direction direction) {
	vector<vector<Cell>> predictionMaze = maze;
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j)
			predictionMaze[i][j].label = 0.0;

	vector<float> transitionProb = getTransitionProb(direction);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			if (maze[i][j].type == CellType::Wall)
				continue;

			for (int k = 0; k < NMOVES; ++k) {
				int newi = i + moves[k].row, newj = j + moves[k].col;
				float prob = transitionProb[k] * maze[i][j].label;

				if (!within(newi, newj, bounds) || maze[newi][newj].type == CellType::Wall)
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