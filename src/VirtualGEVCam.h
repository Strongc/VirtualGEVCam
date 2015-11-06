// -*- coding: gb2312-dos -*-
#ifndef _VIRTUALGEVCAM_H
#define _VIRTUALGEVCAM_H

#include "Inc/color_print/color_print.h"

#include "VirtualDevice.h"
#include "StreamConverter.h"
#include "DeviceGVCP.h"
#include "DeviceGVSP.h"

class VirtualGEVCam
{
  public:
    VirtualGEVCam(const char* szInDataDir, const char* szXmlFileName, const char* szDeviceIni);
    virtual ~VirtualGEVCam();

    // No copying allowed
    VirtualGEVCam(const VirtualGEVCam&);
    void operator=(const VirtualGEVCam&);

    int Init();
    int Starting();
    int DeInit();

  private:
    int UpdateStreamNextFrame(StreamConverter::PixelFormats OutFmt = MV_GVSP_PIX_MONO8);

  private:
    VirtualDevice                _VtDev;
    StreamConverter              _Strm;
    DeviceGVCP                   _Gvcp;
    DeviceGVSP                   _Gvsp;
    void*                        _pThreadGvcp;
    void*                        _pThreadGvsp;

    // Color print
    cp_state_ptr   _cp;
};


#endif /* _VIRTUALGEVCAM_H */
