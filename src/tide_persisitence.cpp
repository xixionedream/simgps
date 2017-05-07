#include "tide_persisitence.h"

char m_sLocusPath[256];
char m_sAroundPath[256];
char m_sLastPath[256];
char m_sStatisticPath[256];


unsigned int g_uiPressMile = 0;
int g_iPress = 0;
extern char g_sLastDataFile[10];
extern char g_sStatisticEachFile[10];
extern char g_sStatisticTotalFile[10];

CPersisitence::CPersisitence(int* ppfd, FILEINFO_S* pstFile1, FILEINFO_S* pstFile2, CConfig* pcConfig, unordered_map<string, vector<GPSVALUE_S> >** ppumGpsInfo, \
								unordered_map<string, vector<GPSVALUE_S> >* pumGpsInfo1, unordered_map<string, vector<GPSVALUE_S> >* pumGpsInfo2,\
								unordered_map<string,GPSVALUE_S>* plastData,unordered_map<string,unsigned long long>* pstatisticData)
:m_ppLogfd(ppfd), m_pstFile1(pstFile1), m_pstFile2(pstFile2), m_pcConfig(pcConfig), m_ppumGpsInfo(ppumGpsInfo), m_pumGpsInfo1(pumGpsInfo1), m_pumGpsInfo2(pumGpsInfo2),m_plastdata(plastData),m_pstatisticdata(pstatisticData)
{
	m_iDelayLoop = 0;
}

CPersisitence::~CPersisitence()
{
	m_iDelayLoop = 0;
}

