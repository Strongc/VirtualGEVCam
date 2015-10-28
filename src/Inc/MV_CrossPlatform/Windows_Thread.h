// -*- coding: gb2312-dos -*-

#ifndef _MV_WINDOWS_THREAD_H__
#define _MV_WINDOWS_THREAD_H__

#ifdef MV_WINDOWS

#include "windows.h"
#include "MV_CrossPlatformDefine.h"

// 初始化一个临界区
void    WINDOWS_InitMutex(MV_Mutex * mutex);
// 释放临界区
void    WINDOWS_EnterMutex(MV_Mutex * mutex);
// 进入临界区
void    WINDOWS_LeaveMutex(MV_Mutex * mutex);
// 退出临界区
void    WINDOWS_DeleteMutex(MV_Mutex * mutex);

// 创建线程
// 成功返回非空指针，失败返回空指针
void*   WINDOWS_CreateThread(MV_THREAD_ATTR* attr ,Thread_CallBack callbackFunc , void* pUser);

// 等待线程正常退出
int     WINDOWS_WaitForThreadEnd(void* pThread);
// 结束线程
int     WINDOWS_EndThread();
// 销毁线程句柄
int     WINDOWS_DestroyThread(void* pThread);

#endif
#endif
