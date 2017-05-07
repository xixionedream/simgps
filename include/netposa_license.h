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

/// ʹ�ñ���key1�ļ�����key1���ݣ�key1�ļ�λ�ڽ�����ģ������Ŀ¼��
/// @key1Type[in] ����Ϊkft_pvg_key1��kft_pva_key1��kft_pcc_key1��kft_gis_key1��kft_pvd_key1
/// @return RmipError
///		ע: �������ʧ�ܣ����ܻ��ڱ���Ŀ¼����key0�ļ�
NETPOSA_LICENSE_API RmipError UpdateNetposaKey1ByLocalFile(KeyFileType key1Type);

/// ʹ���ⲿ���ݸ���key1����
/// @key1Type[in] ����Ϊkft_pvg_key1��kft_pva_key1��kft_pcc_key1��kft_gis_key1��kft_pvd_key1
/// @key1Data[in] �ⲿ�����key1�ļ����ݣ���NULL
/// @key1Len[in] �ⲿ�����key1�ļ����ݳ��ȣ�>0
/// @return RmipError
///		ע: �������ʧ�ܣ����ܻ��ڱ���Ŀ¼����key0�ļ�
NETPOSA_LICENSE_API RmipError UpdateNetposaKey1ByMemoryData(KeyFileType key1Type, const void* key1Data, INT_PTR key1Len);

/// ��ȡ��Ȩ��Ϣ�����ģ�ʹ��ǰ���ȵ���UpdateNetposaKey1ByLocalFile()��UpdateNetposaKey1ByMemoryData()
/// ע��!!!!!! �ڸ����ڼ�GetNetposaKey1()���ص����ݿ��ܷ������
/// @return ʧ�ܷ���NULL
NETPOSA_LICENSE_API const KeyFileContents* GetNetposaKey1();

#endif	// #ifndef _netposa_license_h_
