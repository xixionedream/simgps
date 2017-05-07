#include "statistic.h"

extern char m_sStatisticPath[256];
CStatistic::~CStatistic()
{

}

bool CStatistic::init_server(unsigned int intervalTime,unsigned int cycleNum,std::unordered_map<std::string, unsigned long long>* statisticData,std::unordered_map<std::string, unsigned int>* bakData)
{
	if(statisticData == NULL || bakData == NULL)
		return false;
	if( 0 == intervalTime || 0 == cycleNum)
	{
		NP_LOG(np_log::NP_ERROR,"statistic conf error:intervalTime[%u],cycleNum[%u]\n",intervalTime,cycleNum);
		return false;
	}
		
	m_intervalTime = intervalTime;
	m_cycleNum = cycleNum;
	time_t lTime = time(NULL);
	m_nextTime = lTime + (60*intervalTime - lTime%(60*intervalTime));
	m_timezone = GetTimeZone();
	std::unordered_map<std::string, unsigned long long>::iterator iter1;
	std::unordered_map<std::string, unsigned long long>::iterator iter2;
	for(iter1 = statisticData->begin();iter1!= statisticData->end();++iter1 )
	{
		if( (iter2=m_EachTotalData.find(iter1->first))!= m_EachTotalData.end())
		{
			iter2->second = iter1->second;
		}
		else
		{
			m_EachTotalData.insert(make_pair(iter1->first,iter1->second));
		}
	}

	
	std::unordered_map<std::string, unsigned int>::iterator iter;
	for(iter = bakData->begin();iter!= bakData->end();++iter )
	{
		if( (iter2=m_EachTotalData.find(iter->first))!= m_EachTotalData.end())
		{
			iter2->second = iter2->second + iter->second;
		}
		else
		{
			m_EachTotalData.insert(make_pair(iter->first,iter->second));
		}
	}
	
	return true;
}
void CStatistic::write_data(std::string key)
{	
	std::unordered_map<std::string, unsigned long long>::iterator iter;
	if( (iter=m_EachTotalData.find(key))!= m_EachTotalData.end())
	{
		iter->second = iter->second + 1;
	}
	else
	{
		m_EachTotalData.insert(make_pair(key,1));
	}
	
}

bool CStatistic::start_server()
{
	time_t lTime;
	while (1) {	
		sleep(10);
		lTime = time(NULL);
		if(true == timer(lTime))
		{
			do_statistic(lTime);	
		}
		if(true == timer_day(lTime))
		{
			do_statistic_day(lTime);	
		}
	}
	return false;
}
void CStatistic::do_statistic(time_t time)
{
	struct tm* pstTm = localtime(&time);
	char i_time[15];
	memset(i_time,0,15);
	sprintf(i_time,"%04d%02d%02d%02d%02d",pstTm->tm_year+1900,pstTm->tm_mon+1,pstTm->tm_mday,pstTm->tm_hour,pstTm->tm_min);
	STATISTIC_DATA tValue;
	unsigned long long totalNum = 0;
	std::unordered_map<std::string, unsigned long long>::iterator iter1;
	std::unordered_map<std::string, std::list<STATISTIC_DATA>>::iterator iter2;
	for(iter1 = m_EachTotalData.begin();iter1!= m_EachTotalData.end();++iter1 )
	{
		if( (iter2=m_TimerEachData.find(iter1->first))!= m_TimerEachData.end())
		{
			if(iter2->second.size() == m_cycleNum)
			{
				iter2->second.pop_front();
			}
			tValue.count = iter1->second;
			tValue.time = i_time;
			iter2->second.push_back(tValue);
		}
		else
		{
			tValue.count = iter1->second;
			tValue.time = i_time;
			std::list<STATISTIC_DATA> tmp;
			tmp.push_back(tValue);
			m_TimerEachData.insert(make_pair(iter1->first,tmp));
		}

		totalNum = totalNum + iter1->second;
	}

	tValue.count = totalNum;
	tValue.time = i_time;
	if( m_TimerTotalData.size() == m_cycleNum )
	{
		m_TimerTotalData.pop_front();
	}
	m_TimerTotalData.push_back(tValue);
		
}
void CStatistic::do_statistic_day(time_t time)
{
	char sFileName[64] = {0};
	char pathName[256]= {0};
	
	time_t beforedaytime = time - 86400;
	struct tm* pstTm = localtime(&beforedaytime);
	sprintf(sFileName, "%04d%02d%02d",pstTm->tm_year+1900,pstTm->tm_mon+1,pstTm->tm_mday);
	
	strcpy(pathName, m_sStatisticPath);
	strcat(pathName, "DayData");
	if (access(pathName, F_OK) != 0) {
		mkdir(pathName, 0766);
	}
	strcat(pathName, "/");
	strcat(pathName, sFileName);
	
	int fd = open(pathName, O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (fd < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "open file%s err:%s\n", pathName, strerror(errno));
		return;	
	}

	
	char sValue[256];
	unsigned long long totalNum = 0;
	std::unordered_map<std::string, unsigned long long>::iterator iter1;
	for(iter1 = m_EachTotalData.begin();iter1!= m_EachTotalData.end();++iter1 )
	{
		memset(sValue, 0, sizeof(sValue));
		sprintf(sValue, "%s,%llu\n", iter1->first.c_str(), iter1->second);
		write(fd, sValue, strlen(sValue));	
		totalNum = totalNum + iter1->second;
	}

	char sTNum[24];
	memset(sTNum, 0, sizeof(sTNum));
	sprintf(sTNum, "%llu", totalNum);
	write(fd, sTNum, sizeof(sTNum));

	close(fd);
}

bool CStatistic::timer(time_t time)
{
	if(m_nextTime > time)
			return false;
	m_nextTime = m_nextTime + 60*m_intervalTime;
	return true;
}
bool CStatistic::timer_day(time_t mtime)
{
	mtime = mtime + m_timezone*60*60;
	static time_t nextTime = (time(NULL)+m_timezone*60*60)/86400*86400 + 86400;
	if(nextTime > mtime)
			return false;
	nextTime = nextTime + 86400;
	return true;
}

