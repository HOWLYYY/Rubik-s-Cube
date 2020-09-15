#include "Solver.h"

void Solver::generateSolvingSequence() {
	ActionsTaken.clear();
	placeEdges();
	placeCorners_Up();
	placeDownEdges();
	makeGoodDownCross();
	placeDownCorners();
	rotateDownCorners();
	optimiseActionVector();
}

void Solver::solve(Cube& toSolve)
{
	_CurrentState = toSolve.getState();
	try {
		generateSolvingSequence();
	}
	catch (const std::exception& error)
	{
		ActionsTaken.clear();
		throw std::exception("Error: This cube is unsolvable.");
	}
	if (!is_solved())
		throw std::exception("After algorithm cube still not solved!");
	for (int i = 0; i < ActionsTaken.size(); i++)
		toSolve.rotateCube(ActionsTaken[i]);
}

bool Solver::is_solvable(Cube& toTry) {
	_CurrentState = toTry.getState();
	try {
		generateSolvingSequence();
	}
	catch (const std::exception&) {
		return false;
	}
	return true;
}

void Solver::solve(Cube3D& toSolve)
{
	_CurrentState = toSolve.getState();
	toSolve.setEdgeSpeed(FAST_SOLVING_EDGE_SPEED);
	try {
		generateSolvingSequence();
	}
	catch (const std::exception& error)
	{
		ActionsTaken.clear();
		throw std::exception("This cube is unsolvable.");
	}
	if (!is_solved())
		throw std::exception("After algorithm cube still not solved");
	for (int i = 0; i < ActionsTaken.size(); i++)
		toSolve.rotateCube(ActionsTaken[i]);

	toSolve.setEdgeSpeed(DEFAULT_EDGE_SPEED);
}


struct pair_hash {
	inline std::size_t operator()(const std::pair<int, int>& v) const {
		return v.first * 31 + v.second;
	}
};

bool Solver::is_solved() const noexcept
{
	if (!is_SideSolved(_CurrentState.frontFaces))
		return false;
	if (!is_SideSolved(_CurrentState.backFaces))
		return false;
	if (!is_SideSolved(_CurrentState.upFaces))
		return false;
	if (!is_SideSolved(_CurrentState.downFaces))
		return false;
	if (!is_SideSolved(_CurrentState.leftFaces))
		return false;
	if (!is_SideSolved(_CurrentState.rightFaces))
		return false;
	return true;
}

std::vector<Side> Solver::getActionsToSolve()
{
	return ActionsTaken;
}

void Solver::optimiseActionVector()
{
	for (int i = 2; i < ActionsTaken.size(); i++)
	{
		if ((ActionsTaken[i] == ActionsTaken[i - 1]) && (ActionsTaken[i] == ActionsTaken[i - 2]))
		{
			ActionsTaken.erase(ActionsTaken.begin() + i - 2, ActionsTaken.begin() + i);
			ActionsTaken[i - 2] = reverseDirection(ActionsTaken[i - 2]);
		}
		if (i >= ActionsTaken.size())
			if (ActionsTaken[i - 1] == reverseDirection(ActionsTaken[i]))
			{
				ActionsTaken.erase(ActionsTaken.begin() + i - 1, ActionsTaken.begin() + i + 1);
			}
	}
}

void Solver::takeActionVector(const std::vector<Side>& sequence) noexcept {
	for (int i = 0; i < sequence.size(); i++)
	{
		rotateCube(sequence[i]);
		ActionsTaken.push_back(sequence[i]);
	}
}

bool Solver::is_SideSolved(const Color(&face)[3][3]) const noexcept
{
	for (int i = 0; i < 9; i++)
		if (face[i / 3][i % 3] != face[1][1])
			return false;

	return true;
}

std::vector<int> Solver::countBadUpAngles(const State& _CurrentState) const
{
	std::vector<int> result;
	if (_CurrentState.frontFaces[1][1] != _CurrentState.frontFaces[0][1])
		result.push_back(0);
	if (_CurrentState.rightFaces[1][1] != _CurrentState.rightFaces[0][1])
		result.push_back(1);
	if (_CurrentState.backFaces[1][1] != _CurrentState.backFaces[0][1])
		result.push_back(2);
	if (_CurrentState.leftFaces[1][1] != _CurrentState.leftFaces[0][1])
		result.push_back(3);
	return result;
}

