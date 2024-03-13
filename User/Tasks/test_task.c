#include "cmsis_os.h"
#include "can.h"
#include "dj_motor_driver.h"
#include "stdio.h"
#include "pid.h"
#include "shell.h"
#include "commend.h"
#include <stdarg.h>
#include <stdlib.h>
#include "elog.h"
#include "bsp_sbus.h"
#include "a_led_buzzer_key.h"
#include "dj_motor_driver.h"
#include "bsp_imu.h"
#include "my_usart_recieve.h"
#include "math.h"
		extern PID_T pid1;
		extern PID_T pid2;
		extern float p,i,d;
		extern rc_info_t rc;
		extern imu_t imu;
		extern my_usart_rc_t my_rc;
		extern cimu_t cimu;
		extern void AHRSData2PC(void);
		extern float yaw_current;
void test_task(void const * argument)
{
	/* ��ʼ��elog */
	elog_init();
	/* ����ÿ���������־�����*/
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	//�����־������Ϣ����־TAG
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG);
	//����ʱ�䡢������Ϣ�߳���Ϣ֮�⣬����ȫ�����
	//elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_TIME | ELOG_FMT_P_INFO | ELOG_FMT_T_INFO));
	
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);
	elog_set_text_color_enabled(true);
	/* ����elog */
	elog_start();
	dbus_uart_init();
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET);
	while(1)
	{	
		//AHRSData2PC();
		elog_raw_output("%d,%d,%d\n",my_rc.armer_flag,my_rc.error_x,my_rc.error_y);
		//elog_raw_output(" %d,%d,%d,%d,%d,%d \n",my_rc.vy,my_rc.vx,my_rc.vw,my_rc.yaw_speed,my_rc.pitch_speed,my_rc.shot_frequency);
		osDelay(200);
		
		
	}
}





