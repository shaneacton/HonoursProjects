#include <fstream>

using namespace std;


#include <stdio.h>
#include "sequentialSort.h"

int arrayLengths =10;


int main( int argc, const char* argv[] )
{
	// Prints each argument on the command line.
	for( int i = 0; i < argc; i++ )
	{
		printf( "arg %d: %s\n", i, argv[i] );
	}

	SequentialSort::genArray(arrayLengths);
	SequentialSort::sortArray(0,arrayLengths);
	SequentialSort::printArray();

}