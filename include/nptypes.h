/***
 *nptypes.h - Basic data types
 *    Copyright (c) 2000-2012, NetPosa Technologies,Ltd. All rights reserved.
 *Purpose:
 **************************************************************************************/

#ifndef _nptypes_h_
#define _nptypes_h_

#if _MSC_VER > 1000
	#pragma once
#endif

#include "nprbase.h"
#if defined(WIN32)
	#include <tchar.h>
#elif defined(_NP_IS_LINUX)
	#include <errno.h>
	#include <unistd.h>
	#include <netinet/in.h>
	#include <pthread.h>
	#include <sys/time.h>
	#include <sys/times.h>
#endif
#include "npbasetypes.h"
#if defined(__cplusplus)
	#include <string>
#endif

#if !defined(_T)
	#if defined(_UNICODE) || defined(UNICODE)
		#define _T(s)			L ## s
	#else
		#define _T(s)			s
	#endif
#endif

/* SOCKET相关 */
#ifndef INVALID_SOCKET
	#define INVALID_SOCKET	-1
#endif
#ifndef SOCKET_ERROR
	#define SOCKET_ERROR	-1
#endif

#undef INADDR_NONE
#define INADDR_NONE				(DWORD)0xffffffff

#undef INADDR_ANY
#define INADDR_ANY				(DWORD)0x00000000

#undef ADDR_ANY
#define ADDR_ANY				INADDR_ANY

#undef INADDR_LOOPBACK
#define INADDR_LOOPBACK			(DWORD)0x7f000001

#define INADDR_LOOPBACK_NET		htonl(INADDR_LOOPBACK)

#undef INADDR_BROADCAST
#define INADDR_BROADCAST		(DWORD)0xffffffff

/* bool SockValid(SOCKET sock); */
#define SockValid(sock)			((INT_PTR)(sock) >= 0)

/* bool FDValid(FD_TYPE fd); */
#define FDValid(fd)				((INT_PTR)(fd) >= 0)

/* 离散SOCKET读写结构类型 */
typedef struct
{
	void* buf;
	INT_PTR len;
} SockReadBuf;
typedef struct
{
	const void* data;
	INT_PTR len;
} SockWriteBuf;

typedef DWORD COLORREF;
#ifndef RGB
	#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g))<<8)) | (((DWORD)(BYTE)(b))<<16)))
#endif

#ifndef MAX_PATH
	#define MAX_PATH	260
#endif
typedef char PathNameA[MAX_PATH];
typedef WCHAR PathNameW[MAX_PATH];
typedef TCHAR PathName[MAX_PATH];

/* 描述时间毫秒数(64位)，没有时区之分，即没有系统时间和本地时间的区分 */
/* 绝对时间：[MIN_UTC_MSEL, MAX_UTC_MSEL] */
/* 相对时间：[-MAX_UTC_MSEL, MAX_UTC_MSEL] */
typedef INT64 LTMSEL;

/* 最小UTC LTMSEL时间，GMT: 1970/01/01 00:00:00; BEIJING: 1970/01/01 08:00:00 */
#define MIN_UTC_MSEL			((LTMSEL)0)
/* 最大UTC LTMSEL时间，GMT: 2038/01/18 16:00:00; BEIJING: 2038/01/19 00:00:00 */
#define MAX_UTC_MSEL			((LTMSEL)0x7fff6200 * 1000)

/* 非法UTC LTMSEL时间 */
#define INVALID_UTC_MSEL		((LTMSEL)-1)

#if defined(WIN32)
	typedef HANDLE			OSThread;
#elif defined(_NP_IS_LINUX)
	typedef pthread_t		OSThread;
#endif
#ifndef OSTHREADID_DEFINED
	#define OSTHREADID_DEFINED
	#if defined(WIN32)
		typedef DWORD		OSThreadID;
	#elif defined(_NP_IS_LINUX)
		typedef pthread_t	OSThreadID;
	#endif
#endif
typedef void*				OSThreadRealID;

