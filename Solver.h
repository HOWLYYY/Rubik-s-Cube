#pragma once
#include <Windows.h>
#include <unordered_set>
#include <exception>
#include "Cube.h"
#include "3DCubeShow.h"

class Solver : public Cube {
public:
	void solve(Cube& toSolve);
	void solve(Cube3D& toSolve);
	bool is_solved() const noexcept;
	std::vector<Side> getActionsToSolve();
	void optimiseActionVector();
	bool is_solvable(Cube& toTry);

private:
	const int actions_toBreak = 100;

	const std::vector<Side> INVERT_INPLACE_MIDDLE = { Side::FS, Side::L, Side::D, Side::LS, Side::F, Side::F };
	const std::vector<Side> MOVE_RIGHT_ANGLE_DOWN = { Side::RS, Side::DS, Side::R };
	const std::vector<Side> MOVE_LEFT_ANGLE_DOWN = { Side::L, Side::DS, Side::LS };
	const std::vector<Side> GET_DOWN_ANGLE = { Side::F, Side::L, Side::D, Side::LS, Side::FS };
	const std::vector<Side> SWITCH_OPPOSITE_EDGES = { Side::F, Side::F, Side::B, Side::B, Side::D, Side::D, Side::BS, Side::BS, Side::FS, Side::FS };
	const std::vector<Side> SWITCH_ANGLE_EDGES = { Side::R, Side::R, Side::DS, Side::F, Side::F, Side::D, Side::RS, Side::RS };
	const std::vector<Side> FLIP_RIGHT_ANGLE = { Side::RS, Side::DS, Side::R, Side::D };
	const std::vector<Side> FLIP_LEFT_ANGLE = { Side::L, Side::D, Side::LS, Side::DS };
	const std::vector<Side> MOVE_IMPOSTER_ANGLE = { Side::RS, Side::DS, Side::R };
	const std::vector<Side> DOWNCORNER_RIGHT = { Side::DS, Side::RS, Side::D, Side::R, Side::D, Side::F, Side::DS, Side::FS };
	const std::vector<Side> DOWNCORNER_LEFT = { Side::D, Side::L, Side::DS, Side::LS, Side::DS, Side::FS, Side::D, Side::F };
	const std::vector<Side> MAKE_CROSS = { Side::F, Side::L, Side::D, Side::LS, Side::DS, Side::FS };
	const std::vector<Side> SWAP_EDGES = { Side::L, Side::D, Side::LS, Side::D, Side::L, Side::D, Side::D, Side::LS, Side::D };
	const std::vector<Side> SWAP_CORNERS = { Side::D, Side::L, Side::DS, Side::RS, Side::D, Side::LS, Side::DS, Side::R };
	const std::vector<Side> ROTATE_CORNERS = { Side::LS, Side::US, Side::L, Side::U, Side::LS, Side::US, Side::L, Side::U };

	std::vector<Side> ActionsTaken;

	void takeActionVector(const std::vector<Side>& sequence) noexcept;

	void placeEdges();
	void placeCorners_Up();
	void placeDownEdges();
	void makeGoodDownCross();
	void placeDownCorners();
	void rotateDownCorners();
	void generateSolvingSequence();

	bool is_SideSolved(const Color(&face)[3][3]) const noexcept;
	bool downCorner_inPlace(const std::pair<Side, Side>& location, const Color& search) const;
	bool is_cornerInPlace(const std::pair<Side, Side>& location) const;

	int countSideEdgesInPlace() const noexcept;
	int countGoodDownEdges() const noexcept;
	int countCornersInPlace() const;

	std::vector<int> countBadUpAngles(const State& cubeState) const;
	std::vector<Side> findBadDownEdges() const;

	std::pair<Side, Side> locateEdge(const Color& search);
	std::pair<Side, std::pair<Side, Side>> locateCorner(const Color& search) const;
	std::pair<Side, Side> rotateDownSideCode(const std::pair<Side, Side>& input) const;
	std::pair<Side, Side> locateDownEdge() const noexcept;
	std::pair<Side, Side> locateNotInPlaceEdge() const;
};