#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include "md5.h"
#include "md5.cpp"
#include "md5Generator.cpp"
 
using std::cout; using std::endl;
 
int main(int argc, char *argv[])
{

	// Initialize listening socket

	md5Generator  gen;

	char * md5Str = (char*)"dbc4d84bfcfe2284ba11beffb853a8c4"; // MD5 - 4444
	int seq = 4;

	gen.setMd5(md5Str);
	gen.setSequence(seq);

	gen.process();

	//cout << "md5 of 'grape': " << md5("grape") << endl;

	return 0;
}