void Solver::placeEdges()
{
	int angles_inPlace = 0;
	Color upColor = _CurrentState.upFaces[1][1];
	std::pair<Side, Side> lastEdge = locateEdge(upColor);
	if (_CurrentState.upFaces[0][1] == upColor)
		angles_inPlace++;
	if (_CurrentState.upFaces[2][1] == upColor)
		angles_inPlace++;
	if (_CurrentState.upFaces[1][2] == upColor)
		angles_inPlace++;
	if (_CurrentState.upFaces[1][0] == upColor)
		angles_inPlace++;
	int cycleCounter = 0;
	while (angles_inPlace < 4)
	{
		cycleCounter++;
		if (cycleCounter > actions_toBreak)
		{
			throw std::exception("Problem placing up edges");
		}
		lastEdge = locateEdge(upColor);
		switch (lastEdge.first)
		{
		case Side::L:
		case Side::R:
		case Side::B:
		case Side::F:
			switch (lastEdge.second)
			{
			case Side::U:
				takeActionVector(rotateRotationSide(INVERT_INPLACE_MIDDLE, lastEdge.first));
				angles_inPlace++;
				break;
			case Side::D:
				takeActionVector(rotateRotationSide(GET_DOWN_ANGLE, lastEdge.first));
				break;
			case Side::L:
				takeActionVector(rotateRotationSide(MOVE_LEFT_ANGLE_DOWN, lastEdge.first));
				break;
			case Side::R:
				takeActionVector(rotateRotationSide(MOVE_RIGHT_ANGLE_DOWN, lastEdge.first));
				break;
			}
			break;
		case Side::D:
			switch (lastEdge.second)
			{
			case Side::U:
				while (_CurrentState.upFaces[2][1] == upColor)
				{
					takeActionVector({ Side::U });
				}
				takeActionVector({ Side::F, Side::F });
				angles_inPlace++;
				break;
			case Side::D:
				while (_CurrentState.upFaces[0][1] == upColor)
				{
					takeActionVector({ Side::U });
				}
				takeActionVector({ Side::B, Side::B });
				angles_inPlace++;
				break;
			case Side::L:
				while (_CurrentState.upFaces[1][0] == upColor)
				{
					takeActionVector({ Side::U });
				}
				takeActionVector({ Side::L, Side::L });
				angles_inPlace++;
				break;
			case Side::R:
				while (_CurrentState.upFaces[1][2] == upColor)
				{
					takeActionVector({ Side::U });
				}
				takeActionVector({ Side::R,Side::R });
				angles_inPlace++;
				break;
			}
			break;
		}
	}
	cycleCounter = 0;
	while (countBadUpAngles(_CurrentState).size() > 2)
	{
		takeActionVector({ Side::U });
		if (cycleCounter > actions_toBreak)
			throw std::exception("Can\'t locate up edges");
	}
	std::vector<int> edgesNotInPlace = countBadUpAngles(_CurrentState);
	if (edgesNotInPlace.size() == 2)
	{
		if (edgesNotInPlace[1] - edgesNotInPlace[0] == 2)
			if (edgesNotInPlace[0] == 0)
				takeActionVector(rotateRotationSide(SWITCH_OPPOSITE_EDGES, Side::F));
			else
				takeActionVector(rotateRotationSide(SWITCH_OPPOSITE_EDGES, Side::R));
		else if (edgesNotInPlace[1] - edgesNotInPlace[0] == 1)
		{
			if (edgesNotInPlace[0] == 0)
				takeActionVector(rotateRotationSide(SWITCH_ANGLE_EDGES, Side::F));
			else if (edgesNotInPlace[0] == 1)
				takeActionVector(rotateRotationSide(SWITCH_ANGLE_EDGES, Side::R));
			else if (edgesNotInPlace[0] == 2)
				takeActionVector(rotateRotationSide(SWITCH_ANGLE_EDGES, Side::B));
			else if (edgesNotInPlace[0] == 3)
				takeActionVector(rotateRotationSide(SWITCH_ANGLE_EDGES, Side::L));
		}
		else if ((edgesNotInPlace[1] - edgesNotInPlace[0] == 3) && (edgesNotInPlace[0] == 0))
			takeActionVector(rotateRotationSide(SWITCH_ANGLE_EDGES, Side::L));
	}
	if (countBadUpAngles(_CurrentState).size() != 0)
		throw std::exception("Can\'t make good upper cross");
}

