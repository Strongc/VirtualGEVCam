// -*- coding: gb2312-dos -*-
/***************************************************************************************************
 *
 * 版权信息：版权所有 (c) 2014, 杭州海康威视数字技术股份有限公司, 保留所有权利
 *
 * 文件名称：MVGiGEVisionGVCPDefine.h
 * 摘    要：机器视觉SDK库 GVCP定义头文件
 *
 * 当前版本：V1.0.0
 * 作    者：王川艳、陈祖文、卢易
 * 日    期：2014-12-18
 * 备    注：新建，基于2.0.03版本。
 ***************************************************************************************************/
#ifndef __MV_GVCP_DEFINE_H__
#define __MV_GVCP_DEFINE_H__

/*************************************************************
 * 消息和命令值宏定义。 [0, 0x7FFD] 为 GigE Vision.保留定义
 *************************************************************/

// Discovery Protocol Control
#define MV_GEV_DISCOVERY_CMD            0x0002
#define MV_GEV_DISCOVERY_ACK            0x0003
#define MV_GEV_FORCEIP_CMD              0x0004
#define MV_GEV_FORCEIP_ACK              0x0005
// Streaming Protocol Control
#define MV_GEV_PACKETRESEND_CMD         0x0040
#define MV_GEV_PACKETRESEND_INVALID     0x0041
// Device Memory Access
#define MV_GEV_READREG_CMD              0x0080
#define MV_GEV_READREG_ACK              0x0081
#define MV_GEV_WRITEREG_CMD             0x0082
#define MV_GEV_WRITEREG_ACK             0x0083
#define MV_GEV_READMEM_CMD              0x0084
#define MV_GEV_READMEM_ACK              0x0085
#define MV_GEV_WRITEMEM_CMD             0x0086
#define MV_GEV_WRITEMEM_ACK             0x0087
#define MV_GEV_PENDING_ACK              0x0089
// Asynchronous Events
#define MV_GEV_EVENT_CMD                0x00C0
#define MV_GEV_EVENT_ACK                0x00C1
#define MV_GEV_EVENTDATA_CMD            0x00C2
#define MV_GEV_EVENTDATA_ACK            0x00C3
// Miscellaneous
#define MV_GEV_ACTION_CMD               0x0100
#define MV_GEV_ACTION_ACK               0x0101


/********************************************************************
 *      Standard Status Codes
 ********************************************************************/
#define MV_GEV_STATUS_SUCCESS                               0x0000  // 命令执行成功
#define MV_GEV_STATUS_PACKET_RESEND                         0x0100  // 仅适用于数据包被重新发送
#define MV_GEV_STATUS_NOT_IMPLEMENTED                       0x8001  // 设备不支持的命令
#define MV_GEV_STATUS_INVALID_PARAMETER                     0x8002  // 命令参数有误
#define MV_GEV_STATUS_INVALID_ADDRESS                       0x8003  // 访问了非法地址空间
#define MV_GEV_STATUS_WRITE_PROTECT                         0x8004  // 无法写入寻址寄存器
#define MV_GEV_STATUS_BAD_ALIGNMENT                         0x8005  // 对齐偏移地址损坏或指定的数据量有误
#define MV_GEV_STATUS_ACCESS_DENIED                         0x8006  // 指定的地址当前无法访问，可能被其他程序独占
#define MV_GEV_STATUS_BUSY                                  0x8007  // 请求的资源或服务无法及时响应，稍后会重试
#define MV_GEV_STATUS_DEPRECATED_0                          0x8008  // 从 GigE Vision 2.0 起弃用的状态码
#define MV_GEV_STATUS_DEPRECATED_1                          0x8009  // 从 GigE Vision 2.0 起弃用的状态码
#define MV_GEV_STATUS_DEPRECATED_2                          0x800A  // 从 GigE Vision 2.0 起弃用的状态码
#define MV_GEV_STATUS_DEPRECATED_3                          0x800B  // 从 GigE Vision 2.0 起弃用的状态码
#define MV_GEV_STATUS_PACKET_UNAVAILABLE                    0x800C  // 所请求的数据包不存在，其他错误码不合适时使用
#define MV_GEV_STATUS_DATA_OVERRUN                          0x800D  // GVSP发送端内部缓存溢出
#define MV_GEV_STATUS_INVALID_HEADER                        0x800E  // 消息头非法或内部字段不匹配
#define MV_GEV_STATUS_DEPRECATED_4                          0x800F  // 从 GigE Vision 2.0 起弃用的状态码
#define MV_GEV_STATUS_PACKET_NOT_YET_AVAILABLE              0x8010  // 请求的包还没有被确认
#define MV_GEV_STATUS_PACKET_AND_PREV_REMOVED_FROM_MEMORY   0x8011  // 请求的包和之前的包都无效或不存在，且都已被发送端丢弃了
#define MV_GEV_STATUS_PACKET_REMOVED_FROM_MEMORY            0x8012  // 请求的数据包不存在，且已经被发送端丢弃了
#define MV_GEV_STATUS_NO_REF_TIME                           0x8013  // 该设备没有同步主时钟，没有时间参考
#define MV_GEV_STATUS_PACKET_TEMPORARILY_UNAVAILABLE        0x8014  // 带宽问题，过段时间后重新发送
#define MV_GEV_STATUS_OVERFLOW                              0x8015  // 设备队列或分组数据溢出
#define MV_GEV_STATUS_ACTION_LATE                           0x8016  // 请求了一个在过去的时间的操作
#define MV_GEV_STATUS_ERROR                                 0x8FFF  // 其他错误

