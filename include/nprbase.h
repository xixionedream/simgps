/***
 *nprbase.h - NetPosa basic header
 *    Copyright (c) 2000-2012, NetPosa Technologies,Ltd. All rights reserved.
 *Purpose:
 *    For all npr lib headers.
 **************************************************************************************/

#ifndef _nprbase_h_
#define _nprbase_h_

#if _MSC_VER > 1000
	#pragma once
#endif

/* 检查支持平台 */
#if !defined(WIN32) && !defined(_WIN64) && !defined(__linux__) && !defined(IOS6)
	#error Unsupported platform.
#endif

/* 编译器/平台类型宏定义 */
#if defined(_MSC_VER) && (_MSC_VER <= 1200)		/* <= VC6.0 */
	#define _NP_IS_VC6

#elif defined(_MSC_VER) && (_MSC_VER == 1500)	/* == VC2008 */
	#define _NP_IS_VC2008

#elif defined(_MSC_VER) && (_MSC_VER == 1600)	/* == VC2010 */
	#define _NP_IS_VC2010

#elif defined(_MSC_VER) && (_MSC_VER == 1700)	/* == VC2012 */
	#define _NP_IS_VC2012

#elif defined(__linux__)
	#define _NP_IS_LINUX

	#if defined(__mips__) || defined(__arm__)
		#define _NP_IS_MIPS_ARM
	#endif

#elif defined(IOS6)		/* IOS6, can't define __linux__ */
	#define _NP_IS_LINUX

#endif

#if defined(_M_IX86) || defined(__i386__) \
		|| defined(__x86_64) || defined(__x86_64__)
	#define _NP_IS_X86
#endif

#if defined(_M_IA64) || defined(_M_X64) || defined(_M_IX64)	\
		|| defined(__x86_64) || defined(__x86_64__)
	#define _NP_IS_X64
#endif

/* unicode */
#if defined(_UNICODE) || defined(UNICODE)
	#undef _UNICODE
	#undef UNICODE
	#define _UNICODE 1
	#define UNICODE 1
#endif

#if !defined(NO_WSTRING) && (defined(_NP_IS_MIPS_ARM) || defined(ANDROID_NDK))
	#define NO_WSTRING
#endif

#include "msc_opt.h"

/* 基本系统头文件 */
#if defined(WIN32)
	#ifndef _WINSOCK2API_
		#pragma warning(disable: 4115) /* 'xxx' : named type definition in parentheses) */
		#include <winsock2.h>
		#pragma comment(lib, "ws2_32.lib")
	#endif
	/* 加入WIN32环境的默认系统link库 */
	#pragma comment(lib, "kernel32.lib")
	#pragma comment(lib, "user32.lib")
	#pragma comment(lib, "shell32.lib")
	#pragma comment(lib, "gdi32.lib")
	#pragma comment(lib, "comdlg32.lib")
	#pragma comment(lib, "advapi32.lib")
	#pragma comment(lib, "psapi.lib")
#endif
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#if defined(_MSC_VER) && !defined(_NP_IS_VC6)
	#include <intrin.h>
#endif

/* 系统位数定义 */
#if defined(_MSC_VER)
	#if defined(_NP_IS_X64) || defined(_WIN64)
		#define NPR_SYS_BITS		64
	#elif defined(WIN32)
		#define NPR_SYS_BITS		32
	#elif defined(WIN16)
		#define NPR_SYS_BITS		16
	#endif
#else
	#if defined(__LP64__)			\
			|| defined(_LP64)		\
			|| defined(__64BIT__)	\
			|| defined(__x86_64)	\
			|| defined(__x86_64__)	\
			|| defined(_LP64)		\
			|| defined(__LP64__)	\
			|| defined(__amd64)		\
			|| defined(__amd64__)	\
			|| (64 == __WORDSIZE)
		#define NPR_SYS_BITS		64
	#else
		#define NPR_SYS_BITS		32
	#endif
#endif

/* RDTSC指令支持宏 */
#ifdef NP_RDTSC_SUPPORTED
	#error NP_RDTSC_SUPPORTED defined.
#endif
#if (defined(WIN32) || defined(_WIN64))
	#define NP_RDTSC_SUPPORTED
#elif defined(_NP_IS_LINUX)
	#if defined(_NP_IS_X86)
		#define NP_RDTSC_SUPPORTED
	#endif
#endif

