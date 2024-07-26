#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "uartFunc.h"
#include "bleProtocol.h"

// ************************************************************************************
// ChiuYi BLE commands
//
// package format
// <header><command><length><payload><checksum>
// header: 0xEF12
// command: 1 byte
// length: 1 byte
// payload: length bytes
// checksum: 1 byte. sum from command to payload
//
// baisc information
//
// nevigtion
//
// direction
// command: 0x03
// length: 4+N
// payload:
//	distance: 3 bytes
//	direction: 1 bytes
//		arrived			: 0
//		lower right		: 1
//		right			: 2
//		upper right		: 3
//		straight		: 4
//		upper left		: 5
//		left			: 6
//		lower left		: 7
//		left U turn		: 8
//		right U turn	: 9
//	name:	N bytes
//
// ************************************************************************************

#define RECEIVE_BUFFER_SIZE	512

void printMenu()
{
	printf("******************************************************\r\n");
	printf("Please input following instructions to apply command\r\n");
	printf("0: send arrived\r\n");
	printf("1: send lower right\r\n");
	printf("2: send right\r\n");
	printf("3: send upper right\r\n");
	printf("4: send straight\r\n");
	printf("5: send upper left\r\n");
	printf("6: send left\r\n");
	printf("7: send lower left\r\n");
	printf("8: send left U turn\r\n");
	printf("9: send right U turn\r\n");
	printf("x: exit\r\n");
	printf("******************************************************\r\n");	
}

int main(int argc, char **argv)
{
	bool KeepGoing = true;
	int fdUart;	
	int nread;
	unsigned char buff[RECEIVE_BUFFER_SIZE];	

	unsigned char receiveBuffer[RECEIVE_BUFFER_SIZE];
	int receiveIndex = 0;

	unsigned char cmdBuffer[RECEIVE_BUFFER_SIZE];
	int cmdSize;
	
	char cmd;
	
	if( argc != 2 )
	{
		printf("USAGE: uart_transmit UART_PORT\r\n");
		return -1;
	}

	fdUart = uartOpen(argv[1]);
	uartSetSpeed(fdUart, 115200);

	if (uartSetParity(fdUart,8,1,'N') == -1)
	{
		printf("Set Parity Error\n");
		return -1;
	}
	else
	{
		printf("%s connected\r\n", argv[1]);
	}
	
	while (KeepGoing)
	{
		printMenu();
		std::cin >> cmd;
		
		switch( cmd )
		{
			case '0':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_ARRIVED);
				break;
			case '1':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_LOWER_LEFT);
				break;
			case '2':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_LEFT);
				break;
			case '3':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_UPPER_LEFT);
				break;
			case '4':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_STRAIGHT);
				break;
			case '5':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_UPPER_RIGHT);
				break;
			case '6':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_RIGHT);
				break;
			case '7':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_LOWER_RIGHT);
				break;
			case '8':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_LEFT_U_TURN);
				break;
			case '9':
				bleSend(fdUart, COMMAND_NAVI_DIRECTION, NAVIDIR_RIGHT_U_TURN);
				break;
			case 'a':
				printf("Send (%d)\r\n", bleSendString(fdUart, (char*)"Hi I am Patrick.\r\n"));
				break;
			case 'X':
			case 'x':
				KeepGoing = false;
				break;
				
			default:
				printf("Unknown command (%c)\r\n", cmd);
				break;
		}
		
#if 0	
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
			
			for(int i=0; i<receiveIndex; i++)
			{
				printf("%c", receiveBuffer[i]);
			}
			
			receiveIndex = 0;
		}
#endif		
	}

	close(fdUart);
	
	return 0;
}

// end of file

