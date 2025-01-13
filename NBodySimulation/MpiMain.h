#pragma once
#include <iostream>
#include <vector>
#include <mpi.h>
#include "Body.h"
#include <SFML/Graphics.hpp>

class MpiMain
{
public:
	const double G = 0.000000000066743;
	const double dt = 0.1;
	double elapsedTime = 0;

	MpiMain()
	{

	}

	std::vector<Body> convertArrayToVector(int* array, int size);

	int* convertVectorToArray(std::vector<Body> bodies);

	void worker(int me);

	void executeMain(int argc, char** argv);

	void multipleStepsMPIMasterThread(std::vector<Body>& bodies, double timeFrame, int size);
};

