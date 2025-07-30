#pragma once
#include <iostream>

struct Direction {
	enum class Directions { West = 0, North, East, South };

	// Provide a default value to prevent uninitialized 'value'
	constexpr Direction() : value(West) {}
	constexpr Direction(Directions v) : value(v) {}

	//explicit operator bool() const = delete;

	// Allows comparisons with Enumerator constants.
	constexpr operator Directions() const {
		return value;
	}

	Directions getDirection() const {
        return value;
	}

    int Value() const {
        return (int)value;
    }

	Direction Right() const {
		// (value + 1) % 4 is one step clockwise (right turn)
		return Direction((Directions)(((int)value + 1) % 4));
	}

	Direction Opposite() const {
		// (value + 2) % 4 is backwards (opposite)
		return Direction((Directions)(((int)value + 2) % 4));
	}

	Direction Left() const {
		// (value + 3) % 4 is one step counter-clockwise (left turn)
		return Direction((Directions)(((int)value + 3) % 4));
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

  private:
	Directions value;
};

inline constexpr Direction Direction::North{Direction::Directions::North};
inline constexpr Direction Direction::East{Direction::Directions::East};
inline constexpr Direction Direction::South{Direction::Directions::South};
inline constexpr Direction Direction::West{Direction::Directions::West};

inline std::ostream& operator<<(std::ostream& os, const Direction& d) {
	return os << d.ToString();
}