/* 统一处理C/C++语言的引用运算符'&'、函数默认值 */
#ifdef __cplusplus
	#define REF_OPERATER		&
	#define DEFAULT_PARAM(v)	= v
#else
	#define REF_OPERATER
	#define DEFAULT_PARAM(v)
	#if defined(_MSC_VER)
		#define inline				static __inline
	#elif defined(_NP_IS_MIPS_ARM)
		#define inline				static inline
	#endif
#endif

/* const int SIZEOF(exp); */
#define SIZEOF(exp)				((const int)sizeof(exp))

/* 无限超时时间 */
#ifndef INFINITE
	#define INFINITE			0xFFFFFFFF
#endif

/* const int ARRAY_LEN(ar); */
#ifdef __cplusplus
	#if defined(_MSC_VER)
		#define ARRAY_LEN(ar)	(SIZEOF(ar) / SIZEOF(0[ar]))
	#else
		template<int BYTES_N>
			struct bytes_n_ARRAY_LEN { char b[BYTES_N]; };
		template<typename TYPE, int BYTES_N>
			bytes_n_ARRAY_LEN<BYTES_N> get_bytes_n_ARRAY_LEN(TYPE (&) [BYTES_N])
				{	bytes_n_ARRAY_LEN<BYTES_N> dummyVar; return dummyVar;	}
		#define ARRAY_LEN(x)	SIZEOF(get_bytes_n_ARRAY_LEN(x))
	#endif
#else
	#define ARRAY_LEN(ar)		(SIZEOF(ar) / SIZEOF(0[ar]))
#endif

/* s的成员m的相对偏移地址 */
#define offset_of(s, m)				((int)(INT_PTR)&(((s*)64)->m) - 64) /* 不使用0地址，避免某些编译器警告 */

/* 得到成员size */
#define member_size(c, m)			((int)sizeof(((c*)64)->m))

/* 得到包含类的对象指针，c为类名字，m为c的某个数据成员 */
#define container_pointer(c, m)		(c*)((INT_PTR)this - offset_of(c, m))

/* 初始化结构变量和数组 */
#define ZeroStru(stru)				memset(&(stru), 0, sizeof(stru))
#define ZeroArray(arr)				memset(arr, 0, sizeof(arr))

/* typeid raw_name
 * 例: std::string name = typeid(TYPE).raw_name_os();
 */
#ifdef __cplusplus
	#if defined(_MSC_VER)
		#define raw_name_os		raw_name
	#else
		#define raw_name_os		name
	#endif
#endif

/* 调试断点内嵌语句 */
#if defined(WIN32)
	#if 64==NPR_SYS_BITS
		#define asm_int_3_	__debugbreak()
	#else
		#define asm_int_3_	{	__asm int 3	}
	#endif
#elif defined(_NP_IS_LINUX)
	#define asm_int_3_	__asm__ __volatile__(".byte 0xcc")
#endif

#ifdef __cplusplus
	// 同COMPARE_OPERATOR_BY_LESS()，但不包括 operator==和operator!=
	#define COMPARE_OPERATOR_NO_EQUAL_BY_LESS(class_name)					\
		inline bool operator> (const class_name& l, const class_name& r)	\
			{	return r < l;	}											\
		inline bool operator<= (const class_name& l, const class_name& r)	\
			{	return !(r < l);	}										\
		inline bool operator>= (const class_name& l, const class_name& r)	\
			{	return !(l < r);	}


	// 已经实现bool operator<，利用bool operator<实现其它比较operator
	#define COMPARE_OPERATOR_BY_LESS(class_name)							\
		COMPARE_OPERATOR_NO_EQUAL_BY_LESS(class_name)						\
		inline bool operator!= (const class_name& l, const class_name& r)	\
			{	return l < r || r < l;	}									\
		inline bool operator== (const class_name& l, const class_name& r)	\
			{	return !(l != r);	}
#endif

/* linux epoll支持检测宏 */
#if defined(_NP_IS_LINUX) && !defined(IOS6)
	#if !(__GNUC_PATCHLEVEL__==2 && __GNUC__==3 && __GNUC_MINOR__==2)
		#define LINUX_EPOLL_SUPPORTED
	#endif
#endif

#endif	/* #ifndef _nprbase_h_ */
 
/* netposa_pubsdk 链接辅助头文件 */ 
#ifndef _pubsdk_link_statement_h_ 
#include "pubsdk_link_statement.h" 
#endif 
