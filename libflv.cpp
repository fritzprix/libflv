/*
 * libflv.cpp
 *
 *  Created on: 2016. 3. 9.
 *      Author: innocentevil_win
 */



#include "libflv.hpp"


#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

#ifdef __DBG
#define __LOG(...)		::printf(__VA_ARGS__)
#else
#define __LOG(...)
#endif

typedef struct {
	uint8_t _[3];
} uint24_t;


typedef struct flv_header {
	uint8_t		sig[3];
	uint8_t		ver;
#define FLV_HEADER_FLAG_HAS_AUDIO		((uint8_t) 1 << 2)
#define FLV_HEADER_FLAG_HAS_VIDEO		((uint8_t) 1)
	uint8_t		flag;
	uint32_t	offset;		// typically same as the size of flv_header
}__attribute__((packed)) flv_header_t;

typedef struct flv_tag {
	uint8_t		type;
	uint24_t	size;
	uint24_t	time_stm;
	uint8_t		time_stm_ex;
	uint24_t	stream_id;

}__attribute__((packed)) flv_tag_t;

const char FLV_SIG[] = {
	'F','L','V'
};


FLVStream::FLVStream() { }

FLVStream::~FLVStream() { }


FLVFileStream::FLVFileStream(const char* file)
{
	filename = new std::string(file);
	fd = 0;
	if(fd < 0)
	{
		perror("File Not Found \n");
		exit(ENFILE);
	}
}

FLVFileStream::~FLVFileStream()
{
	if(fd > 0)
	{
		::close(fd);
	}
}

int FLVFileStream::open()
{
	if(fd)
	{
		perror("File already opened!! \n");
		return EXIT_FAILURE;
	}

	fd = ::open(filename->c_str(), O_BINARY | O_RDWR);
	if(fd < 0)
	{
		perror("File Not Found \n");
		exit(ENFILE);
	}
	return EXIT_SUCCESS;
}

int FLVFileStream::close()
{
	if(fd)
	{
		::close(fd);
	}
	delete filename;
	filename = NULL;
	return 0;
}

ssize_t FLVFileStream::write(const char* wb,size_t sz)
{
	if(!wb || !sz)
		return -1;
	return ::write(fd, wb, sz);
}

ssize_t FLVFileStream::read(char* rb, size_t sz)
{
	if(!rb || !sz)
		return -1;
	return ::read(fd, rb, sz);
}

FLVSocketStream::FLVSocketStream(const char* host,int port, bool is_server)
{
	this->is_server = is_server;
	sfd = ::socket(AF_INET, SOCK_STREAM, 0);
	clisfd = 0;
	if(sfd < 0)
	{
		perror("fail to open Socket !!\n");
		exit(ESOCKTNOSUPPORT);
	}

	host_address = new sockaddr_in;
	host_address->sin_family = AF_INET;
	host_address->sin_port = htons(port);
	host_address->sin_addr.s_addr = inet_addr(host);

	client_address = new sockaddr_in;
}

FLVSocketStream::~FLVSocketStream()
{
	if(sfd > 0)
	{
		::close(sfd);
	}
	delete host_address;
}

int FLVSocketStream::open()
{
	int result = EXIT_SUCCESS;
	if(is_server)
	{
		if((result = ::bind(sfd,(sockaddr*) host_address, sizeof(sockaddr))) != EXIT_SUCCESS)
		{
			perror("Can't bind socket!! \n");
			exit(result);
		}
		::listen(sfd,0);
		int client_addrlen = sizeof(sockaddr);
		clisfd  = ::accept(sfd, (sockaddr*) client_address, &client_addrlen);
		if(clisfd < 0)
		{
			perror("Socket Error");
			exit(clisfd);
		}
	}
	else
	{
		if((result = ::connect(sfd,(sockaddr*)host_address, sizeof(sockaddr))) != EXIT_SUCCESS)
		{
			perror("Can't connect to server! \n");
			exit(result);
		}
	}
	return EXIT_SUCCESS;
}

int FLVSocketStream::close()
{
	if(sfd > 0)
	{
		::close(sfd);
	}

	if(clisfd > 0)
	{
		::close(clisfd);
	}
	if(host_address)
	{
		delete host_address;
	}
	if(client_address)
	{
		delete client_address;
	}
	return 0;
}

ssize_t FLVSocketStream::write(const char* wb, size_t sz)
{
	if(!wb || !sz)
		return -1;
	ssize_t rsz = 0;
	if(is_server)
	{
		rsz = ::send(clisfd, wb, sz, 0);
	}
	else
	{
		rsz = ::send(sfd, wb, sz , 0);
	}
	return rsz;
}

ssize_t FLVSocketStream::read(char* rb, size_t sz)
{
	if(!rb || !sz)
		return -1;
	ssize_t rsz = 0;
	if(is_server)
	{
		rsz = ::recv(clisfd, rb, sz, 0);
	}
	else
	{
		rsz = ::recv(sfd, rb, sz, 0);
	}
	return rsz;
}

FLVParser::FLVParser(FLVStream* stream,size_t bsz)
{
	flvstream = stream;
	parse_buffer = new uint8_t[bsz];
}

FLVParser::~FLVParser()
{
	flvstream = NULL;
	delete parse_buffer;
}

int FLVParser::parse(callback cb, bool is_realtime)
{
	flv_header_t* header;
	if(flvstream->open())
	{
		perror("Fail to open flv stream! \n");
		return -1;
	}
	flvstream->read((char*) parse_buffer, sizeof(flv_header_t));
	header = (flv_header_t*) parse_buffer;
	if(!memcmp(header->sig , FLV_SIG, sizeof(FLV_SIG)))
	{
		__LOG("Valid FLV Header!\n");
	}
	else
	{
		perror("Invalid FLV stream! \n");
		return -1;
	}
	__LOG("Version : %d\n",header->ver);
	__LOG("Has Video Track : %d\n",(header->flag & FLV_HEADER_FLAG_HAS_VIDEO)? 1 : 0);
	__LOG("Has Audio Track : %d\n",(header->flag & FLV_HEADER_FLAG_HAS_AUDIO)? 1 : 0);

	if(flvstream->close())
	{
		perror("Error in closing flv stream! \n");
		return -1;
	}
	return EXIT_SUCCESS;
}
