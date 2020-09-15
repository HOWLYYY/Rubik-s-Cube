#include <exception>
#include "CubeIO.h"


void CubeIO::save(const State& cube, std::ofstream& output) const
{
	saveFace(output, cube.frontFaces, "FRONT");
	saveFace(output, cube.backFaces, "BACK");
	saveFace(output, cube.upFaces, "UP");
	saveFace(output, cube.downFaces, "DOWN");
	saveFace(output, cube.leftFaces, "LEFT");
	saveFace(output, cube.rightFaces, "RIGHT");

}

const CubeIO& CubeIO::load(std::ifstream& input)
{
	State newState;
	std::string faceName;
	for (int i = 0; i < 6; i++)
	{
		input >> faceName;
		if (faceName == "FRONT")
			loadFace(input, newState.frontFaces);
		else if (faceName == "BACK")
			loadFace(input, newState.backFaces);
		else if (faceName == "UP")
			loadFace(input, newState.upFaces);
		else if (faceName == "DOWN")
			loadFace(input, newState.downFaces);
		else if (faceName == "LEFT")
			loadFace(input, newState.leftFaces);
		else if (faceName == "RIGHT")
			loadFace(input, newState.rightFaces);
		else
			throw std::runtime_error("Bad face name");
	}
	setState(newState);
	return *this;
}

Side CubeIO::parseRotation(const std::string& action) const
{
	if (action == "F")
		return Side::F;
	else if (action == "B")
		return Side::B;
	else if (action == "U")
		return Side::U;
	else if (action == "D")
		return Side::D;
	else if (action == "L")
		return Side::L;
	else if (action == "R")
		return Side::R;
	else if (action == "F\'")
		return Side::FS;
	else if (action == "B\'")
		return Side::BS;
	else if (action == "U\'")
		return Side::US;
	else if (action == "D\'")
		return Side::DS;
	else if (action == "L\'")
		return Side::LS;
	else if (action == "R\'")
		return Side::RS;
	else if (action == "END")
		return Side::NO;
	else
		throw std::runtime_error("Bad action");
}

void CubeIO::saveFace(std::ofstream& output, const Color(&face)[3][3], const std::string& faceName) const
{
	output << faceName << std::endl;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			output << static_cast<int>(face[i][j]) << ' ';
		}
		output << std::endl;
	}
	if (!output.good())
		throw std::runtime_error("Problem with file");
}

void CubeIO::loadFace(std::ifstream& input, Color(&face)[3][3])
{
	unsigned int value;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			input >> value;
			face[i][j] = static_cast<Color>(value);
		}
}

const std::string CubeIO::convertRotation(const Side& rotation) const {
	switch (rotation)
	{
	case Side::U:
		return "U ";
		break;
	case Side::US:
		return "U\'";
		break;
	case Side::D:
		return "D ";
		break;
	case Side::DS:
		return "D\'";
		break;
	case Side::L:
		return "L ";
		break;
	case Side::LS:
		return "L\'";
		break;
	case Side::R:
		return "R ";
		break;
	case Side::RS:
		return "R\'";
		break;
	case Side::F:
		return "F ";
		break;
	case Side::FS:
		return "F\'";
		break;
	case Side::B:
		return "B ";
		break;
	case Side::BS:
		return "B\'";
		break;
	default:
		throw std::exception("Bad rotation");
	}
}

void CubeIO::outputActionVector(const std::vector<Side>& actions) const {
	if (actions.empty())
	{
		std::cout << "No actions has been taken" << std::endl;
		return;
	}
	std::cout << "Actions taken " << actions.size() << ":" << std::endl;
	for (int i = 0; i < actions.size(); i++)
	{
		std::cout << convertRotation(actions[i]) << ' ';
	}
	std::cout << std::endl;
}