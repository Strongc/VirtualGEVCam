#include "VIrtualGEVCam.h"
#include <iostream>

using namespace std;
using namespace MVComponent;

VirtualGEVCam::VirtualGEVCam(const char* szInDataDir, const char* szXmlFileName, const char* szDeviceIni)
        : _VtDev(szXmlFileName, szDeviceIni), _Strm(szInDataDir),
          _Gvcp((Device*)&_VtDev), _Gvsp((Device*)&_VtDev, (StreamConverter*)&_Strm),
          _pThreadGvcp(MV_NULL), _pThreadGvsp(MV_NULL)
{
    _cp = cp_init();
}

VirtualGEVCam::~VirtualGEVCam()
{
    cp_close(_cp);
}

int VirtualGEVCam::UpdateStreamNextFrame(StreamConverter::PixelFormats OutFmt)
{
    int nRet = MV_OK;
    string strFileName;

    if ((nRet = _Strm.GetNextFrame()) != MV_OK)
    {
        return nRet;
    }

    Device::virtual_addr_t pData;
    size_t                 nLen;
    uint32_t               nSizeX, nSizeY;
    _Strm.GetImageData(pData, nLen, nSizeX, nSizeY, OutFmt);
    _VtDev.SetReg((Device::virtual_addr_t)REG_XML_Width_RegAddr, nSizeX);
    _VtDev.SetReg((Device::virtual_addr_t)REG_XML_Height_RegAddr, nSizeY);

    return nRet;
}

int VirtualGEVCam::Init()
{
    int nRet = MV_OK;

    // Init virtual device
    if ((nRet = _VtDev.Init()) != MV_OK)
    {
        cp_print(_cp, CP_FG_RED, "[WARN]");
        cout << "[VirtualDevice::Init] Init virtual device fail!!" << endl;
        return nRet;
    }


    // Init stream converter
    if ((nRet = _Strm.Init()) != MV_OK)
    {
        cp_print(_cp, CP_FG_RED, "[WARN]");
        cout << "[StreamConverter::Init] Init stream converter fail!!" << endl;
        return nRet;
    }


    // Init GVCP socket
    if ((nRet = _Gvcp.Init()) != MV_OK)
    {
        cp_print(_cp, CP_FG_RED, "[WARN]");
        cout << "[DeviceGvcp::Init] Init GVCP socket fail!!" << endl;
        return nRet;
    }


    // Init GVSP socket
    if ((nRet = _Gvsp.Init()) != MV_OK)
    {
        cp_print(_cp, CP_FG_RED, "[WARN]");
        cout << "[DeviceGvsp::Init] Init GVSP socket fail!!" << endl;
        return nRet;
    }


    return nRet;
}

int VirtualGEVCam::Starting()
{
    _pThreadGvcp = MV_CreateThread(MV_NULL, DeviceGVCP::HandlingControlPacket, (&(this->_Gvcp)));
    if (_pThreadGvcp == MV_NULL)
    {
        return -1;
    }

    _pThreadGvsp = MV_CreateThread(MV_NULL, DeviceGVSP::HandlingStreamPacket, (&(this->_Gvsp)));
    if (_pThreadGvsp == MV_NULL)
    {
        return -1;
    }

    // Wait for cancel
    // _bCancel
    while (!this->_VtDev.IsCancel())
    {
        // UpdateStreamNextFrame();
        Sleep(100);
    }

    MV_WaitForThreadEnd(_pThreadGvcp);
    MV_WaitForThreadEnd(_pThreadGvsp);

    return MV_OK;
}

int VirtualGEVCam::DeInit()
{
    int nRet = MV_OK;

    _Strm.DeInit();
    _Gvcp.DeInit();
    _Gvsp.DeInit();

    return nRet;
}