void Solver::placeCorners_Up()
{
	Color upColor = _CurrentState.upFaces[1][1];
	std::pair<Side, std::pair<Side, Side>> lastCorner = locateCorner(upColor);
	int looper = 0;
	int cornersInPlace = 0;
	int cycleCounter = 0;
	while (cornersInPlace < 4)
	{
		cycleCounter++;
		if (cycleCounter > actions_toBreak)
			throw std::exception("Problem placing up corners");
		lastCorner = locateCorner(upColor);
		if (lastCorner.first != Side::NO)
			if (lastCorner.first == Side::D)
			{
				if (!downCorner_inPlace(lastCorner.second, upColor))
				{
					std::pair<Side, Side> buffer = lastCorner.second;
					int deepCycleCounter = 0;
					while (!downCorner_inPlace(buffer, upColor))
					{
						deepCycleCounter++;
						if (deepCycleCounter > actions_toBreak)
							throw std::exception("Problem placing up corners on downside");
						takeActionVector({ Side::D });
						buffer = rotateDownSideCode(buffer);
					}
					lastCorner = locateCorner(upColor);
				}
				switch (lastCorner.second.first)
				{
				case Side::U:
					switch (lastCorner.second.second)
					{
					case Side::L:
						takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, Side::F));
						takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, Side::F));
						takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, Side::F));
						break;
					case Side::R:
						takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, Side::F));
						takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, Side::F));
						takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, Side::F));
						break;
					}
					break;
				case Side::D:
					switch (lastCorner.second.second)
					{
					case Side::L:
						takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, Side::B));
						takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, Side::B));
						takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, Side::B));
						break;
					case Side::R:
						takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, Side::B));
						takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, Side::B));
						takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, Side::B));
						break;
					}
					break;
				}
			}
			else
			{
				if (lastCorner.second.first == Side::U)
				{
					if (lastCorner.second.second == Side::L)
						takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, lastCorner.first));
					else
						takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, lastCorner.first));
				}
				else
				{
					if (lastCorner.second.first == Side::U)
					{
						if (lastCorner.second.second == Side::L)
							takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, lastCorner.first));
						else
							takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, lastCorner.first));
					}
					else
					{
						std::pair<int, int> coordinates;
						if ((lastCorner.first == Side::L) && (lastCorner.second.second == Side::L) || (lastCorner.first == Side::B) && (lastCorner.second.second == Side::R))
							coordinates = { 0, 0 };
						else if ((lastCorner.first == Side::L) && (lastCorner.second.second == Side::R) || (lastCorner.first == Side::F) && (lastCorner.second.second == Side::L))
							coordinates = { 2, 0 };
						else if ((lastCorner.first == Side::R) && (lastCorner.second.second == Side::L) || (lastCorner.first == Side::F) && (lastCorner.second.second == Side::R))
							coordinates = { 2, 2 };
						else if ((lastCorner.first == Side::R) && (lastCorner.second.second == Side::R) || (lastCorner.first == Side::B) && (lastCorner.second.second == Side::L))
							coordinates = { 0, 2 };
						while (_CurrentState.upFaces[coordinates.first][coordinates.second] == upColor)
						{
							takeActionVector({ Side::U });

						}
						if (lastCorner.second.second == Side::R)
							takeActionVector(rotateRotationSide(FLIP_RIGHT_ANGLE, lastCorner.first));
						else
							takeActionVector(rotateRotationSide(FLIP_LEFT_ANGLE, lastCorner.first));
						while (countBadUpAngles(_CurrentState).size() > 2)
							takeActionVector({ Side::U });
					}
				}
			}
		if ((_CurrentState.upFaces[2][2] == upColor) && (_CurrentState.frontFaces[0][2] != _CurrentState.frontFaces[1][1]) && (_CurrentState.rightFaces[0][0] != _CurrentState.rightFaces[1][1]))
		{
			takeActionVector(rotateRotationSide(MOVE_IMPOSTER_ANGLE, Side::F));
			continue;
		}
		if ((_CurrentState.upFaces[2][0] == upColor) && (_CurrentState.frontFaces[0][0] != _CurrentState.frontFaces[1][1]) && (_CurrentState.leftFaces[0][2] != _CurrentState.leftFaces[1][1]))
		{
			takeActionVector(rotateRotationSide(MOVE_IMPOSTER_ANGLE, Side::L));
			continue;
		}
		if ((_CurrentState.upFaces[0][0] == upColor) && (_CurrentState.leftFaces[0][0] != _CurrentState.leftFaces[1][1]) && (_CurrentState.backFaces[0][2] != _CurrentState.backFaces[1][1]))
		{
			takeActionVector(rotateRotationSide(MOVE_IMPOSTER_ANGLE, Side::B));
			continue;
		}
		if ((_CurrentState.upFaces[0][2] == upColor) && (_CurrentState.rightFaces[0][2] != _CurrentState.rightFaces[1][1]) && (_CurrentState.backFaces[0][0] != _CurrentState.backFaces[1][1]))
		{
			takeActionVector(rotateRotationSide(MOVE_IMPOSTER_ANGLE, Side::R));
			continue;
		}
		cornersInPlace = 0;
		if ((_CurrentState.upFaces[0][0] == upColor) && (_CurrentState.leftFaces[0][0] == _CurrentState.leftFaces[1][1]) && (_CurrentState.backFaces[0][2] == _CurrentState.backFaces[1][1]))
			cornersInPlace++;
		if ((_CurrentState.upFaces[0][2] == upColor) && (_CurrentState.rightFaces[0][2] == _CurrentState.rightFaces[1][1]) && (_CurrentState.backFaces[0][0] == _CurrentState.backFaces[1][1]))
			cornersInPlace++;
		if ((_CurrentState.upFaces[2][0] == upColor) && (_CurrentState.leftFaces[0][2] == _CurrentState.leftFaces[1][1]) && (_CurrentState.frontFaces[0][0] == _CurrentState.frontFaces[1][1]))
			cornersInPlace++;
		if ((_CurrentState.upFaces[2][2] == upColor) && (_CurrentState.rightFaces[0][0] == _CurrentState.rightFaces[1][1]) && (_CurrentState.frontFaces[0][2] == _CurrentState.frontFaces[1][1]))
			cornersInPlace++;
	}
	if (!is_SideSolved(_CurrentState.upFaces))
		throw std::exception("Up side is unsolvable");
}

