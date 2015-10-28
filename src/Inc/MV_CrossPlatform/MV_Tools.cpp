// -*- coding: gb2312-dos -*-

#include <stdio.h>
#include "MV_CrossPlatform.h"

//const char* MV_CreateGUID()
//{
//    static char strGuid[MAX_GUID_LENGTH] = {0};
//    char chBuffer[MAX_GUID_LENGTH] = {0};
//    GUID guid;
//    if (S_OK == ::CoCreateGuid(&guid))
//    {
//        _snprintf_s(chBuffer, /*sizeof(buf)*/MAX_GUID_LENGTH
//            , "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"      /*{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}*/
//            , guid.Data1
//            , guid.Data2
//            , guid.Data3
//            , guid.Data4[0], guid.Data4[1]
//        , guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
//        , guid.Data4[6], guid.Data4[7]
//        );
//    }
//
//    memcpy(strGuid, chBuffer, strlen(chBuffer));
//
//    return (const char*)strGuid;
//}
