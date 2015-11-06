// -*- coding: gb2312-dos -*-
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // 包含windows.h前需要定义一个宏，去除winsock.h
#include "DeviceGVSP.h"

using namespace std;
using namespace MVComponent;

DeviceGVSP::DeviceGVSP(Device* pDev, StreamConverter* pStrm)
        : _nBlockId(1), _nPacketId(0), _nPacketSize(0), _nPacketDelay(0),
          _pDevice(pDev), _pStrm(pStrm)
{
    _cp = cp_init();
}

DeviceGVSP::~DeviceGVSP()
{
    cp_close(_cp);
}

int DeviceGVSP::Init()
{
    int nRet = MV_OK;
    uint32_t nSrcPort = 0;

    try
    {
        _UdpSkt.Open();
        _UdpSkt.SetDontfragment(true);
        _UdpSkt.SetBuffsize(64 << 10, 0);  // set send buffer 64K
        _UdpSkt.BindOnPort(0);

        nSrcPort = _UdpSkt.GetAddressPort();
        _pDevice->SetReg((unsigned char*)MV_REG_StreamChannelSourcePort0, nSrcPort);
    }
    catch (SocketException& SktEx)
    {
        cp_print(_cp, CP_FG_RED, "[WARN]");
        cout << SktEx << endl;
        nRet = MV_E_NETER;
        return nRet;
    }

    return nRet;
}

int DeviceGVSP::DeInit()
{
    int nRet = MV_OK;

    try
    {
        _UdpSkt.Close();
    }
    catch (SocketException& SktEx)
    {
        cp_print(_cp, CP_FG_RED, "[WARN]");
        cout << SktEx << endl;
        nRet = MV_E_NETER;
        return nRet;
    }

    return nRet;
}

int DeviceGVSP::Start()
{
    int nRet = MV_OK;

    uint32_t nPort = 0, nPacketSize = 0, nPacketDelay = 0, nDstAddress = 0, nCapability = 0,
           nConfig = 0, nZone = 0, nZoneDirect;
    _pDevice->GetReg((unsigned char*)MV_REG_StreamChannelPort0, nPort);
    _pDevice->GetReg((unsigned char*)MV_REG_StreamChannelDestinationAddress0, nDstAddress);
    _pDevice->GetReg((unsigned char*)MV_REG_StreamChannelPacketSize0, nPacketSize);
    _pDevice->GetReg((unsigned char*)MV_REG_StreamChannelPacketDelay0, nPacketDelay);
    _pDevice->GetReg((unsigned char*)MV_REG_StreamChannelCapability0, nCapability);
    _pDevice->GetReg((unsigned char*)MV_REG_StreamChannelConfiguration0, nConfig);
    _pDevice->GetReg((unsigned char*)MV_REG_StreamChannelZoneDirection0, nZoneDirect);
    _pDevice->GetReg((unsigned char*)MV_REG_StreamChannelZone0, nZone);

    try
    {
        nDstAddress = htonl(nDstAddress);
        _Host.SetAddressIp(inet_ntoa(*((struct in_addr*)&nDstAddress)));
        _Host.SetAddressPort(nPort & 0xffff);

        this->_nPacketSize = (nPacketSize & 0xffff);
        this->_nPacketDelay = nPacketDelay;

        // TODO: deal with the other parameters
    }
    catch (SocketException& SktEx)
    {
        cp_print(_cp, CP_FG_RED, "[WARN]");
        cout << SktEx << endl;
        nRet = MV_E_NETER;
        return nRet;
    }

    return nRet;
}