void Solver::placeDownEdges()
{
	int edgesInPlace = 0;
	std::pair<Side, Side> location;
	int cycleCounter = 0;
	while (countSideEdgesInPlace() < 4)
	{
		cycleCounter++;
		if (cycleCounter > actions_toBreak)
			throw std::exception("Problem placing side edges");
		location = locateDownEdge();
		if (location.first != Side::NO)
		{
			switch (location.second)
			{
			case Side::R:
				takeActionVector(rotateRotationSide(DOWNCORNER_RIGHT, location.first));
				break;
			case Side::L:
				takeActionVector(rotateRotationSide(DOWNCORNER_LEFT, location.first));
				break;
			default:
				break;
			}
			continue;
		}
		else
		{
			location = locateNotInPlaceEdge();
			switch (location.second)
			{
			case Side::R:
				takeActionVector(rotateRotationSide(DOWNCORNER_RIGHT, location.first));
				break;
			case Side::L:
				takeActionVector(rotateRotationSide(DOWNCORNER_LEFT, location.first));
				break;
			default:
				takeActionVector({ Side::D });
				break;
			}
			continue;
		}
		takeActionVector({ Side::D });
	}
}

void Solver::makeGoodDownCross()
{
	int cycleCounter = 0;
	while (countGoodDownEdges() != 4)
	{
		cycleCounter++;
		if (cycleCounter > actions_toBreak)
			throw std::exception("Problem making down cross");
		if (countGoodDownEdges() == 2)
		{
			int deepCycleCounter = 0;
			while (((_CurrentState.downFaces[0][1] != _CurrentState.downFaces[1][1]) && (_CurrentState.downFaces[1][0] != _CurrentState.downFaces[1][1])) ||
				((_CurrentState.downFaces[1][0] != _CurrentState.downFaces[1][1]) && (_CurrentState.downFaces[1][2] != _CurrentState.downFaces[1][1])))
			{
				deepCycleCounter++;
				if (deepCycleCounter > actions_toBreak)
					throw std::exception("Problem rotating down cross");
				takeActionVector({ Side::D });
			}
			takeActionVector(MAKE_CROSS);
			if (countGoodDownEdges() != 4)
				takeActionVector(rotateRotationSide(MAKE_CROSS, Side::R));
		}
		else
		{
			takeActionVector(MAKE_CROSS);
		}
	}
	cycleCounter = 0;
	while (findBadDownEdges().size() != 0)
	{
		cycleCounter++;
		if (cycleCounter > actions_toBreak)
			throw std::exception("Problem placing down edges");
		int deepCycleCounter = 0;
		while (findBadDownEdges().size() > 2)
		{
			deepCycleCounter++;
			if (deepCycleCounter > actions_toBreak)
				throw std::exception("Problem rotating down side");
			takeActionVector({ Side::D });
		}
		std::vector<Side> badEdge = findBadDownEdges();
		if (badEdge.size() == 2)
		{
			switch (badEdge[1])
			{
			case Side::R:
			case Side::B:
			case Side::L:
				takeActionVector(rotateRotationSide(SWAP_EDGES, badEdge[0]));
				break;
			case Side::F:
				takeActionVector(rotateRotationSide(SWAP_EDGES, badEdge[0]));
				break;
			}
		}
	}

}

