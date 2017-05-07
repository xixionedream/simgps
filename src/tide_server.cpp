

#include "tide_server.h"
#include "basic_http.h"
#include <iostream>

#include "gdal.h"
#include "ogrsf_frmts.h"
bool g_isstart_pubsub = false;
bool g_isstart_statistic = false;
char g_sLocusPath[64];
char g_sAroundPath[64];
char g_sLastPath[64];
char g_sGpsidPath[64];
char g_sMaxTime[15];
char g_sLastDataFile[10]="lastdata";
char g_sStatisticEachFile[10]="eachdate";
char g_sStatisticTotalFile[10]="totaldata";
char g_sGpsInfoFile[15]="gpsinfodata";
extern GPSINFO_S g_gpsInfoColumn;

//review zhushi

CConfig CGpsServer::m_cConf;
std::shared_ptr<BASIC_MODEL_INFO> CGpsServer::m_pbasic_info = std::shared_ptr<BASIC_MODEL_INFO>(new BASIC_MODEL_INFO());

CGpsServer::CGpsServer()
{
	m_pLogfd = -1;	
}

CGpsServer::~CGpsServer()
{
	m_pLogfd = -1;
	if (m_stFile1.m_fd) 
	{
		close(m_stFile1.m_fd);
		m_stFile1.m_fd = -1;
	}
	if (m_stFile2.m_fd) 
	{
		close(m_stFile2.m_fd);
		m_stFile2.m_fd = -1;
	}
	if (m_iCurTimfd) 
	{
		close(m_iCurTimfd);
		m_iCurTimfd = -1;
	}
	
	m_gpsid_writer.CloseFile();
}

int CGpsServer::BinaryServer(int iSocket)
{
	m_cBinaryRecv.SetSocket(iSocket);
	TIDECLIENT_S* pstTideClientInfo = new TIDECLIENT_S;
	pstTideClientInfo->m_iSocket = iSocket;
	pstTideClientInfo->m_psPacket = m_psWriteBuff;
	//memset(m_psWriteBuff, 0, BUFFER_SIZE);
	//NPLOG_DEBUG("m_cCircularQueue.size=%d\n", m_cCircularQueue.size());
	if (m_cBinaryRecv.TcpRecv(pstTideClientInfo->m_psPacket, PRO_HEAD_LEN) <= 0) 
	{
		delete pstTideClientInfo;
		return -1;			
	}
	*(pstTideClientInfo->m_psPacket + PRO_HEAD_LEN) = 0;
	int iValLen = 0;	
	iValLen = ntohl(*(int*)(pstTideClientInfo->m_psPacket + 6));

	if ((m_cBinaryRecv.TcpRecv(pstTideClientInfo->m_psPacket + PRO_HEAD_LEN, iValLen)) > 0) 
	{
		//write mem and Persisitence
		char sPerMaxTime[14];
		int iMaxTimeLen = strlen(g_sMaxTime);
		memcpy(sPerMaxTime, g_sMaxTime, iMaxTimeLen);
		sPerMaxTime[iMaxTimeLen] = 0;
		*(pstTideClientInfo->m_psPacket + PRO_HEAD_LEN + iValLen) = 0;
		CWriteData cWriteData(&m_pLogfd, pstTideClientInfo, &m_pumGpsInfo, g_sMaxTime,&m_gpsid_writer, m_predis_proxy.get(),m_pstatistic);
		if (cWriteData.WriteDataCore() < 0) 
		{
			return -1;
		}
		
		iMaxTimeLen = strlen(g_sMaxTime);
		if (strncmp(sPerMaxTime, g_sMaxTime, iMaxTimeLen) != 0) 
		{	
			lseek(m_iCurTimfd, 0, SEEK_SET);	
			write(m_iCurTimfd, g_sMaxTime, iMaxTimeLen);
		}
	}
	else 
	{
		delete pstTideClientInfo;
		NP_LOG(np_log::NP_ERROR, "recv err:%s", strerror(errno));
		return -1;	
	}
	delete pstTideClientInfo;
	return 0;
}

