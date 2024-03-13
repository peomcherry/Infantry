#ifndef _OPS_ACTION_H
#define _OPS_ACTION_H

#include "main.h"
#include "usart.h"
typedef  struct
{
	float pos_x;//����X--ZBx
	float pos_y;//����Y--ZBy
	float zangle;//�����
	float xangle;//������
	float yangle;//�����
	float w_z;//�������

	float set_pos_x;
	float set_pos_y;
	float set_zangle;
	int move_flag;
	
} ops_t;

void opsusart_receive_handler(UART_HandleTypeDef *huart,DMA_HandleTypeDef*hdma_usart_rx);
ops_t* get_location_control_point(void);
#endif