void Solver::placeDownCorners()
{
	int cornersInPlace;
	int cycleCounter = 0;
	while (countCornersInPlace() == 0)
	{
		cycleCounter++;
		if (cycleCounter > actions_toBreak)
			throw std::exception("Problem placing 1 downcorner");
		takeActionVector(SWAP_CORNERS);
	}
	cycleCounter = 0;
	while (countCornersInPlace() != 4)
	{
		cornersInPlace = countCornersInPlace();
		if (is_cornerInPlace({ Side::U, Side::R }))
			while (cornersInPlace == countCornersInPlace())
			{
				cycleCounter++;
				takeActionVector(rotateRotationSide(SWAP_CORNERS, Side::R));
				if (cycleCounter > actions_toBreak)
					throw std::exception("Problem placing downcorners");
			}
		else if (is_cornerInPlace({ Side::U, Side::L }))
			while (cornersInPlace == countCornersInPlace())
			{
				cycleCounter++;
				takeActionVector(rotateRotationSide(SWAP_CORNERS, Side::F));
				if (cycleCounter > actions_toBreak)
					throw std::exception("Problem placing downcorners");
			}
		else if (is_cornerInPlace({ Side::D, Side::R }))
			while (cornersInPlace == countCornersInPlace())
			{
				cycleCounter++;
				takeActionVector(rotateRotationSide(SWAP_CORNERS, Side::B));
				if (cycleCounter > actions_toBreak)
					throw std::exception("Problem placing downcorners");
			}
		else if (is_cornerInPlace({ Side::D, Side::L }))
			while (cornersInPlace == countCornersInPlace())
			{
				cycleCounter++;
				takeActionVector(rotateRotationSide(SWAP_CORNERS, Side::L));
				if (cycleCounter > actions_toBreak)
					throw std::exception("Problem placing downcorners");
			}
	}
}

void Solver::rotateDownCorners()
{
	Color downColor = _CurrentState.downFaces[1][1];
	int cycleCounter = 0;
	while ((_CurrentState.downFaces[0][0] != downColor) || (_CurrentState.downFaces[0][2] != downColor) ||
		(_CurrentState.downFaces[2][0] != downColor) || (_CurrentState.downFaces[2][2] != downColor))
	{
		cycleCounter++;
		if (cycleCounter > actions_toBreak)
			throw std::exception("Problem rotating downcorners");
		int deeplCycleCounter = 0;
		while (_CurrentState.downFaces[0][0] == downColor)
		{
			deeplCycleCounter++;
			takeActionVector({ Side::D });
			if ((_CurrentState.downFaces[0][0] == downColor) && (_CurrentState.downFaces[0][2] == downColor) &&
				(_CurrentState.downFaces[2][0] == downColor) && (_CurrentState.downFaces[2][2] == downColor))
				break;
			if (deeplCycleCounter > actions_toBreak)
				throw std::exception("Problem rotating downside");
		}
		takeActionVector(rotateRotationSide(ROTATE_CORNERS, Side::F));
	}
	cycleCounter = 0;
	while (_CurrentState.frontFaces[2][1] != _CurrentState.frontFaces[1][1])
	{
		cycleCounter++;
		takeActionVector({ Side::D });
		if (cycleCounter > actions_toBreak)
			throw std::exception("Problem rotating downside to final");
	}
}

std::pair<Side, Side> Solver::locateEdge(const Color& search)
{
	std::pair<Side, Side> result;
	std::pair<int, int> coordinates = { 0, 1 };
	result.second = static_cast<Side>(static_cast<int>(Side::U) - 1);
	bool flag = false;
	for (int i = 0; i < 4; i++)
	{
		result.second = static_cast<Side>(static_cast<int>(result.second) + 1);
		if (_CurrentState.frontFaces[coordinates.first][coordinates.second] == search)
		{
			result.first = Side::F;
			return result;
		}
		if (_CurrentState.leftFaces[coordinates.first][coordinates.second] == search)
		{
			result.first = Side::L;
			return result;
		}
		if (_CurrentState.rightFaces[coordinates.first][coordinates.second] == search)
		{
			result.first = Side::R;
			return result;
		}
		if (_CurrentState.backFaces[coordinates.first][coordinates.second] == search)
		{
			result.first = Side::B;
			return result;
		}
		if (_CurrentState.downFaces[coordinates.first][coordinates.second] == search)
		{
			result.first = Side::D;
			return result;
		}

		if (!flag)
		{
			coordinates.first += 2;
			if (coordinates.first == 4)
			{
				coordinates = { 1, 0 };
				flag = true;
			}
		}
		else
		{
			coordinates.second += 2;
		}
	}
	return result;
}

