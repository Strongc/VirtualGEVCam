// -*- coding: gb2312-dos -*-
/** @file       MV_Thread.cpp
 *  @note       HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 *  @brief      跨平台线程管理类
 *
 *  @author     Machine Vision Team
 *  @date       2014/12/12
 *  @note       创建;
 *
 *  @warning    当前使用的GPU模式，非单独解码，还包含编码;
 */
#include "MV_CrossPlatform.h"

#if defined(MV_WINDOWS)
#include "Windows_Thread.h"
#elif defined(MV_LINUX)
#include "Linux_Thread.h"
#endif


// 初始化一个临界区
void    MV_InitMutex(MV_Mutex * mutex)
{
#if defined(MV_WINDOWS)
    WINDOWS_InitMutex(mutex);
#elif defined(MV_LINUX)
    LINUX_InitMutex(mutex);
#endif
}

// 释放临界区
void    MV_EnterMutex(MV_Mutex * mutex)
{
#if defined(MV_WINDOWS)
    WINDOWS_EnterMutex(mutex);
#elif defined(MV_LINUX)
    LINUX_Enterutex(mutex);
#endif
}

// 进入临界区
void    MV_LeaveMutex(MV_Mutex * mutex)
{
#if defined(MV_WINDOWS)
    WINDOWS_LeaveMutex(mutex);
#elif defined(MV_LINUX)
    LINUX_LeaveMutex(mutex);
#endif
}

// 退出临界区
void    MV_DeleteMutex(MV_Mutex * mutex)
{
#if defined(MV_WINDOWS)
    WINDOWS_DeleteMutex(mutex);
#elif defined(MV_LINUX)
    LINUX_DeleteMutex(mutex);
#endif
}

// 创建线程
void*   MV_CreateThread(MV_THREAD_ATTR* attr ,Thread_CallBack callbackFunc , void* pUser)
{
#if defined(MV_WINDOWS)
    return WINDOWS_CreateThread(attr, callbackFunc, pUser);
#elif defined(MV_LINUX)
    return LINUX_CreateThread(attr, callbackFunc, pUser);
#endif
}

// 等待线程正常退出
int     MV_WaitForThreadEnd(void* pThread)
{
#if defined(MV_WINDOWS)
    return WINDOWS_WaitForThreadEnd(pThread);
#elif defined(MV_LINUX)
    return LINUX_WaitForThreadEnd(pThread);
#endif
}

// 结束线程
int     MV_EndThread()
{
#if defined(MV_WINDOWS)
    return WINDOWS_EndThread();
#elif defined(MV_LINUX)
    return LINUX_EndThread();
#endif
}

// 销毁线程句柄
int     MV_DestroyThread(void* pThread)
{
#if defined(MV_WINDOWS)
    return WINDOWS_DestroyThread(pThread);
#elif defined(MV_LINUX)
    return LINUX_DestroyThread(pThread);
#endif
}
