// -*- coding: gb2312-dos -*-
/* @file           Device.h
 * @copyright      HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
 * @brief          Device 定义设备接口的纯虚类
 *
 * @author         zhenglinjun
 * @date           2015-10-10
 *
 * @note
 *
 * @warning
 */

#ifndef _DEVICE_H
#define _DEVICE_H

#if defined __WIN32 || defined __WIN64 || defined WIN32 || defined WIN64
#define WINDOWS
#endif

#include "stdint.h"
#include <stdlib.h>
#include "Inc/MVErrorDefine.h"
#include "Inc/MVGigEVisionDeviceDefine.h"

class Device
{
  public:
    typedef unsigned char* virtual_addr_t;

  public:
    Device() {};
    virtual ~Device() {};

    virtual const MvCamCtrl::MV_CC_DEVICE_INFO* GetDeviceInfo() = 0;

    virtual int GetReg(virtual_addr_t RegAddr, uint32_t& Data) = 0;
    virtual int SetReg(virtual_addr_t RegAddr, const uint32_t Data) = 0;
    virtual int GetMem(virtual_addr_t MemAddr, void* Data, const size_t Count) = 0;
    virtual int SetMem(virtual_addr_t MemAddr, const void* Data, const size_t Count) = 0;

    virtual uint32_t GetAcquisitionState() = 0;
    virtual void SetTriggerFrequency(double frequency) = 0;
    virtual uint32_t GetControlChannelPrivilege() = 0;
    virtual void SetControlChannelPrivilege(uint32_t privilege) = 0;
    virtual uint32_t GetPayload() = 0;
    virtual uint32_t GetHeartbeatTimeout() = 0;

    virtual bool IsCancel() = 0;
};


#endif /* _DEVICE_H */
