// -*- coding: gb2312-dos -*-
/****************************************************************************************************
 *
 * 版权信息：版权所有 (c) 2015, 杭州海康威视数字技术股份有限公司, 保留所有权利
 *
 * 文件名称：MvErrorDefine.h
 * 摘    要：错误码定义
 *
 * 当前版本：1.0.0.0
 * 作    者：陈祖文
 * 日    期：2015-01-28
 * 备    注：新建
 ***************************************************************************************************/

#ifndef _MV_ERROR_DEFINE_H_
#define _MV_ERROR_DEFINE_H_

#define MV_OK                   0           ///< 成功，无错误
#define MV_E_HANDLE             0x80000000  ///< 错误或无效的句柄
#define MV_E_SUPPORT            0x80000001  ///< 不支持的功能
#define MV_E_BUFOVER            0x80000002  ///< 缓存已满
#define MV_E_CALLORDER          0x80000003  ///< 函数调用顺序错误
#define MV_E_PARAMETER          0x80000004  ///< 错误的参数
#define MV_E_RESOURCE           0x80000006  ///< 资源申请失败
#define MV_E_NETER              0x80000007  ///< 网络相关错误
#define MV_E_NODATA             0x80000008  ///< 无数据
#define MV_E_PRECONDITION       0x80000009  ///< 前置条件有误，或运行环境已发生变化
#define MV_E_PACKET             0x80000080  ///< 网络包数据错误
#define MV_E_SYS                0x80000081  ///< 系统调用出错

// GenICam系列错误
#define MV_E_GC_GENERIC         0x80000100  ///< 通用错误
#define MV_E_GC_ARGUMENT        0x80000101  ///< 参数非法
#define MV_E_GC_RANGE           0x80000102  ///< 值超出范围
#define MV_E_GC_PROPERTY        0x80000103  ///< 属性
#define MV_E_GC_RUNTIME         0x80000104  ///< 运行环境有问题
#define MV_E_GC_LOGICAL         0x80000105  ///< 逻辑错误
#define MV_E_GC_ACCESS          0x80000106  ///< 访问权限有误
#define MV_E_GC_TIMEOUT         0x80000107  ///< 超时
#define MV_E_GC_DYNAMICCAST     0x80000108  ///< 转换异常

//GEV_STATUS对应的错误码
#define MV_E_NOT_IMPLEMENTED    0x800000A1  ///< 命令不被设备支持
#define MV_E_INVALID_ADDRESS    0x800000A2  ///< 访问的目标地址不存在
#define MV_E_WRITE_PROTECT      0x800000A3  ///< 目标地址不可写
#define MV_E_ACCESS_DENIED      0x800000A4  ///< 访问无权限
#define MV_E_BUSY               0x800000A5  ///< 设备忙，或网络断开
#define MV_E_UNKNOW             0x800000ff  ///< 未知的错误

#define MV_E_UPG_FILE_MISMATCH     0x80000210 ///< 升级固件不匹配
#define MV_E_UPG_LANGUSGE_MISMATCH 0x80000211 ///< 升级固件语言不匹配
#define MV_E_UPG_CONFLICT          0x80000212 ///< 升级冲突（设备已经在升级了再次请求升级即返回此错误）
#define MV_E_UPG_INNER_ERR         0x80000213 ///< 升级时相机内部出现错误

#endif //_MV_ERROR_DEFINE_H_
