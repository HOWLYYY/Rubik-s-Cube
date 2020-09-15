#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include "Cube.h"

class CubeIO : public Cube {
public:
	void save(const State&, std::ofstream& output) const;
	const CubeIO& load(std::ifstream& input);
	void outputActionVector(const std::vector<Side>& actions) const;
	Side parseRotation(const std::string& action) const;
private:
	void saveFace(std::ofstream& output, const Color(&face)[3][3], const std::string& faceName) const;
	void loadFace(std::ifstream& input, Color(&face)[3][3]);
	const std::string convertRotation(const Side& rotation) const;
};