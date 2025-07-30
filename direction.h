#pragma once
#include <iostream>

enum {PATH, WALL};

struct Direction {
	enum class Directions { North, East, South, West };
	Directions value;

	constexpr Direction(Directions v) : value(v) {}
	Direction() {}

	Direction Left() const {
		// Cast value to int, add 3, mod 4, cast back to Direction
		return Direction((Directions)(((int)value + 3) % 4));
	}

	Direction Right() const {
		// (value + 1) % 4 is one step clockwise (right turn)
		return Direction((Directions)(((int)value + 1) % 4));
	}

	// Optional: helper for output
	const char* ToString() const {
		switch (value) {
			case Directions::North:
				return "North";
			case Directions::East:
				return "East";
			case Directions::South:
				return "South";
			case Directions::West:
				return "West";
			default:
				return "Unknown";
		}
	}

	static const Direction North;
	static const Direction East;
	static const Direction South;
	static const Direction West;
};

inline constexpr Direction Direction::North{Direction::Directions::North};
inline constexpr Direction Direction::East{Direction::Directions::East};
inline constexpr Direction Direction::South{Direction::Directions::South};
inline constexpr Direction Direction::West{Direction::Directions::West};

inline std::ostream& operator<<(std::ostream& os, const Direction& d) {
	return os << d.ToString();
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