int CPersisitence::Core()
{
	FILE* pfd;
	char sCommand[256];
	char sDelayLoop[10];
	char sPressMile[10];
	char sThreshold[10];
	char sIsPress[2];
	char sPersisitTime[5];
	memset(sPersisitTime, 0, sizeof(sPersisitTime));	
	memset(m_sLocusPath, 0, 256);	
	memset(m_sAroundPath, 0, 256);	

	m_pcConfig->ConfGetSecVal("PERSISIT", "AROUND_PATH", m_sAroundPath);	
	m_pcConfig->ConfGetSecVal("PERSISIT", "LOCUS_PATH", m_sLocusPath);
	m_pcConfig->ConfGetSecVal("PERSISIT", "LAST_PATH", m_sLastPath);
	m_pcConfig->ConfGetSecVal("PERSISIT", "STATISTIC_PATH", m_sStatisticPath);
	m_pcConfig->ConfGetSecVal("PERSISIT", "DELAY_LOOP", sDelayLoop);
	m_pcConfig->ConfGetSecVal("PERSISIT", "DELAY_LOOP", sDelayLoop);
	m_pcConfig->ConfGetSecVal("PERSISIT", "PRESS_MILE", sPressMile);
	m_pcConfig->ConfGetSecVal("PERSISIT", "IS_PRESS", sIsPress);
	m_pcConfig->ConfGetSecVal("PERSISIT", "PERSISIT_THRESHOLD", sThreshold);
	m_pcConfig->ConfGetSecVal("PERSISIT", "PERSISIT_TIME", sPersisitTime);

	g_uiPressMile = atoi(sPressMile);
	g_iPress = atoi(sIsPress);
	m_iDelayLoop = atoi(sDelayLoop);	
	int iPersisitThreshold = atoi(sThreshold);
	int iPersisitTime = 600;
	int iTotalTime = 0;
	if (strlen(sPersisitTime) > 0) {
		iPersisitTime = atoi(sPersisitTime);
	}
	if (access(m_sLocusPath, F_OK) < 0) {
		mkdir(m_sLocusPath, 0766);	
	}
	if (access(m_sAroundPath, F_OK) < 0) {
		mkdir(m_sAroundPath, 0766);	
	}
	//struct timeval stBeginTime, stEndTime;
	struct stat stFile;
	while (1) {
		memset(&stFile, 0 , sizeof(stFile));
		fstat(*m_ppLogfd, &stFile);
		if (stFile.st_size < iPersisitThreshold && iTotalTime < iPersisitTime) {					
			sleep(m_iDelayLoop);
			iTotalTime += m_iDelayLoop;
			continue;
		}
		iTotalTime = 0;	
		FILEINFO_S* pPersisitFile = NULL;
		unordered_map<string, vector<GPSVALUE_S> >* m_pumPersisitGpsInfo;
		unordered_map<string, vector<GPSVALUE_S> >* m_pumGpsInfo = NULL;	
		int iLogfd = -1;

		//clear mem
		if (*m_ppumGpsInfo == m_pumGpsInfo1) {
			m_pumGpsInfo2->clear();	 
		} else if (*m_ppumGpsInfo == m_pumGpsInfo2) {
			m_pumGpsInfo1->clear();	
		} else {
			return -1;
		}
/*	    //clear file		
		memset(sCommand, 0, sizeof(sCommand));
		if (*m_ppLogfd == m_pstFile1->m_fd) {			
			sprintf(sCommand, "cat /dev/null > %s", m_pstFile2->m_sFileName.c_str());			
		} else if (*m_ppLogfd == m_pstFile2->m_fd) {
			sprintf(sCommand, "cat /dev/null > %s", m_pstFile1->m_sFileName.c_str());
		} else {
			return -1;
		}
		if ((pfd = popen(sCommand, "r")) == NULL) {
				return -1;
		}
		pclose(pfd);
*/
		
		if (*m_ppLogfd == m_pstFile1->m_fd) {
			iLogfd = m_pstFile2->m_fd;
			pPersisitFile = m_pstFile1;
		} else if (*m_ppLogfd == m_pstFile2->m_fd) {
			iLogfd = m_pstFile1->m_fd;
			pPersisitFile = m_pstFile2;
		} else {
			return -1;
		}

		if (*m_ppumGpsInfo == m_pumGpsInfo1) {
			m_pumGpsInfo = m_pumGpsInfo2;
			m_pumPersisitGpsInfo = m_pumGpsInfo1; 
		} else if (*m_ppumGpsInfo == m_pumGpsInfo2) {
			m_pumGpsInfo = m_pumGpsInfo1;
			m_pumPersisitGpsInfo = m_pumGpsInfo2; 
		} else {
			return -1;
		}
		*m_ppLogfd = iLogfd;
		*m_ppumGpsInfo = m_pumGpsInfo; 
		CThread cThreadLocus;
		CThread cThreadAround;
		CThread cThreadLast;
		
		cThreadLocus.CThreadSetStartRoutine(MemToLocusFile);
		if (cThreadLocus.CThreadCreate(m_pumPersisitGpsInfo)) {
			return -1;
		}
		
		cThreadAround.CThreadSetStartRoutine(MemToAroundFile);
		if (cThreadAround.CThreadCreate(m_pumPersisitGpsInfo)) {
			return -1;
		}

		cThreadLast.CThreadSetStartRoutine(MemToLastFile);
		if (cThreadLast.CThreadCreate(this)) {
			return -1;
		}
		
		cThreadAround.CThreadJoin();
		cThreadLocus.CThreadJoin();
		cThreadLast.CThreadJoin();
		//clear file
		memset(sCommand, 0, sizeof(sCommand));
		sprintf(sCommand, "cat /dev/null > %s", pPersisitFile->m_sFileName.c_str());
		if ((pfd = popen(sCommand, "r")) == NULL) {
			return -1;
		}
		pclose(pfd);
	}

	return 0;
}

