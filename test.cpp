/*
 * test.cpp
 *
 *  Created on: 2016. 3. 9.
 *      Author: innocentevil_win
 */
#include <stdio.h>
#include <stdlib.h>

#include "libflv.hpp"


static void onTagMatch(uint8_t type, void* data, size_t sz, uint32_t pts);

int main(){

	FLVStream* stream = new FLVFileStream("bowling_360.flv");
	FLVParser* parser = new FLVParser(stream, 1 << 20);


	parser->parse(onTagMatch,true);
	return EXIT_SUCCESS;
}


static void onTagMatch(uint8_t type, void* data, size_t sz, uint32_t pts)
{
	switch(type)
	{
	case TAG_TYPE_VIDEO:
		::printf("video\n");
		break;
	case TAG_TYPE_AUDIO:
		::printf("audio\n");
		break;
	case TAG_TYPE_SCRIPT:
		::printf("script\n");
		break;
	}
}