int CGpsServer::GpsInit(const char* psConfigFileName)
{

	RegisterOGRTAB();


	//IGN SIGPIPE
	signal(SIGPIPE, SIG_IGN);	
	//create write buffer
	m_psWriteBuff = (char*)malloc(BUFFER_SIZE);
	if (m_psWriteBuff == NULL) 
	{
		NP_LOG(np_log::NP_ERROR, "malloc err.");
		return -1;	
	}
	//create memery pool
	pthread_mutex_t stMemPoolLock;
	pthread_cond_t stMemPoolCond;
	pthread_mutex_init(&stMemPoolLock, NULL);			
	pthread_cond_init(&stMemPoolCond, NULL);			
	CCircularQueue<char*> cCircularQueue;
	cCircularQueue.init(MEM_SIZE);
	char* psMem = NULL;
	for (unsigned short usNum = 0; usNum < MEM_SIZE; ++usNum) 
	{
		psMem = (char*)malloc(MEM_BUFFER);
		cCircularQueue.push(psMem);
	}
	
	m_stMemPool.m_pcCircularQueue = &cCircularQueue;	
	m_stMemPool.m_pMemPoolLock = &stMemPoolLock;
	m_stMemPool.m_pMemPoolCond = &stMemPoolCond;

	char save_file_days[8];
	char statistic_time[8];
	char statistic_cycleNum[8];
	m_cConf.ConfGetSecVal("PERSISIT","AROUND_PATH",g_sAroundPath);
	m_cConf.ConfGetSecVal("PERSISIT","LOCUS_PATH",g_sLocusPath);
	m_cConf.ConfGetSecVal("PERSISIT","LAST_PATH",g_sLastPath);
	m_cConf.ConfGetSecVal("PERSISIT","GPSID_PATH",g_sGpsidPath);
	m_cConf.ConfGetSecVal("STATISTIC","STATISTIC_TIME",statistic_time);
	m_cConf.ConfGetSecVal("STATISTIC","STATISTIC_CYCLENUM",statistic_cycleNum);
	m_cConf.ConfGetSecVal("OTHER", "DATA_SAVE_DAYS", save_file_days);


	m_file_helper.set_save_file_days(atoi(save_file_days));
	//gpsid file data to mem
	m_gpsid_writer.Init(g_sGpsidPath);
	//parse redis config
	char is_start_redis[4];
	m_cConf.ConfGetSecVal("REDIS","IS_START",is_start_redis);
	if(atoi(is_start_redis) != 0)
	{
		g_isstart_pubsub = true;
	}
	//parse statistic config
	char is_start_statistic[4];
	m_cConf.ConfGetSecVal("STATISTIC","IS_START",is_start_statistic);
	if(atoi(is_start_statistic) != 0)
	{
		g_isstart_statistic = true;
	}
	//templog file
	if (LogToMem() < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "save to mem err.");
		return -1;		
	}
	//lastdata file
	if (LastDataToMem() < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "lastdata save to mem err.");
		return -1;		
	}

	//statisticdata file
	if (StatisticDataToMem() < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "statisticdata save to mem err.");
		return -1;		
	}

	//gpsinfo file
	if (GpsInfoToMem() < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "gpsinfo data save to mem err.");
		return -1;		
	}
	
	//Persisitence to file  thread
	CPersisitence cPersisitence(&m_pLogfd, &m_stFile1, &m_stFile2, &m_cConf, &m_pumGpsInfo, &m_umGpsInfo1, &m_umGpsInfo2,&m_lastdata,&m_statisticdata);
	CThread cPersisitenceThead;
	cPersisitenceThead.CThreadSetStartRoutine(PersistThreadFunc);	//f
	cPersisitenceThead.CThreadCreate(&cPersisitence);	

	//sleep(1);
	//read maxtime
	memset(g_sMaxTime, 0, sizeof(g_sMaxTime));
	memset(g_sLocusPath, 0, sizeof(g_sLocusPath));
	m_cConf.ConfGetSecVal("PERSISIT", "LOCUS_PATH", g_sLocusPath);
	char sCurTime[256];
	memset(sCurTime, 0, sizeof(sCurTime));
	sprintf(sCurTime, "%sCurTime", g_sLocusPath);
	m_iCurTimfd = open(sCurTime, O_RDWR, 0666);
	if (m_iCurTimfd  > 0) 
	{
		read(m_iCurTimfd, g_sMaxTime, 10);		
		close(m_iCurTimfd);
	}
	m_iCurTimfd = -1;

	m_iCurTimfd = open(sCurTime, O_CREAT | O_RDWR, 0666);
	if (m_iCurTimfd < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "open file[%s] err:%s", sCurTime, strerror(errno));
		return -1;
	}	

	setlocale(LC_ALL, "zh_CN.utf8");

	//statistic service
	CStatistic cStatistic;
	if(false ==cStatistic.init_server(atoi(statistic_time),atoi(statistic_cycleNum),&m_statisticdata,&m_bakData))
	{
		NP_LOG(np_log::NP_ERROR, "init statistic error");
		return -1;
	}
	CThread statistic_thread;
	statistic_thread.CThreadSetStartRoutine(statistic_thread_func);
	statistic_thread.CThreadCreate(&cStatistic);
	m_pstatistic = &cStatistic;
	
	//delete out date gpsdata thread
	CThread clean_thread;
	clean_thread.CThreadSetStartRoutine(clean_thread_func);
	clean_thread.CThreadCreate(this);

	//rest service
	CThread ccprest_thread;
	ccprest_thread.CThreadSetStartRoutine(cpprest_thread_func);
	ccprest_thread.CThreadCreate(this);
	
	//Binary write server
	CThread cBinaryThread;
	cBinaryThread.CThreadSetStartRoutine(BinaryThreadFunc);
	cBinaryThread.CThreadCreate(this);	

	//redis pub/sub server thread
	CThread pubsub_thread;
	pubsub_thread.CThreadSetStartRoutine(subpub_thread_func);
	pubsub_thread.CThreadCreate(this);
	
	cPersisitenceThead.CThreadJoin();
	clean_thread.CThreadJoin();
	pubsub_thread.CThreadJoin();
	ccprest_thread.CThreadJoin();
	cBinaryThread.CThreadJoin();	
	pthread_mutex_destroy(&stMemPoolLock);			
	pthread_cond_destroy(&stMemPoolCond);	
	for (unsigned short usNum = 0; usNum < MEM_SIZE; ++usNum) 
	{
		psMem = cCircularQueue.front();
		if (psMem) 
		{
			free(psMem);
		}
		cCircularQueue.pop();
	}
	free(m_psWriteBuff);
	m_cConf.ConfClose();
	return 0;
}

