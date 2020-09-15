#pragma once
#include <thread>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include "Cube.h"
#include "CubeIO.h"

const GLfloat LITTLE_SIZE = 0.25;
const float GAP = 0.01;
const GLfloat CUBE_SPEED = 1;
const GLfloat DEFAULT_EDGE_SPEED = 3;
const GLfloat FAST_SOLVING_EDGE_SPEED = 15;

enum class MenuAction {
	NO_ACTION,
	SOLVE,
	READ,
	SAVE,
	ROTATE,
	RANDOMIZE,
	RESET,
	CHECK
};

class Cube3D : public Cube {
public:
	Cube3D();
	~Cube3D();
	void setState(const State& newState) noexcept;
	void setState(volatile const State& newState) noexcept;
	void setEdgeSpeed(const float& speed) noexcept;
	void rotateCube(Side rotation) noexcept;
	void resetMenuAction() noexcept;
	void randomize() noexcept;
	MenuAction getMenuAction() const noexcept;
private:
	std::thread threadDrawing;
	void Update3DState() noexcept;
	void UpdateState() noexcept;

};