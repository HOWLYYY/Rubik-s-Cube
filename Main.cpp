#include <thread>
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include "3DCubeShow.h"
#include "Solver.h"

int main(int argc, char** argv)
{
	Cube3D NewCube;
	CubeIO file;
	Solver NewSolver;
	std::ofstream output;
	std::ifstream input;
	std::string bufferString;
	while (true) {
		switch (NewCube.getMenuAction())
		{
		case MenuAction::ROTATE:
			std::cout << "Enter cube rotations in format of F B U D L R F\' B\' U\' D\' L\' R\'. After all actions enter END." << std::endl;
			std::cin >> bufferString;
			while (bufferString != "END")
			{
				try {
					NewCube.rotateCube(file.parseRotation(bufferString));
				}
				catch (std::runtime_error re)
				{
					std::cerr << re.what() << std::endl;
				}
				std::cin >> bufferString;
			}
			NewCube.resetMenuAction();
			std::cout << "OK" << std::endl;
			break;
		case MenuAction::SAVE:
			std::cout << "Enter filename to save cube" << std::endl;
			std::cin >> bufferString;
			output.open(bufferString, std::ios_base::out);
			try {
				file.save(NewCube.getState(), output);
			}
			catch (std::runtime_error err)
			{
				std::cerr << err.what() << std::endl;
			}
			output.close();
			NewCube.resetMenuAction();
			break;
		case MenuAction::READ:
			std::cout << "Enter filename to load cube" << std::endl;
			std::cin >> bufferString;
			input.open(bufferString, std::ios_base::in);
			try {
				NewCube.setState(file.load(input).getState());
			}
			catch (std::runtime_error re)
			{
				std::cerr << re.what() << std::endl;
			}
			catch (std::bad_cast bc)
			{
				std::cerr << bc.what() << std::endl;
			}
			input.close();
			NewCube.resetMenuAction();
			break;
		case MenuAction::RANDOMIZE:
			NewCube.randomize();
			NewCube.resetMenuAction();
			break;
		case MenuAction::RESET:
			NewCube.setState(defaultState);
			NewCube.resetMenuAction();
			break;
		case MenuAction::SOLVE:
			try {
				NewSolver.solve(NewCube);
				std::vector<Side> solvingSequence = NewSolver.getActionsToSolve();
				file.outputActionVector(solvingSequence);
			}
			catch (const std::exception& error)
			{
				std::cerr << error.what() << std::endl;
			}
			NewCube.resetMenuAction();
			break;
		case MenuAction::CHECK:
			if (NewSolver.is_solvable(NewCube))
			{
				std::cout << "This cube is solvable" << std::endl;
			}
			else
			{
				std::cout << "This cube is unsolvable" << std::endl;
			}
			NewCube.resetMenuAction();
			break;
		}
	}
	return 0;
}