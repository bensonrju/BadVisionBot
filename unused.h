#ifndef UNUSED_H_INCLUDED
#define UNUSED_H_INCLUDED

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


Direction windyMove(Direction intendedDirection) {
	int chance = (rand() % 100) + 1; // 1 to 100

	// 70% to move intended direction
	if (chance <= 70)
		return intendedDirection;

	// 20% to move left of intended direction
	else if (chance <= 90)
		return intendedDirection.Left();

	// 10% to move right of intended direction
	else
		return intendedDirection.Right();
}


#endif // UNUSED_H_INCLUDED


void execute(vector<vector<Cell>>& maze) {
	int rows = maze.size(), columns = maze[0].size(), directions = NMOVES;
	//[W,N,E,S]
	//false/0: path
	//true/1: wall

	vector<bool> sensory(4);
	vector<vector<Cell>> predictionMaze;
	//0. Get Initial Location Probabilities
	// S1 Prior Probability

	initProb(maze);

	//cout << "Initial Location Probabilities" << endl;
	//print(maze);

	//1. Sensing: [0, 0, 0, 1] //Filtering after Evidence
	// S1 Posterior Probability
	sensory = {false, false, false, true};
	sensing(maze, rows, columns, sensory);

	//2. Moving north-ward  //Windy Movement Probability Prediction
	// S2 Prior Probability
	movingProb(maze, rows, columns, Direction::North);

	//3. Sensing: [1, 0, 0, 0] //Filtering after Evidence
	sensory = {true, false, false, false};
	sensing(maze, rows, columns, sensory);

	//4. Moving north-ward  //Windy Movement Probability Prediction
	movingProb(maze, rows, columns, Direction::North);

	//5. Sensing: [1, 1, 0, 0] //Filtering after Evidence
	sensory = {true, true, false, false};
	sensing(maze, rows, columns, sensory);

	//6. Moving east-ward   //Windy Movement Probability Prediction
	movingProb(maze, rows, columns, Direction::East);

	//7. Sensing: [0, 1, 1, 0] //Filtering after Evidence
	sensory = {false, true, true, false};
	sensing(maze, rows, columns, sensory);
}