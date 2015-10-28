#ifndef __LOG_H__
#define __LOG_H__

#if defined __WIN32 || defined __WIN64 || defined WIN32 || defined WIN64
#define WINDOWS
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string>
#ifdef WINDOWS
#include <time.h>
#endif

#include "Inc/color_print/color_print.h"

class Log
{
  public:
    typedef enum
    {
        PRINT_DEBUG = 0,
        PRINT_INFO,
        PRINT_WARN
    } PRINT_TYPE;

  public:
    Log() {};
    virtual ~Log() {};

    std::string GetCurTime(struct tm* ptimeinfo)
    {
        char chGlobalTime[32] = {0};
        if (ptimeinfo == NULL)
        {
            struct tm timeinfo;
            ptimeinfo = &timeinfo;
        }
        time_t rawtime;
        time(&rawtime);
#ifdef WINDOWS
        localtime_s(ptimeinfo, &rawtime);
        // localtime: current_year = tm_year + 1900;    localtime: tm_mon (0 ~ 11; January = 0 )
        sprintf(chGlobalTime, "%4d-%02d-%02d-%02d-%02d-%02d", ptimeinfo->tm_year + 1900, ptimeinfo->tm_mon + 1,
                ptimeinfo->tm_mday, ptimeinfo->tm_hour, ptimeinfo->tm_min, ptimeinfo->tm_sec);
#else
        // TODO
#endif
        return std::string(chGlobalTime);
    }

    double GetMilliseconds()
    {
#ifdef WINDOWS
        LARGE_INTEGER ticks;
        QueryPerformanceCounter(&ticks);
        LARGE_INTEGER resolution;
        QueryPerformanceFrequency(&resolution);
        double dticks = (double)ticks.QuadPart;
        double dresolution = (double)resolution.QuadPart;
        return dticks / dresolution * 1000.0;
#else
        // TODO
        return 0.0;
#endif
    }

    void CPPrint(PRINT_TYPE enType, char *szFormat, ...)
    {
        char szInfo[512];
        va_list ArgumentList;
        cp_state_ptr cp = cp_init();

        switch(enType)
        {
#ifdef _DEBUG
            case PRINT_DEBUG:
                {
                    va_start(ArgumentList, szFormat);
                    snprintf(szInfo, 512, szFormat, ArgumentList);
                    va_end(ArgumentList);
                    cp_print(cp, CP_FG_BLUE, szInfo);
                }
                break;
#endif
            case PRINT_INFO:
                {
                    va_start(ArgumentList, szFormat);
                    snprintf(szInfo, 512, szFormat, ArgumentList);
                    va_end(ArgumentList);
                    cp_print(cp, CP_DEF, szInfo);
                }
                break;
            case PRINT_WARN:
                {
                    va_start(ArgumentList, szFormat);
                    snprintf(szInfo, 512, szFormat, ArgumentList);
                    va_end(ArgumentList);
                    cp_print(cp, CP_FG_RED, szInfo);
                }
            default:
                break;
        }
        cp_close(cp);
    }

    void Print(PRINT_TYPE enType, char *szFormat, ...)
    {
        char szInfo[512];
        va_list ArgumentList;

        switch(enType)
        {
#ifdef _DEBUG
            case PRINT_DEBUG:
                {
                    va_start(ArgumentList, szFormat);
                    snprintf(szInfo, 512, szFormat, ArgumentList);
                    va_end(ArgumentList);
                    printf(szInfo);
                }
                break;
#endif
            case PRINT_INFO:
                {
                    va_start(ArgumentList, szFormat);
                    snprintf(szInfo, 512, szFormat, ArgumentList);
                    va_end(ArgumentList);
                    printf(szInfo);
                }
                break;
            case PRINT_WARN:
                {
                    va_start(ArgumentList, szFormat);
                    snprintf(szInfo, 512, szFormat, ArgumentList);
                    va_end(ArgumentList);
                    printf(szInfo);
                }
            default:
                break;
        }
    }

  private:

};


#endif /* __LOG_H__ */
