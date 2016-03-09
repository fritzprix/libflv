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
#include <string>
#include <winsock2.h>


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

class FLVParser {
	typedef void (*callback)(uint8_t type,void* data, size_t sz);
public:
	FLVParser(FLVStream* stream,size_t bsz);
	~FLVParser();
	int parse(callback cb,bool is_realtime);
private:
	uint8_t*	parse_buffer;
	FLVStream*	flvstream;
};


#endif /* LIBFLV_HPP_ */