void* CPersisitence::MemToLocusFile(void* pPara)
{
	unordered_map<string, vector<GPSVALUE_S> >* m_pumPersisitGpsInfo = (unordered_map<string, vector<GPSVALUE_S> >*)pPara;
	unordered_map<string, vector<GPSVALUE_S> >::iterator iterGpsInfo;
	GPSVALUE_S* pstGpsValus = NULL;		
	unsigned short usNum = 0;
	int iGranularity = 0;
	int fdLocus = -1; 
	char sValue[512];
	int iValueLength = 0;
	char sLocusFileName[256], sLocusIndexFileName[256];
	struct stat stFile;
	LOCUSINDEX_S stLocusIndex;

	long lCurTime = 0;
	double dBaseLat = 0.00;
	double dBaseLong = 0.00;
	unsigned ulLatDistance = 0;	
	unsigned ulLongDistance = 0;	
	long lPressLat = 0;
	long lPressLong = 0;
	long lAbsoluteLat = 0;
	long lAbsoluteLong = 0;
	int iLen = 0;
	map<string, int> mLocusFile;
	map<string, int> mLocusFileIndex;
	map<string, int>::iterator iterLocusFile;
	map<string, int>::iterator iterLocusFileIndex;
	
	for (iterGpsInfo = m_pumPersisitGpsInfo->begin(); iterGpsInfo != m_pumPersisitGpsInfo->end(); ++iterGpsInfo) {
		//locus
		unsigned long ulKey = Fnv64a(iterGpsInfo->first.c_str(), iterGpsInfo->first.length());				
		int iLChanged = -1;
		int fdLocusIndex = -1;
		memset(&stLocusIndex, 0, sizeof(stLocusIndex));				
		bool bFirstWirte = true;	
		long lPreTime = 0;
		long lPreLat = 0;
		long lPreLong = 0;
		
		for (usNum = 0; usNum < iterGpsInfo->second.size(); ++usNum) {	
			pstGpsValus = &((iterGpsInfo->second)[usNum]);
			iGranularity = Atoi(pstGpsValus->m_sTime, 8, 2);
			
			if (iLChanged != iGranularity) {
				bFirstWirte = true;	
				if (fdLocusIndex > 0) { 
					write(fdLocusIndex, &stLocusIndex, sizeof(stLocusIndex));
				}
				memset(sLocusFileName, 0, sizeof(sLocusFileName));
				sprintf(sLocusFileName, "%s%d_%lu", m_sLocusPath, Atoi(pstGpsValus->m_sTime, 0, 10), ulKey % MOD);		
					
				memset(sLocusIndexFileName, 0, sizeof(sLocusIndexFileName));
				//sprintf(sLocusIndexFileName, "%s%d_%lu_index", m_sLocusPath, Atoi(pstGpsValus->m_sTime, 0, 10), ulKey % MOD);		
				strcpy(sLocusIndexFileName, sLocusFileName);
				strcat(sLocusIndexFileName, "_index");
				
				iterLocusFile = mLocusFile.find(sLocusFileName);
				if (iterLocusFile != mLocusFile.end()) {	
					fdLocus = iterLocusFile->second;
				} else {
					fdLocus = open(sLocusFileName, O_CREAT | O_RDWR | O_APPEND, 0666);
					if (fdLocus < 0) {
						NP_LOG(np_log::NP_ERROR, "open file%s err:%s\n", sLocusFileName, strerror(errno));
						return NULL;	
					}
					mLocusFile.insert(pair<string, int>(sLocusFileName, fdLocus));
				}

				fstat(fdLocus, &stFile);
				memset(&stLocusIndex, 0, sizeof(stLocusIndex));				
				stLocusIndex.m_ulKey = ulKey;	
				stLocusIndex.m_uiOffset = stFile.st_size;	
					
				iterLocusFileIndex = mLocusFileIndex.find(sLocusIndexFileName);
				if (iterLocusFileIndex != mLocusFileIndex.end()) {	
					fdLocusIndex = iterLocusFileIndex->second;
				} else {
					fdLocusIndex = open(sLocusIndexFileName, O_CREAT | O_RDWR | O_APPEND, 0666);
					if (fdLocusIndex < 0) {
						NP_LOG(np_log::NP_ERROR, "open file%s err:%s\n", sLocusIndexFileName, strerror(errno));
						return NULL;
					}
					mLocusFileIndex.insert(pair<string, int>(sLocusIndexFileName, fdLocusIndex));
				}
				iLChanged = iGranularity;
			}

			if (bFirstWirte) {	
				lPreTime = atol(pstGpsValus->m_sTime);
				dBaseLat = atof(pstGpsValus->m_sLat);
				dBaseLong = atof(pstGpsValus->m_sLong);	
				lAbsoluteLat = (long)(dBaseLat * PRECISION);
				lAbsoluteLong = (long)(dBaseLong * PRECISION);	
				/*
				memset(sValue, 0, sizeof(sValue));
				sprintf(sValue, "%ld,%ld,%ld,%s|", lPreTime, lPreLat, lPreLong, pstGpsValus->m_sRemain);
				iValueLength = strlen(sValue);
				write(fdLocus, sValue, iValueLength);
				stLocusIndex.m_uiLength += iValueLength; 
				*/
				iValueLength = ltos(lPreTime, sValue);
				sValue[iValueLength] = ',';
				iValueLength += 1;	
				iValueLength += ltos(lAbsoluteLat, &sValue[iValueLength]);
				sValue[iValueLength] = ',';
				iValueLength += 1;	
				iValueLength += ltos(lAbsoluteLong, &sValue[iValueLength]);
				sValue[iValueLength] = ',';
				iValueLength += 1;	
				iLen = strlen(pstGpsValus->m_sRemain);
				memcpy(&sValue[iValueLength], pstGpsValus->m_sRemain, iLen);
				iValueLength += iLen;	
				sValue[iValueLength] = '|';
				iValueLength += 1;	
					
				write(fdLocus, sValue, iValueLength);
				stLocusIndex.m_uiLength += iValueLength; 
				//
				if (g_iPress) {
					GetNTUByDistance(g_uiPressMile, dBaseLat, dBaseLong, ulLatDistance, ulLongDistance);
					lPreLat = lAbsoluteLat;
					lPreLong = lAbsoluteLong;
				}
				bFirstWirte = false;
				
			} else {
				//get NTU D-value by miles
				dBaseLat = atof(pstGpsValus->m_sLat);
				dBaseLong = atof(pstGpsValus->m_sLong);	
				if (g_iPress) {
					lPressLat = (long)(dBaseLat * PRECISION) - lPreLat;
					lPressLong = (long)(dBaseLong * PRECISION) - lPreLong;
					if (abs(lPressLat) <= ulLatDistance && abs(lPressLong) <= ulLongDistance) {
						continue;	
					}

					GetNTUByDistance(g_uiPressMile, dBaseLat, dBaseLong, ulLatDistance, ulLongDistance);
					lPreLat = (long)(dBaseLat * PRECISION);
					lPreLong = (long)(dBaseLong * PRECISION);
				}
				//time
				lCurTime = atol(pstGpsValus->m_sTime);
				iValueLength = ltos(lCurTime - lPreTime, sValue);
				sValue[iValueLength] = ',';
				iValueLength += 1;	
				//latitude
				iValueLength += ltos((long)(dBaseLat * PRECISION) - lAbsoluteLat, &sValue[iValueLength]);
				sValue[iValueLength] = ',';
				iValueLength += 1;	
				//longtitude
				iValueLength += ltos((long)(dBaseLong * PRECISION) - lAbsoluteLong, &sValue[iValueLength]);
				sValue[iValueLength] = ',';
				iValueLength += 1;	
				//expand
				iLen = strlen(pstGpsValus->m_sRemain);
				memcpy(&sValue[iValueLength], pstGpsValus->m_sRemain, iLen);
				iValueLength += iLen;	
				sValue[iValueLength] = '|';
				iValueLength += 1;	
					
				write(fdLocus, sValue, iValueLength);
				stLocusIndex.m_uiLength += iValueLength; 
			
				/*	
				sprintf(sValue, "%ld,%ld,%ld,%s|", lCurTime - lPreTime, (long)(atof(pstGpsValus->m_sLat) * 1000000) - lPreLat,\
												 	(long)(atof(pstGpsValus->m_sLong) * 1000000) - lPreLong, pstGpsValus->m_sRemain);
				write(fdLocus, sValue, iValueLength);
				stLocusIndex.m_uiLength += iValueLength; 
				*/
			}
		}
		//write index
		write(fdLocusIndex, &stLocusIndex, sizeof(stLocusIndex));
	}
	
	for (iterLocusFile = mLocusFile.begin(); iterLocusFile != mLocusFile.end(); ++iterLocusFile) {
		close(iterLocusFile->second);
	}
	for (iterLocusFileIndex = mLocusFileIndex.begin(); iterLocusFileIndex != mLocusFileIndex.end(); ++iterLocusFileIndex) {
		close(iterLocusFileIndex->second);
	}
	return pPara;
}

