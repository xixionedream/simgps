#ifndef _GPSSERVER_H
#define _GPSSERVER_H
#include <unordered_map>
#include "tcp.h"
#include "config.h"
#include "thread.h"
#include "circular_queue.h"
#include "thread_pool.h"

#include "tide_persisitence.h"
#include "realpos_query.h"
#include "lastpos_query.h"
#include "realaround_query.h"
#include "hislocus_query.h"
#include "online_query.h"
#include "hisaround_query.h"
#include "data_writer.h"
#include "gpsid_writer.h"
#include "gpsid_query.h"
#include "file_manager.h"
#include "hiredis_proxy.h"
#include "basic_info.h"
#include "statistic.h"
#include "statistic_query.h"
#include "gpsinfo_query.h"

class CGpsServer
{
public:
	CGpsServer();
	virtual ~CGpsServer();	
	int GpsInit(const char*);
		
private:	
	int GpsTcp(const char* psAddr, const char* psPort, char cSocketFuncType);
	int BinaryServer(int iSocket);
	int LogToMem();	
	int WriteToMem(char*);
	int LastDataToMem();	
	int StatisticDataToMem();
	int GpsInfoToMem();
	static void* PersistThreadFunc(void*);
	static void* BinaryThreadFunc(void*);
	static void* clean_thread_func(void* pparam);
	static void* cpprest_thread_func(void* pparam);
	static void* subpub_thread_func(void* pparam);
	static void* statistic_thread_func(void* pParam);

public:
	static CConfig m_cConf;
	int m_iCurTimfd;
	char* m_psWriteBuff;
	//save to mem and file
	unordered_map<string, vector<GPSVALUE_S> >* m_pumGpsInfo;	
	unordered_map<string, vector<GPSVALUE_S> > m_umGpsInfo1;	
	unordered_map<string, vector<GPSVALUE_S> > m_umGpsInfo2;	
	GpsidWriter m_gpsid_writer;
public:
	int m_pLogfd;
	std::shared_ptr<hiredis_proxy> m_predis_proxy;
	CStatistic* m_pstatistic;
	FILEINFO_S m_stFile1;
	FILEINFO_S m_stFile2;
	FILEINFO_S m_stLastData;
	CTcp m_cBinaryRecv;	
	CTcp m_cJsonRecv;	
	unordered_map<string,GPSVALUE_S> m_lastdata;
	unordered_map<string,unsigned long long> m_statisticdata;
	unordered_map<string,unsigned int> m_bakData;
	//memery pool
	MEMPOOL_S m_stMemPool;
	static std::shared_ptr<BASIC_MODEL_INFO> m_pbasic_info;

	FILEINFO_S m_gpsInfoFile;
	unordered_map<string,string> m_gpsinfodata;
private:
	file_manager m_file_helper;
};

#endif