#define INVALID_OSTHREAD			((OSThread)0)
#define INVALID_OSTHREADID			((OSThreadID)0)
#define INVALID_OSTHREAD_REAL_ID	((OSThreadRealID)0)

#if defined(WIN32)
	typedef DWORD			OSProcessID;
#else
	typedef int				OSProcessID;
#endif
/* 注：必须和INVALID_OSTHREADID相等，在'PIDT.h'中需要统一处理它们， */
/*     WIN32的idle线程ID也被当做无效PID */
#define INVALID_OSPROCESSID			((OSProcessID)0)

#if defined(WIN32)
	typedef HANDLE OSPollHandle;	/* 参考class COSPoll */
	#define INVALID_OSPOLL_HANDLE	((OSPollHandle)NULL)
	#define OSPollHandleValid(hd)	(INVALID_OSPOLL_HANDLE != (hd))
#elif defined(_NP_IS_LINUX)
	typedef int OSPollHandle;
	#define INVALID_OSPOLL_HANDLE	((OSPollHandle)-1)
	#define OSPollHandleValid(hd)	((hd) >= 0)
#endif

#ifdef __cplusplus
	struct NullClass {};
#endif

/* DWORD MAKEFOURCC(ch0, ch1, ch2, ch3); */
#undef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)		\
		((DWORD)(BYTE)(ch0)					\
		| ((DWORD)(BYTE)(ch1) << 8)			\
		| ((DWORD)(BYTE)(ch2) << 16)		\
		| ((DWORD)(BYTE)(ch3) << 24))

/* 字序环境判断 */
BOOL IsSmallEndian();

void MakeTU32(BYTE* dataPtr, DWORD v);
DWORD GetTU32(const BYTE* dataPtr);

/* 得到调用此函数的线程ID */
/* 注意：linux环境需要长时间记录一个线程的ID，最好用OSThreadRealID() */
OSThreadID OSThreadSelf();

/* 得到真实线程ID，linux下的线程OSThreadSelf()可能并不是真实ID */
OSThreadRealID OSThreadIDToRealID(OSThreadID id);

#if defined(WIN32)
	/* 获取Windows版本信息 */
	typedef enum WindowsVersion__
	{
		Invalid_WinVer = -1,
		Unknown_WinVer = 0,
		Win32s,
		Windows_95_98,
		Windows_NT,
		Windows_2000,
		Windows_XP,
		Windows_Longhorn,
		Windows_Vista,
		Windows_7,
		Windows_8,
	} WindowsVersion;
	WindowsVersion GetWindowsVersion();
	const char* GetWindowsVersionText();
#endif

#if defined(_NP_IS_LINUX)
	/* 与Windows的Sleep()完全等价 */
	void LinuxSleep(DWORD ms);
#endif

/* 获取当前编译器名 */
const char* GetCompilerName();

/* 获取当前模块的编译器二进制类型 */
const char* GetCompileBinaryType();

/* 判断当前进程是否为控制台类型程序 */
BOOL IsConsoleApplication();

#if defined(WIN32)
	/* 判断当前进程是否处于被调试状态 */
	BOOL IsByDebugging();
#endif

#ifdef __cplusplus
	/// 线程池任务基类
	struct NPBaseTask
	{
		NPBaseTask* m_nextPoolTask; ///< 连接到下一个任务，线程池需要，见"taskpool.h"
		virtual ~NPBaseTask() {}
		/// 线程池调度到这个任务时执行
		virtual void OnPoolTask() = 0;
	};

	/// 基础timer相关类型
	struct OSTimerID__ { void* dummy; };
	typedef OSTimerID__* NPTimerID;
	typedef void (*NPTimerCallback)(void* param, NPTimerID id);
	struct ITimerCall
	{
		virtual ~ITimerCall() {}
		/// timer调用时间到时执行
		virtual void OnTimer(NPTimerID id) = 0;
	};
#endif /* #ifdef __cplusplus */