void* CPersisitence::MemToAroundFile(void* pPara)
{
	unordered_map<string, vector<GPSVALUE_S> >* m_pumPersisitGpsInfo = (unordered_map<string, vector<GPSVALUE_S> >*)pPara;
	unordered_map<string, vector<GPSVALUE_S> >::iterator iterGpsInfo;
	GPSVALUE_S* pstGpsValus = NULL;
	char sAroundFileName[256];	
	int iGranularity = 0;
	int fdAround = -1;
	unsigned short usNum = 0;
	char sGeoKey[10], sGeoKeyOld[10];
	char sHour[11];
	char sValue[256];
	char* psVal;
	int iAChanged = -1;	
	int iValLength = 0;
	int iLen = 0;
	
	memset(sGeoKeyOld, 0, sizeof(sGeoKeyOld));	
	map<string, int> mFileIndex;
	map<string, int>::iterator iterFileIndex;
	for (iterGpsInfo = m_pumPersisitGpsInfo->begin(); iterGpsInfo != m_pumPersisitGpsInfo->end(); ++iterGpsInfo) {
		//history around
		for (usNum = 0; usNum < iterGpsInfo->second.size(); ++usNum) {	
			pstGpsValus = &((iterGpsInfo->second)[usNum]);
			iGranularity = Atoi(pstGpsValus->m_sTime, 8, 2);
			//get geo code
			GeohashEncode5(atof(pstGpsValus->m_sLat), atof(pstGpsValus->m_sLong), sGeoKey,  HASHCODE_LENGTH - 1);
			//geohash_encode(atof(pstGpsValus->m_sLat.c_str()), atof(pstGpsValus->m_sLong.c_str()), sGeoKey, HASHCODE_LENGTH);

			if (iAChanged != iGranularity || strncmp(sGeoKey, sGeoKeyOld, HASHCODE_LENGTH - 1) != 0) {
				memset(sAroundFileName, 0, sizeof(sAroundFileName));
				//sprintf(sAroundFileName, "%s%d_%s", m_sAroundPath, Atoi(pstGpsValus->m_sTime, 0, 10), sGeoKey);		
				memset(sHour, 0, 11);
				memcpy(sHour, pstGpsValus->m_sTime, 10);	
				strcpy(sAroundFileName, m_sAroundPath);
				strcat(sAroundFileName, sHour);
				strcat(sAroundFileName, "_");
				strcat(sAroundFileName, sGeoKey);

				iterFileIndex = mFileIndex.find(sAroundFileName);
				if (iterFileIndex != mFileIndex.end()) {	
					fdAround = iterFileIndex->second;
				} else {
					if (mFileIndex.size() >= MAX_FILEFD) {
						iterFileIndex = mFileIndex.begin();
						close(iterFileIndex->second);
						mFileIndex.erase(iterFileIndex);
					}
					fdAround = open(sAroundFileName, O_CREAT | O_RDWR | O_APPEND, 0666);
					if (fdAround < 0) {
						NP_LOG(np_log::NP_ERROR, "open file%s err:%s\n", sAroundFileName, strerror(errno));
						return NULL;	
					}
					mFileIndex.insert(make_pair(sAroundFileName, fdAround));
				}
				iAChanged = iGranularity;
				strcpy(sGeoKeyOld, sGeoKey);
			}
			/*memset(sValue, 0, sizeof(sValue));
			sprintf(sValue, "%s,%s,%s,%s,%s", iterGpsInfo->first.c_str(), pstGpsValus->m_sTime, pstGpsValus->m_sLat, pstGpsValus->m_sLong, pstGpsValus->m_sRemain);
			write(fdAround, sValue, strlen(sValue));
			*/
					
			psVal = sValue;
			iLen = iterGpsInfo->first.length();
			memcpy(psVal, iterGpsInfo->first.c_str(), iLen);
			psVal += iLen;
			*psVal++ = ',';
			iValLength = iLen + 1;
			
			iLen = strlen(pstGpsValus->m_sTime);
			memcpy(psVal, pstGpsValus->m_sTime, iLen);	
			psVal += iLen;		
			*psVal++ = ',';
			iValLength += iLen + 1;
				
			iLen = strlen(pstGpsValus->m_sLat);
			memcpy(psVal, pstGpsValus->m_sLat, iLen);	
			psVal += iLen;
			*psVal++ = ',';
			iValLength += iLen + 1;

			iLen = strlen(pstGpsValus->m_sLong);
			memcpy(psVal, pstGpsValus->m_sLong, iLen);	
			psVal += iLen;
			*psVal++ = ',';
			iValLength += iLen + 1;
			
			iLen = strlen(pstGpsValus->m_sRemain);
			memcpy(psVal, pstGpsValus->m_sRemain, iLen);	
			psVal += iLen;
			*psVal++ = '\n';
			iValLength += iLen + 1;
			
			write(fdAround, sValue, iValLength);
		}
	}
		
	for (iterFileIndex = mFileIndex.begin(); iterFileIndex != mFileIndex.end(); ++iterFileIndex) {
		close(iterFileIndex->second);
	}
	
	return pPara;
}
void* CPersisitence::MemToLastFile(void* pPara)
{
	CPersisitence* persisitence = (CPersisitence*)pPara;
	unordered_map<string, vector<GPSVALUE_S> >* m_pumPersisitGpsInfo;
	unordered_map<string, vector<GPSVALUE_S> >::iterator iterGpsInfo;
	unordered_map<string, GPSVALUE_S >::iterator iterLastData;
	unordered_map<string, unsigned long long >::iterator iterStatisticData;
	
	char sLastFileName[256];	
	char sStatisticEachFileName[256];
	char sStatisticTotalFileName[256];
	int fdLast = -1;
	int fdSe = -1;
	int fdSt = -1;
	char sValue[256];
	unsigned long long totalNum = 0;
	
	if (*persisitence->m_ppumGpsInfo == persisitence->m_pumGpsInfo1)
	{
		m_pumPersisitGpsInfo = persisitence->m_pumGpsInfo2; 
	} 
	else if (*persisitence->m_ppumGpsInfo == persisitence->m_pumGpsInfo2) 
	{
		m_pumPersisitGpsInfo = persisitence->m_pumGpsInfo1; 
	} 
	else 
	{
		return NULL;
	}
	memset(sLastFileName, 0, sizeof(sLastFileName));
	strcpy(sLastFileName, m_sLastPath);
	strcat(sLastFileName, g_sLastDataFile);

	memset(sStatisticEachFileName, 0, sizeof(sStatisticEachFileName));
	strcpy(sStatisticEachFileName, m_sStatisticPath);
	strcat(sStatisticEachFileName, g_sStatisticEachFile);

	memset(sStatisticTotalFileName, 0, sizeof(sStatisticTotalFileName));
	strcpy(sStatisticTotalFileName, m_sStatisticPath);
	strcat(sStatisticTotalFileName, g_sStatisticTotalFile);
	
	for (iterGpsInfo = m_pumPersisitGpsInfo->begin(); iterGpsInfo != m_pumPersisitGpsInfo->end(); ++iterGpsInfo)
	{
		
		if( (iterLastData = persisitence->m_plastdata->find(iterGpsInfo->first)) != persisitence->m_plastdata->end())
		{
			iterLastData->second = iterGpsInfo->second.back();
		}
		else
		{
			persisitence->m_plastdata->insert(make_pair(iterGpsInfo->first, iterGpsInfo->second.back()));
		}
		
		if((iterStatisticData = persisitence->m_pstatisticdata->find(iterGpsInfo->first)) !=  persisitence->m_pstatisticdata->end() )
		{
			iterStatisticData->second = iterStatisticData->second + iterGpsInfo->second.size();
		}
		else
		{
			persisitence->m_pstatisticdata->insert(make_pair(iterGpsInfo->first, iterGpsInfo->second.size()));
		}			
	}

	fdLast = open(sLastFileName, O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (fdLast < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "open file%s err:%s\n", sLastFileName, strerror(errno));
		return NULL;	
	}

	fdSe = open(sStatisticEachFileName, O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (fdSe < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "open file%s err:%s\n", sStatisticEachFileName, strerror(errno));
		return NULL;	
	}

	fdSt = open(sStatisticTotalFileName, O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (fdSt < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "open file%s err:%s\n", sStatisticTotalFileName, strerror(errno));
		return NULL;	
	}
	
	for(iterStatisticData = persisitence->m_pstatisticdata->begin();iterStatisticData != persisitence->m_pstatisticdata->end();++iterStatisticData)
	{
		memset(sValue, 0, sizeof(sValue));
		sprintf(sValue, "%s,%llu\n", iterStatisticData->first.c_str(), iterStatisticData->second);
		write(fdSe, sValue, strlen(sValue));	
		totalNum = totalNum + iterStatisticData->second;
	}

	memset(sValue, 0, sizeof(sValue));
	sprintf(sValue, "%llu\n", totalNum);
	write(fdSt, sValue, strlen(sValue));
	
	for(iterLastData = persisitence->m_plastdata->begin();iterLastData != persisitence->m_plastdata->end();++iterLastData)
	{
		memset(sValue, 0, sizeof(sValue));
		sprintf(sValue, "%s,%s,%s,%s,%s\n", iterLastData->first.c_str(), iterLastData->second.m_sTime, iterLastData->second.m_sLat, iterLastData->second.m_sLong, iterLastData->second.m_sRemain);
		write(fdLast, sValue, strlen(sValue));
	}
	close(fdLast);
	close(fdSe);
	close(fdSt);
	return pPara;
}

