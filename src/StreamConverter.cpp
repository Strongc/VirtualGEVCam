// -*- coding: gb2312-dos -*-
#include "StreamConverter.h"
#include "Inc/FreeImage/FreeImage.h"

#include "stdint.h"
#include <iostream>
#include <fstream>

using namespace std;

StreamConverter::StreamConverter(const std::string strDirName)
        : _hFind(INVALID_HANDLE_VALUE), _strDirName(strDirName), _strCurFileName("")
{
    _yuvMatrix[0] = 0.299;     _yuvMatrix[1] = 0.587;     _yuvMatrix[2] = 0.114;
    _yuvMatrix[3] = -0.168736; _yuvMatrix[4] = -0.331264; _yuvMatrix[5] = 0.5;
    _yuvMatrix[6] = 0.5;       _yuvMatrix[7] = -0.418688; _yuvMatrix[8] = -0.081312;

    if (_strDirName[_strDirName.length()-1] == '\\')
    {
        _strDirName.erase(_strDirName.length()-1, 1);
    }

    _nFrameId          = 1;  // TODO
    _fTriggerFrequency = 0;  // TODO

    _pStreamBuffer = NULL;
    _pImageData    = NULL;
    _nImageLen     = 0;
    _nSizeX        = 0;
    _nSizeY        = 0;
}

StreamConverter::~StreamConverter()
{
    ::FindClose(_hFind);
}

int StreamConverter::Init()
{
    int nRet = MV_OK;

    _hFind = ::FindFirstFile((_strDirName + "\\*.*").c_str(), &_FindFileData);

    if (INVALID_HANDLE_VALUE == _hFind)
    {
        cout << "StreamConverter::Init() FindFirstFile (" << _strDirName << ") fail." << endl;
        return -1;
    }

    do
    {
        if ((_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
            && !(_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // NOT directory
            _strCurFileName = _strDirName + "\\" + string(_FindFileData.cFileName);
            break;
        }

        if (!FindNextFile(_hFind, &_FindFileData))
        {
            _strCurFileName = "";
            return -1;
        }
    }
    while (true);


    // allocate a memory buffer
    _pStreamBuffer = new unsigned char[IMAGE_FILE_MAX_SIZE];
    if (_pStreamBuffer == NULL)
    {
        return -1;
    }
    _pImageData = _pStreamBuffer;

    return nRet;
}

int StreamConverter::DeInit()
{
    int nRet = MV_OK;

    if (_pStreamBuffer)
    {
        delete[] _pStreamBuffer;
        _pStreamBuffer = NULL;
    }

    return nRet;
}

int StreamConverter::GetNextFrame(string& strFileName, PixelFormats OutFmt)
{
    int nRet = -1;

    do
    {
        if (!FindNextFile(_hFind, &_FindFileData))
        {
            // Traversal all files
            _hFind = ::FindFirstFile((_strDirName + "\\*.*").c_str(), &_FindFileData);
        }

        if ((_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
            && !(_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // NOT directory
            _strCurFileName = _strDirName + "\\" + string(_FindFileData.cFileName);
            strFileName = _strCurFileName;
            nRet = MV_OK;
            break;
        }
    }
    while (true);

    _OutFmt = OutFmt;
    Lock();
    nRet = UpdateImageData();
    Unlock();

    return nRet;
}

void StreamConverter::GetImageData(Device::virtual_addr_t& pData, size_t& nLen, uint32_t& nSizeX, uint32_t& nSizeY)
{
    pData  = _pImageData;
    nLen   = _nImageLen;
    nSizeX = _nSizeX;
    nSizeY = _nSizeY;
}

int StreamConverter::UpdateImageData()
{
    int nRet = MV_OK;

    fstream file;

    file.open(_strCurFileName.c_str(), ios::in | ios::binary);

    if (file.is_open())
    {
        file.seekg(0, ios::end);
        _nImageLen = file.tellg();
        file.seekg(0, ios::beg);
        if (_nImageLen <= IMAGE_FILE_MAX_SIZE)
        {
            file.read((char*)_pImageData, _nImageLen);
            file.close();
        }
        else
        {
            _nImageLen = 0;
            _pImageData = NULL;
            _nSizeX = _nSizeY = 0;
            return -1;
        }


        // TODO: OutFmt == MV_GVSP_PIX_MONO8
        // RAW File
        if (_strCurFileName.substr(_strCurFileName.length() - 4, 4) == string(".raw"))
        {
            // TODO:
            _nSizeX = 1920;
            _nSizeY = 1440;

            return nRet;
        }

        /*

        // NOT RAW
        FREE_IMAGE_FORMAT fiFormat;
        FIBITMAP *inImage;
        uint32_t nBpp = 0;

        // attach the binary data to a memory stream
        FIMEMORY *hmem = FreeImage_OpenMemory((BYTE*)_pImageMem, nFileSize);

        // get the file type
        fiFormat = FreeImage_GetFileTypeFromMemory(hmem, 0);
        if (fiFormat == FIF_UNKNOWN)
        {
        nLen = 0;
        pData = NULL;
        nSizeX = nSizeY = 0;
        return -1;
        }
        else if (fiFormat == FIF_JPEG || fiFormat == FIF_PNG || fiFormat == FIF_BMP)
        {
        // RGB
        // load an image from the memory stream
        inImage = FreeImage_LoadFromMemory(fiFormat, hmem, 0);

        // operate as a regular file
        nBpp = FreeImage_GetBPP(inImage);
        nSizeX = FreeImage_GetWidth(inImage);
        nSizeY = FreeImage_GetHeight(inImage);

        // TODO:
        FreeImage_Unload(inImage);
        }

        // always close the memory stream
        FreeImage_CloseMemory(hmem);

        */
    }
    else
    {
        _nImageLen = 0;
        _pImageData = NULL;
        _nSizeX = _nSizeY = 0;
        return -1;
    }

    return nRet;
}

void StreamConverter::Lock()
{
    _BufMutex.Lock();
}

void StreamConverter::Unlock()
{
    _BufMutex.Unlock();
}
