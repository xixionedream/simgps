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
		   err == RMIP_ERR_CONNECT_SERVER      //连接服务器失败
		|| err == RMIP_ERR_SEND                //发送失败
		|| err == RMIP_ERR_RECV                //接收失败
		|| err == RMIP_ERR_BAD_IP_OR_HOST_NAME //错误的IP地址或主机名称
		|| err == RMIP_ERR_SERVER_WAS_DIED     //服务器没有响应
		|| err == RMIP_ERR_RECV_TIME_OUT       //接收失败
		|| err == RMIP_ERR_PVG_DISCONNECT      //服务器在断线状态
		|| err == RMIP_ERR_PING                //服务器PING不通
	//	|| err == RMIP_ERR_CONNECT_TO_DEVICE   //连接设备失败（一般是网络错误）
		;
}

static __inline BOOL RMIP_ERR_SessionBroken(int err)
{
	return RMIP_ERR_NetworkCommunication(err)
		|| err == RMIP_ERR_NOT_CONNECTED       //尚未连接服务器
		|| err == RMIP_ERR_MALFORMED_REPLY     //客户端检测到无法解析的应答
		|| err == RMIP_ERR_ALREADY_CONNECTED   //已经连接了服务器
		|| err == RMIP_ERR_NOT_LOGIN           //用户没有登录
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

// 返回RmipError
#define RMIP_CATCH_ALL_RMIPERR()		RMIP_CATCH_ALL_PARAM(((RmipError)err))
// 无返回值
#define RMIP_CATCH_ALL_VOID()			RMIP_CATCH_ALL_PARAM(;)
// 返回NULL
#define RMIP_CATCH_ALL_NULL()			RMIP_CATCH_ALL_PARAM(NULL)

#endif	/* #ifdef __cplusplus */


typedef enum
{
	ROFS_STATUS_UNKNOWN	= 0, // 无法设别的文件系统（未格式化状态）
	ROFS_STATUS_CLOSED, // 设备关闭状态（文件系统正常，无任何动作）
	ROFS_STATUS_OPENED, // 设备打开状态（录像就绪中）
	ROFS_STATUS_REPAIRING, // 正在修复
} RofsStatus;

#endif /* _rmiperr_h_ */