int CGpsServer::GpsTcp(const char* psAddr, const char* psPort, char cSocketFuncType)
{
	CTcp cTcp;	
	if (cTcp.TcpInit(psAddr, atoi(psPort)) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"ctcp.TcpInit err:[%s]", strerror(errno));
		return -1;
	}
	//cTcp.SetBuff(64 * 1024);
	int iEpollfd = epoll_create(EPOLL_NUM);
	if (iEpollfd < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"epoll_create err:[%s]", strerror(errno));
		return -1;
	}

	struct epoll_event stEpollEvent;
	int iSocket = cTcp.GetSocket();
	stEpollEvent.events = EPOLLIN;
	stEpollEvent.data.fd = iSocket;

	if (epoll_ctl(iEpollfd, EPOLL_CTL_ADD, iSocket, &stEpollEvent) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"epoll_ctl EPOLL_CTL_ADD err:[%s]", strerror(errno));
		return -1;
	}

	struct epoll_event stEv, stEvs[WAIT_NUMS];
	int iCount = 0;
	int iNewfd = -1;
	while (1) 
	{
		memset(stEvs, 0, sizeof(stEvs));
		int iEpollNum = epoll_wait(iEpollfd, stEvs, WAIT_NUMS, -1);
		if (iEpollNum < 0) 
		{
			NP_LOG(np_log::NP_ERROR,"epoll_wait err:[%s]", strerror(errno));
			return -1;
		} else if (iEpollNum == 0) 
		{
			np_log::NP_STDOUT("epoll_wait time out.");
			continue;
		}
		for (iCount = 0; iCount < iEpollNum; ++iCount) 
		{
			if (stEvs[iCount].data.fd == iSocket) 
			{
				iNewfd = cTcp.TcpAccept();
				if (iNewfd < 0) 
				{
					NP_LOG(np_log::NP_ERROR,"ctcp.accept err:[%s]", strerror(errno));
					return -1;
				}
				
				//if (fcntl(iNewfd, F_SETFL, fcntl(iNewfd, F_GETFL, 0) | O_NONBLOCK) >= 0) {
				CSock cSocket;
				cSocket.SetSocket(iNewfd);
				if (cSocket.SetTimeout(3, 0) == 0) 
				{	
					memset(&stEv, 0, sizeof(stEv));
					stEv.events = EPOLLIN | EPOLLET;    //
					stEv.data.fd = iNewfd;

					if (epoll_ctl(iEpollfd, EPOLL_CTL_ADD, iNewfd, &stEv) < 0) 
					{
						NP_LOG(np_log::NP_ERROR,"epoll_ctl EPOLL_CTL_ADD err:[%s]", strerror(errno));
						return -1;
					}
				}
			} 
			else if ((stEvs[iCount].events & EPOLLIN) && cSocketFuncType == BINARY_SOCKET) 
		    {
				if (BinaryServer(stEvs[iCount].data.fd) < 0) 
				{
					stEv.events = EPOLLERR;
					stEv.data.fd = stEvs[iCount].data.fd;
					if (epoll_ctl(iEpollfd, EPOLL_CTL_DEL, stEvs[iCount].data.fd, &stEv) < 0) 
					{
						NP_LOG(np_log::NP_ERROR,"epoll_ctl EPOLL_CTL_DEL err:[%s]", strerror(errno));
						return -1;
					}
					NP_LOG(np_log::NP_INFO,"close socket:%d", stEvs[iCount].data.fd);
					close(stEvs[iCount].data.fd);
				}
			}
		}
	}		
	cTcp.CloseSocket();	

	return 0;
}
int CGpsServer::GpsInfoToMem()
{
	char sFileName1[256];
	char sFileNameBak[256];
	memset(sFileName1, 0, sizeof(sFileName1));
	memset(sFileNameBak, 0, sizeof(sFileNameBak));
	
	m_cConf.ConfGetSecVal("PERSISIT", "GPSINFO_PATH", sFileName1);
	strcat(sFileName1,g_sGpsInfoFile);
	strcpy(sFileNameBak,sFileName1);
	strcat(sFileNameBak,".bak");

	int fd = open(sFileName1, O_CREAT | O_RDWR | O_APPEND, 0666);
	if (fd < 0) 
	{
		return -1;
	}
	m_gpsInfoFile.m_sFileName = sFileName1;
	
	//read a line
	char sBuffer[1024];
	char m_id[24];
	char m_value[512];

	FILE* fp = fdopen(fd, "r");
	while (fgets(sBuffer, sizeof(sBuffer), fp)) 
	{
		memset(m_id, 0, sizeof(m_id));
		memset(m_value, 0, sizeof(m_value));
		
		
		*(sBuffer + strlen(sBuffer) - 1) = 0;
    	char* psPos = strchr(sBuffer,',');
    	if (psPos == NULL) 
		{
			memset(sBuffer, 0, sizeof(sBuffer));
   	    	continue;
   		}
        sscanf(sBuffer, "%[^|]|%s", m_id, m_value);   
        unordered_map<string, string>::iterator iter;
        if ((iter = m_gpsinfodata.find(m_id)) != m_gpsinfodata.end()) 
        {
            (iter->second) = (m_value);
        } 
        else 
        {
            m_gpsinfodata.insert(make_pair(m_id, m_value));
        }
		memset(sBuffer, 0, sizeof(sBuffer));

		if(g_gpsInfoColumn.initFlag == 0)
		{
			int i = 0;
			char srcStr[512] = {0};
			char tmpStr[32][64] = {0};
			strcpy(srcStr,m_value);
			char* token = strtok(srcStr, ",");
    		while( token != NULL )
    		{
        		/* While there are tokens in "string" */
				strcpy(tmpStr[i],token);
				i++;
        		/* Get next token: */
        		token = strtok( NULL, ",");
    		}

			for(int j = 0; j< i; ++j)
			{
				char* token = strtok(tmpStr[j], "=");
				if(token != NULL)
				{
					g_gpsInfoColumn.columnName.push_back(token);
				}
			}
			g_gpsInfoColumn.columnName.push_back(GPS_ID);
			g_gpsInfoColumn.initFlag = 1;
		}
	}
	fclose(fp);
	close(fd);
	
	char w_value[1024];
	int fdbak = open(sFileNameBak, O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (fdbak < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "open file%s err:%s\n", sFileNameBak, strerror(errno));
		return -1;	
	}
	for(unordered_map<string, string>::iterator iter = m_gpsinfodata.begin();iter !=  m_gpsinfodata.end();++iter)
	{
		sprintf(w_value,"%s|%s\n",iter->first.c_str(),iter->second.c_str());
		write(fdbak, w_value, strlen(w_value));
	}
	close(fdbak);
	if(rename(sFileNameBak,sFileName1)<0)
	{
		NP_LOG(np_log::NP_ERROR, "rename file%s err:%s\n", sFileNameBak, strerror(errno));
		return -1;	
	}

	fd = open(sFileName1, O_CREAT | O_RDWR | O_APPEND, 0666);
	if (fd < 0) 
	{
		return -1;
	}
	m_gpsInfoFile.m_fd = fd;
	
	return 0;
}

