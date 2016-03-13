/*
 * libflv.hpp
 *
 *  Created on: 2016. 3. 9.
 *      Author: innocentevil_win
 */

#ifndef LIBFLV_HPP_
#define LIBFLV_HPP_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#define TAG_TYPE_AUDIO			((uint8_t) 8)
#define TAG_TYPE_VIDEO			((uint8_t) 9)
#define TAG_TYPE_SCRIPT			((uint8_t) 18)



class FLVStream {
public:
	FLVStream();
	virtual ~FLVStream();
	virtual int open() = 0;
	virtual ssize_t write(const char* wb, size_t sz) = 0;
	virtual ssize_t read(char* rb, size_t sz) = 0;
	virtual int close() = 0;

};


class FLVFileStream : public FLVStream {
public :
	FLVFileStream(const char* fname);
	~FLVFileStream();
	int open();
	ssize_t write(const char* wb, size_t sz);
	ssize_t read(char* rb, size_t sz);
	int close();
private:
	int fd;
	std::string* filename;
};


class FLVSocketStream : public FLVStream {
public:
	FLVSocketStream(const char* host, int port,bool is_server);
	~FLVSocketStream();
	int open();
	ssize_t write(const char* wb, size_t sz);
	ssize_t read(char* rb, size_t sz);
	int close();
private:
	sockaddr_in* host_address;
	sockaddr_in* client_address;
	bool is_server;
	int sfd;
	int clisfd;
};

class FLVWriter {
public:
	FLVWriter(FLVStream* stream, size_t bsz);
	~FLVWriter();
	int writeEncodedVideo();
	int writeEncdoedAudio();
};

class FLVParser {
	typedef void (*callback)(uint8_t type,void* data, size_t sz, uint32_t pts);
public:
	FLVParser(FLVStream* stream,size_t bsz);
	~FLVParser();
	int parse(callback cb,bool is_realtime);
private:
	uint8_t*	parse_buffer;
	FLVStream*	flvstream;
};


#endif /* LIBFLV_HPP_ */
