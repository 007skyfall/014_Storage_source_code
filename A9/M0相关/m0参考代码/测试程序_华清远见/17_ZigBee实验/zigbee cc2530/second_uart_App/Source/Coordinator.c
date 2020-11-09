#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "Coordinator.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
// This list should be filled with Application specific Cluster IDs.
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
  GENERICAPP_CLUSTERID
};
/*
 简单描述符的结构体  用于描述一个zigbee的设备节点；
typedef struct
{
  byte          EndPoint;
  uint16        AppProfId;
  uint16        AppDeviceId;
  byte          AppDevVer:4;
  byte          Reserved:4;             // AF_V1_SUPPORT uses for AppFlags:4.
  byte          AppNumInClusters;
  cId_t         *pAppInClusterList;
  byte          AppNumOutClusters;
  cId_t         *pAppOutClusterList;
} SimpleDescriptionFormat_t;
*/
const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
  GENERICAPP_ENDPOINT,              //  int Endpoint;            10
  GENERICAPP_PROFID,                //  uint16 AppProfId[2];     0x0F04
  GENERICAPP_DEVICEID,              //  uint16 AppDeviceId[2];   0x0001
  GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;       0
  GENERICAPP_FLAGS,                 //  int   AppFlags:4;        0
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;  1
  (cId_t *)GenericApp_ClusterList,  //  byte *pAppInClusterList; GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS]
  GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;  1
  (cId_t *)GenericApp_ClusterList   //  byte *pAppInClusterList; GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS]
};
/*
typedef struct
{
  byte endPoint;
  byte *task_id;  // Pointer to location of the Application task ID.
  SimpleDescriptionFormat_t *simpleDesc;
  afNetworkLatencyReq_t latencyReq;
} endPointDesc_t;
*/
endPointDesc_t GenericApp_epDesc;   //节点描述符
byte GenericApp_TaskID;             //任务优先级
byte GenericApp_TransID;            //数据发送序列号
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt );



void GenericApp_Init(byte task_id)
{
    
      GenericApp_TaskID  = task_id;
      GenericApp_TransID = 0;
      GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;  //10
      GenericApp_epDesc.task_id = &GenericApp_TaskID;    
      GenericApp_epDesc.simpleDesc =(SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
      GenericApp_epDesc.latencyReq = noLatencyReqs;
      afRegister(&GenericApp_epDesc); 
      osal_start_timerEx(GenericApp_TaskID, UART_SEND_EVENT, 5000);
}

UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
     afIncomingMSGPacket_t *MSGpkt;
     if ( events & SYS_EVENT_MSG )
     {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
        while ( MSGpkt )
        {
            switch ( MSGpkt->hdr.event )
            { 
              case AF_INCOMING_MSG_CMD:
                GenericApp_MessageMSGCB( MSGpkt );
                break;
              default:
                break;  
            }   
         osal_msg_deallocate( (uint8 *)MSGpkt );    
         MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID ); 
        }
         return (events ^ SYS_EVENT_MSG);
     }
       return 0;
}

void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
 // static char flag = 0;
  unsigned char buf[24];
     switch ( pkt->clusterId )
      {
        case GENERICAPP_CLUSTERID:
          osal_memcpy(buf, pkt->cmd.Data, 24);
          HalUARTWrite ( HAL_UART_PORT_0, buf, 24);
          break;
        default:
          break;
       
      }
}




