int CGpsServer::LogToMem()
{
	char sLogFileName1[256];
	char sLogFileName2[256];
	memset(sLogFileName2, 0, sizeof(sLogFileName2));
	memset(sLogFileName1, 0, sizeof(sLogFileName1));
	m_cConf.ConfGetSecVal("TEMP_LOG", "TEMP_LOG1", sLogFileName1);
	m_cConf.ConfGetSecVal("TEMP_LOG", "TEMP_LOG2", sLogFileName2);

	int pTempLogfd = open(sLogFileName1, O_CREAT | O_RDWR | O_APPEND, 0666);
	if (pTempLogfd < 0) 
	{
		return -1;
	}
	m_stFile1.m_fd = pTempLogfd;
	m_stFile1.m_sFileName = sLogFileName1;
	//default init fd
	pTempLogfd = open(sLogFileName2, O_CREAT | O_RDWR | O_APPEND, 0666);
	if (pTempLogfd < 0) 
	{
		return -1;
	}
	m_stFile2.m_fd = pTempLogfd;
	m_stFile2.m_sFileName = sLogFileName2;
	//default init mem
	m_pLogfd = m_stFile1.m_fd;
	m_pumGpsInfo = &m_umGpsInfo1;
	char sBuffer[512];
	memset(sBuffer, 0, sizeof(sBuffer));
	//read a line
	FILE* fp = fdopen(m_stFile1.m_fd, "w+");
	while (fgets(sBuffer, sizeof(sBuffer), fp)) 
	{
		*(sBuffer + strlen(sBuffer) - 1) = 0;
		WriteToMem(sBuffer);
		memset(sBuffer, 0, sizeof(sBuffer));
	}

	fp = fdopen(m_stFile2.m_fd, "w+");
	bool bFirstFlag = false;
	while (fgets(sBuffer, sizeof(sBuffer), fp)) 
	{
		*(sBuffer + strlen(sBuffer) - 1) = 0;
		if (!bFirstFlag) 
		{
			m_pLogfd = m_stFile2.m_fd;
			m_pumGpsInfo = &m_umGpsInfo2;
			bFirstFlag = true;
		}
		WriteToMem(sBuffer);
		memset(sBuffer, 0, sizeof(sBuffer));
	}
	
	return 0;
}
int CGpsServer::StatisticDataToMem()
{
	char sStatisticFileName[256];
	memset(sStatisticFileName, 0, sizeof(sStatisticFileName));
	m_cConf.ConfGetSecVal("PERSISIT","STATISTIC_PATH", sStatisticFileName);
	strcat(sStatisticFileName,g_sStatisticEachFile);

	int pTempfd = open(sStatisticFileName, O_CREAT | O_RDONLY, 0666);
	if (pTempfd < 0) 
	{
		return -1;
	}
	
	//read a line
	char sBuffer[512];
	char sKey[50];
	unsigned long long sCount = 0;

	FILE* fp = fdopen(pTempfd, "r");
	while (fgets(sBuffer, sizeof(sBuffer), fp)) 
	{
		*(sBuffer + strlen(sBuffer) - 1) = 0;
    	char* psPos = strchr(sBuffer,',');
    	if (psPos == NULL) 
		{
			memset(sBuffer, 0, sizeof(sBuffer));
   	    	continue;
   		}
        sscanf(sBuffer, "%[^,],%20llu", sKey, &sCount);   
        unordered_map<string, unsigned long long>::iterator iter;
        if ((iter = m_statisticdata.find(sKey)) != m_statisticdata.end()) 
        {
            (iter->second) = (sCount);
        } 
        else 
        {
            m_statisticdata.insert(make_pair(sKey, sCount));
        }
		memset(&sKey, 0, sizeof(sKey));
		memset(sBuffer, 0, sizeof(sBuffer));
		sCount = 0;
	}

	fclose(fp);
	return 0;
}

