#ifndef __BLE_PROTOCOL_H__
#define __BLE_PROTOCOL_H__

// ******************************************
// global type define
// ******************************************
typedef enum __BLE_COMMANDS__
{
	COMMAND_NONE = 0,
	COMMAND_DRIVE_INTO,
	COMMAND_ESTIMATE_MILES,
	COMMAND_NAVI_DIRECTION,
	COMMAND_CALLING_INFO,
	COMMAND_MSG_KEY,
}BLE_COMMANDS;

typedef enum __NAVI_DIRECTIONS_
{
	NAVIDIR_ARRIVED=0,
	NAVIDIR_LOWER_LEFT,
	NAVIDIR_LEFT,
	NAVIDIR_UPPER_LEFT,
	NAVIDIR_STRAIGHT,
	NAVIDIR_UPPER_RIGHT,
	NAVIDIR_RIGHT,
	NAVIDIR_LOWER_RIGHT,
	NAVIDIR_LEFT_U_TURN,
	NAVIDIR_RIGHT_U_TURN,
}NAVI_DIRECTIONS;

typedef void (*BLE_CALLBACK)(unsigned char, unsigned char);
// ******************************************
// BLE device receiver thread process
//
// Paramaters
//    none
//
// Return value
//    none
// ******************************************
void *bleProc(void *ptr);

// ******************************************
// send data to BLE device via UART port
// 
// Paramaters
//    cmd: BLE command
//    param: paramaters
//
// Return value
//    0: success
//    others: fail
// ******************************************
int bleSend(int fd, BLE_COMMANDS cmd, int param);

void bleSetCallback(BLE_CALLBACK func);
int bleSendString(int fd, char *string);

#endif // end of __BLE_PROTOCOL_H__

// end of file