std::pair<Side, std::pair<Side, Side>> Solver::locateCorner(const Color& search) const
{
	std::pair<Side, Side> location;
	Side face = Side::NO;
	for (int i = 0; i < 3; i += 2)
	{
		for (int j = 0; j < 3; j += 2)
		{
			if (i == 0)
			{
				location.first = Side::U;
			}
			else
			{
				location.first = Side::D;
			}
			if (j == 0)
			{
				location.second = Side::L;
			}
			else
			{
				location.second = Side::R;
			}
			if (_CurrentState.downFaces[i][j] == search)
			{
				face = Side::D;
				return { face, location };
			}
		}
	}
	for (int i = 0; i < 3; i += 2)
	{
		for (int j = 0; j < 3; j += 2)
		{
			face = Side::NO;
			if (i == 0)
			{
				location.first = Side::U;
			}
			else
			{
				location.first = Side::D;
			}
			if (j == 0)
			{
				location.second = Side::L;
			}
			else
			{
				location.second = Side::R;
			}

			if (_CurrentState.frontFaces[i][j] == search)
			{
				face = Side::F;
			}
			if (_CurrentState.backFaces[i][j] == search)
			{
				face = Side::B;
			}
			if (_CurrentState.leftFaces[i][j] == search)
			{
				face = Side::L;
			}
			if (_CurrentState.rightFaces[i][j] == search)
			{
				face = Side::R;
			}
			if (face != Side::NO)
				return { face, location };
		}
	}
	return { Side::NO, {Side::NO, Side::NO} };
}

bool Solver::downCorner_inPlace(const std::pair<Side, Side>& location, const Color& search) const
{
	bool result;
	switch (location.first)
	{
	case Side::U:
		switch (location.second)
		{
		case Side::L:
			result = (_CurrentState.downFaces[0][0] == search) && (_CurrentState.frontFaces[2][0] == _CurrentState.leftFaces[1][1]) &&
				(_CurrentState.leftFaces[2][2] == _CurrentState.frontFaces[1][1]);
			return result;
			break;
		case Side::R:
			result = (_CurrentState.downFaces[0][2] == search) && (_CurrentState.rightFaces[2][0] == _CurrentState.frontFaces[1][1]) &&
				(_CurrentState.frontFaces[2][2] == _CurrentState.rightFaces[1][1]);
			return result;
			break;
		}
		break;
	case Side::D:
		switch (location.second)
		{
		case Side::L:
			result = (_CurrentState.downFaces[2][0] == search) && (_CurrentState.leftFaces[2][0] == _CurrentState.backFaces[1][1]) &&
				(_CurrentState.backFaces[2][2] == _CurrentState.leftFaces[1][1]);
			return result;
			break;
		case Side::R:
			result = (_CurrentState.downFaces[2][2] == search) && (_CurrentState.rightFaces[2][2] == _CurrentState.backFaces[1][1]) &&
				(_CurrentState.backFaces[2][0] == _CurrentState.rightFaces[1][1]);
			return result;
			break;
		}
	}
	return false;
}

std::pair<Side, Side> Solver::rotateDownSideCode(const std::pair<Side, Side>& input) const
{
	std::pair<Side, Side> result = input;
	switch (input.first)
	{
	case Side::U:
		switch (input.second)
		{
		case Side::R:
			return { Side::D, Side::R };
		case Side::L:
			return { Side::U, Side::R };
		}
		break;
	case Side::D:
		switch (input.second)
		{
		case Side::R:
			return { Side::D, Side::L };
		case Side::L:
			return { Side::U, Side::L };
		}
		break;
	}
	return { Side::NO, Side::NO };
}

std::pair<Side, Side> Solver::locateDownEdge() const noexcept
{
	std::pair<Side, Side> result = { Side::NO, Side::NO };
	if (_CurrentState.frontFaces[2][1] == _CurrentState.frontFaces[1][1])
	{
		if (_CurrentState.leftFaces[1][1] == _CurrentState.downFaces[0][1])
			result = { Side::F, Side::L };
		if (_CurrentState.rightFaces[1][1] == _CurrentState.downFaces[0][1])
			result = { Side::F, Side::R };
	}
	if (_CurrentState.rightFaces[2][1] == _CurrentState.rightFaces[1][1])
	{
		if (_CurrentState.frontFaces[1][1] == _CurrentState.downFaces[1][2])
			result = { Side::R, Side::L };
		if (_CurrentState.backFaces[1][1] == _CurrentState.downFaces[1][2])
			result = { Side::R, Side::R };
	}
	if (_CurrentState.leftFaces[2][1] == _CurrentState.leftFaces[1][1])
	{
		if (_CurrentState.frontFaces[1][1] == _CurrentState.downFaces[1][0])
			result = { Side::L, Side::R };
		if (_CurrentState.backFaces[1][1] == _CurrentState.downFaces[1][0])
			result = { Side::L, Side::L };
	}
	if (_CurrentState.backFaces[2][1] == _CurrentState.backFaces[1][1])
	{
		if (_CurrentState.leftFaces[1][1] == _CurrentState.downFaces[2][1])
			result = { Side::B, Side::R };
		if (_CurrentState.rightFaces[1][1] == _CurrentState.downFaces[2][1])
			result = { Side::B, Side::L };
	}
	return result;
}

