// -*- coding: gb2312-dos -*-
/* @file           DeviceGVSP.h
 * @copyright      HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
 * @brief          DeviceGVSP 设备端 GVSP 协议实现
 *
 * @author         zhenglinjun
 * @date           2015-10-10
 *
 * @note
 *
 * @warning
 */

#ifndef _DEVICEGVSP_H
#define _DEVICEGVSP_H

#if defined __WIN32 || defined __WIN64 || defined WIN32 || defined WIN64
#define WINDOWS
#endif

#define MV_GIGEVISION_MAJOR_VERSION 1
#define GVSP_PACKET_MAX_SIZE        9000

#include "stdint.h"
#include <string>
#include <iostream>

#include "Inc/Socket/Socket.h"
#include "Inc/color_print/color_print.h"

#include "Inc/MVErrorDefine.h"
#include "Inc/MVGiGEVisionGVCPDefine.h"
#include "Inc/MVGiGEVisionGVSPDefine.h"
#include "Inc/MVGiGEVisionGVCPRegisterDefine.h"
#include "Inc/MVGiGEVisionPixelFormatDefine.h"
#include "Inc/MV_CrossPlatform/MV_CrossPlatform.h"

#include "Device.h"
#include "StreamConverter.h"

class DeviceGVSP
{
  public:
    DeviceGVSP(Device* pDev, StreamConverter* pStrm);
    virtual ~DeviceGVSP();

    // No copying allowed
    DeviceGVSP(const DeviceGVSP&);
    void operator=(const DeviceGVSP&);

    int Init();
    int DeInit();

    static ThreadReturnType MV_STDCALL HandlingStreamPacket(void* Arg);

  private:
    int Start();
    uint32_t PacketLeader(uint32_t nSizeX, uint32_t nSizeY,
                          GVSP_PACKET_PAYLOAD_TYPE PayloadType = GVSP_PT_UNCOMPRESSED_IMAGE,
                          int nPixelFmt = MV_GVSP_PIX_MONO8);
    uint32_t PacketPayload(Device::virtual_addr_t pCurData, size_t nDataSize);
    uint32_t PacketTrailer(uint32_t nSizeY, GVSP_PACKET_PAYLOAD_TYPE PayloadType = GVSP_PT_UNCOMPRESSED_IMAGE);

  private:
    MVComponent::UDP     _UdpSkt;
    MVComponent::Address _Host;

    char                 _cGvspPacket[GVSP_PACKET_MAX_SIZE];
    uint16_t             _nBlockId;
    uint32_t             _nPacketId;
    uint32_t             _nPacketSize;
    uint32_t             _nPacketDelay;

    Device*              _pDevice;
    StreamConverter*     _pStrm;
    cp_state_ptr         _cp;
};


#endif /* _DEVICEGVSP_H */
