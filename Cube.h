#pragma once
#include <vector>
#include <string>
enum class Color {
	RED = 0,
	GREEN = 1,
	YELLOW = 2,
	BLUE = 3,
	WHITE = 4,
	ORANGE = 5,
	DEFAULT = 6
};

enum class Side {
	F = 0, B = 1, U = 2, D = 3, L = 4, R = 5,
	FS = 6, BS = 7, US = 8, DS = 9, LS = 10, RS = 11,
	NO = 12
};

struct State {
	Color frontFaces[3][3];
	Color backFaces[3][3];
	Color upFaces[3][3];
	Color downFaces[3][3];
	Color leftFaces[3][3];
	Color rightFaces[3][3];
};

const State defaultState = {
	 {
	 {Color::WHITE, Color::WHITE, Color::WHITE}, //front
	 {Color::WHITE, Color::WHITE, Color::WHITE},
	 {Color::WHITE, Color::WHITE, Color::WHITE}
	 },
	 {
	 {Color::YELLOW, Color::YELLOW, Color::YELLOW}, //back
	 {Color::YELLOW, Color::YELLOW, Color::YELLOW},
	 {Color::YELLOW, Color::YELLOW, Color::YELLOW}
	 },
	 {
	 {Color::GREEN, Color::GREEN, Color::GREEN}, //up
	 {Color::GREEN, Color::GREEN, Color::GREEN},
	 {Color::GREEN, Color::GREEN, Color::GREEN}
	 },
	 {
	 {Color::BLUE, Color::BLUE, Color::BLUE}, //down
	 {Color::BLUE, Color::BLUE, Color::BLUE},
	 {Color::BLUE, Color::BLUE, Color::BLUE}
	 },
	 {
	 {Color::ORANGE, Color::ORANGE, Color::ORANGE}, //left
	 {Color::ORANGE, Color::ORANGE, Color::ORANGE},
	 {Color::ORANGE, Color::ORANGE, Color::ORANGE}
	 },
	 {
	 {Color::RED, Color::RED, Color::RED}, //right
	 {Color::RED, Color::RED, Color::RED},
	 {Color::RED, Color::RED, Color::RED}
	 }
};

class Cube {
public:
	Cube() noexcept;
	Cube(const State& newState) noexcept;
	void setState(const State& newState) noexcept;
	const State& getState() const noexcept;
	void rotateCube(const Side& rotation) noexcept;
	void randomize() noexcept;
	std::vector<Side> rotateRotationSide(const std::vector<Side>& sequence, const Side& rotor) const noexcept;
	bool isClockwise(const Side& action) const noexcept;
	Side reverseDirection(const Side& action) const noexcept;
	Side invertSide(const Side& value) const noexcept;
protected:
	State _CurrentState;
private:
	void rotateFace(Color(&face)[3][3], bool clockwise) noexcept;
	Side rotateRotationSide(const Side& value, const Side& rotor) const noexcept;
};