// -*- coding: gb2312-dos -*-

#include <stdlib.h>
#include "MV_CrossPlatform.h"

#define IS_2_POW_N(X)   (((X)&(X-1)) == 0)

// 对齐申请内存
void*   MV_Aligned_Malloc(unsigned int size , unsigned int alignment)
{
    unsigned int ptr, r_ptr;
    unsigned int *reptr;

    if (!IS_2_POW_N(alignment))
    {
        return MV_NULL;
    }

    alignment = (alignment > sizeof(void *) ? alignment : sizeof(void *));

    if ((ptr = (unsigned int)malloc(size + alignment + sizeof(void *))) == ((unsigned int)MV_NULL))
        return MV_NULL;

    r_ptr = (ptr + alignment + sizeof(void *)) & ~(alignment -1);
    reptr = (unsigned int *)(r_ptr - sizeof(void *));
    *reptr = ptr;

    return (void *)r_ptr;
}


// 对齐释放内存
void    MV_Aligned_Free(void* memblock)
{
    try
    {
        unsigned int ptr;

        if (memblock == MV_NULL)
            return;

        ptr = (unsigned int)memblock;

        ptr = (ptr & ~(sizeof(void *) -1)) - sizeof(void *);

        ptr = *((unsigned int *)ptr);
        free((void *)ptr);
    }
    catch (...)
    {
        return ;
    }

    return ;
}

/** @fn     MV_RellocBufferAdapt(unsigned char** ppBuf, unsigned int nDataLen, unsigned int* pnSize)
 *  @brief  自适应分配缓存
 *  @param  ppBuf           [IN][OUT]   - 待分配缓存的指针
 nDataLen        [IN]        - 计划分配的大小
 pnSize          [IN][OUT]   - 原缓存大小
 *
 *  @return 成功，返回IMGPROC_OK；失败，返回错误码
 */
int     MV_RellocBufferAdapt(unsigned char** ppBuf, unsigned int nDataLen, unsigned int* pnSize)
{
    if (MV_NULL == *ppBuf)
    {
        *ppBuf = (unsigned char*)MV_Aligned_Malloc(nDataLen, 64);
        if (MV_NULL == *ppBuf)
        {
            return -1;
        }

        *pnSize = nDataLen;
        memset(*ppBuf, 0, nDataLen);
    }
    else
    {
        if (nDataLen > *pnSize)
        {
            MV_Aligned_Free(*ppBuf);
            *ppBuf = (unsigned char*)MV_Aligned_Malloc(nDataLen, 64);
            if (MV_NULL == *ppBuf)
            {
                return -1;
            }

            *pnSize = nDataLen;
            memset(*ppBuf, 0, nDataLen);
        }
    }

    return 0;
}