#if defined(_MSC_VER) && (32==NPR_SYS_BITS && !defined(GetWindowLongPtr))
	#define GetWindowLongPtrA	GetWindowLongA
	#define GetWindowLongPtrW	GetWindowLongW
	#if defined(_UNICODE) || defined(UNICODE)
		#define GetWindowLongPtr	GetWindowLongPtrW
	#else
		#define GetWindowLongPtr	GetWindowLongPtrA
	#endif

	#define SetWindowLongPtrA	SetWindowLongA
	#define SetWindowLongPtrW	SetWindowLongW
	#if defined(_UNICODE) || defined(UNICODE)
		#define SetWindowLongPtr	SetWindowLongPtrW
	#else
		#define SetWindowLongPtr	SetWindowLongPtrA
	#endif
#endif

#ifndef EXTERN_C
	#ifdef __cplusplus
		#define EXTERN_C	extern "C"
	#else
		#define EXTERN_C	extern
	#endif
#endif

/* 基本内存分配函数指针类型 */
typedef void* (*NPAllocFuncPtr)(INT_PTR);
typedef void (*NPFreeFuncPtr)(void*);

#ifdef __cplusplus
	template<typename CHAR_TYPE, int chars /*= 32*/>
	struct StackTempCharBuf_ { CHAR_TYPE ch[chars]; };
#endif

/* tstring */
#ifdef __cplusplus
	#if defined(_UNICODE) || defined(UNICODE)
		#ifndef NO_WSTRING
			typedef std::wstring tstring;
		#endif
	#else
		typedef std::string tstring;
	#endif
	#define empty_tstring	tstring()
#endif

/* struct IPP */
#if defined(_MSC_VER)
	#pragma warning(disable: 4201)	/* nonstandard extension used : nameless struct/union */
#endif
#pragma pack(push, 1)
/* 用于统一记录IP和port，方便统一使用、管理、调试 */
struct IPP
{
	union
	{
		struct
		{
			union
			{
				DWORD ip_; /* 如果不作特别说明，ip_使用网络字节序 */
				struct
				{
					BYTE bip0_, bip1_, bip2_, bip3_;
				};
			};
			union
			{
				INT32 port_; /* 如果不作特别说明，port_使用本地字节序 */
				BYTE bport_[sizeof(INT32)/sizeof(BYTE)];
			};
		};
		INT64 i64_;
	};

#ifdef __cplusplus
	IPP() : ip_(INADDR_NONE), port_(-1) {}
	IPP(DWORD ip, int port) : ip_(ip), port_((INT32)port) {}

	IPP(const char* ippStr, int default_port); /* 实现见"ipcvt.cpp" */
	IPP(const WCHAR* ippStr, int default_port);

	bool IsValid() const
		{	return INADDR_NONE!=ip_ && port_>=0;	}

	bool operator< (const IPP& r) const
		{	return i64_ < r.i64_;	}
	bool operator<= (const IPP& r) const
		{	return i64_ <= r.i64_;	}
	bool operator> (const IPP& r) const
		{	return i64_ > r.i64_;	}
	bool operator>= (const IPP& r) const
		{	return i64_ >= r.i64_;	}
	bool operator== (const IPP& r) const
		{	return i64_ == r.i64_;	}
	bool operator!= (const IPP& r) const
		{	return i64_ != r.i64_;	}
#endif	/* #ifdef __cplusplus */
};
#pragma pack(pop)

#ifdef __cplusplus
	#if defined(_MSC_VER)
		static INT64 volatile s_INVALID_IPP_ = -1;
		#define INVALID_IPP		((const IPP&)s_INVALID_IPP_)
	#else
		#define INVALID_IPP		IPP()
	#endif
#else
	typedef struct IPP IPP;
	static INT64 volatile s_INVALID_IPP_ = -1;
	#define INVALID_IPP		(*(const IPP*)&s_INVALID_IPP_)
#endif

