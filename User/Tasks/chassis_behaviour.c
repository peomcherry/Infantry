#include "chassis_behaviour.h"
#include "math.h"
uint16_t Vz=0;
#define CHASSIS_OPEN_RC_SCALE 0.2 // in CHASSIS_OPEN mode, multiply the value. ��chassis_open ģ���£�ң�������Ըñ������͵�can��
extern float yaw_current;
#define yaw yaw_current/180*3.14f
float beta=0;
/**
  * @brief          ͨ���жϽ��ٶ����ж���̨�Ƿ񵽴Ｋ��λ��
  * @param          ��Ӧ��Ľ��ٶȣ���λrad/s
  * @param          ��ʱʱ�䣬����GIMBAL_CALI_STEP_TIME��ʱ������
  * @param          ��¼�ĽǶ� rad
  * @param          �����ĽǶ� rad
  * @param          ��¼�ı���ֵ raw
  * @param          �����ı���ֵ raw
  * @param          У׼�Ĳ��� ���һ�� ��һ
  */
#define chassis_cali_gyro_judge(gyro, cmd_time, angle_set, angle, ecd_set, ecd, step) \
    {                                                                                \
        if ((gyro) < GIMBAL_CALI_GYRO_LIMIT)                                         \
        {                                                                            \
            (cmd_time)++;                                                            \
            if ((cmd_time) > GIMBAL_CALI_STEP_TIME)                                  \
            {                                                                        \
                (cmd_time) = 0;                                                      \
                (angle_set) = (angle);                                               \
                (ecd_set) = (ecd);                                                   \
                (step)++;                                                            \
            }                                                                        \
        }                                                                            \
    }





static void chassis_zero_force_control(fp32 *vx_can_set, fp32 *vy_can_set, fp32 *wz_can_set, chassis_move_t *chassis_move_rc_to_vector)
{
    if (vx_can_set == NULL || vy_can_set == NULL || wz_can_set == NULL || chassis_move_rc_to_vector == NULL)
    {
        return;
    }
		for(uint8_t i = 0; i<4;i++){
    chassis_move_rc_to_vector->Wheel_Speed[i].give_current = 0.0f;
		chassis_move_rc_to_vector->Wheel_Dir[i].give_current = 0.0f;
		}
}
static void chassis_no_move_control(fp32 *Vx_Set, fp32 *Vy_Set, fp32 *Vw_Set, chassis_move_t *Chassis_Move_Rc_to_Vector)
{
    if (Vx_Set == NULL || Vy_Set == NULL || Vw_Set == NULL || Chassis_Move_Rc_to_Vector == NULL)
    {
        return;
    }
		
        *Vx_Set = 0.0f;
        *Vy_Set = 0.0f;
        *Vw_Set = 0.0f;
}

static void chassis_remote_move_control(fp32 *Vx_Set, fp32 *Vy_Set, fp32 *Vw_Set, chassis_move_t *Chassis_Move_Rc_to_Vector)
{
    if (Vx_Set == NULL || Vy_Set == NULL || Vw_Set == NULL || Chassis_Move_Rc_to_Vector == NULL)
    {
        return;
    }


//    *Vx_Set = Chassis_Move_Rc_to_Vector->Chassis_RC->ch1 *CHASSIS_OPEN_RC_SCALE;
//    *Vy_Set = Chassis_Move_Rc_to_Vector->Chassis_RC->ch2 *CHASSIS_OPEN_RC_SCALE;
//    *Vw_Set = Chassis_Move_Rc_to_Vector->Chassis_RC->ch3 *CHASSIS_OPEN_RC_SCALE;

		*Vx_Set = Chassis_Move_Rc_to_Vector->my_chassis_rc->vx *CHASSIS_OPEN_RC_SCALE*5;
    *Vy_Set = Chassis_Move_Rc_to_Vector->my_chassis_rc->vy *CHASSIS_OPEN_RC_SCALE*5;
    *Vw_Set =- Chassis_Move_Rc_to_Vector->my_chassis_rc->vw *CHASSIS_OPEN_RC_SCALE*5;
//		
//		*Vx_Set =*Vy_Set * cos(yaw_current/180*3.14f) + *Vx_Set * sin(yaw_current/180*3.14f); 
//    *Vy_Set = *Vy_Set * sin(yaw_current/180*3.14f) + *Vx_Set * cos(yaw_current/180*3.14f);
		float vx= *Vx_Set;
		float vy=*Vy_Set;
		*Vx_Set =(-1.f)*vy * sin(yaw_current/180*3.14f) + vx * cos(yaw_current/180*3.14f); 
    *Vy_Set = vy * cos(yaw_current/180*3.14f) + vx * sin(yaw_current/180*3.14f);
//		beta = atan(*Vy_Set/(*Vx_Set));
//		Vz = sqrt(*Vx_Set * (*Vx_Set) + (*Vy_Set) *(*Vy_Set));
//		if(yaw+beta >=0 && yaw+beta <=1.57){
//			*Vx_Set = Vz*cos(yaw+beta);
//			*Vy_Set = Vz*sin(yaw+beta);
//		}else if(yaw +beta >1.57 && yaw +beta <=3.14){
//			*Vx_Set = -1*Vz*cos(3.14f-yaw+beta);
//			*Vy_Set = Vz*sin(3.14f-yaw+beta);
//		}else if(yaw +beta >180 &&yaw +beta<=270){
//			*Vx_Set = -1*Vz*cos(yaw+beta-3.14f);
//			*Vy_Set = -1*Vz*sin(yaw+beta-3.14f);			
//		}else{
//			*Vx_Set = Vz*sin(6.28f-yaw+beta);
//			*Vy_Set = -1*Vz*cos(6.28f-yaw+beta);			
//		}

    return;
}



