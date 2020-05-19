#ifndef __USART3_H
#define __USART3_H 
#include "sys.h"
#include "stdio.h"

#define USART3_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.

extern uint8_t USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern uint8_t  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern uint16_t USART3_RX_STA;   						//接收数据状态

void usart3_init(uint32_t bound);
void u3_printf(char* fmt,...);
#endif	   
















