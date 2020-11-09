#include "data_global.h"
#include "uart_cache.h"
#include "sqlite_link_list.h"

pthread_cond_t cond_sqlite;//            数据库线程被唤醒条件变量
pthread_cond_t cond_analysis;//          数据分析线程被唤醒条件变量
pthread_cond_t cond_uart_cmd;//          M0控制命令发送线程被唤醒条件变量
pthread_cond_t cond_client_request;//    处理消息队列里请求的线程被唤醒条件变量
pthread_cond_t cond_infrared;       //   红外(按键模拟)监测线程被唤醒条件变量
pthread_cond_t cond_buzzer;       //     A8蜂鸣器控制线程被唤醒条件变量
pthread_cond_t cond_led;        //     A8LED模块线程被唤醒条件变量
pthread_cond_t cond_camera;      //      摄像头模块控制线程被唤醒条件变量   
pthread_cond_t cond_sms;         //      短信模块控制线程被唤醒条件变量
pthread_cond_t cond_refresh;      //     更新共享内存里的实时数据被唤醒条件变量
pthread_cond_t cond_refresh_updata;

pthread_mutex_t mutex_slinklist;    //   数据库缓存互斥锁
pthread_mutex_t mutex_sqlite;        //  数据库线程互斥锁
pthread_mutex_t mutex_analysis;       // 数据分析线程互斥锁    
//pthread_mutex_t mutex_analysis_chaek_env;        

pthread_mutex_t mutex_uart_cmd;        //M0控制命令发送线程互斥锁 
pthread_mutex_t mutex_client_request;  //处理消息队列里请求的线程互斥锁
pthread_mutex_t mutex_infrared;        //红外(按键模拟)监测线程互斥锁
pthread_mutex_t mutex_buzzer;          //A8蜂鸣器控制线程互斥锁 
pthread_mutex_t mutex_led;             //A8LED模块线程互斥锁
pthread_mutex_t mutex_camera;          //摄像头模块控制线程互斥锁
pthread_mutex_t mutex_sms;             //短信模块控制线程互斥锁
pthread_mutex_t mutex_refresh;         //更新共享内存里的实时数据互斥锁
pthread_mutex_t mutex_refresh_updata;
pthread_mutex_t mutex_global;          //全局变量保护互斥锁
pthread_mutex_t mutex_linklist;        //接收数据缓存互斥锁

struct storage_info storage_RT[STORAGE_NUM];//         各仓库实时信息

unsigned char dev_sms_cmd;             //A8短信模块操作掩码
unsigned char dev_infrared_mask;       //A8按键模块操作掩码
unsigned char dev_buzzer_mask;         //A8蜂鸣器模块操作掩码
unsigned char dev_led_mask;            //A8 LED模块操作掩码
unsigned char dev_camera_mask;         //A8摄像头模块操作掩码
unsigned char dev_uart_mask;           //A8串口发送命令掩码

uart_cache_list m0_cache_head, m0_cache_tail;

char recive_phone[12] = "18911947233";
char center_phone[12] = "13800100500";

int dev_infrared_fd;                   //A8按键模块设备节点
int dev_buzzer_fd;                     //A8蜂鸣器模块设备节点
int dev_led_fd;                        //A8 LED模块设备节点
int dev_camera_fd;                     //A8摄像头模块设备节点  
int dev_sms_fd;                        //A8短信模块设备节点
int dev_uart_fd;                       //A8U转串设备节点
 
int msgid;                             //消息队列ID
int shmid;                             //共享内存ID                      
int semid;                             //信号灯集ID

char cgi_status;
char qt_status;


/**** sqlite module ****/
int storageNum;
int goodsKinds;

struct env_info_clien_addr env_info_clien_addr_s;
struct env_info_clien_addr all_info_RT;

slinklist slinkHead, slinkTail;

void sendMsgQueue (long type, unsigned char text)
{
	struct msg msgbuf;
	msgbuf.type = 1L;
	msgbuf.msgtype = type;
	msgbuf.text[0] = text;
	msgsnd (msgid, &msgbuf, sizeof (msgbuf) - sizeof (long), 0);
}