ThreadReturnType MV_STDCALL DeviceGVSP::HandlingStreamPacket(void* Arg)
{
    int nRet = MV_OK;

    DeviceGVSP* pDeviceGvsp = (DeviceGVSP*)Arg;
    Device::virtual_addr_t pStreamBuffer = NULL;
    size_t nStreamSize;
    size_t nPerDataSize;
    uint32_t nSizeX, nSizeY;
    uint32_t nLen;
    stringstream ss;

    pDeviceGvsp->_nBlockId = 1;
    do
    {
        if (pDeviceGvsp->_pDevice->GetAcquisitionState() == 0
            || pDeviceGvsp->_pDevice->GetControlChannelPrivilege() == 0)
        {
            if (pStreamBuffer != NULL)
            {
                pStreamBuffer = NULL;
                ss << "[HandlingStreamPacket] Stop stream" << endl;
                cout << ss.rdbuf();
            }
            Sleep(100);
        }
        else
        {
            // Start trainsmitting data ......
            if ((nRet = pDeviceGvsp->Start()) != MV_OK || (pDeviceGvsp->_nPacketSize <= 0))
            {
                cp_print(pDeviceGvsp->_cp, CP_FG_RED, "[WARN]");
                ss << "[HandlingStreamPacket] DeviceGvcp::Start fail!!!" << endl;
                cout << ss.rdbuf();
                return nRet;
            }
            pDeviceGvsp->_pStrm->GetImageData(pStreamBuffer, nStreamSize, nSizeX, nSizeY);
            if (pStreamBuffer == NULL)
            {
                cp_print(pDeviceGvsp->_cp, CP_FG_RED, "[WARN]");
                ss << "[HandlingStreamPacket] StreamConverter::GetImageData fail!!!" << endl;
                cout << ss.rdbuf();
                return nRet;
            }
            ss << "[HandlingStreamPacket] Get frame image \"" << pDeviceGvsp->_pStrm->GetCurrentFileName() << "\"\n"
               << "[HandlingStreamPacket]  --> " << pDeviceGvsp->_Host << "  Trainsmitting data (blockid = "
               << pDeviceGvsp->_nBlockId << ") ......\n";
            cout << ss.rdbuf();
            if ((nRet = pDeviceGvsp->_pStrm->GetNextFrame()) != MV_OK)
            {
                cp_print(pDeviceGvsp->_cp, CP_FG_RED, "[WARN]");
                ss << "[HandlingStreamPacket] StreamConverter::GetNextFrame fail!!!" << endl;
                cout << ss.rdbuf();
                return nRet;
            }

            // Data Leader
            pDeviceGvsp->_nPacketId = 0;
            nLen = pDeviceGvsp->PacketLeader(nSizeX, nSizeY);

            try
            {
                pDeviceGvsp->_UdpSkt.Send(pDeviceGvsp->_Host, pDeviceGvsp->_cGvspPacket, nLen);
            }
            catch (SocketException& SktEx)
            {
                cp_print(pDeviceGvsp->_cp, CP_FG_RED, "[WARN]");
                cout << SktEx << endl;
                nRet = MV_E_NETER;
                return nRet;
            }

            // Data Payload
            // IP Header + UDP Header + GVSP Header + data
            nPerDataSize = pDeviceGvsp->_nPacketSize - 28 - sizeof(GVSP_PACKET_HEADER);
            // nPerDataSize = 5000;

            // Enter StreamConverter buffer
            pDeviceGvsp->_pStrm->Lock();
            for (Device::virtual_addr_t pCurData = pStreamBuffer; pCurData <= pStreamBuffer + nStreamSize; )
            {
                // Every data Payload
                ++(pDeviceGvsp->_nPacketId);
                if ((pDeviceGvsp->_nPacketId) == GVSP_PACKET_ID_MAX)
                {
                    return -1;
                }
                nLen = pDeviceGvsp->PacketPayload(pCurData, ((pCurData+nPerDataSize <= pStreamBuffer+nStreamSize) ?
                                                             (nPerDataSize) : (nStreamSize % nPerDataSize)));
                pCurData += nPerDataSize;

                try
                {
                    pDeviceGvsp->_UdpSkt.Send(pDeviceGvsp->_Host, pDeviceGvsp->_cGvspPacket, nLen);
                }
                catch (SocketException& SktEx)
                {
                    cp_print(pDeviceGvsp->_cp, CP_FG_RED, "[WARN]");
                    cout << SktEx << endl;
                    nRet = MV_E_NETER;
                    return nRet;
                }

                // Waiting
                // Sleep(pDeviceGvsp->_nPacketDelay);
                // Sleep(2);
            }
            pDeviceGvsp->_pStrm->Unlock();
            // Exit StreamConverter buffer

            // Data Trailer
            ++(pDeviceGvsp->_nPacketId);
            if ((pDeviceGvsp->_nPacketId) == GVSP_PACKET_ID_MAX)
            {
                return -1;
            }
            nLen = pDeviceGvsp->PacketTrailer(nSizeY);

            try
            {
                pDeviceGvsp->_UdpSkt.Send(pDeviceGvsp->_Host, pDeviceGvsp->_cGvspPacket, nLen);
            }
            catch (SocketException& SktEx)
            {
                cp_print(pDeviceGvsp->_cp, CP_FG_RED, "[WARN]");
                cout << SktEx << endl;
                nRet = MV_E_NETER;
                return nRet;
            }

            // Block ID ++
            if ((++(pDeviceGvsp->_nBlockId)) == 0)
            {
                pDeviceGvsp->_nBlockId = 1;
            }
        }
    }
    while (!pDeviceGvsp->_pDevice->IsCancel());

    return nRet;
}