int CGpsServer::LastDataToMem()
{
	char sLastFileName[256];
	memset(sLastFileName, 0, sizeof(sLastFileName));
	m_cConf.ConfGetSecVal("PERSISIT","LAST_PATH", sLastFileName);
	strcat(sLastFileName,g_sLastDataFile);

	int pTempfd = open(sLastFileName, O_CREAT | O_RDONLY, 0666);
	if (pTempfd < 0) 
	{
		return -1;
	}
	
	//read a line
	char sBuffer[512];
	char sKey[50];
   	GPSVALUE_S stValue;
    char* psPos;
	FILE* fp = fdopen(pTempfd, "r");
	while (fgets(sBuffer, sizeof(sBuffer), fp)) 
	{
		*(sBuffer + strlen(sBuffer) - 1) = 0;
	
		char* psBegin = sBuffer;
    	psPos = strchr(psBegin, GPS_VAL_SEP);
    	if (psPos == NULL) 
		{
   	    	return -1;
   		}
        memset(sKey, 0, sizeof(sKey));
        memcpy(sKey, psBegin, psPos - psBegin);
        psBegin = psPos + 1;
        
        
        psPos = strchr(psBegin, GPS_VAL_SEP);
        if (psPos == NULL) 
        {
            return -1;
        }
        memcpy(stValue.m_sTime, psBegin, psPos - psBegin);
        psBegin = psPos + 1;
        
        psPos = strchr(psBegin, GPS_VAL_SEP);
        if (psPos == NULL) 
        {
            return -1;
        }
        memcpy(stValue.m_sLat, psBegin, psPos - psBegin);
        psBegin = psPos + 1;
        
        psPos = strchr(psBegin, GPS_VAL_SEP);
        if (psPos == NULL) 
        {
            return -1;
        }
        memcpy(stValue.m_sLong, psBegin, psPos - psBegin);
        psBegin = psPos + 1;
        
        strcpy(stValue.m_sRemain, psBegin);
        unordered_map<string, GPSVALUE_S>::iterator iterGpsInfo;
        if ((iterGpsInfo = m_lastdata.find(sKey)) != m_lastdata.end()) 
        {
            (iterGpsInfo->second) = (stValue);
        } 
        else 
        {
            m_lastdata.insert(make_pair(sKey, stValue));
        }
		memset(&stValue, 0, sizeof(sBuffer));
		memset(sBuffer, 0, sizeof(sBuffer));
	}

	fclose(fp);
	return 0;
}

