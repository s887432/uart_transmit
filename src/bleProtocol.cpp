#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

#include "uartFunc.h"
#include "bleProtocol.h"
// ******************************************
// global variables
// ******************************************
// ******************************************
// internal constant
// ******************************************
#define CMD_HEADER_1	0xEF
#define CMD_HEADER_2	0x12

#define HEADER_OFFSET	0
#define HEADER_SIZE		2

#define COMMAND_OFFSET	2
#define COMMAND_SIZE	1

#define LENGTH_OFFSET	3
#define LENGTH_SIZE		1

#define PAYLOAD_OFFSET	4

#define RECEIVE_BUFFER_SIZE	512
// ******************************************
// internal functions
// ******************************************
void debugBuffer(unsigned char *buf, int size)
{
	for(int i=0; i<size; i++)
	{
		printf("%02X", buf[i]);
		if( i < size -1 )
		{
			printf(", ");
		}
	}
	
	printf("\r\n");
}

int makeBufHeader(unsigned char *buf)
{
    buf[0] = CMD_HEADER_1;
    buf[1] = CMD_HEADER_2;
    
    return 2;
}

unsigned char makeBufChecksum(unsigned char *buf, int size)
{
    unsigned int sum = 0;
    int i;
    
    for(i=0; i<size; i++)
    {
        sum += buf[i];
    }
    
    return (sum&0xFF);
}

int makeCmdNaviDir(unsigned char *buf, unsigned char val)
{
    int offset = 0;
    
    // header
    offset += makeBufHeader(buf);
    
	// command
    buf[offset++] = COMMAND_NAVI_DIRECTION;

	// length
    buf[offset++] = 4+4;	// TODO... what is the name?
    
    // payload: distance
    buf[offset++] = 1;
    buf[offset++] = 2;
    buf[offset++] = 3;
    
    // payload: direction
    buf[offset++] = val;
    
    // payload: name
    buf[offset++] = 'P';
    buf[offset++] = 'a';
    buf[offset++] = 't';
    buf[offset++] = 'o';
    
    buf[offset] = makeBufChecksum(buf, offset);
    offset++;
    
    return offset;
}

static unsigned char cmd_checksum(unsigned char *buffer, unsigned char size)
{
	unsigned char i;
	unsigned int sum=0;
	
	for(i=0; i<size; i++)
	{
		sum += buffer[i];
	}
	
	return (sum&0xFF);
}

// re-position the buffer if 0xEF 0x12 is not in beginning of buffer
int bleCheckCommand(unsigned char *buffer, int length, unsigned char *cmdBuf)
{
	int cmdSize = 0;
	int header = -1;
	int tail = 0;
	int index = 0;

	int package_size = 0;

	// find header
	for(int i=0; i<length-1; i++)
	{
		if( buffer[i] == CMD_HEADER_1 && buffer[i+1] == CMD_HEADER_2 )
		{
			header = i;
			index = i+HEADER_SIZE;
			
			break;
		}
	}

	if( header >= 0 )
	{
		// header found
		if( (index + COMMAND_SIZE) <= length )
		{
			// command existed
			index += COMMAND_SIZE;
		}
		else
		{
			// command package is not complete
			return cmdSize;
		}

		// command found
		if( (index + LENGTH_SIZE) <= length )
		{
			// length existed
			package_size = buffer[index] * 256 + buffer[index+1];
			index += COMMAND_SIZE;			
		}
		else
		{
			// command package is not complete
			return cmdSize;
		}

		// package size found
		if( (index + package_size) <= length )
		{
			// payload existed
			index += package_size;
		}
		else
		{
			// command package is not complete
			return cmdSize;
		}

		// payload found
		if( (index + 2) <= length )
		{
			// tail existed
			index += 2;
			tail = index -1;
		}
		else
		{
			// command package is not complete
			return cmdSize;
		}

		// command is complete
		memcpy(cmdBuf, buffer+header, index);
		cmdSize = tail - header;

		// move buffer
		if( length >= index )
		{
			memcpy(buffer, buffer+index, length-index-2);
		}
	}

	if( header >= 0 && tail >=0 )
	{
		// command package is complete
		
	}

	return cmdSize;
}

// ******************************************
// extern functions
// ******************************************
#if 0
// ******************************************
// BLE device receiver thread process
//
// Paramaters
//    none
//
// Return value
//    none
// ******************************************
void *bleProc(void *ptr)
{
	int nread;
	unsigned char buff[RECEIVE_BUFFER_SIZE];	

	unsigned char receiveBuffer[RECEIVE_BUFFER_SIZE];
	int receiveIndex = 0;

	unsigned char cmdBuffer[RECEIVE_BUFFER_SIZE];
	int cmdSize;

	char dev[]  = "/dev/ttyS2";

	fdUart = uartOpen(dev);
	uartSetSpeed(fdUart, 115200);

	if (uartSetParity(fdUart,8,1,'N') == -1)
	{
		printf("Set Parity Error\n");
		exit (0);
	}

	while (1)
	{
		while((nread = read(fdUart, buff, 512))>0)
		{
			if( nread+receiveIndex >= RECEIVE_BUFFER_SIZE )
			{
				printf("receiver buffer overflow. it must be something wrong.\r\n");
				// TODO...
				receiveIndex = 0;
			} 

			memcpy(receiveBuffer+receiveIndex, buff, nread); 
			receiveIndex += nread;

			if( (cmdSize = checkCommand(receiveBuffer, receiveIndex, cmdBuffer)) > 0 )
			{
				cmd_proc(cmdBuffer, cmdSize);
				receiveIndex = 0;
			}
		}
	}

	close(fdUart);
	return NULL;
}
#endif
// ******************************************
// send data to BLE device via UART port
// 
// Paramaters
//    cmd: the command which defined at bleProtocol.h
//    param: paramaters
//
// Return value
//    0: success
//    others: fail
// ******************************************
int bleSend(int fd, BLE_COMMANDS cmd, int param)
{
	unsigned char buf[64];
    int size = 0;

	switch( cmd )
	{
		case COMMAND_NAVI_DIRECTION:
			printf("[BLE] ready to send command[%d][%d]\r\n", cmd, param);
			size = makeCmdNaviDir(buf, param);
			break;

		default:
			break;
	}

	if( size > 0 )
	{
		printf("size=%d\r\n", size);
		write(fd, buf, size);
		printf("[BLE] [%d] bytes are sent\r\n", size);
		debugBuffer(buf, size);
	}

	return 0;
}

void bleStop(void)
{
}

void bleSetCallback(BLE_CALLBACK func)
{
	bleCallback = func;
}

int bleSendString(int fd, char *string)
{
	if( fd >= 0 )
		return uart_writes(fd, string);
	else
		return -1;
}

// end of file