std::pair<Side, Side> Solver::locateNotInPlaceEdge() const
{
	std::pair<Side, Side> result = { Side::NO, Side::NO }, currentSide;
	std::pair<Color, Color> corner;
	std::unordered_set<std::pair<int, int>, pair_hash> possibleSides;
	possibleSides.insert({ static_cast<int>(_CurrentState.frontFaces[1][1]), static_cast<int>(_CurrentState.rightFaces[1][1]) });
	possibleSides.insert({ static_cast<int>(_CurrentState.rightFaces[1][1]), static_cast<int>(_CurrentState.backFaces[1][1]) });
	possibleSides.insert({ static_cast<int>(_CurrentState.backFaces[1][1]), static_cast<int>(_CurrentState.leftFaces[1][1]) });
	possibleSides.insert({ static_cast<int>(_CurrentState.leftFaces[1][1]), static_cast<int>(_CurrentState.frontFaces[1][1]) });

	possibleSides.insert({ static_cast<int>(_CurrentState.rightFaces[1][1]), static_cast<int>(_CurrentState.frontFaces[1][1]) });
	possibleSides.insert({ static_cast<int>(_CurrentState.backFaces[1][1]), static_cast<int>(_CurrentState.rightFaces[1][1]) });
	possibleSides.insert({ static_cast<int>(_CurrentState.leftFaces[1][1]), static_cast<int>(_CurrentState.backFaces[1][1]) });
	possibleSides.insert({ static_cast<int>(_CurrentState.frontFaces[1][1]), static_cast<int>(_CurrentState.leftFaces[1][1]) });

	currentSide = { Side::F, Side::L };
	if (((_CurrentState.frontFaces[1][0] != _CurrentState.frontFaces[1][1]) || (_CurrentState.leftFaces[1][2] != _CurrentState.leftFaces[1][1])) &&
		(possibleSides.find({ static_cast<int>(_CurrentState.frontFaces[1][0]), static_cast<int>(_CurrentState.leftFaces[1][2]) }) != possibleSides.end()))
	{
		return currentSide;
	}

	currentSide = { Side::F, Side::R };
	if (((_CurrentState.frontFaces[1][2] != _CurrentState.frontFaces[1][1]) || (_CurrentState.rightFaces[1][0] != _CurrentState.rightFaces[1][1])) &&
		(possibleSides.find({ static_cast<int>(_CurrentState.frontFaces[1][2]), static_cast<int>(_CurrentState.rightFaces[1][0]) }) != possibleSides.end()))
	{
		return currentSide;
	}

	currentSide = { Side::B, Side::L };
	if (((_CurrentState.backFaces[1][0] != _CurrentState.backFaces[1][1]) || (_CurrentState.rightFaces[1][2] != _CurrentState.rightFaces[1][1])) &&
		(possibleSides.find({ static_cast<int>(_CurrentState.backFaces[1][0]), static_cast<int>(_CurrentState.rightFaces[1][2]) }) != possibleSides.end()))
	{
		return currentSide;
	}

	currentSide = { Side::B, Side::R };
	if (((_CurrentState.backFaces[1][2] != _CurrentState.backFaces[1][1]) || (_CurrentState.leftFaces[1][0] != _CurrentState.leftFaces[1][1])) &&
		(possibleSides.find({ static_cast<int>(_CurrentState.backFaces[1][2]), static_cast<int>(_CurrentState.leftFaces[1][0]) }) != possibleSides.end()))
	{
		return currentSide;
	}

	return result;
}

int Solver::countSideEdgesInPlace() const noexcept
{
	int result = 0;
	if ((_CurrentState.backFaces[1][2] == _CurrentState.backFaces[1][1]) && (_CurrentState.leftFaces[1][0] == _CurrentState.leftFaces[1][1]))
		result++;
	if ((_CurrentState.backFaces[1][0] == _CurrentState.backFaces[1][1]) && (_CurrentState.rightFaces[1][2] == _CurrentState.rightFaces[1][1]))
		result++;
	if ((_CurrentState.frontFaces[1][2] == _CurrentState.frontFaces[1][1]) && (_CurrentState.rightFaces[1][0] == _CurrentState.rightFaces[1][1]))
		result++;
	if ((_CurrentState.frontFaces[1][0] == _CurrentState.frontFaces[1][1]) && (_CurrentState.leftFaces[1][2] == _CurrentState.leftFaces[1][1]))
		result++;
	return result;
}