int CGpsServer::WriteToMem(char* psValue)
{
    char sKey[50];
    GPSVALUE_S stValue;
    char* psBegin = psValue;

    char* psPos = strchr(psBegin, GPS_VAL_SEP);
    if (psPos == NULL) 
	{
        return -1;
    }
    memset(sKey, 0, sizeof(sKey));
    memcpy(sKey, psBegin, psPos - psBegin);
    psBegin = psPos + 1;


    psPos = strchr(psBegin, GPS_VAL_SEP);
    if (psPos == NULL) 
	{
        return -1;
    }
    memcpy(stValue.m_sTime, psBegin, psPos - psBegin);
    psBegin = psPos + 1;
      
    psPos = strchr(psBegin, GPS_VAL_SEP);
    if (psPos == NULL) 
	{
        return -1;
    }
    memcpy(stValue.m_sLat, psBegin, psPos - psBegin);
    psBegin = psPos + 1;
    
    psPos = strchr(psBegin, GPS_VAL_SEP);
    if (psPos == NULL) 
	{
        return -1;
    }
    memcpy(stValue.m_sLong, psBegin, psPos - psBegin);
    psBegin = psPos + 1;

    strcpy(stValue.m_sRemain, psBegin);
    unordered_map<string, vector<GPSVALUE_S> >::iterator iterGpsInfo;
    if ((iterGpsInfo = m_pumGpsInfo->find(sKey)) != m_pumGpsInfo->end()) 
	{
        (iterGpsInfo->second).push_back(stValue);
    } 
	else 
	{
        vector<GPSVALUE_S> vecVal;
        vecVal.push_back(stValue);
        m_pumGpsInfo->insert(make_pair(sKey, vecVal));
    }

	unordered_map<string,unsigned int>::iterator iter;
	if((iter = m_bakData.find(sKey)) != m_bakData.end())
	{
		iter->second = iter->second + 1;
	}
	else
	{
		m_bakData.insert(make_pair(sKey,1));
	}
    return 0;
}