static void chassis_local_move_control(fp32 *Vx_Set, fp32 *Vy_Set, fp32 *Vw_Set, chassis_move_t *Chassis_Move_Rc_to_Vector)
{
	if (Vx_Set == NULL || Vy_Set == NULL || Vw_Set == NULL || Chassis_Move_Rc_to_Vector == NULL)
    {
        return;
    }
		int Kp_V0 = 3;
    int Kp_W = 1500;//40000

    int V0_MAX = 200;
    int V0_MIN = -200;
    int W_MAX = 200;
    int W_MIN = -200;


    //�����ٶ�
    float Vx = 0; //x�����ٶ�
    float Vy = 0; //y�����ٶ�
    float V0 = 0; //�����˶������ٶ�
    float W = 0;  //��ת�ٶ� ˳ʱ��Ϊ��
    int deta=0;
    static float V0_last = 0; //��һʱ�̵����˶��ٶȴ�С,�����ٶȵ������޷�
    //�����ٶȷ���Ƕ�
    float Angle_speed;          //�����ٶȷ�����X��������н� [0 , 180]
    float Angle_speed_current;  //��֪ħ���ȡ�ĽǶ�(yaw) [-180,180]
		
		float X_target = Chassis_Move_Rc_to_Vector->Postion_X_Set;
	  float Y_target = Chassis_Move_Rc_to_Vector->Postion_Y_Set;
		float Angle_target = Chassis_Move_Rc_to_Vector->Chassis_Yaw_Set;
    float X_current   =Chassis_Move_Rc_to_Vector->Postion_X;
		float Y_current=Chassis_Move_Rc_to_Vector->Postion_Y;
		float Angle_current =Chassis_Move_Rc_to_Vector->Chassis_Yaw;
    //�Ƕ�ת��Ϊ����
		#define PI  3.1415926f
    Angle_target = Angle_target / 180.0 * PI;
    Angle_current = Angle_current / 180.0 * PI;
    //Angle_speed = Angle_current;
    /**�����ٶȷ���*/
		if (X_current == X_target)
		 Angle_speed = PI / 2.0;
    else
		 Angle_speed = atan((Y_target - Y_current) / (X_target - X_current));
    if (Angle_speed < 0)
        Angle_speed = Angle_speed + PI;
    if (Y_target > Y_current)
		Angle_speed_current = PI / 2 - (Angle_speed + Angle_current);
    else
        Angle_speed_current = 3 * PI / 2 - (Angle_speed + Angle_current);

#define abs(x)        ((x>0)? (x): (-x))

    /**��V0*/
    V0 = Kp_V0 * sqrt((Y_target - Y_current) * (Y_target - Y_current) + (X_target - X_current) * (X_target - X_current));
    /**V0�޷�*/
    deta = V0 - V0_last;
    if (deta > 50) {
        deta = 50 * deta / abs(deta);
    }
    V0 = V0_last + deta;
    V0_last = V0;
    /** ����Vx Vy W */
    V0 = V0 < V0_MAX ? V0 : V0_MAX;
    V0 = V0 > V0_MIN ? V0 : V0_MIN;

    Vx = V0 * sin(Angle_speed_current);
    Vy = V0 * cos(Angle_speed_current);

    W = Kp_W * (Angle_target - Angle_current);
    W = W < W_MAX ? W : W_MAX;
    W = W > W_MIN ? W : W_MIN;
		
		*Vx_Set = Vx;
    *Vy_Set = Vy;
    *Vw_Set = W;

    return;
}