/*The IETF has attributed port number 3956 as the standard GVCP port*/
#define MV_GVCP_PUBLIC_PORT             3956
/*The GVCP header provides an 8-bit field containing the hexadecimal key value 0x42. This enables devices and applications to identify GVCP packets.*/
#define MV_GVCP_CMD_KEY_VALUE           0x42

#define MV_GVCP_CMD_FLAG_NEED_ACK       0x01   //see Figure 15-1: Command Message Header
#define MV_GVCP_CMD_FLAG_BROADCAST_ACK  0x10   //see Figure 16-3: FORCEIP_CMD Message
#define MV_GVCP_CMD_FLAG_EXTEND_ID      0x10   //see Figure 16-13: PACKETRESEND_CMD Message
#define MV_GVCP_CMD_FLAG_ACTION_TIME    0x80   //see Figure 16-17: ACTION_CMD Message

/************************************************************************
 *                 see Table 8-1: GVCP Packet Header Size
 * IP header (options not allowed)        20
 * UDP header                             8
 * GVCP header                            8
 * Max. GVCP payload                      540
 * Total                                  576
 ************************************************************************/
#define MV_IP_HEADER_LEN                20
#define MV_UDP_HEADER_LEN               8
/*576-20(IP HEADER)-8(UDP HEADER)-8(GVCP HEADER)*/
#define MV_GVCP_MAX_PAYLOAD_LEN         540
/*576-20(IP HEADER)-8(UDP HEADER)*/
#define MV_GVCP_MAX_MSG_LEN             548
/*[R-044c] Applications and devices MUST send GVCP packets containing at most 576 bytes*/
#define MV_GVCP_PACKET_MAX_LEN          576

#define MV_GVCP_HEARTBEAT_TIMEOUT       3000  //Factory Default
#define MV_GVCP_HEARTBEAT_INTERVAL      500   //ms

/*The initial value of req_id is not specified, but it cannot be 0.*/
#define MV_GVCP_REQ_ID_INIT             1

/*540 / 4 = 135*/
#define MV_GVCP_READ_REG_MAX_NUM        135
/*540 / 8 = 67.5 ~= 67*/
#define MV_GVCP_WRITE_REG_MAX_NUM       67

/*540 - 4 = 536*/
#define MV_GVCP_READ_MEM_MAX_LEN        536
#define MV_GVCP_WRITE_MEM_MAX_LEN       536

#define MV_GVCP_URL_MAX_LEN             512

#define MV_GVCP_SC_X_ADDRESS_UNIT       0x40

/*IP HEADER(20)+UDP HEADER(8)+GVSP HEADER(8)+PAYLOAD DATA LEN(1000)*/
#define MV_GVCP_GVSP_MAX_PACKET_SIZE    1036    //1280//1048

#define MV_GVCP_MAX_SCPSx_VALUE         (1024 * 10)
#define MV_GVCP_MIN_SCPSx_VALUE         (64)
#define MV_GVCP_DEFAULT_SCPSx_VALUE     (1500)



#define MV_GVSP_MAX_PAYLOAD_SIZE        (MV_GVCP_GVSP_MAX_PACKET_SIZE - 36)
#define MV_GVCP_GVSP_MAX_DELAY_SEC      3      // Gvsp获取过程中，如果超过3s没有收到数据，则认为接收超时

