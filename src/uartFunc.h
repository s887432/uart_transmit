#ifndef __UARTFUNC_H__
#define __UARTFUNC_H__

int uartOpen(char *Dev);
void uartSetSpeed(int fd, int speed);
int uartSetParity(int fd,int databits,int stopbits,int parity);

int uart_writen(int fd, char *buf, size_t buf_len);
int uart_writes(int fd, char *string);

#endif // end of __UARTFUNC_H__

// end of file

