#include <iostream>
#include "MultithreadedMain.h"
#include "MpiMain.h"

void executeMPI(int argc, char** argv)
{
	MpiMain mpiVersion{};

	mpiVersion.executeMain(argc, argv);
}

void executeMultiThreaded()
{
	MultithreadedMain multithreadedVersion{};

	multithreadedVersion.executeMain();
}

int main(int argc, char** argv)
{	
	//executeMultiThreaded();

	executeMPI(argc, argv);

	return 0;
}
