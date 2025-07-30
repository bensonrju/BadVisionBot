#pragma once
#include <iostream>
#include <string>



enum Direction {WEST, NORTH, EAST, SOUTH};

inline std::ostream& operator<<(std::ostream& os, const Direction& d) {
	string cardDirs[] = {"West", "North", "East", "South"};
	return os << cardDirs[d];
}

/*
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
*/