#define MV_GVCP_CCP_EXCLUSIVE_ACCESS    0x1
#define MV_GVCP_CCP_CONTROL_ACCESS      0x2
#define MV_GVCP_CCP_CONTROL_SWITCH      0x6

/************************************************************************
 * GVCP_HEADER 分两种，定义为 CMD_MSG_HEADER 和 ACK_MSG_HEADER
 ************************************************************************/
// 命令信息头定义
typedef struct _CMD_MSG_HEADER_
{
    unsigned char       cKeyValue;      // [8bits] 0x42, GVCP定义的关键码

    /*
     * 高4bits是每个命令消息的特殊字段，低4bits是GVCP头的通用字段；
     * 其中最低位表示是否需要ACK；
     * 如未使用，全部bits都必须置0；
     *    0   1   2   3   4   5   6   7
     * +---+---+---+---+---+---+---+---+
     * |    special    |    common     |
     * +---+---+---+---+---+---+---+---+
     */
    unsigned char       cFlg;           // [8bits]
    unsigned short      nCommand;       // [16bits] 参考消息值宏定义
    unsigned short      nLength;        // [16bits] 该结构体之后的负载数据长度,通常为8
    unsigned short      nReqId;         // [16bits] 请求ID，由应用程序生成。不能为0
} CMD_MSG_HEADER;

// 确认信息头定义
typedef struct _ACK_MSG_HEADER_
{
    unsigned short      nStatus;         // [16bits] 请求操作的状态。参考状态码宏定义
    unsigned short      nAckMsgValue;    // [16bits] 确认消息值 参考消息值宏定义
    unsigned short      nLength;         // [16bits] 该结构体之后的负载数据长度
    unsigned short      nAckId;          // [16bits] 响应ID，与接收的CMD信息中的req_id相同
} ACK_MSG_HEADER;

/************************************************************************
 *  GVCP_PAYLOAD  负载定义。根据命令和确认信息的不同，有各自的定义
 ************************************************************************/

/***************************************************
 *  1. [CMD] 设备发现，不需要负载 。
 *     flag字段说明：bit 0~2, 保留,置0；bit 3, 允许应答消息广播；bit 7 必须置1。
 *    【该命令设备必须支持】
 ***************************************************/

/***************************************************
 *  2. [ACK] 设备应答，负载定义为结构体 DISCOVERY_ACK_MSG
 *    【该命令设备必须支持】
 ***************************************************/
typedef struct _DISCOVERY_ACK_MSG_
{
    unsigned short nMajorVer;
    unsigned short nMinorVer;
    unsigned int   nDeviceMode;
    unsigned short nRes1;
    unsigned short nMacAddrHigh;
    unsigned int   nMacAddrLow;
    unsigned int   nIpCfgOption;
    unsigned int   nIpCfgCurrent;       //IP configuration:bit31-static bit30-dhcp bit29-lla
    unsigned int   nRes2[3];
    unsigned int   nCurrentIp;
    unsigned int   nRes3[3];
    unsigned int   nCurrentSubNetMask;
    unsigned int   nRes4[3];
    unsigned int   nDefultGateWay;
    unsigned char  chManufacturerName[32];
    unsigned char  chModelName[32];
    unsigned char  chDeviceVersion[32];
    unsigned char  chManufacturerSpecificInfo[48];
    unsigned char  chSerialNumber[16];
    unsigned char  chUserDefinedName[16];
} DISCOVERY_ACK_MSG;

/***************************************************
 *  3. [CMD] 强制IP，负载定义为结构体 FORCEIP_CMD_MSG
 *     flag字段说明：bit 0~2, 保留,置0；bit 3, 允许应答消息广播；bit 7 必须置1。
 *    【该命令设备必须支持】
 ***************************************************/
typedef struct _FORCEIP_CMD_MSG_
{
    unsigned char       nReserved0[2];
    unsigned short      nMacAddrHigh;
    unsigned int        nMacAddrLow;
    unsigned int        nReserved1[3];
    unsigned int        nStaticIp;      // 如果此字段为0，设备必须在全部的网络接口上重启IP配置，而不需要回复确认消息;
    unsigned int        nReserved2[3];
    unsigned int        nStaticSubNetMask;
    unsigned int        nReserved3[3];
    unsigned int        nStaticDefaultGateWay;

} FORCEIP_CMD_MSG;

/***************************************************
 *  4. [ACK] 强制IP，不需要负载。但IP包要使用新的IP发送
 *    【该命令设备必须支持】
 ***************************************************/

