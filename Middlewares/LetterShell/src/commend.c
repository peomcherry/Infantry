/*
 * commend.c
 *
 *  Created on: 2023年3月2日
 *      Author: Administrator
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commend.h"
#include "shell.h"
#include "elog.h"
#include "cmsis_os.h"

#include "chassis_task.h"
char letter_recievebuf[100];
extern chassis_move_t Chassis_Move;
extern void set_pos(float X_target, float Y_target, float Angle_target,
             float X_current, float Y_current, float Angle_current,
							 chassis_move_t *Chassis_Move_Control_Loop) ;	
extern void Set_speed(int Velocity_X, int Velocity_Y, int W,chassis_move_t *Chassis_Move_Control_Loop);
						 
var_list_t var_list;
var_list_t* _var_begin = 0;

#define var_logInfo(...) elog_i("CMD_VAR",__VA_ARGS__);

int a = -1234567890;
int a1 = 0;
char b = 1;
double c = -1.123456789;
#define __NAME(restrc,x,name) strcpy(name,#x)

//请在这个函数中添加变量使该变量可以在命令行实时查看和修改
//注册方式如下，单一变量和数组注册方式不同
//使用：   var 或 /var all  列出所有注册变量
//        var a         查看变量a的值
//        var a 1.2     修改变量a的值为1.2
//默认变量均为有符合类型，超过类型大小的数会显示负数
void var_init(void)
{

  //测试使用
    //注册变量；变量，变量大小
    VAR_CMD_REGISTER(b, _8);
    VAR_CMD_REGISTER(a1, _32);
    VAR_CMD_REGISTER(c, _lf);//double类型



//    var_register(&gps_tau1201.latitude,"curlat",_lf);
//    var_register(&gps_tau1201.longitude,"curlon",_lf);

//将变量a注册
    VAR_CMD_REGISTER(a, _32);//4字节的
	//extern pid_t TripodHead_pid;
	//var_register(&TripodHead_pid.p,"kp",_f);
	//var_register(&TripodHead_pid.i,"ki",_f);
	//var_register(&TripodHead_pid.d,"kd",_f);
	//var_register(&target,"t",_f);
       // gps_latitude[i] = gps_control.gps_location.latitude_temp[i];
        //gps_longitude[i] = gps_control.gps_location.longitude_temp[i];
//    var_arr_register(gps_control.gps_location.latitude_temp,"gpslat",_lf,max);
//    var_arr_register(gps_control.gps_location.longitude_temp,"gpslon",_lf,max);

}

/////////////////////////////////Commend Begin//////////////////////////////////////
//请在 Commend Begin 和 Commend End 之间编写命令，并在命令函数后面跟上宏Commend Begin导出命令函数
//为方便使用，高移植性和统一规范，统一使用 int function(int argc, char** argv)格式
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), data, data, creat or change data);
//int sfud(int argc, char** argv)
//{
//	extern void sfud_test(uint32_t addr);
//	if(argc == 1)
//	{
//		//sfud
//		log_i("you have paeam, doing sfud test");
//		sfud_test(0x00000);
//		return 0;
//	}
//	if(argc >= 2)
//	{
//		if(0 == strcmp(argv[1],"test"))
//		{
///////////////////sfud tset ... ////////////////
//			if(argc >= 3)
//			{
//				int addr = strtol(argv[2],NULL,0);
//				if(addr == 0x00800000)
//				{
//					log_i("addr error");
//					return -1;
//				}
//				sfud_test(addr);

//			}else
//			{
//				sfud_test(0x00000);
//				return 0;
//			}
////////////////////////////////////////////////
///////////////////sfud info////////////////////
//		if(0 == strcmp(argv[1],"info"))
//		{
//		
//		}
////////////////////////////////////////////////			
//		}
//	
//	}



//		return -1;
//}SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), sfud, sfud, spi flash driver commends);



int reboot(int argc, char** argv)
{
	elog_w("CMD_REBOOT","The system will reboot...");
	osDelay(100);
	HAL_NVIC_SystemReset();
	return 0;

}SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), reboot, reboot, spi flash driver commends);
//打印已有命令
int ls(int argc, char** argv)
{
	elog_raw_output("reboot\nsets\nsetp\nsend\n");
	osDelay(100);
	return 0;

}SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), ls, ls, spi flash driver commends);

//使用 支持有空格的输入
//send this is a string with some spaces或者send “this is a string with some spaces”
//“this is a string with some spaces”会保存在letter_recievebuf里
int send(int argc, char** argv)
{
		if(argc==1){
			elog_raw_output("format error!\r\n");
			return 0;
		}else{
			char temp[100];
			strcpy(temp,argv[1]);
			for(char i =2;i<argc;i++){
				sprintf(temp,"%s %s",temp,argv[i]);
			}
			strcpy(letter_recievebuf,temp);
			elog_raw_output("%s\n",letter_recievebuf);
		}
		return 0;
}SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), send, send, spi flash driver commends);
//使用 
//sets vx vy vw （整数）
//设置底盘速度
int set_chassis_speed(int argc, char** argv)
{
		if(argc!=4){
			elog_raw_output("format error!!!you should input sets vx vy vw end with '\\n'\r\n");
			return 0;
		}else{
			int vx=atoi(argv[1]);
			int vy=atoi(argv[2]);
			int vw=atoi(argv[3]);
			Set_speed(vx,vy,vw,&Chassis_Move);
		}
		return 0;
}SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), sets, set_chassis_speed, spi flash driver commends);
//使用 
//setp x_target y_target zangle_target （整数）
//设置底盘定位
int set_chassis_position(int argc, char** argv)
{
		if(argc!=4){
			elog_raw_output("format error!!!you should input setp x_target y_target zangle_target end with '\\n'\r\n");
			return 0;
		}else{
			int spx=atoi(argv[1]);
			int spy=atoi(argv[2]);
			int sw=atoi(argv[3]);
			set_pos(Chassis_Move.Postion_X,Chassis_Move.Postion_Y,Chassis_Move.Chassis_Yaw,spx,spy,sw,&Chassis_Move);
		}
		return 0;
}SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), setp, set_chassis_position, spi flash driver commends);














/////////////////////////////////Commend End/////////////////////////////////////////////////
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), data, data, creat or change data);


//参数一命令
//参数2 变量名
//参数3 变量值//可选
void var(int argc, char** argv)
{
    var_list_t* _var = _var_begin;
    var_list_t* _var_second=0;
    int is_assign=0;
    //var_logInfo("var enter\r\n",0);
    if(argc==1)
    {
        var_logInfo("\r\n");
       while (_var)
       {
                var_logInfo("\t-- %s \r\n", _var->name);
              _var = _var->next;

       }
       return;

    }

    if (argc > 4) {
        var_logInfo("paramer error\r\n");
        return;
    }

    if(0==strcmp(argv[1], "all")){

        while (_var)
        {
            var_logInfo("\t--%s \r\n", _var->name);
            _var = _var->next;

        }
        return;
    }

    while (0!=strcmp(argv[1], _var->name))
    {

        _var = _var->next;
        if (_var == 0)
        {
            var_logInfo("We don't have that variable here\r\n");
            return;
        }//未找到变量
    }//已经找到变量
    if(argc==4){

        if(strcmp(argv[2],"=")==0){
                if(argc==4){
                    _var_second=_var_begin;
                    while (0!=strcmp(argv[3], _var_second->name))
                    {

                        _var_second = _var_second->next;
                        if (_var_second == 0)
                        {
                            var_logInfo("There's no variable here(var tow)\r\n");
                            return;
                        }//未找到变量
                    }//已经找到变量
                    is_assign=1;
                }

            }
    }else is_assign=0;



    switch (_var->type)
    {

    case _8:
        if(argc == 3 && is_assign==0){
            *(char*)_var->body = (char)atoi(argv[2]);
        }else if(is_assign==1){
            *(char*)_var->body = *(char*)_var_second->body;
        }
        var_logInfo("%s is %d\r\n", _var->name, *(char*)_var->body);
        break;
    //case _8:
    //    var_logInfo("%s is %d", _var->name, *(unsigned char*)_var->body);

    //    break;
    case _16:
        if(argc == 3&& is_assign==0){
            *(short*)_var->body = (short)atoi(argv[2]);
        }else if(is_assign==1){
            *(short*)_var->body = *(short*)_var_second->body;
        }
        var_logInfo("%s is %d\r\n", _var->name, *(short*)_var->body);

        break;
    //case _u16:
    //    var_logInfo("%s is %d", _var->name, *( unsigned short*)_var->body);

    //    break;
    case _32:
        if(argc == 3&& is_assign==0){
            *(int*)_var->body = atoi(argv[2]);
        }else if(is_assign==1){
            //var_logInfo("test\r\n",0);

            *(int*)_var->body = *(int*)_var_second->body;
        }
        var_logInfo("%s is %d\r\n", _var->name, *(int*)_var->body);

        break;
    //case _u32:
    //    var_logInfo("%s is %d", _var->name, *(unsigned int*)_var->body);
    //    break;
    case _64:
        if(argc == 3&& is_assign==0){
            *(long long*)_var->body = atoi(argv[2]);
        }else if(is_assign==1){
            *(long long*)_var->body = *(long long*)_var_second->body;
        }
        var_logInfo("%s is %d\r\n", _var->name, *(int32_t*)_var->body);//由于使用microlib所以只能输出32位字符输出

        break;
    //case _u64:
    //    var_logInfo("%s is %d", _var->name, *(unsigned long long*)_var->body);

    //    break;
    case _f:
        if(argc == 3&& is_assign==0){
            *(float*)_var->body = (float)atof(argv[2]);
        }else if(is_assign==1){
            *(float*)_var->body = *(float*)_var_second->body;
        }
        var_logInfo("%s is %f\r\n", _var->name, *(float*)_var->body);

        break;
    case _lf:
        if(argc == 3&& is_assign==0){
            *(double*)_var->body = atof(argv[2]);
        }else if(is_assign==1){
            *(double*)_var->body = *(double*)_var_second->body;
        }
        var_logInfo("%s is %lf\r\n", _var->name, *(double*)_var->body);

        break;
    default:
        break;
    }
}SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), var, var, see or change variable);

void var_arr_register(void *var,const char* name, var_type_e type,int size)
{
    char name1[20]={0};
    switch (type)
        {

        case _8:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((char*)var)[i],name1,type);
            }
            break;
			
			
        //case _8:
        //    var_logInfo("%s is %d", _var->name, *(unsigned char*)_var->body);

        //    break;
        case _16:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((short*)var)[i],name1,type);
            }
            break;
        //case _u16:
        //    var_logInfo("%s is %d", _var->name, *( unsigned short*)_var->body);

        //    break;
        case _32:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((int*)var)[i],name1,type);
            }
            break;
        //case _u32:
        //    var_logInfo("%s is %d", _var->name, *(unsigned int*)_var->body);
        //    break;
        case _64:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((long long*)var)[i],name1,type);
            }
            break;
        //case _u64:
        //    var_logInfo("%s is %d", _var->name, *(unsigned long long*)_var->body);

        //    break;
        case _f:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((float*)var)[i],name1,type);
            }
            break;
        case _lf:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((double*)var)[i],name1,type);
            }
            break;
        default:
            break;
        }



}
void var_register(void* var,const char* name, var_type_e type)
{
    var_list_t* p = (var_list_t*)malloc(sizeof(var_list_t));

    if (p == NULL)return;
    var_list_t* p2;
    if (var != NULL)p->body = var;
    else return;
    if (name != NULL)
    {
        int len=strlen(name);
        char* pname = (char*)malloc(sizeof(char)*len+1);
        strcpy(pname,name);
        p->name = pname;
    }
    else return;
    p->type = type;
    p->next = 0;
    if (!_var_begin) {//
        _var_begin = p;
        return;
    }
    p2 = _var_begin;
    while ((p2->next)!=0)
    {
        p2 = p2->next;
    }//找到最后一个变量
    p2->next = p;//添加变量到单向链表
}


