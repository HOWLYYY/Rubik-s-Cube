#include <ctime>
#include <iostream>
#include "Cube.h"

Cube::Cube() noexcept
{
	setState(defaultState);
}

Cube::Cube(const State& newState) noexcept
{
	setState(newState);
}

void Cube::setState(const State& newState) noexcept
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			_CurrentState.frontFaces[i][j] = newState.frontFaces[i][j];
			_CurrentState.backFaces[i][j] = newState.backFaces[i][j];
			_CurrentState.upFaces[i][j] = newState.upFaces[i][j];
			_CurrentState.downFaces[i][j] = newState.downFaces[i][j];
			_CurrentState.leftFaces[i][j] = newState.leftFaces[i][j];
			_CurrentState.rightFaces[i][j] = newState.rightFaces[i][j];
		}
}

const State& Cube::getState() const noexcept
{
	return _CurrentState;
}

void Cube::rotateFace(Color(&face)[3][3], const bool clockwise) noexcept
{
	Color buffer[3][3];
	if (clockwise)
	{
		for (int i = 0; i < 9; i++)
		{
			buffer[i / 3][i % 3] = face[i % 3][2 - i / 3];
		}
	}
	else
	{
		for (int i = 0; i < 9; i++)
		{
			buffer[i % 3][i / 3] = face[2 - i / 3][i % 3];
		}
	}
	for (int i = 0; i < 9; i++)
		face[i / 3][i % 3] = buffer[i / 3][i % 3];
}