void* CGpsServer::PersistThreadFunc(void* pParam)
{
	CPersisitence* pcPersisit = (CPersisitence*)pParam;
	while(true)
	{
		if (pcPersisit->Core() < 0) 
		{
			NP_LOG(np_log::NP_ERROR, "Persisitence err.");
			kill(getpid(), SIGKILL);
		}
	}
	NP_LOG(np_log::NP_ERROR, "exception exit persistence data service.");
	kill(getpid(), SIGKILL);
	return NULL;
}

void* CGpsServer::statistic_thread_func(void* pParam)
{
	CStatistic* pcStatistic = (CStatistic*)pParam;
	if(g_isstart_statistic)
	{
		int rest_error_times = 0;
		while(true)
		{
			try
			{
				if (pcStatistic->start_server() < 0) 
				{
					NP_LOG(np_log::NP_ERROR, "statistic err.");
				}
			}
			catch (std::exception& ex)
			{
				cout<<"exception:" << ex.what() << std::endl;
				rest_error_times++;
				sleep(SLEEP_ONE_SECOND);
				NP_LOG(np_log::NP_ERROR, "statistic err:%s, err times:%d", ex.what(), rest_error_times);
			}
		}
		NP_LOG(np_log::NP_ERROR, "exception exit statistic data service.");
		kill(getpid(), SIGKILL);
	}
	return NULL;
}

void* CGpsServer::clean_thread_func(void* pparam)
{
	CGpsServer* pgps_server = (CGpsServer*)pparam;
	pgps_server->m_file_helper.push_delfile_dir(g_sLocusPath);
	pgps_server->m_file_helper.push_delfile_dir(g_sAroundPath);

	int current_hour = -1;
	int current_minutes = -1;
	while (true)
	{
		try
		{
			get_local_h_m(current_hour, current_minutes);
			if (current_hour == CLEAN_TIME_HOUR  && current_minutes == CLEAN_TIME_MIN)
			{
				//delete out date data
				//1.locus file
				pgps_server->m_file_helper.read_files_to_vec(g_sLocusPath);
				pgps_server->m_file_helper.clean_outdate_files();
				//1.around file
				pgps_server->m_file_helper.read_files_to_vec(g_sAroundPath);
				pgps_server->m_file_helper.clean_outdate_files();
			}

			sleep(SLEEP_ONE_MIN);
		}
		catch (std::exception& ex)
		{
			sleep(SLEEP_ONE_MIN);
			NP_LOG(np_log::NP_ERROR, "clean gps out date data err:%s.", ex.what());
		}
	}
	NP_LOG(np_log::NP_ERROR, "exception exit clean data service.");
	kill(getpid(), SIGKILL);
	return NULL;
}

