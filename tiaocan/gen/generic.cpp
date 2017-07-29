// Genetic Algorithm Hello World
// http://www.generation5.org/content/2003/gahelloworld.asp

#include <iostream>					// for cout etc.

#include <fstream>
#include <vector>					// for vector class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()
#include "generic.h"


using namespace std;				// polluting global namespace, but hey...


int main(int argc, char *argv[] )
{

	srand(unsigned(time(NULL)));
	graph Ggraph;
	const char *filename = argv[1];
	readdata(filename,Ggraph);
        Ga_For_MCF(Ggraph);
	return 0;
}