uint32_t DeviceGVSP::PacketLeader(uint32_t nSizeX, uint32_t nSizeY, GVSP_PACKET_PAYLOAD_TYPE PayloadType, int nPixelFmt)
{
    GVSP_PACKET_HEADER* pHeader = (GVSP_PACKET_HEADER*) (_cGvspPacket);
    pHeader->status        = htons(MV_GEV_STATUS_SUCCESS);
    pHeader->block_id      = htons(_nBlockId);
    pHeader->packet_fmt_id = htonl((GVSP_PACKET_FMT_LEADER << 24) | (_nPacketId & 0xffffff));

    GVSP_IMAGE_DATA_LEADER* pDataLeader = (GVSP_IMAGE_DATA_LEADER*)(_cGvspPacket + sizeof(GVSP_PACKET_HEADER));
    pDataLeader->reserved       = 0;
    pDataLeader->payload_type   = htons(PayloadType);
    pDataLeader->timestamp_high = htonl(0);  // TODO
    pDataLeader->timestamp_low  = htonl(0);
    pDataLeader->pixel_format   = htonl(nPixelFmt); // TODO
    pDataLeader->size_x         = htonl(nSizeX);
    pDataLeader->size_y         = htonl(nSizeY);
    pDataLeader->offset_x       = htonl(0);  // TODO
    pDataLeader->offset_y       = htonl(0);
    pDataLeader->padding_x      = htons(0);
    pDataLeader->padding_y      = htons(0);

    return (sizeof(GVSP_PACKET_HEADER) + sizeof(GVSP_IMAGE_DATA_LEADER));
}

uint32_t DeviceGVSP::PacketPayload(Device::virtual_addr_t pCurData, size_t nDataSize)
{
    GVSP_PACKET_HEADER* pHeader = (GVSP_PACKET_HEADER*) (_cGvspPacket);
    pHeader = (GVSP_PACKET_HEADER*) (_cGvspPacket);
    pHeader->status        = htons(MV_GEV_STATUS_SUCCESS);
    pHeader->block_id      = htons(_nBlockId);
    pHeader->packet_fmt_id = htonl((GVSP_PACKET_FMT_PAYLOAD_GENERIC << 24) | (_nPacketId & 0xffffff));

    char* pDataPayload = _cGvspPacket + sizeof(GVSP_PACKET_HEADER);
    memcpy(pDataPayload, pCurData, nDataSize);

    return (sizeof(GVSP_PACKET_HEADER) + nDataSize);
}

uint32_t DeviceGVSP::PacketTrailer(uint32_t nSizeY, GVSP_PACKET_PAYLOAD_TYPE PayloadType)
{
    GVSP_PACKET_HEADER* pHeader = (GVSP_PACKET_HEADER*) (_cGvspPacket);
    pHeader = (GVSP_PACKET_HEADER*) (_cGvspPacket);
    pHeader->status        = htons(MV_GEV_STATUS_SUCCESS);
    pHeader->block_id      = htons(_nBlockId);
    pHeader->packet_fmt_id = htonl((GVSP_PACKET_FMT_TRAILER << 24) | (_nPacketId & 0xffffff));

    GVSP_IMAGE_DATA_TRAILER* pDataTrailer = (GVSP_IMAGE_DATA_TRAILER*)(_cGvspPacket + sizeof(GVSP_PACKET_HEADER));
    pDataTrailer->reserved     = 0;
    pDataTrailer->payload_type = htons(PayloadType);
    pDataTrailer->size_y       = htonl(nSizeY);

    return (sizeof(GVSP_PACKET_HEADER) + sizeof(GVSP_IMAGE_DATA_TRAILER));
}
