// netposa_license.h

#ifndef _netposa_license_h_
#define _netposa_license_h_

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include "netposa_license_pub.h"
#include "rmiperr.h"

#if defined(__GNUC__) && __GNUC__>=4
	#define NETPOSA_LICENSE_DECL	__attribute__((visibility("default")))
#else
	#define NETPOSA_LICENSE_DECL
#endif

#ifdef __cplusplus
	#define NETPOSA_LICENSE_API		extern "C" NETPOSA_LICENSE_DECL
#else
	#define NETPOSA_LICENSE_API		NETPOSA_LICENSE_DECL
#endif

/// 使用本地key1文件更新key1数据（key1文件位于进程主模块所在目录）
/// @key1Type[in] 可以为kft_pvg_key1、kft_pva_key1、kft_pcc_key1、kft_gis_key1、kft_pvd_key1
/// @return RmipError
///		注: 如果更新失败，则总会在本地目录生成key0文件
NETPOSA_LICENSE_API RmipError UpdateNetposaKey1ByLocalFile(KeyFileType key1Type);

/// 使用外部数据更新key1数据
/// @key1Type[in] 可以为kft_pvg_key1、kft_pva_key1、kft_pcc_key1、kft_gis_key1、kft_pvd_key1
/// @key1Data[in] 外部传入的key1文件数据，非NULL
/// @key1Len[in] 外部传入的key1文件数据长度，>0
/// @return RmipError
///		注: 如果更新失败，则总会在本地目录生成key0文件
NETPOSA_LICENSE_API RmipError UpdateNetposaKey1ByMemoryData(KeyFileType key1Type, const void* key1Data, INT_PTR key1Len);

/// 获取授权信息上下文，使用前请先调用UpdateNetposaKey1ByLocalFile()或UpdateNetposaKey1ByMemoryData()
/// 注意!!!!!! 在更新期间GetNetposaKey1()返回的内容可能发生变更
/// @return 失败返回NULL
NETPOSA_LICENSE_API const KeyFileContents* GetNetposaKey1();

#endif	// #ifndef _netposa_license_h_
