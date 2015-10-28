// -*- coding: gb2312-dos -*-
/** @file       MV_CrossPlatform.h
 *  @note       HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *  @brief      Machine Vision CrossPlatform define
 *
 *  @author     Machine Vision Team
 *  @date       2014/12/12
 *  @note       创建;
 *
 *  @warning
 */
#ifndef _MV_CROSSPLATFORM_H_
#define _MV_CROSSPLATFORM_H_

#include "MV_CrossPlatformDefine.h"

// 跨平台接口定义
#ifdef __cplusplus
extern "C"
{
#endif

    /************************************************************************
     *      线程相关
     ************************************************************************/

    // 初始化一个临界区
    void    MV_InitMutex(MV_Mutex * mutex);
    // 释放临界区
    void    MV_EnterMutex(MV_Mutex * mutex);
    // 进入临界区
    void    MV_LeaveMutex(MV_Mutex * mutex);
    // 退出临界区
    void    MV_DeleteMutex(MV_Mutex * mutex);
    // 创建线程
    void*   MV_CreateThread(MV_THREAD_ATTR* attr ,Thread_CallBack callbackFunc , void* pUser);
    // 等待线程正常退出
    int     MV_WaitForThreadEnd(void* pThread);
    // 结束线程
    int     MV_EndThread();
    // 销毁线程句柄
    int     MV_DestroyThread(void* pThread);



    /************************************************************************
     *      内存对齐分配和释放
     ************************************************************************/

    // 对齐申请内存
    void*   MV_Aligned_Malloc(unsigned int size , unsigned int alignment);
    // 对齐释放内存
    void    MV_Aligned_Free(void* memblock);

    // 返回值0表示成功
    int     MV_RellocBufferAdapt(unsigned char** ppBuf, unsigned int nDataLen, unsigned int* pnSize);



    /************************************************************************
     *      日志相关
     ************************************************************************/

    // 获取当前系统时间，精确到秒级
    void    MV_GetCurTime(char* strTime);
    // 直接打印日志信息
    void    MV_PrintInfo(MV_PRINT_TYPE enType, char *szFormat, ...);
    // 写日志文件-输入生成的文件路径
    void    MV_DumpLogBystr( char* pFile, char *fmt, ...);
    // 写日志文件-输入日志文件指针
    void    MV_DumpLogByfp( FILE* fp, char *fmt, ...);

    double  MV_GetMilliseconds();


    // 一些工具函数
    //const char* MV_CreateGUID();

#ifdef __cplusplus
}
#endif

/************************************************************************
 *      把锁的声明和初始化合并
 ************************************************************************/
class CMVMutex
{
  public:
    CMVMutex()
    {
        MV_InitMutex(&m_Mutex);
    }
    ~CMVMutex()
    {
        MV_DeleteMutex(&m_Mutex);
    }
    void Lock()
    {
        MV_EnterMutex(&m_Mutex);
    }
    void Unlock()
    {
        MV_LeaveMutex(&m_Mutex);
    }

  private:
    MV_Mutex    m_Mutex;
};

class CMVLock
{
  public:
    CMVLock(CMVMutex* pMutex = MV_NULL)
    {
        pMutex->Lock();
        m_pMutex = pMutex;
    }
    ~CMVLock()
    {
        m_pMutex->Unlock();
    }
  private:
    CMVMutex*   m_pMutex;
};

#endif //_MV_CROSSPLATFORM_H_
