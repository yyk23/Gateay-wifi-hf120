/*
 * userSmart.h
 *
 *  Created on: 2015-09-16
 *      Author: Tim Li
 */

#ifndef USERSMAT_H_
#define USERSMAT_H_

#include "ssv_types.h"
#include "smartComm.h"
#include "process.h"

#if defined(SMART_USER)

//callback define
typedef void (*callback)(U8*, U32);

#define TOTAL_FILTER_NUM 4
#define CHECK_FILTER_TYPE(a, b) ((U16)((a >> b) & 0x0001))
#define SET_FILTER_TABLE(num) SET_FILTER_TABLE##num()

/* filter type */
#define RECE_ONLY_BEACON__MANAG	               0x0001
#define RECE_ONLY_BROADCAST_DATA               0x0002
#define RECE_ALL_MANAG                                         0x0004
#define RECE_ALL_DATA                                              0x0008

struct user_smart_hdr_3addr {
    le16_t frame_control;
    le16_t duration_id;
    U8 addr1[6];
    U8 addr2[6];
    U8 addr3[6];
    le16_t seq_ctrl;
    //U8 data[0];		/**< Beginning of frame data */ remove this data from ieee80211_hrd_3addr
} __attribute__ ((packed));

typedef struct _user_smart_beacon_1st {     //split due to variable ssid string and length
    struct user_smart_hdr_3addr header;
    /* SSID, supported rates */
    U64     time_stamp;
    U16     beacon_interval;
    U16     capability;
    U8      eid_ssid;              //0x00
    U8      length_ssid;        //0x20
    //S8      ssid[CFG_SOFTAP_SSID_LENGTH];
    //S8      ssid[SSID_MAC_LENGTH];  //  1:seperater chractere '-' ,6:MAC address
} __attribute__ ((packed)) user_smart_beacon_1st;

typedef struct _user_smart_beacon_2nd {
    U8      eid_rates;          //0x01
    U8      length_rates;       //0x8
    U8      rates[8];           //0x82-0x84-8B-0x96-0x12-0x24-0x48-0x6c
    U8      eid_dsps;         //0x3
    U8      length_dsps;  //0x1
    U8      channel;        //ex:0x1 
    U8      eid_tim;        //0x5        
    U8      length_tim;   //4 //0x4 
    U8      dtim_count;  //0x0
    U8      dtim_period;    //0x1
    U8      bitmap_control; //0x0
    U8      part_virt_bmap; //0x0
} __attribute__ ((packed)) user_smart_beacon_2nd;

typedef struct _userSmart {
    U8 userSmartCh;
    U8 userSmartEnable;
    U8 callBackFrameEnable;
    U8 filterTable[TOTAL_FILTER_NUM];
    void (* callBackFrame)(U8 *data, U32 num);
} userSmart;

void AutoScanAP(struct process *p);
void userSmartSetChannel(U8 ch);
void SetUserReceFilter(U16 type);
U8 EnableUserReceFilter(void);
void DisableUserReceFilter(void);
void SetCallBackFunction(callback function);
U8 EnableCallBackFrame(void);
void DisableCallBackFrame(void);
void EnableUserSmartMode(void);
U8 DisableUserSmartMode(void);
void rx_process_userSmart(U8 *data, U32 len) ATTRIBUTE_SECTION_SRAM;
U8 enableTxBeacon();
U8 disableTxBeacon();

#endif

#endif /* AIRKISS_H_ */