void Cube::rotateCube(const Side& rotation) noexcept
{
	State bufferState;
	switch (rotation)
	{
	case Side::FS:
		rotateFace(_CurrentState.frontFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[2][i] = _CurrentState.rightFaces[i][0];
			bufferState.leftFaces[i][2] = _CurrentState.upFaces[2][2 - i];
			bufferState.downFaces[0][i] = _CurrentState.leftFaces[i][2];
			bufferState.rightFaces[i][0] = _CurrentState.downFaces[0][2 - i];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.upFaces[2][i] = bufferState.upFaces[2][i];
			_CurrentState.downFaces[0][i] = bufferState.downFaces[0][i];
			_CurrentState.leftFaces[i][2] = bufferState.leftFaces[i][2];
			_CurrentState.rightFaces[i][0] = bufferState.rightFaces[i][0];
		}
		break;
	case Side::F:
		rotateFace(_CurrentState.frontFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[2][i] = _CurrentState.leftFaces[2 - i][2];
			bufferState.leftFaces[i][2] = _CurrentState.downFaces[0][i];
			bufferState.downFaces[0][i] = _CurrentState.rightFaces[2 - i][0];
			bufferState.rightFaces[i][0] = _CurrentState.upFaces[2][i];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.upFaces[2][i] = bufferState.upFaces[2][i];
			_CurrentState.downFaces[0][i] = bufferState.downFaces[0][i];
			_CurrentState.leftFaces[i][2] = bufferState.leftFaces[i][2];
			_CurrentState.rightFaces[i][0] = bufferState.rightFaces[i][0];
		}
		break;
	case Side::B:
		rotateFace(_CurrentState.backFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[0][i] = _CurrentState.rightFaces[i][2];
			bufferState.leftFaces[i][0] = _CurrentState.upFaces[0][2 - i];
			bufferState.downFaces[2][i] = _CurrentState.leftFaces[i][0];
			bufferState.rightFaces[i][2] = _CurrentState.downFaces[2][2 - i];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.upFaces[0][i] = bufferState.upFaces[0][i];
			_CurrentState.downFaces[2][i] = bufferState.downFaces[2][i];
			_CurrentState.leftFaces[i][0] = bufferState.leftFaces[i][0];
			_CurrentState.rightFaces[i][2] = bufferState.rightFaces[i][2];
		}
		break;
	case Side::BS:
		rotateFace(_CurrentState.backFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[0][i] = _CurrentState.leftFaces[2 - i][0];
			bufferState.leftFaces[i][0] = _CurrentState.downFaces[2][i];
			bufferState.downFaces[2][i] = _CurrentState.rightFaces[2 - i][2];
			bufferState.rightFaces[i][2] = _CurrentState.upFaces[0][i];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.upFaces[0][i] = bufferState.upFaces[0][i];
			_CurrentState.downFaces[2][i] = bufferState.downFaces[2][i];
			_CurrentState.leftFaces[i][0] = bufferState.leftFaces[i][0];
			_CurrentState.rightFaces[i][2] = bufferState.rightFaces[i][2];
		}
		break;
	case Side::U:
		rotateFace(_CurrentState.upFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.frontFaces[0][i] = _CurrentState.rightFaces[0][i];
			bufferState.rightFaces[0][i] = _CurrentState.backFaces[0][i];
			bufferState.backFaces[0][i] = _CurrentState.leftFaces[0][i];
			bufferState.leftFaces[0][i] = _CurrentState.frontFaces[0][i];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.frontFaces[0][i] = bufferState.frontFaces[0][i];
			_CurrentState.rightFaces[0][i] = bufferState.rightFaces[0][i];
			_CurrentState.backFaces[0][i] = bufferState.backFaces[0][i];
			_CurrentState.leftFaces[0][i] = bufferState.leftFaces[0][i];
		}
		break;
	case Side::US:
		rotateFace(_CurrentState.upFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.frontFaces[0][i] = _CurrentState.leftFaces[0][i];
			bufferState.rightFaces[0][i] = _CurrentState.frontFaces[0][i];
			bufferState.backFaces[0][i] = _CurrentState.rightFaces[0][i];
			bufferState.leftFaces[0][i] = _CurrentState.backFaces[0][i];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.frontFaces[0][i] = bufferState.frontFaces[0][i];
			_CurrentState.rightFaces[0][i] = bufferState.rightFaces[0][i];
			_CurrentState.backFaces[0][i] = bufferState.backFaces[0][i];
			_CurrentState.leftFaces[0][i] = bufferState.leftFaces[0][i];
		}
		break;
	case Side::D:
		rotateFace(_CurrentState.downFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.frontFaces[2][i] = _CurrentState.leftFaces[2][i];
			bufferState.rightFaces[2][i] = _CurrentState.frontFaces[2][i];
			bufferState.backFaces[2][i] = _CurrentState.rightFaces[2][i];
			bufferState.leftFaces[2][i] = _CurrentState.backFaces[2][i];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.frontFaces[2][i] = bufferState.frontFaces[2][i];
			_CurrentState.rightFaces[2][i] = bufferState.rightFaces[2][i];
			_CurrentState.backFaces[2][i] = bufferState.backFaces[2][i];
			_CurrentState.leftFaces[2][i] = bufferState.leftFaces[2][i];
		}
		break;
	case Side::DS:
		rotateFace(_CurrentState.downFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.frontFaces[2][i] = _CurrentState.rightFaces[2][i];
			bufferState.rightFaces[2][i] = _CurrentState.backFaces[2][i];
			bufferState.backFaces[2][i] = _CurrentState.leftFaces[2][i];
			bufferState.leftFaces[2][i] = _CurrentState.frontFaces[2][i];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.frontFaces[2][i] = bufferState.frontFaces[2][i];
			_CurrentState.rightFaces[2][i] = bufferState.rightFaces[2][i];
			_CurrentState.backFaces[2][i] = bufferState.backFaces[2][i];
			_CurrentState.leftFaces[2][i] = bufferState.leftFaces[2][i];
		}
		break;
	case Side::RS:
		rotateFace(_CurrentState.rightFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[i][2] = _CurrentState.backFaces[2 - i][0];
			bufferState.backFaces[i][0] = _CurrentState.downFaces[2 - i][2];
			bufferState.downFaces[i][2] = _CurrentState.frontFaces[i][2];
			bufferState.frontFaces[i][2] = _CurrentState.upFaces[i][2];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.upFaces[i][2] = bufferState.upFaces[i][2];
			_CurrentState.downFaces[i][2] = bufferState.downFaces[i][2];
			_CurrentState.frontFaces[i][2] = bufferState.frontFaces[i][2];
			_CurrentState.backFaces[i][0] = bufferState.backFaces[i][0];
		}
		break;
	case Side::R:
		rotateFace(_CurrentState.rightFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[i][2] = _CurrentState.frontFaces[i][2];
			bufferState.backFaces[i][0] = _CurrentState.upFaces[2 - i][2];
			bufferState.downFaces[i][2] = _CurrentState.backFaces[2 - i][0];
			bufferState.frontFaces[i][2] = _CurrentState.downFaces[i][2];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.upFaces[i][2] = bufferState.upFaces[i][2];
			_CurrentState.downFaces[i][2] = bufferState.downFaces[i][2];
			_CurrentState.frontFaces[i][2] = bufferState.frontFaces[i][2];
			_CurrentState.backFaces[i][0] = bufferState.backFaces[i][0];
		}
		break;
	case Side::LS:
		rotateFace(_CurrentState.leftFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[i][0] = _CurrentState.frontFaces[i][0];
			bufferState.backFaces[i][2] = _CurrentState.upFaces[2 - i][0];
			bufferState.downFaces[i][0] = _CurrentState.backFaces[2 - i][2];
			bufferState.frontFaces[i][0] = _CurrentState.downFaces[i][0];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.upFaces[i][0] = bufferState.upFaces[i][0];
			_CurrentState.downFaces[i][0] = bufferState.downFaces[i][0];
			_CurrentState.frontFaces[i][0] = bufferState.frontFaces[i][0];
			_CurrentState.backFaces[i][2] = bufferState.backFaces[i][2];
		}
		break;
	case Side::L:
		rotateFace(_CurrentState.leftFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[i][0] = _CurrentState.backFaces[2 - i][2];
			bufferState.backFaces[i][2] = _CurrentState.downFaces[2 - i][0];
			bufferState.downFaces[i][0] = _CurrentState.frontFaces[i][0];
			bufferState.frontFaces[i][0] = _CurrentState.upFaces[i][0];
		}
		for (int i = 0; i < 3; i++)
		{
			_CurrentState.upFaces[i][0] = bufferState.upFaces[i][0];
			_CurrentState.downFaces[i][0] = bufferState.downFaces[i][0];
			_CurrentState.frontFaces[i][0] = bufferState.frontFaces[i][0];
			_CurrentState.backFaces[i][2] = bufferState.backFaces[i][2];
		}
		break;
	default:
		break;
	}
}

