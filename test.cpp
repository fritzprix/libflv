/*
 * test.cpp
 *
 *  Created on: 2016. 3. 9.
 *      Author: innocentevil_win
 */
#include <stdio.h>
#include <stdlib.h>

#include "libflv.hpp"

int main(int argc, const char* argv[]){

	FLVStream* stream = new FLVFileStream("car_drive.flv");
	FLVParser* parser = new FLVParser(stream, 1 << 20);
	parser->parse(NULL,TRUE);
	return EXIT_SUCCESS;
}
