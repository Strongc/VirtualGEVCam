// -*- coding: gb2312-dos -*-

#include "Windows_Thread.h"
#include "MV_CrossPlatformDefine.h"

#ifdef MV_WINDOWS
#include "process.h"

// 初始化一个临界区
void    WINDOWS_InitMutex(MV_Mutex* mutex)
{
    InitializeCriticalSection(mutex);
    return;
}

// 释放临界区
void    WINDOWS_EnterMutex(MV_Mutex* mutex)
{
    EnterCriticalSection(mutex);
    return ;
}

// 进入临界区
void    WINDOWS_LeaveMutex(MV_Mutex* mutex)
{
    LeaveCriticalSection(mutex);
    return ;
}

// 退出临界区
void    WINDOWS_DeleteMutex(MV_Mutex* mutex)
{
    DeleteCriticalSection(mutex);
    return ;
}

// 创建线程
// 成功返回非空指针，失败返回空指针
void*   WINDOWS_CreateThread(MV_THREAD_ATTR* attr,Thread_CallBack callbackFunc, void* pUser)
{
    void*           pThread         = MV_NULL;
    unsigned int    nThreadID       = 0;
    int             threadPriority  = THREAD_PRIORITY_HIGHEST;

    pThread = (void*) _beginthreadex(MV_NULL, 0, callbackFunc, pUser, 0, &nThreadID);
    if (pThread == MV_NULL)
    {
        return MV_NULL;
    }

    if (attr != MV_NULL )
    {
        switch (attr->threadPriority)
        {
            case MV_THREAD_PRIORITY_LOWEST:
                threadPriority = THREAD_PRIORITY_LOWEST;
                break;
            case MV_THREAD_PRIORITY_BELOW_NORMAL:
                threadPriority = THREAD_PRIORITY_BELOW_NORMAL;
                break;
            case MV_THREAD_PRIORITY_NORMAL:
                threadPriority = THREAD_PRIORITY_NORMAL;
                break;
            case MV_THREAD_PRIORITY_ABOVE_NORMAL:
                threadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
                break;
            case MV_THREAD_PRIORITY_HIGHEST:
                threadPriority = THREAD_PRIORITY_HIGHEST;
                break;
            default:
                threadPriority = THREAD_PRIORITY_ERROR_RETURN;
                break;
        }
    }

    if (!SetThreadPriority(pThread, threadPriority))
    {
        return MV_NULL;
    }

    return pThread;
}

// 等待线程正常退出
int     WINDOWS_WaitForThreadEnd(void* pThread)
{
    WaitForSingleObject(pThread,INFINITE);
    return 0;
}

// 结束线程
int     WINDOWS_EndThread()
{
    _endthreadex(0);
    return 0;
}

// 销毁线程句柄
int     WINDOWS_DestroyThread(void* pThread)
{
    CloseHandle(pThread);
    return 0;
}

#endif
