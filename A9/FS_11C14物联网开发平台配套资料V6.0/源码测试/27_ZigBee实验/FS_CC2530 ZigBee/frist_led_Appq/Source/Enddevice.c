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
endPointDesc_t GenericApp_epDesc;   //节点描述符
byte GenericApp_TaskID;             //任务优先级
byte GenericApp_TransID;            //数据发送序列号
devStates_t GenericApp_NwkState;
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt );
void GenericApp_SendTheMessage(void);


void GenericApp_Init(byte task_id)
{
    
      GenericApp_TaskID  = task_id;
      GenericApp_NwkState = DEV_INIT;
      GenericApp_TransID = 0;
      GenericApp_epDesc.endPoint = GENERICAPP_ENDPOINT;  //10
      GenericApp_epDesc.task_id = &GenericApp_TaskID;    
      GenericApp_epDesc.simpleDesc =(SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
      GenericApp_epDesc.latencyReq = noLatencyReqs;
      afRegister(&GenericApp_epDesc);      
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
              case ZDO_STATE_CHANGE:
                GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
                if((GenericApp_NwkState == DEV_END_DEVICE))
                {
                osal_start_timerEx( GenericApp_TaskID,GENERICAPP_SEND_MSG_EVT,
                              GENERICAPP_SEND_MSG_TIMEOUT );
                }
               break;
              default:
              break;
            }
          osal_msg_deallocate( (uint8 *)MSGpkt );
          // Next
          MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( GenericApp_TaskID );
          }
      }
    if ( events & GENERICAPP_SEND_MSG_EVT )
    {
      // Send "the" message
      GenericApp_SendTheMessage();
      // Setup to send message again
      osal_start_timerEx( GenericApp_TaskID,
                        GENERICAPP_SEND_MSG_EVT,
                      GENERICAPP_SEND_MSG_TIMEOUT );
      // return unprocessed events
      return (events ^ GENERICAPP_SEND_MSG_EVT);
    } 
      return 0;
}
void GenericApp_SendTheMessage( void )
{
  char theMessageData[] = "123";
   afAddrType_t GenericApp_DstAddr;
  GenericApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  GenericApp_DstAddr.endPoint = GENERICAPP_ENDPOINT;
  GenericApp_DstAddr.addr.shortAddr = 0x0000;  
  
  if ( AF_DataRequest( &GenericApp_DstAddr, &GenericApp_epDesc,
                       GENERICAPP_CLUSTERID,
                       (byte)osal_strlen( theMessageData ) + 1,
                       (byte *)&theMessageData,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
  }
  else
  {
    // Error occurred in request to send.
  }
}