bool Cube::isClockwise(const Side& action) const noexcept
{
	return (action == Side::U) || (action == Side::D) || (action == Side::F) || (action == Side::B) || (action == Side::L) || (action == Side::R);
}

Side Cube::reverseDirection(const Side& action) const  noexcept {
	int number = static_cast<int>(action);
	if (number > 5)
	{
		return static_cast<Side>(number - 6);
	}
	else
	{
		return static_cast<Side>(number + 6);
	}
}

Side Cube::invertSide(const Side& value) const noexcept {
	int number = static_cast<int>(value) ^ 1;
	return static_cast<Side>(number);
}

void Cube::randomize() noexcept
{
	srand(time(0));
	unsigned int rotation;
	setState(defaultState);
	for (int i = 0; i < 100; i++)
	{
		rotation = rand() % 13;
		rotateCube(static_cast<Side>(rotation));
	}
}

Side Cube::rotateRotationSide(const Side& value, const Side& rotor) const noexcept {
	Side buffer = value;
	if (!isClockwise(value))
		buffer = reverseDirection(buffer);
	switch (rotor)
	{
	case Side::R:
		switch (buffer)
		{
		case Side::F:
			buffer = Side::R;
			break;
		case Side::R:
			buffer = Side::B;
			break;
		case Side::B:
			buffer = Side::L;
			break;
		case Side::L:
			buffer = Side::F;
			break;
		}
		break;
	case Side::L:
		switch (buffer) {
		case Side::L:
		case Side::R:
		case Side::F:
		case Side::B:
			buffer = invertSide(rotateRotationSide(buffer, Side::R));
			break;
		}
		break;
	case Side::U:
		switch (buffer)
		{
		case Side::F:
			buffer = Side::U;
			break;
		case Side::U:
			buffer = Side::B;
			break;
		case Side::B:
			buffer = Side::D;
			break;
		case Side::D:
			buffer = Side::F;
			break;
		}
		break;
	case Side::D:
		switch (buffer)
		{
		case Side::U:
		case Side::B:
		case Side::D:
		case Side::F:
			buffer = invertSide(rotateRotationSide(buffer, Side::U));
			break;
		}
		break;
	case Side::B:
		switch (buffer)
		{
		case Side::L:
		case Side::R:
		case Side::F:
		case Side::B:
			buffer = invertSide(buffer);
			break;
		}
		break;
	default:
		break;
	}
	if (!isClockwise(value))
		buffer = reverseDirection(buffer);

	return buffer;
}

std::vector<Side> Cube::rotateRotationSide(const std::vector<Side>& sequence, const Side& rotor) const noexcept {
	std::vector<Side> result;
	Side buffer;
	for (int i = 0; i < sequence.size(); i++)
	{
		buffer = rotateRotationSide(sequence[i], rotor);
		result.push_back(buffer);
	}
	return result;
}

