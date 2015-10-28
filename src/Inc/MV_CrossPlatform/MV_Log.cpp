// -*- coding: gb2312-dos -*-

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include "MV_CrossPlatform.h"


void    MV_GetCurTime(char* strTime)
{
    //获取当前系统时间
    time_t rawtime;
    struct tm  timeinfo;
    time ( &rawtime );
    localtime_s(&timeinfo, &rawtime);
    char chGlobalTime[32] = {0};
    // localtime: current_year = tm_year + 1900;    localtime: tm_mon (0 ~ 11; January = 0 )
    sprintf_s(chGlobalTime, 32, "%4d-%02d-%02d-%02d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
              timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    if (strTime)
    {
        strcpy_s(strTime, 32, chGlobalTime);
    }
}

void    MV_PrintInfo(MV_PRINT_TYPE enType, char *szFormat, ...)
{
    switch(enType)
    {
#ifdef _DEBUG
        case MV_PRINT_DEBUG:
            {
                char szInfo[512];
                va_list ArgumentList;

                va_start(ArgumentList, szFormat);
                vsprintf_s(szInfo, 512, szFormat, ArgumentList);
                va_end(ArgumentList);

#if defined(MV_WINDOWS)
                OutputDebugString(szInfo);
#else
                printf("%s", szInfo);
#endif
            }
            break;
#endif
        case MV_PRINT_WHENEVER:
            {
                char szInfo[512];
                va_list ArgumentList;

                va_start(ArgumentList, szFormat);
                vsprintf_s(szInfo, 512, szFormat, ArgumentList);
                va_end(ArgumentList);

#if defined(MV_WINDOWS)
                OutputDebugString(szInfo);
#else
                printf("%s", szInfo);
#endif
            }
            break;
        default:
            break;
    }

    return;
}

void    MV_DumpLogBystr( char* pFile, char *fmt, ...)
{
    FILE* fp;
    va_list args;
    char   szInfo[512];

    //获取当前系统时间
    time_t rawtime;
    struct tm  timeinfo;
    time ( &rawtime );
    localtime_s(&timeinfo, &rawtime);
    char chGlobalTime[40] = {0};
    // localtime: current_year = tm_year + 1900;    localtime: tm_mon (0 ~ 11; January = 0 )
    sprintf_s(chGlobalTime, 40, "[%d-%d-%d %d:%d:%d] ", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
              timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    va_start(args, fmt);
    vsprintf_s(szInfo, 512, fmt, args);
    va_end(args);

    fopen_s(&fp, pFile, "ab+");
    if(fp)
    {
        fwrite(chGlobalTime, strlen(chGlobalTime), 1, fp);
        fwrite(szInfo, strlen(szInfo), 1, fp);
        fflush(fp);
        fclose(fp);
    }
}

void    MV_DumpLogByfp( FILE* fp, char *fmt, ...)
{
    va_list args;
    char   szInfo[512] = {0};

    //获取当前系统时间
    time_t rawtime;
    struct tm  timeinfo;
    time ( &rawtime );
    localtime_s(&timeinfo, &rawtime);
    char chGlobalTime[40] = {0};
    // localtime: current_year = tm_year + 1900;    localtime: tm_mon (0 ~ 11; January = 0 )
    sprintf_s(chGlobalTime, 40, "[%d-%d-%d %d:%d:%d] ", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
              timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    va_start(args, fmt);
    vsprintf_s(szInfo, 512, fmt, args);
    va_end(args);

    if(fp)
    {
        fwrite(chGlobalTime, strlen(chGlobalTime), 1, fp);
        fwrite(szInfo, strlen(szInfo), 1, fp);
        fflush(fp);
    }
}

double MV_GetMilliseconds()
{
#ifdef WIN32
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    LARGE_INTEGER resolution;
    QueryPerformanceFrequency(&resolution);
    double dticks = (double)ticks.QuadPart;
    double dresolution = (double)resolution.QuadPart;
    return dticks / dresolution * 1000.0;
#else
    return 0.0;
#endif
}


// 创建文件夹

// 以时间命名log文件
