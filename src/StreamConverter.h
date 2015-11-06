// -*- coding: gb2312-dos -*-
/* @file           StreamConverter.h
 * @copyright      HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
 * @brief          StreamConverter 图片文件转化为所要求数据流
 *
 * @author         zhenglinjun
 * @date           2015-10-10
 *
 * @note
 *
 * @warning
 */

#ifndef _STREAMCONVERTER_H
#define _STREAMCONVERTER_H

#if defined __WIN32 || defined __WIN64 || defined WIN32 || defined WIN64
#define WINDOWS
#endif

#include "stdint.h"
#include <string>
#define WIN32_LEAN_AND_MEAN   // 包含windows.h前需要定义一个宏，去除winsock.h
#include <windows.h>

#define IMAGE_FILE_MAX_SIZE 4 << 20  // 4MB

#include "Inc/MVErrorDefine.h"
#include "Inc/MVGiGEVisionPixelFormatDefine.h"
#include "Inc/MV_CrossPlatform/MV_CrossPlatform.h"

#include "Device.h"

class StreamConverter
{
  public:
    typedef unsigned int PixelFormats;

  public:
    StreamConverter(const std::string strDirName);
    virtual ~StreamConverter();

    int Init();
    int DeInit();

    int GetNextFrame();
    void GetImageData(Device::virtual_addr_t& pData, size_t& nLen, uint32_t& nSizeX, uint32_t& nSizeY,
                      PixelFormats OutFmt = MV_GVSP_PIX_MONO8);
    std::string GetCurrentFileName();

    void Lock();
    void Unlock();

  private:
    int FindDirectory();
    int FindArchive();
    int UpdateImageData();

  private:
    HANDLE          _hFindDir;
    HANDLE          _hFindArch;
    WIN32_FIND_DATA _FindFileDataDir;
    WIN32_FIND_DATA _FindFileDataArch;
    std::string     _strRootDir;
    std::string     _strCurDirName;
    std::string     _strCurArchName;

    PixelFormats    _OutFmt;
    float           _yuvMatrix[9];

    uint32_t    _nFrameId;
    double      _fTriggerFrequency;  // TODO

    CMVMutex       _BufMutex;
    unsigned char* _pStreamBuffer;
    unsigned char* _pImageData;
    size_t         _nImageLen;
    uint32_t       _nSizeX;
    uint32_t       _nSizeY;
};


#endif /* _STREAMCONVERTER_H */
