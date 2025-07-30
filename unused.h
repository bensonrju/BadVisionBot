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
