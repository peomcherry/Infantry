#include "my_usart_recieve.h"
#include "stdio.h"

my_usart_rc_t my_rc;
int8_t my_receieve_buff[20];
char cimu_buff[25];
cimu_t cimu;
void my_rc_judge(my_usart_rc_t *rc){
//		if(rc->vw>0)rc->vw=-500;else if(rc->vw>0)rc->vw=500;else rc->vw=0;
		return;
}

void my_usart_solver(int8_t*my_receieve_buff){

		for(uint8_t i =0;i<20;i++){
			if(my_receieve_buff[i]==0x6b){
				my_rc.armer_flag= my_receieve_buff[++i];
				my_rc.error_x= my_receieve_buff[++i];
				my_rc.error_y= my_receieve_buff[++i];
				my_rc_judge(&my_rc);
				break;
			}
			memset(my_receieve_buff,0,20);
			return;
		}
}



void my_usart_handle(UART_HandleTypeDef*my_usart){
		
		if(__HAL_UART_GET_FLAG(my_usart,UART_FLAG_IDLE)!=RESET)
    {
			__HAL_UART_CLEAR_IDLEFLAG(my_usart);//�����־λ
			HAL_UART_Receive_DMA(my_usart, my_receieve_buff,20);
		  HAL_UART_DMAStop(my_usart); //ֹͣDMA���գ���ֹ���ݳ���
			my_usart_solver(my_receieve_buff);
      HAL_UART_Receive_DMA(my_usart, my_receieve_buff,20);
		}		
	
}

my_usart_rc_t* get_my_usart_rc_point(void){

 return &my_rc;

}
void cimu_solver(char * cimu_buff){

    /*���ַ��� str �дӵڶ����ַ���ʼ�������ݣ�������ƥ���ʽ "%f,%f:"������ɹ��������������������ַ����ĸ�ʽ����ȷ�ģ��Զ��Ž�β�����򷵻�ֵ����2*/
	if (sscanf(cimu_buff, ":%f,%f,%f:", &cimu.Yaw,&cimu.Pitch,&cimu.Roll) != 3)
    {
				memset(cimu_buff,0,20); 
			
        return;
    }
	
		

}
void cimu_handle(UART_HandleTypeDef*my_usart){

	if(__HAL_UART_GET_FLAG(my_usart,UART_FLAG_IDLE)!=RESET)
    {
			__HAL_UART_CLEAR_IDLEFLAG(my_usart);//�����־λ
			HAL_UART_Receive_DMA(my_usart, cimu_buff,25);
		  HAL_UART_DMAStop(my_usart); //ֹͣDMA���գ���ֹ���ݳ���
			cimu_solver(cimu_buff);
      HAL_UART_Receive_DMA(my_usart, cimu_buff,25);
		}		
	


}


