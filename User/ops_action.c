#include "ops_action.h"
#include "stdio.h"

#define OpsUsart_BUFFERSIZE 255	

ops_t ops={
0,0,0,0,0,0,0,0,0,0
};
uint8_t Rx_len_OpsUsart;
uint8_t ReceiveBuff_OpsUsart[OpsUsart_BUFFERSIZE]; 
void opsusart_receive_handler(UART_HandleTypeDef *huart,DMA_HandleTypeDef*hdma_usart_rx){
		 uint32_t temp;//���㴮�ڽ��յ������ݸ���
    static union
    {
        uint8_t date[24];
        float ActVal[6];
    } posture;
			if(RESET != __HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE))//���Ϊ����2����
			{
					__HAL_UART_CLEAR_IDLEFLAG(huart);//����жϱ�־
					HAL_UART_DMAStop(huart);//ֹͣDMA����
					temp  = __HAL_DMA_GET_COUNTER(hdma_usart_rx);//��ȡDMA��ǰ���ж���δ��� hdma_usart6_rx
					Rx_len_OpsUsart =  OpsUsart_BUFFERSIZE - temp; //���㴮�ڽ��յ������ݸ��� Rx_len_Huart6
					/*************************************************************************/
					//�������ݴ���
					if(Rx_len_OpsUsart==28)
					{
							for(int i=0; i<24; i++)
							{
									posture.date[i]=ReceiveBuff_OpsUsart[i+2]; //ReceiveBuff_Huart6
							}
							ops.zangle=-posture.ActVal[0];
							ops.xangle=posture.ActVal[1];
							ops.yangle=posture.ActVal[2];
							ops.pos_x=posture.ActVal[3];
							ops.pos_y=posture.ActVal[4];
							ops.w_z=posture.ActVal[5];
					}
					/*************************************************************************/
					//���¿�����һ�ν���
					//memset(ReceiveBuff_Huart2,0,sizeof(ReceiveBuff_Huart2));
					Rx_len_OpsUsart=0;//�������ݳ�������
					HAL_UART_Receive_DMA(&huart2,ReceiveBuff_OpsUsart,OpsUsart_BUFFERSIZE);//������һ�ν���

			}
}
ops_t* get_location_control_point(void){
	return &ops;
}