/* YUV420帧 */
struct YuvFrame
{
	const BYTE* y;
	const BYTE* u;
	const BYTE* v;
	void* alignDummy; /* 结构体对齐占位成员 */
	INT32 w;
	INT32 h;
	INT32 stride_y;
	INT32 stride_uv;
/*
#ifdef __cplusplus
	YuvFrame()
	{
		y = u = v = NULL;
		alignDummy = NULL;
		w = h = stride_y = stride_uv = 0;
	}
#endif*/
};

/* 使用OS_FILENAME_MATCH_CASE以解决平台路径名大小写区分差异 */
#if defined(WIN32)
	#define OS_FILENAME_MATCH_CASE	false
#else
	#define OS_FILENAME_MATCH_CASE	true
#endif

/* 默认trim字符集 */
#define STRDK_TRIM_CHARS_A			"\x20\r\n\t"
#define STRDK_TRIM_CHARS_W			L"\x20\r\n\t"
#define STRDK_TRIM_CHARS			_T("\x20\r\n\t")

/* 默认路径保留字符集 */
#define PATH_FILE_INVALID_CHARS_A	"<>:|/\\\""
#define PATH_FILE_INVALID_CHARS_W	L"<>:|/\\\""
#define PATH_FILE_INVALID_CHARS		_T("<>:|/\\\"")

/* 全局名字检查（文件名、Windows命名互斥、命名信号量、命名事件、命名页面映射、文件名等等） */
BOOL OSGlobalNameCheck(LPCTSTR name);

#ifdef __cplusplus
	/// 整数流水序列号生成器
	template<typename LockBase /*= CLocker, CNullLocker...*/,
			typename IntType /*= INT32, INT64, INT_PTR ...*/
			>
	class IntSerialKeyGenerater : private LockBase
	{
	private:
		IntSerialKeyGenerater(const IntSerialKeyGenerater&);
		IntSerialKeyGenerater& operator= (const IntSerialKeyGenerater&);

		IntType nextKey_;

	public:
		explicit IntSerialKeyGenerater(DWORD spinCount = 0)
			: LockBase(spinCount)
			, nextKey_(1)
			{}

		// 获取下一个流水序列号（不包括0，0一般可作为无效流水序列号用）
		IntType GetNext()
		{
			this->LockBase::Lock();
			IntType key = nextKey_;
			++nextKey_;
			if(0 == nextKey_)
				nextKey_ = 1;
			this->LockBase::Unlock();
			return key;
		}
	};
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++ 内联函数实现 +++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
union endian_check_union_
{
	WORD w;
	BYTE ch[2];
};

inline BOOL IsSmallEndian()
{
	union endian_check_union_ check;
	check.w = 0x1234;
	return (0x34 == check.ch[0]);
}

inline void MakeTU32(BYTE* dataPtr, DWORD v)
{
	BYTE* vp = (BYTE*)&v;
	if(IsSmallEndian())
	{
		dataPtr[0] = vp[0];
		dataPtr[1] = vp[1];
		dataPtr[2] = vp[2];
		dataPtr[3] = vp[3];
	}
	else
	{
		dataPtr[0] = vp[3];
		dataPtr[1] = vp[2];
		dataPtr[2] = vp[1];
		dataPtr[3] = vp[0];
	}
}

inline DWORD GetTU32(const BYTE* dataPtr)
{
	DWORD v=0;
	BYTE* vp = (BYTE*)&v;
	if(IsSmallEndian())
	{
		vp[0] = dataPtr[0];
		vp[1] = dataPtr[1];
		vp[2] = dataPtr[2];
		vp[3] = dataPtr[3];
	}
	else
	{
		vp[0] = dataPtr[3];
		vp[1] = dataPtr[2];
		vp[2] = dataPtr[1];
		vp[3] = dataPtr[0];
	}
	return v;
}

inline OSThreadID OSThreadSelf()
{
#if defined(WIN32)
	return GetCurrentThreadId();
#elif defined(_NP_IS_LINUX)
	return pthread_self();
#endif
}

