/* npbasetypes.h
 *
 * 基本数据类型定义
 */

#ifndef _npbasetypes_h_
#define _npbasetypes_h_

#if _MSC_VER > 1000
	#pragma once
#endif

#include "nprbase.h"

/* 宏 */
#ifndef IN
	#define IN
#endif
#ifndef OUT
	#define OUT
#endif

#ifndef CONST
	#define CONST const
#endif

#ifndef VOID
	#define VOID void
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL 0
	#else
		#define NULL ((void*)0)
	#endif
#endif

#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif

#ifndef __cplusplus
	#undef true
	#undef false
	#define true 1
	#define false 0
#endif

/* 基本数据类型 */
#ifndef _OBJC_OBJC_H_
	typedef int						BOOL;
#endif
typedef int							BOOLi;  /* for MacOS */

typedef int							INT;
typedef unsigned int				UINT;
typedef short						SHORT;
typedef unsigned short				USHORT;
typedef long						LONG;
typedef unsigned long				ULONG;

typedef char						CHAR;
typedef wchar_t						WCHAR;

typedef signed char					INT8;
typedef unsigned char				UINT8;
typedef signed short				INT16;
typedef unsigned short				UINT16;

#if defined(_MSC_VER)
	#if 64==NPR_SYS_BITS
		typedef signed __int32		INT32;
		typedef unsigned __int32	UINT32;
	#else
		typedef signed int			INT32;
		typedef unsigned int		UINT32;
	#endif
	typedef signed __int64			INT64;
	typedef unsigned __int64		UINT64;
	#define I64CONST(x)				x
	#define U64CONST(x)				x
#else
	#if 32==NPR_SYS_BITS || 64==NPR_SYS_BITS
		typedef int					INT32;
		typedef unsigned int		UINT32;
	#else
		typedef signed long			INT32;
		typedef unsigned long		UINT32;
	#endif
	typedef signed long long		INT64;
	typedef unsigned long long		UINT64;
	#define I64CONST(x)				x##LL
	#define U64CONST(x)				x##LLU
#endif

typedef unsigned char				BYTE;
typedef unsigned short				WORD;
#if defined(_NP_IS_LINUX) && 64==NPR_SYS_BITS
	typedef unsigned int			DWORD;
#else
	typedef unsigned long			DWORD;
#endif

#if 16==NPR_SYS_BITS
	typedef short					INT_PTR;
	typedef unsigned short			UINT_PTR;
#elif 32==NPR_SYS_BITS
	#if !defined(WIN32)
		typedef int					INT_PTR;
		typedef unsigned int		UINT_PTR;
	#endif
#elif 64==NPR_SYS_BITS
	typedef INT64					INT_PTR;
	typedef UINT64					UINT_PTR;
#else
	#error Unsupported platform.
#endif

typedef void*						LPVOID;
typedef const void*					LPCVOID;

typedef char*						LPSTR;
typedef const char*					LPCSTR;
typedef WCHAR*						LPWSTR;
typedef const WCHAR*				LPCWSTR;

#if defined(_UNICODE) || defined(UNICODE)
	typedef WCHAR					TCHAR;
#else
	typedef char					TCHAR;
#endif
typedef TCHAR*						LPTSTR;
typedef const TCHAR*				LPCTSTR;

#ifndef DECLARE_HANDLE
	#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__* name
#endif

/* 句柄类型等 */
#if !defined(_MSC_VER)
	DECLARE_HANDLE(HANDLE);
	DECLARE_HANDLE(HINSTANCE);
	typedef HINSTANCE HMODULE;
	typedef int (*PROC)();
	typedef PROC FARPROC;
#endif

/* SOCKET相关 */
#if defined(_MSC_VER)
	typedef SOCKET					FD_TYPE;
#else
	typedef int						FD_TYPE;
	typedef int						SOCKET;
#endif

#endif	/* #ifndef _npbasetypes_h_ */
