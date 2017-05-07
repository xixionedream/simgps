// rmiperr.h

#ifndef _rmiperr_h_
#define _rmiperr_h_

#if _MSC_VER > 1000
	#pragma once
#endif

#include "nptypes.h"

typedef enum
{
#define TCPS_DEFINE_ERROR(symbolName, errorNumber, errorDescription) \
	symbolName = errorNumber,
	#include "tcps_err_msg.h"
#undef TCPS_DEFINE_ERROR

#define RMIP_DEFINE_ERROR(symbolName, errorNumber, errorTitle) \
	symbolName = errorNumber,
	#include "rmiperr_msg.h"
#undef RMIP_DEFINE_ERROR

	RMIP_ERR_PARM = RMIP_ERR_INVALID_PARAM,
	RMIP_ERR_BASE = 0

} RmipError;

static __inline BOOL RMIP_ERR_NetworkCommunication(int err)
{
	return
		   err == RMIP_ERR_CONNECT_SERVER      //���ӷ�����ʧ��
		|| err == RMIP_ERR_SEND                //����ʧ��
		|| err == RMIP_ERR_RECV                //����ʧ��
		|| err == RMIP_ERR_BAD_IP_OR_HOST_NAME //�����IP��ַ����������
		|| err == RMIP_ERR_SERVER_WAS_DIED     //������û����Ӧ
		|| err == RMIP_ERR_RECV_TIME_OUT       //����ʧ��
		|| err == RMIP_ERR_PVG_DISCONNECT      //�������ڶ���״̬
		|| err == RMIP_ERR_PING                //������PING��ͨ
	//	|| err == RMIP_ERR_CONNECT_TO_DEVICE   //�����豸ʧ�ܣ�һ�����������
		;
}

static __inline BOOL RMIP_ERR_SessionBroken(int err)
{
	return RMIP_ERR_NetworkCommunication(err)
		|| err == RMIP_ERR_NOT_CONNECTED       //��δ���ӷ�����
		|| err == RMIP_ERR_MALFORMED_REPLY     //�ͻ��˼�⵽�޷�������Ӧ��
		|| err == RMIP_ERR_ALREADY_CONNECTED   //�Ѿ������˷�����
		|| err == RMIP_ERR_NOT_LOGIN           //�û�û�е�¼
		;
}


static __inline void SetRmipErr(int r)
{
	int err;
	if(r>=RMIP_OK || r<=-0x100000)
		err = RMIP_ERROR;
	else
		err = ((-r)|0x20000000);
#if defined(WIN32)
	SetLastError((DWORD)err);
#elif defined(_NP_IS_LINUX)
	errno = err;
#endif
}

#ifdef __cplusplus

#define RMIP_CATCH_ALL_PARAM_EX(err_proc_exp, ret_exp)	\
	catch(volatile RmipError err)		\
	{									\
		err_proc_exp;					\
		err = err;						\
		return ret_exp;					\
	}									\
	catch(volatile int err)				\
	{									\
		err_proc_exp;					\
		err = err;						\
		return ret_exp;					\
	}									\
	catch(std::bad_alloc)				\
	{									\
		err_proc_exp;					\
		volatile int err = RMIP_ERR_MEMORY;		\
		err = err;						\
		return ret_exp;					\
	}									\
	NP_BEGIN_CATCH_ALL()				\
		err_proc_exp;					\
		volatile int err = RMIP_ERR_RUN_EXCEPTION;	\
		err = err;						\
		return ret_exp;					\
	NP_END_CATCH_ALL()


#define RMIP_CATCH_ALL_PARAM(ret_exp)	RMIP_CATCH_ALL_PARAM_EX((void)0, ret_exp)

// ����RmipError
#define RMIP_CATCH_ALL_RMIPERR()		RMIP_CATCH_ALL_PARAM(((RmipError)err))
// �޷���ֵ
#define RMIP_CATCH_ALL_VOID()			RMIP_CATCH_ALL_PARAM(;)
// ����NULL
#define RMIP_CATCH_ALL_NULL()			RMIP_CATCH_ALL_PARAM(NULL)

#endif	/* #ifdef __cplusplus */


typedef enum
{
	ROFS_STATUS_UNKNOWN	= 0, // �޷������ļ�ϵͳ��δ��ʽ��״̬��
	ROFS_STATUS_CLOSED, // �豸�ر�״̬���ļ�ϵͳ���������κζ�����
	ROFS_STATUS_OPENED, // �豸��״̬��¼������У�
	ROFS_STATUS_REPAIRING, // �����޸�
} RofsStatus;

#endif /* _rmiperr_h_ */
