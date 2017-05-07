// pubsdk_link_statement.h

#ifndef _pubsdk_link_statement_h_
#define _pubsdk_link_statement_h_

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#ifdef USE_SMART_EX_LIB
	#ifndef NPCRT_DECL
		#define NPCRT_DECL
	#endif
	#ifndef NPLOG_DECL
		#define NPLOG_DECL
	#endif
	#ifndef TCPS_DECL
		#define TCPS_DECL
	#endif
#endif

#if defined(WIN32)
	#if defined(USE_SMART_EX_LIB)
		#ifdef __AFX_H__
			#ifdef _DEBUG
				#if defined(USE_DEBUG_STATIC_SMART_LIB)
					#pragma comment(lib, "SmartMFC_Ex_d.lib")
				#else
					#pragma comment(lib, "SmartMFC_Ex_NoDI.lib")
				#endif
			#else
				#pragma comment(lib, "SmartMFC_Ex.lib")
			#endif
		#else
			#ifdef _DEBUG
				#if defined(USE_DEBUG_STATIC_SMART_LIB)
					#pragma comment(lib, "SmartSDK_Ex_d.lib")
				#else
					#pragma comment(lib, "SmartSDK_Ex_NoDI.lib")
				#endif
			#else
				#pragma comment(lib, "SmartSDK_Ex.lib")
			#endif
		#endif
	#else
		#ifdef __AFX_H__
			#ifdef _DEBUG
				#if defined(USE_DEBUG_STATIC_SMART_LIB)
					#pragma comment(lib, "SmartMFC_d.lib")
				#else
					#pragma comment(lib, "SmartMFC_NoDI.lib")
				#endif
			#else
				#pragma comment(lib, "SmartMFC.lib")
			#endif
		#else
			#ifdef _DEBUG
				#if defined(USE_DEBUG_STATIC_SMART_LIB)
					#pragma comment(lib, "SmartSDK_d.lib")
				#else
					#pragma comment(lib, "SmartSDK_NoDI.lib")
				#endif
			#else
				#pragma comment(lib, "SmartSDK.lib")
			#endif
		#endif

		#if (_MSC_VER <= 1200) // <= VC6.0
			#ifdef _DEBUG
				#pragma comment(lib, "npcrt_6d.lib")
				#pragma comment(lib, "nplog_6d.lib")
				#pragma comment(lib, "tcps_6d.lib")
			#else
				#pragma comment(lib, "npcrt_6.lib")
				#pragma comment(lib, "nplog_6.lib")
				#pragma comment(lib, "tcps_6.lib")
			#endif
		#elif (_MSC_VER == 1500) // VC2008
			#ifdef _DEBUG
				#pragma comment(lib, "npcrt_9d.lib")
				#pragma comment(lib, "nplog_9d.lib")
				#pragma comment(lib, "tcps_9d.lib")
			#else
				#pragma comment(lib, "npcrt_9.lib")
				#pragma comment(lib, "nplog_9.lib")
				#pragma comment(lib, "tcps_9.lib")
			#endif
		#elif (_MSC_VER == 1600) // VC2010
			#ifdef _DEBUG
				#pragma comment(lib, "npcrt_10d.lib")
				#pragma comment(lib, "nplog_10d.lib")
				#pragma comment(lib, "tcps_10d.lib")
			#else
				#pragma comment(lib, "npcrt_10.lib")
				#pragma comment(lib, "nplog_10.lib")
				#pragma comment(lib, "tcps_10.lib")
			#endif
		#elif (_MSC_VER == 1700) // VC2012
			#ifdef _DEBUG
				#pragma comment(lib, "npcrt_11d.lib")
				#pragma comment(lib, "nplog_11d.lib")
				#pragma comment(lib, "tcps_11d.lib")
			#else
				#pragma comment(lib, "npcrt_11.lib")
				#pragma comment(lib, "nplog_11.lib")
				#pragma comment(lib, "tcps_11.lib")
			#endif
		#else
			#error "Compiler not supported!"
		#endif
	#endif

	#if (_MSC_VER >= 1500) // >= VC2008
		#pragma comment(linker, "/nodefaultlib:LIBCMT.lib")
	#endif

	#if defined(_DEBUG) && (_MSC_VER <= 1200) // <= VC6.0
		#pragma comment(linker, "/ignore:4099")
	#endif
#endif

#endif	// #ifndef _pubsdk_link_statement_h_