void* CGpsServer::cpprest_thread_func(void* pparam)
{
	CGpsServer* pgps_server = (CGpsServer*)pparam;
	char crest_port[10];
	memset(crest_port, 0, sizeof(crest_port));
	pgps_server->m_cConf.ConfGetSecVal("SERVER", "REST_PORT", crest_port);
	int rest_error_times = 0;
	basic_http::pgps_server = pgps_server;
	while (true)
	{
		try
		{
			basic_http basic_server;
			basic_server.init_http_server(atoi(crest_port));
			//start server
			basic_server.start_server();
			for (;;)
			{
				basic_server.loop(1000);
			}
			basic_server.close_server();
		}
		catch (std::exception& ex)
		{
			cout<<"exception:" << ex.what() << std::endl;
			rest_error_times++;
			sleep(SLEEP_ONE_SECOND);
			NP_LOG(np_log::NP_ERROR, "rest err:%s, err times:%d", ex.what(), rest_error_times);
		}
	}	
	NP_LOG(np_log::NP_ERROR, "exception exit rest service.");
	kill(getpid(), SIGKILL);
	return NULL;
}

void* CGpsServer::BinaryThreadFunc(void* pParam)
{
	CGpsServer* pcServer = (CGpsServer*)pParam;
	char sBinaryPort[10];
	memset(sBinaryPort, 0, sizeof(sBinaryPort));
	pcServer->m_cConf.ConfGetSecVal("SERVER", "BINARY_PORT", sBinaryPort);
	np_log::NP_STDOUT("* start binary server on port %s", sBinaryPort);
	while(true)
	{
		if (pcServer->GpsTcp("", sBinaryPort, BINARY_SOCKET) < 0) 
		{
			NP_LOG(np_log::NP_ERROR, "binary service error:%s!", strerror(errno));
			kill(getpid(), SIGKILL);
		}
	}
	NP_LOG(np_log::NP_ERROR, "exception exit binary write data service.");
	kill(getpid(), SIGKILL);
	return NULL;
}

void* CGpsServer::subpub_thread_func(void* pparam)
{
	CGpsServer* pgps_server = (CGpsServer*)pparam;
	if(g_isstart_pubsub)
	{
		char redis_host[32];
		char redis_port[8];
		char buffer_size[10];
		char subscribe_channel[64];
		char publish_channel[64];
		char persistence_subfile[64];

		pgps_server->m_cConf.ConfGetSecVal("REDIS","REDIS_HOST",redis_host);
		pgps_server->m_cConf.ConfGetSecVal("REDIS","REDIS_PORT",redis_port);
		pgps_server->m_cConf.ConfGetSecVal("REDIS","SUBSCRIBE_CHANNEL",subscribe_channel);
		pgps_server->m_cConf.ConfGetSecVal("REDIS","BUFFER_SIZE",buffer_size);
		pgps_server->m_cConf.ConfGetSecVal("REDIS", "PUBLISH_CHANNEL", publish_channel);
		pgps_server->m_cConf.ConfGetSecVal("REDIS", "PERSISTENCE_FILE", persistence_subfile);

		np_log::NP_STDOUT("* The server will start publis/subscribe service.");
		np_log::NP_STDOUT("* It depends on redis.the redis server host is %s:%s", redis_host,redis_port);
		pgps_server->m_predis_proxy = std::shared_ptr<hiredis_proxy>(new hiredis_proxy(publish_channel,subscribe_channel, \
		atoi(buffer_size), redis_host, atoi(redis_port)));
		//it's a loop method
		//std::cout << "start gps publish/subscribed service." <<std::endl;
		pgps_server->m_predis_proxy->start_service(persistence_subfile);
		NP_LOG(np_log::NP_ERROR, "exception exit pubsub data service.");
		kill(getpid(), SIGKILL);
	}
	return NULL;
}