/***************************************************
 *  5. [CMD] 读寄存器，负载为一系列32bits的寄存器地址（至少一个）
 *     flag字段说明：bit 0~2, 保留,置0；bit 3, 允许应答消息广播；bit 7 必须置1。
 *    【该命令设备必须支持】
 ***************************************************/

/**************************************************
 *  6. [ACK] 读寄存器，负载为一系列32bits的值，这些值从CMD的寄存器地址中获取。
 *    【该命令设备必须支持】
 ***************************************************/

/***************************************************
 *  7. [CMD] 写寄存器，负载为 WRITEREG_CMD_MSG 数组（至少一个，最多67个）
 *     lag字段说明：bit 0~2, 保留,置0；bit 3, 允许应答消息广播。
 *    【该命令设备必须支持】
 ***************************************************/
typedef struct _WRITEREG_CMD_MSG_
{
    unsigned long       nRegAddress;   // 寄存器地址
    unsigned int        nRegData;      // 地址对应的数据
} WRITEREG_CMD_MSG;

/***************************************************
 *  8. [ACK] 写寄存器，负载为 WRITEREG_ACK_MSG。但第n个写入错误时，返回错误信息。之后的命令数据丢弃。
 *    【该命令设备必须支持】
 ***************************************************/
#define MV_MAX_WRITEREG_INDEX      67
typedef struct _WRITEREG_ACK_MSG_
{
    unsigned short      nReserved;   // 置0
    unsigned short      nIndex;      // 表示第n（0-66）个写入出错。如果都正确，填67
} WRITEREG_ACK_MSG;

/***************************************************
 *  9. [CMD] 读内存，负载为 READMEM_CMD_MSG
 *     flag字段说明：bit 0~2, 保留,置0；bit 3, 允许应答消息广播；bit 7 必须置1。
 *    【该命令设备必须支持】
 ***************************************************/
typedef struct _READMEM_CMD_MSG_
{
    unsigned long       nMemAddress;    // 4bytes 对齐的地址
    unsigned short      nReserved;      // 置0
    unsigned short      nMemLen;        // 读取的byte数量。（4的倍数）
} READMEM_CMD_MSG;

/***************************************************
 * 10. [ACK] 读内存，负载为 READMEM_ACK_MSG
 *    【该命令设备必须支持】
 ***************************************************/
typedef struct _READMEM_ACK_MSG_
{
    unsigned long       nMemAddress;    // 4bytes 对齐的地址
    unsigned char       chReadMemData[MV_GVCP_MAX_PAYLOAD_LEN]; // 读取的内存数据
} READMEM_ACK_MSG;

/***************************************************
 * 11. [CMD] 写内存，负载为 WRITEMEM_CMD_MSG
 *     flag字段说明：bit 0~2, 保留,置0；bit 3, 允许应答消息广播。
 *    【该命令设备必须支持】
 ***************************************************/
typedef struct _WRITEMEM_CMD_MSG_
{
    unsigned long       nMemAddress;    // 4bytes 对齐的地址
    unsigned char       chWriteMemData[MV_GVCP_MAX_PAYLOAD_LEN]; // 待写的内存数据
} WRITEMEM_CMD_MSG;

/***************************************************
 * 12. [ACK] 写内存，负载为 WRITEMEM_ACK_MSG
 *    【该命令设备必须支持】
 ***************************************************/
typedef struct _WRITEMEM_ACK_MSG_
{
    unsigned short      nReserved;   // 置0
    unsigned short      nIndex;      // 成功，填总写入数据量；失败，表示第n（0-535）个写入出错。
} WRITEMEM_ACK_MSG;


/***************************************************
 * 13. [CMD] 重发，负载为 PACKETRESEND_CMD_MSG
 *     flag字段说明：bit 0~2, 保留,置0；
 *                   bit 3, -- 置1，表示 block_id 使用64bits， packet_id 使用32bits
 *                          -- 置0，表示 block_id 使用16bits， packet_id 使用24bits
 *    【该命令设备选择性支持】MV_REG_GVCPCapability(0x0934) 的 value_bit29 置1时支持。
 ***************************************************/