//���⣬���������Ϊģʽ����
chassis_behaviour_mode_e Chassis_Behaviour_Mode; //= CHASSIS_REMOTE_MOVE;

/**
  * @brief          ͨ���߼��жϣ���ֵ"chassis_behaviour_mode"������ģʽ
  * @param[in]      chassis_move_mode: ��������
  * @retval         none
  */
void chassis_behaviour_mode_set(chassis_move_t *Chassis_Move_Mode)
{
    if (Chassis_Move_Mode == NULL)
    {
        return;
    }
		Chassis_Behaviour_Mode = CHASSIS_REMOTE_MOVE;
    //������Ϊģʽѡ��һ�����̿���ģʽ
    //����Լ����߼��жϽ�����ģʽ
//    if(Chassis_Move_Mode->Chassis_RC->ch5 == 1)
//    {
//        Chassis_Behaviour_Mode = CHASSIS_ZERO_FORCE;
//    }
//    else if(Chassis_Move_Mode->Chassis_RC->ch5 == 2)
//    {
//        Chassis_Behaviour_Mode = CHASSIS_NO_MOVE;
//    }
//    else if(Chassis_Move_Mode->Chassis_RC->ch5 == 2)
//    {
//        Chassis_Behaviour_Mode = CHASSIS_LOCATING_MOVE;
//    }
//    else if(Chassis_Move_Mode->Chassis_RC->ch5 ==3)
//    {
//        Chassis_Behaviour_Mode = CHASSIS_REMOTE_MOVE;
//    }


    //������Ϊģʽѡ��һ�����̿���ģʽ
    if (Chassis_Behaviour_Mode == CHASSIS_ZERO_FORCE) //����
    {
        Chassis_Move_Mode->Chassis_Mode = CHASSIS_ZERO_FORCE;
    }
    else if (Chassis_Behaviour_Mode == CHASSIS_NO_MOVE) //�̶�
    {
        Chassis_Move_Mode->Chassis_Mode = CHASSIS_NO_MOVE;
    }
    else if (Chassis_Behaviour_Mode == CHASSIS_LOCATING_MOVE) //��λ
    {
        Chassis_Move_Mode->Chassis_Mode = CHASSIS_LOCATING_MOVE;
    }
    else if (Chassis_Behaviour_Mode == CHASSIS_REMOTE_MOVE) //ң��
    {
        Chassis_Move_Mode->Chassis_Mode = CHASSIS_REMOTE_MOVE;
    }else if (Chassis_Behaviour_Mode == CHASSIS_DEBUG_MOED) //����
    {
        Chassis_Move_Mode->Chassis_Mode = CHASSIS_DEBUG_MOED;
    }
}

//	 CHASSIS_ZERO_FORCE,                   //��������, ��û�ϵ�����
//  CHASSIS_NO_MOVE,                      //���̱��ֲ���
//	 CHASSIS_LOCATING_MOVE,                //���̶�λ
//	 CHASSIS_REMOTE_MOVE                   //ң�ؿ���
void chassis_behaviour_control_set(fp32 *Vx_Set, fp32 *Vy_Set, fp32 *Vw_Set, chassis_move_t *Chassis_Move_Rc_To_Vector)
{
    if (Vx_Set == NULL || Vy_Set == NULL || Vw_Set == NULL || Chassis_Move_Rc_To_Vector == NULL)
    {

        return;
    }
    //������Ϊģʽѡ��һ�����̿���ģʽ

    if (Chassis_Behaviour_Mode == CHASSIS_ZERO_FORCE)  //��������, ��û�ϵ�����
    {
        chassis_zero_force_control(Vx_Set, Vy_Set, Vw_Set, Chassis_Move_Rc_To_Vector);
    }
    else if (Chassis_Behaviour_Mode == CHASSIS_NO_MOVE)  //�̶����� ���ƶ�
    {
        chassis_no_move_control(Vx_Set, Vy_Set, Vw_Set, Chassis_Move_Rc_To_Vector);
    }
    else if (Chassis_Behaviour_Mode == CHASSIS_LOCATING_MOVE)//���̶�λ��ע��Ŷ
    {
			chassis_local_move_control(Vx_Set, Vy_Set, Vw_Set, Chassis_Move_Rc_To_Vector);
    }
    else if (Chassis_Behaviour_Mode == CHASSIS_REMOTE_MOVE)
    {
        chassis_remote_move_control(Vx_Set, Vy_Set, Vw_Set, Chassis_Move_Rc_To_Vector);
    }


    //ң��������ģʽ
    //����̨��ĳЩģʽ�£����ʼ���� ���̲���
//    if (gimbal_cmd_to_chassis_stop())
//    {
//        chassis_behaviour_mode = CHASSIS_NO_MOVE;
//    }

}


