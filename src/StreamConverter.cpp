// -*- coding: gb2312-dos -*-
#include "StreamConverter.h"
#include "Inc/FreeImage/FreeImage.h"

#include "stdint.h"
#include <iostream>
#include <fstream>

using namespace std;

StreamConverter::StreamConverter(const std::string strRootDir)
        : _hFindDir(INVALID_HANDLE_VALUE), _hFindArch(INVALID_HANDLE_VALUE),
          _strRootDir(strRootDir), _strCurDirName(""), _strCurArchName("")
{
    _yuvMatrix[0] = 0.299;     _yuvMatrix[1] = 0.587;     _yuvMatrix[2] = 0.114;
    _yuvMatrix[3] = -0.168736; _yuvMatrix[4] = -0.331264; _yuvMatrix[5] = 0.5;
    _yuvMatrix[6] = 0.5;       _yuvMatrix[7] = -0.418688; _yuvMatrix[8] = -0.081312;

    if (_strRootDir[_strRootDir.length()-1] == '\\')
    {
        _strRootDir.erase(_strRootDir.length()-1, 1);
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
}

int StreamConverter::Init()
{
    int nRet = MV_OK;


    // Allocate a memory buffer
    _pStreamBuffer = new unsigned char[IMAGE_FILE_MAX_SIZE];
    if (_pStreamBuffer == NULL)
    {
        return -1;
    }
    _pImageData = _pStreamBuffer;


    // Find first frame file
    _hFindDir = ::FindFirstFile((_strRootDir+"\\*").c_str(), &_FindFileDataDir);
    if (INVALID_HANDLE_VALUE == _hFindDir)
    {
        cout << "[StreamConverter]::Init() FindFirstFile (" << _strRootDir << ") NOT exist." << endl;
        return -1;
    }

    do
    {
        if (FindDirectory() == MV_OK)
        {
            _hFindArch = ::FindFirstFile((_strRootDir+"\\"+_strCurDirName+"\\*.*").c_str(), &_FindFileDataArch);
            if (INVALID_HANDLE_VALUE == _hFindArch)
            {
                continue;
            }

            if (FindArchive() == MV_OK)
            {
                // Find frame file
                break;
            }
            else
            {
                cout << "[StreamConverter]::Init() NO archive under " << _strRootDir+"\\"+_strCurDirName << endl;
                continue;
            }
        }
        else
        {
            cout << "[StreamConverter]::Init() find frame file fail!! (NO frame file under "
                 << _strRootDir << ")" << endl;
            return -1;
        }
    }
    while (true);

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

    ::FindClose(_hFindDir);
    ::FindClose(_hFindArch);

    return nRet;
}

int StreamConverter::GetNextFrame(string& strFileName, PixelFormats OutFmt)
{
    int nRet = -1;

    if (FindArchive() != MV_OK)
    {
        do
        {
            if (FindDirectory() == MV_OK)
            {
                _hFindArch = ::FindFirstFile((_strRootDir+"\\"+_strCurDirName+"\\*.*").c_str(), &_FindFileDataDir);
                if (INVALID_HANDLE_VALUE == _hFindArch)
                {
                    continue;
                }

                if (FindArchive() == MV_OK)
                {
                    // Find frame file
                    strFileName = _strRootDir+"\\"+_strCurDirName+"\\"+_strCurArchName;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                // Loop goto the first directory
                _hFindDir = ::FindFirstFile((_strRootDir+"\\*").c_str(), &_FindFileDataDir);
                continue;
            }
        }
        while (true);
    }


    _OutFmt = OutFmt;
    Lock();
    nRet = UpdateImageData();
    Unlock();

    return nRet;
}

/** @fn          FindDirectory
 *  @brief       Find directory, get the first directory or next directory
 *  @param[in]   none
 *  @param[out]  none
 *  @return      MV_OK -- Find next directory
 *               -1    -- No more directory
 **/
int StreamConverter::FindDirectory()
{
    int nRet = MV_OK;

    do
    {
        if ((_FindFileDataDir.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // Directory
            _strCurDirName = string(_FindFileDataDir.cFileName);
            if (_strCurDirName != string(".") && _strCurDirName != string(".."))
            {
                break;
            }
        }

        if (!FindNextFile(_hFindDir, &_FindFileDataDir))
        {
            // Traversal end
            _strCurDirName = "";
            nRet = -1;
            break;
        }
    }
    while (true);

    return nRet;
}

/** @fn          FindArchive
 *  @brief       Find archive, get the first archive or next archive
 *  @param[in]   none
 *  @param[out]  none
 *  @return      MV_OK -- Find next archive
 *               -1    -- No more archive
 **/
int StreamConverter::FindArchive()
{
    int nRet = MV_OK;

    do
    {
        if ((_FindFileDataArch.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
            && !(_FindFileDataArch.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // NOT directory
            _strCurArchName = string(_FindFileDataArch.cFileName);
            break;
        }

        if (!FindNextFile(_hFindArch, &_FindFileDataArch))
        {
            // Traversal end
            _strCurArchName = "";
            nRet = -1;
            break;
        }
    }
    while (true);

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

    file.open((_strRootDir+"\\"+_strCurDirName+"\\"+_strCurArchName).c_str(), ios::in | ios::binary);

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
        if (_strCurArchName.substr(_strCurArchName.length() - 4, 4) == string(".raw"))
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