int Solver::countGoodDownEdges() const noexcept
{
	int result = 0;
	if (_CurrentState.downFaces[0][1] == _CurrentState.downFaces[1][1])
		result++;
	if (_CurrentState.downFaces[2][1] == _CurrentState.downFaces[1][1])
		result++;
	if (_CurrentState.downFaces[1][0] == _CurrentState.downFaces[1][1])
		result++;
	if (_CurrentState.downFaces[1][2] == _CurrentState.downFaces[1][1])
		result++;
	return result;
}

std::vector<Side> Solver::findBadDownEdges() const
{
	std::vector<Side> result;
	if (_CurrentState.frontFaces[2][1] != _CurrentState.frontFaces[1][1])
		result.push_back(Side::F);
	if (_CurrentState.rightFaces[2][1] != _CurrentState.rightFaces[1][1])
		result.push_back(Side::R);
	if (_CurrentState.backFaces[2][1] != _CurrentState.backFaces[1][1])
		result.push_back(Side::B);
	if (_CurrentState.leftFaces[2][1] != _CurrentState.leftFaces[1][1])
		result.push_back(Side::L);
	return result;
}

bool Solver::is_cornerInPlace(const std::pair<Side, Side>& location) const
{
	bool result;
	std::unordered_set<int> angle;
	switch (location.first)
	{
	case Side::U:
		switch (location.second)
		{
		case Side::R:
			angle.insert(static_cast<int>(_CurrentState.downFaces[0][2]));
			angle.insert(static_cast<int>(_CurrentState.frontFaces[2][2]));
			angle.insert(static_cast<int>(_CurrentState.rightFaces[2][0]));
			if ((angle.find(static_cast<int>(_CurrentState.downFaces[1][1])) != angle.end()) &&
				(angle.find(static_cast<int>(_CurrentState.frontFaces[1][1])) != angle.end()) &&
				(angle.find(static_cast<int>(_CurrentState.rightFaces[1][1])) != angle.end()))
				return true;
			else
				return false;
			break;
		case Side::L:
			angle.insert(static_cast<int>(_CurrentState.downFaces[0][0]));
			angle.insert(static_cast<int>(_CurrentState.frontFaces[2][0]));
			angle.insert(static_cast<int>(_CurrentState.leftFaces[2][2]));
			if ((angle.find(static_cast<int>(_CurrentState.downFaces[1][1])) != angle.end()) &&
				(angle.find(static_cast<int>(_CurrentState.frontFaces[1][1])) != angle.end()) &&
				(angle.find(static_cast<int>(_CurrentState.leftFaces[1][1])) != angle.end()))
				return true;
			else
				return false;
			break;
		}
		break;
	case Side::D:
		switch (location.second)
		{
		case Side::R:
			angle.insert(static_cast<int>(_CurrentState.downFaces[2][2]));
			angle.insert(static_cast<int>(_CurrentState.backFaces[2][0]));
			angle.insert(static_cast<int>(_CurrentState.rightFaces[2][2]));
			if ((angle.find(static_cast<int>(_CurrentState.downFaces[1][1])) != angle.end()) &&
				(angle.find(static_cast<int>(_CurrentState.backFaces[1][1])) != angle.end()) &&
				(angle.find(static_cast<int>(_CurrentState.rightFaces[1][1])) != angle.end()))
				return true;
			else
				return false;
			break;
		case Side::L:
			angle.insert(static_cast<int>(_CurrentState.downFaces[2][0]));
			angle.insert(static_cast<int>(_CurrentState.backFaces[2][2]));
			angle.insert(static_cast<int>(_CurrentState.leftFaces[2][0]));
			if ((angle.find(static_cast<int>(_CurrentState.downFaces[1][1])) != angle.end()) &&
				(angle.find(static_cast<int>(_CurrentState.backFaces[1][1])) != angle.end()) &&
				(angle.find(static_cast<int>(_CurrentState.leftFaces[1][1])) != angle.end()))
				return true;
			else
				return false;
			break;
		}
	}
	throw std::exception("Attempt to find not existing down angle");
}

int Solver::countCornersInPlace() const
{
	int result = 0;
	if (is_cornerInPlace({ Side::U, Side::R }))
		result++;
	if (is_cornerInPlace({ Side::U, Side::L }))
		result++;
	if (is_cornerInPlace({ Side::D, Side::R }))
		result++;
	if (is_cornerInPlace({ Side::D, Side::L }))
		result++;
	return result;
}