inline OSThreadRealID OSThreadIDToRealID(OSThreadID id)
{
#if defined(_NP_IS_LINUX)
	#if __GNUC__>=4 && 32==NPR_SYS_BITS
		return (OSThreadRealID)((DWORD*)id)[18];
	#else
		return (OSThreadRealID)id;
	#endif
#else
	return (OSThreadRealID)(INT_PTR)id;
#endif
}

#if defined(WIN32)
	inline WindowsVersion GetWindowsVersion()
	{
		static WindowsVersion sWinVer = Invalid_WinVer;
		if(Invalid_WinVer != sWinVer)
			return sWinVer;

	#if defined(_WINBASE_)
		{
			DWORD ver;
			OSVERSIONINFOEX osvi;
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			if(!GetVersionEx((OSVERSIONINFO*)&osvi))
			{
				osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
				if(!GetVersionEx((OSVERSIONINFO*)&osvi))
					return (sWinVer=Unknown_WinVer);
			}

		#if defined(_MSC_VER) && (_MSC_VER >= 1500) /* >=VC2008 */
			if(VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
			{
				if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
				{
					if(osvi.wProductType == VER_NT_WORKSTATION)
						return (sWinVer=Windows_Vista);
					else
						return (sWinVer=Windows_Longhorn);
				}
				else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
					return (sWinVer=Windows_8);
				else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
					return (sWinVer=Windows_7);
				else if(osvi.dwMajorVersion == 5)
				{
					if(osvi.dwMinorVersion == 0)
						return (sWinVer=Windows_2000);
					else
						return (sWinVer=Windows_XP);
				}
				return (sWinVer=Windows_NT);
			}
		#endif

			ver = GetVersion();

			if(ver < 0x80000000)
			{
				if((WORD)ver == 0x0206)
					return (sWinVer=Windows_8);
				else if((WORD)ver == 0x0106)
					return (sWinVer=Windows_7);
				else if((WORD)ver == 0x0006)
					return (sWinVer=Windows_Vista); /* 忽略 Windows_Longhorn */
				else if(osvi.dwMajorVersion == 5)
				{
					if(osvi.dwMinorVersion == 0)
						return (sWinVer=Windows_2000);
					else
						return (sWinVer=Windows_XP);
				}
				return (sWinVer=Windows_NT);
			}
			else if((DWORD)(LOBYTE(LOWORD(ver))) < 4)
				return (sWinVer=Win32s);
			else
				return (sWinVer=Windows_95_98);
		}
	#else
		ASSERT(FALSE);
		return (sWinVer=Unknown_WinVer);
	#endif
	}

	inline const char* GetWindowsVersionText()
	{
		WindowsVersion ver = GetWindowsVersion();
		const char* verTexts[] =
		{
			"Unknown_WinVer",
			"Win32s",
			"Windows_95/98",
			"Windows_NT",
			"Windows_2000",
			"Windows_XP",
			"Windows_Vista",
			"Windows_Longhorn",
			"Windows_7",
			"Windows_8",
		};
		if(0<=ver && ver<(int)(sizeof(verTexts)/sizeof(verTexts[0])))
			return verTexts[ver];
		return "Invalid_WinVer";
	}
#endif

#if defined(_NP_IS_LINUX)
	inline DWORD LinuxGetTickCount()
	{
		static DWORD clocks_sec = 0; /* 操作系统时间片频率（时间片数/每秒） */
		clock_t clocks;
		struct tms dummy;
		if(0 == clocks_sec)
			clocks_sec = sysconf(_SC_CLK_TCK); /* 常见系统版本都为100 */
		clocks = times(&dummy);		/* glibc有bug，不能传NULL */
		if(-1 == clocks)
			clocks = -1 * errno;	/* mips和arms均需要这样，系统bug？？？ */
		if(100 == clocks_sec)
			return (DWORD)(clocks * 10);
		else
			return (DWORD)((INT64)clocks * 1000 / clocks_sec);
		/* 某些平台可能需要下面的旧的实现方式，此法效率要低一些
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (DWORD)((LTMSEL)tv.tv_sec*1000 + (tv.tv_usec+500)/1000);
		*/
	}

	inline void LinuxSleep(DWORD ms)
	{
		useconds_t const max_usleep_usec = 1000*1000;
		if(0 == ms)
		{
	#if !defined(_NP_IS_MIPS_ARM) && !defined(IOS6)
			pthread_yield();
	#endif
			sched_yield();
		}
		else if(1 == ms)
		{
			usleep(1000);
		}
		else if(INFINITE == ms)
		{
			while(1)
				usleep(max_usleep_usec);
		}
		else if(ms <= max_usleep_usec/1000)
		{
			usleep(ms*1000);
		}
		else
		{
			INT64 usec = (INT64)ms * 1000;
			while(usec > 0)
			{
				/* usec2 = min(usec, max_usleep_usec); */
				useconds_t usec2 = (usec < (INT64)max_usleep_usec ? (useconds_t)usec : max_usleep_usec);
				usleep(usec2);
				usec -= usec2;
			}
		}
	}
#endif

inline const char* GetCompilerName()
{
	const char* compiler =
#if defined(_NP_IS_VC6)
				"VC6"
#elif defined(_NP_IS_VC2008)
				"VC2008"
#elif defined(_NP_IS_VC2010)
				"VC2010"
#elif defined(_NP_IS_VC2012)
				"VC2012"
#elif defined(_NP_IS_LINUX)
	#ifdef _NP_IS_MIPS_ARM
				"MIPS_ARM_"
	#endif
				"GCCx.x"
#else
				"UNKNOWN_COMPILER"
#endif
				;

#if defined(_NP_IS_LINUX)
	{
		/* 改写为GCC的正确版本号 */
		static int sInited = 0;
		static char ch[32];
		if(0 == sInited)
		{
			int l = (int)strlen(compiler);
			strcpy(ch, compiler);
			ch[l-3] = (char)('0' + __GNUC__);
			ch[l-1] = (char)('0' + __GNUC_MINOR__);
			sInited = 1;
		}
		return ch;
	}
#else
	return compiler;
#endif
}

inline const char* GetCompileBinaryType()
{
	const char* bin_type =
#ifdef _DEBUG
				"DEBUG"
#else
				"RELEASE"
#endif

#if 64==NPR_SYS_BITS
				"_x64"
#endif

#if defined(_UNICODE) || defined(UNICODE)
				"_UNICODE"
#endif
				;
	return bin_type;
}

inline BOOL IsConsoleApplication()
{
#if defined(WIN32)
	static BOOL inited = false;
	static BOOL is = false;
	if(!inited)
	{
		typedef HWND (WINAPI* NFGetConsoleWindow)();
		NFGetConsoleWindow fnGetConsoleWindow =
				(NFGetConsoleWindow)GetProcAddress(
										GetModuleHandle(_T("kernel32.dll")),
										"GetConsoleWindow"
										);
		is = (fnGetConsoleWindow && fnGetConsoleWindow());
		inited = true;
	}
	return is;
#else
	return true;
#endif
}

#if defined(WIN32)
	inline BOOL IsByDebugging()
	{
		typedef BOOL (WINAPI* FNIsDebuggerPresent)();
		static FNIsDebuggerPresent fnIsDebuggerPresent = NULL;
		if(NULL == fnIsDebuggerPresent)
		{
			HMODULE mod = GetModuleHandle(_T("kernel32.dll"));
			fnIsDebuggerPresent = (FNIsDebuggerPresent)GetProcAddress(mod, "IsDebuggerPresent");
		}
		return (fnIsDebuggerPresent && fnIsDebuggerPresent());
	}
#endif

inline BOOL OSGlobalNameCheck(LPCTSTR name)
{
	LPCTSTR tp = name;
	while(*tp)
	{
		LPCTSTR pc = PATH_FILE_INVALID_CHARS;
		while(*pc)
		{
			if(*tp == *pc)
				return false; /* 发现非法字符 */
			++pc;
		}
		++tp;
		if(tp-name >= MAX_PATH-32)
			return false;
	}

	return true;
}

#endif /* #ifndef _nptypes_h_ */
