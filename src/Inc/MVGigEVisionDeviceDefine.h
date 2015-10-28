// -*- coding: gb2312-dos -*-
#ifndef _MVGIGEVISIONDEFINE_H
#define _MVGIGEVISIONDEFINE_H

#include "MVPixelType.h"

namespace MvCamCtrl
{
// 设备类型定义
#define MV_UNKNOW_DEVICE        0x00000000          // 未知设备类型，保留意义
#define MV_GIGE_DEVICE          0x00000001          // GigE设备
#define MV_1394_DEVICE          0x00000002          // 1394-a/b 设备
#define MV_USB_DEVICE           0x00000004          // USB3.0 设备
#define MV_CAMERALINK_DEVICE    0x00000008          // CameraLink设备

typedef struct _MV_GIGE_DEVICE_INFO_
{
    unsigned int        nIpCfgOption;
    unsigned int        nIpCfgCurrent;                   // IP configuration:bit31-static bit30-dhcp bit29-lla
    unsigned int        nCurrentIp;                      // curtent ip
    unsigned int        nCurrentSubNetMask;              // curtent subnet mask
    unsigned int        nDefultGateWay;                  // current gateway
    unsigned char       chManufacturerName[32];
    unsigned char       chModelName[32];
    unsigned char       chDeviceVersion[32];
    unsigned char       chManufacturerSpecificInfo[48];
    unsigned char       chSerialNumber[16];
    unsigned char       chUserDefinedName[16];

    unsigned int        nNetExport;                      // 网口IP地址
} MV_GIGE_DEVICE_INFO;

// 设备信息
typedef struct _MV_CC_DEVICE_INFO_
{
    // common info
    unsigned short      nMajorVer;
    unsigned short      nMinorVer;
    unsigned int        nDeviceMode;
    unsigned int        nMacAddrHigh;               // MAC 地址
    unsigned int        nMacAddrLow;

    unsigned int        nTLayerType;                // 设备传输层协议类型，e.g. MV_GIGE_DEVICE

    union
    {
        MV_GIGE_DEVICE_INFO stGigEInfo;
        // more ...
    };

} MV_CC_DEVICE_INFO;

// 网络传输的相关信息
typedef struct _MV_NETTRANS_INFO_
{
    long long       nReviceDataSize;    // 已接收数据大小  [统计StartGrabbing和StopGrabbing之间的数据量]
    int             nThrowFrameCount;   // 丢帧数量
    int             nReserved;

}MV_NETTRANS_INFO;


// 最多支持的传输层实例个数
#define MV_MAX_TLS_NUM          8
// 最大支持的设备个数
#define MV_MAX_DEVICE_NUM      256

// 设备信息列表
typedef struct _MV_CC_DEVICE_INFO_LIST_
{
    unsigned int            nDeviceNum;                         // 在线设备数量
    MV_CC_DEVICE_INFO*      pDeviceInfo[MV_MAX_DEVICE_NUM];     // 支持最多256个设备

}MV_CC_DEVICE_INFO_LIST;


// 输出帧的信息
typedef struct _MV_FRAME_OUT_INFO_
{
    unsigned short      nWidth;             // 图像宽
    unsigned short      nHeight;            // 图像高
    MvGvspPixelType     enPixelType;        // 像素格式

    /*以下字段暂不支持*/
    unsigned int        nFrameNum;          // 帧号
    unsigned int        nDevTimeStampHigh;  // 时间戳高32位
    unsigned int        nDevTimeStampLow;   // 时间戳低32位
    long long           nHostTimeStamp;     // 主机生成的时间戳

    unsigned int        nReserved[4];       // 保留
}MV_FRAME_OUT_INFO;


// 采集模式
typedef enum _MV_CAM_ACQUISITION_MODE_
{
    MV_ACQ_MODE_SINGLE      = 0,            // 单帧模式
    MV_ACQ_MODE_MUTLI       = 1,            // 多帧模式
    MV_ACQ_MODE_CONTINUOUS  = 2,            // 持续采集模式

}MV_CAM_ACQUISITION_MODE;

// GigEVision IP Configuration
#define MV_IP_CFG_STATIC        0x01000000
#define MV_IP_CFG_DHCP          0x02000000
#define MV_IP_CFG_LLA           0x04000000


// 信息类型
#define MV_MATCH_TYPE_NET_DETECT        0x00000001      // 网络流量和丢包信息

// 全匹配的一种信息结构体
typedef struct _MV_ALL_MATCH_INFO_
{
    unsigned int    nType;          // 需要输出的信息类型，e.g. MV_MATCH_TYPE_NET_DETECT
    void*           pInfo;          // 输出的信息缓存，由调用者分配
    unsigned int    nInfoSize;      // 信息缓存的大小

}MV_ALL_MATCH_INFO;

//  网络流量和丢包信息反馈结构体，对应类型为 MV_MATCH_TYPE_NET_DETECT
typedef struct _MV_MATCH_INFO_NET_DETECT_
{
    long long       nReviceDataSize;    // 已接收数据大小  [统计StartGrabbing和StopGrabbing之间的数据量]
    long long       nLostPacketCount;   // 丢失的包数量
    unsigned int    nLostFrameCount;    // 丢帧数量
    unsigned int    nReserved;          // 保留
}MV_MATCH_INFO_NET_DETECT;

//  异常消息类型
#define MV_EXCEPTION_DEV_DISCONNECT     0x00008001      // 设备断开连接


// 打开设备的权限模式
#define MV_ExclusivePrivilege                                      1
#define MV_ExclusivePrivilegeWithSwitchoverKey                     2
#define MV_ControlPrivilege                                        3
#define MV_ControlPrivilegeWithSwitchoverKey                       4
#define MV_ControlSwitchoverEnablePrivilege                        5
#define MV_ControlSwitchoverEnablePrivilegeWithSwitchoverKey       6
#define MV_MonitorPrivilege                                        7
} // MvCamCtrl

#endif /* _MVGIGEVISIONDEFINE_H */