typedef struct _PACKETRESEND_CMD_MSG_
{
    unsigned short      nStreamChannelIdx;   // 流通道的序号

    /*
     * 如果flag的bit3置0，该字段表示 block_id，此时该字段不允许置0；
     * 如果flag的bit3置1，该字段保留，置0
     */
    unsigned short      nBlockId;

    /*
     * 如果flag的bit3置0，仅使用低24bits，高8bits置0；
     * 如果flag的bit3置1，32bits 都使用
     */
    unsigned int        nFirstPacketId;

    /*
     * last >= first；
     * 如果flag的bit3置0，仅使用低24bits，高8bits置0，如果last为 0xffffff，发送所有包
     * 如果flag的bit3置1，32bits 都使用，如果last为 0xffffffff，发送所有包
     */
    unsigned int        nLastPacketId;

    // 仅当flag的bit3置1，才包含以下字段
    unsigned int        nBlockIdHigh;
    unsigned int        nBlockIdLow;
} PACKETRESEND_CMD_MSG;

/***************************************************
 * 14. [ACK] 重发，没有定义此消息类型
 ***************************************************/

/***************************************************
 * 15. [ACK] 等待，负载为 PENDING_ACK_MSG
 *    【该命令设备选择性支持】MV_REG_PendingTimeout(0x0958)
 ***************************************************/
typedef struct _PENDING_ACK_MSG_
{
    unsigned short      reserved;               // 设置为0
    unsigned short      time_to_completion;     // 单位为ms

} PENDING_ACK_MSG;

/***************************************************
 * 16. [CMD] 重发，负载为 ACTION_CMD_MSG
 *     flag字段说明：bit 1~3, 保留,置0；
 *                   bit 0, -- 置1，表示 包含 action_time 字段
 *                          -- 置0，表示 不包含 action_time 字段
 *    【该命令设备选择性支持】MV_REG_GVCPCapability(0x0934) 的 value_bit25 置1时支持。
 ***************************************************/
typedef struct _ACTION_CMD_MSG_
{
    unsigned int        nDeviceKey;               //
    unsigned int        nGroupKey;
    unsigned int        nGroupMask;

    // 当flag的bit0置1时，才包含以下字段
    unsigned int        nActionTimeHigh;
    unsigned int        nActionTimeLow;
} ACTION_CMD_MSG;

/**************************************************
 * 17. [CMD] 重发，负载为 EVENT_CMD_MSG
 *      flag字段说明：bit 0~2, 保留,置0；
 *                    bit 3, -- 置1，表示 block_id 使用64bits， packet_id 使用32bits
 *                           -- 置0，表示 block_id 使用16bits， packet_id 使用24bits
 *    【该命令设备选择性支持】MV_REG_GVCPCapability(0x0934) 的 value_bit31 置1时支持。
 ***************************************************/
// note: 单个结构，使用时是个列表
typedef struct _EVENT_CMD_MSG_
{
    unsigned short      reserved;
    unsigned short      event_identifier;
    unsigned short      stream_channel_index;

    /*
     * 如果flag的bit3置0，该字段表示 block_id，此时该字段不允许置0；
     * 如果flag的bit3置1，该字段保留，置0
     */
    unsigned short      block_id;

    // 仅当flag的bit3置1，才包含以下2个字段
    unsigned int        block_id_high;
    unsigned int        block_id_low;

    unsigned int        timestamp_high;
    unsigned int        timestamp_low;
} EVENT_CMD_MSG;

/***************************************************
 * 18. [CMD] 重发，负载为 EVENTDATA_CMD_MSG
 *       flag字段说明：bit 0~2, 保留,置0；
 *                   bit 3, -- 置1，表示 block_id 使用64bits， packet_id 使用32bits
 *                          -- 置0，表示 block_id 使用16bits， packet_id 使用24bits
 *    【该命令设备选择性支持】
 ***************************************************/
typedef struct _EVENTDATA_CMD_MSG_
{
    unsigned short      nReserved;
    unsigned short      nEventId;
    unsigned short      nStreamChannelIdx;

    /*
     * 如果flag的bit3置0，该字段表示 block_id，此时该字段不允许置0；
     * 如果flag的bit3置1，该字段保留，置0
     */
    unsigned short      nBlockId;

    // 仅当flag的bit3置1，才包含以下2个字段
    unsigned int        nBlockIdHigh;
    unsigned int        nBlockIdLow;

    unsigned int        nTimeStampHigh;
    unsigned int        nTimeStampLow;
} EVENTDATA_CMD_MSG;

#define ExclusivePrivilege                                      1
#define ExclusivePrivilegeWithSwitchoverKey                     2
#define ControlPrivilege                                        3
#define ControlPrivilegeWithSwitchoverKey                       4
#define ControlSwitchoverEnablePrivilege                        5
#define ControlSwitchoverEnablePrivilegeWithSwitchoverKey       6
#define MonitorPrivilege                                        7

#endif  // __MV_GVCP_DEFINE_H__
