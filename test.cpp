/*
 * test.cpp
 *
 *  Created on: 2016. 3. 9.
 *      Author: innocentevil_win
 */
#include <stdio.h>
#include <stdlib.h>

#include "libflv.hpp"
#include "srs_librtmp.hpp"

static void onTagMatch(uint8_t type, void* data, size_t sz, uint32_t pts);

int main(int argc, const char* argv[]){


	srs_rtmp_t rtmp_handle;
	rtmp_handle = srs_rtmp_create("rtmp://192.168.0.100/live/360");
	FLVStream* stream = new FLVFileStream("car_drive.flv");
	FLVParser* parser = new FLVParser(stream, 1 << 20);


	parser->parse(onTagMatch,TRUE);
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
