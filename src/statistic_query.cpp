#include "statistic_query.h"

extern char m_sStatisticPath[256];
#define READ_SIZE (1024 * 1024 * 32)

void statistic_add_json_response(std::string key,std::string time,unsigned long long count,Json::Value& json_response)
{
	Json::Value key_value;
	char scount[64] = {0};
	sprintf(scount,"%llu",count);
	
	key_value[REST_TIME] = time;
	key_value[REST_NUM] = scount;	
	json_response[key].append(key_value);
}

void CRestQryStatistic::init(std::unordered_map<std::string, std::list<STATISTIC_DATA>>* pTimerEach,std::list<STATISTIC_DATA>* pTimerTotal,std::unordered_map<std::string, unsigned long long>* pReal)
{
	m_pTimerEachData = pTimerEach;
	m_pTimerTotalData = pTimerTotal;
	m_pEachTotalData = pReal;
}
void CRestQryStatistic::query_statistic_realeach(Json::Value& json_response,PQUERY_STATISTIC_REALEACH_CON pquery_con)
{
	try
	{
        time_t lTime = time(NULL);
        struct tm* pstTm = localtime(&lTime);
        char i_time[15];
        memset(i_time,0,15);
        sprintf(i_time,"%04d%02d%02d%02d%02d%02d",pstTm->tm_year+1900,pstTm->tm_mon+1,pstTm->tm_mday,pstTm->tm_hour,pstTm->tm_min,pstTm->tm_sec);
        
        std::unordered_map<std::string, std::list<STATISTIC_DATA>>::iterator iter;
        std::unordered_map<std::string, unsigned long long>::iterator iterreal;
        std::list<STATISTIC_DATA>::iterator iterlist,lastiter;
        std::list<std::string>::iterator itercon;
        for(itercon = pquery_con->key_lists.begin();itercon != pquery_con->key_lists.end();++itercon)
        {
        	if(pquery_con->type == QUERY_CUMULATIVE)
        	{
        		 if((iter=m_pTimerEachData->find(*itercon))!= m_pTimerEachData->end())
          		  {
           		     for(iterlist = iter->second.begin();iterlist != iter->second.end();++iterlist)
            		 {
            		 	statistic_add_json_response(*itercon,iterlist->time,iterlist->count,json_response);
                	}
            	}
        	}

		    if(pquery_con->type == QUERY_DIFFER)
        	{
        		 if((iter=m_pTimerEachData->find(*itercon))!= m_pTimerEachData->end())
          		  {
          		  		if(!iter->second.empty())
          		  		{
          		  			iterlist = iter->second.begin();
          		  			for(lastiter=iter->second.begin(),++iterlist;iterlist != iter->second.end();++iterlist)
            				{
								statistic_add_json_response(*itercon,iterlist->time,iterlist->count - lastiter->count,json_response);
								lastiter = iterlist;
                			}
          		  		
							if((iterreal = m_pEachTotalData->find(*itercon))!= m_pEachTotalData->end())
            				{
            					statistic_add_json_response(*itercon,i_time,iterreal->second - lastiter->count,json_response);
            				}
          		  		}	
            	 }
				 continue;
        	}
		   
            if((iterreal = m_pEachTotalData->find(*itercon))!= m_pEachTotalData->end())
            {
            	statistic_add_json_response(*itercon,i_time,iterreal->second,json_response);
                
            }
        }
        json_response[REST_CODE] = REST_OK;
        json_response[REST_MSG] = REST_SUCCESS;
        
        
	}
	catch (std::exception & ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query statistic error:%s\n", ex.what());
	}
}
void CRestQryStatistic::query_statistic_realtotal(Json::Value& json_response,PQUERY_STATISTIC_REALTOTAL_CON pquery_con)
{
	try
	{

        std::list<STATISTIC_DATA>::iterator iter,lastiter;
		unsigned long long curTotal = 0;
        std::unordered_map<std::string, unsigned long long>::iterator iterreal;
        for(iterreal = m_pEachTotalData->begin();iterreal !=m_pEachTotalData->end();++iterreal)
        {
            curTotal = curTotal + iterreal->second;
        }
        
        time_t lTime = time(NULL);
        struct tm* pstTm = localtime(&lTime);
        char i_time[15];
        memset(i_time,0,15);
        sprintf(i_time,"%04d%02d%02d%02d%02d%02d",pstTm->tm_year+1900,pstTm->tm_mon+1,pstTm->tm_mday,pstTm->tm_hour,pstTm->tm_min,pstTm->tm_sec);
        Json::Value key_value;

		
		if(pquery_con->type == QUERY_CUMULATIVE)
        {
        	for(iter = m_pTimerTotalData->begin();iter != m_pTimerTotalData->end();++iter)
        	{
        		statistic_add_json_response(REST_RESULT,iter->time,iter->count,json_response);
        	}
		}

		if(pquery_con->type == QUERY_DIFFER)
        {
        	if(!m_pTimerTotalData->empty())
        	{
        		iter = m_pTimerTotalData->begin();
        		for(lastiter = m_pTimerTotalData->begin(),++iter;iter != m_pTimerTotalData->end();++iter)
        		{
        			statistic_add_json_response(REST_RESULT,iter->time,iter->count - lastiter->count,json_response);
					lastiter = iter;
        		}
				statistic_add_json_response(REST_RESULT,i_time,curTotal - lastiter->count,json_response);
        	}
        	json_response[REST_CODE] = REST_OK;
        	json_response[REST_MSG] = REST_SUCCESS;
			return;
		}
        
        statistic_add_json_response(REST_RESULT,i_time,curTotal,json_response);
              
        json_response[REST_CODE] = REST_OK;
        json_response[REST_MSG] = REST_SUCCESS;
        
        
	}
	catch (std::exception & ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query statistic error:%s\n", ex.what());
	}
}

void CRestQryStatistic::query_statistic_daytotal(Json::Value& json_response,PQUERY_STATISTIC_DAYTOTAL_CON pquery_con)
{
	try
	{         		
		char sPathName[256] = {0};
		strcpy(sPathName, m_sStatisticPath);
		strcat(sPathName, "DayData/");
		int endTime = atoi(pquery_con->end_time.c_str());
		int startTime = atoi(pquery_con->begin_time.c_str());
		reduce_days2(startTime, 2);
		int nowDay = get_local_time_days();
		std::vector<STATISTIC_DATA> sData;
		do
		{
			char sFileName[256] = {0};
			strcpy(sFileName, sPathName);
			STATISTIC_DATA tmpData;
			char i_time[11];
			memset(i_time,0,11);
			sprintf(i_time,"%d",endTime);
			strcat(sFileName, i_time);
			
			if(nowDay == endTime)
			{
				unsigned long long curTotal = 0;
        		std::unordered_map<std::string, unsigned long long>::iterator iterreal;
        		for(iterreal = m_pEachTotalData->begin();iterreal !=m_pEachTotalData->end();++iterreal)
        		{
            		curTotal = curTotal + iterreal->second;
        		}
				tmpData.time = i_time;
				tmpData.count = curTotal;
				sData.push_back(tmpData);
				reduce_days2(endTime, 1);
				continue;
			}

			reduce_days2(endTime, 1);
			int fd = open(sFileName, O_RDONLY, 0666);
			if (fd < 0)
			{			
				continue;
			}

			if (lseek(fd, -24, SEEK_END) < 0)
			{
				close(fd);
				continue;
			}

			char sReadBuf[64];
			memset(sReadBuf, 0, sizeof(sReadBuf));
			read(fd, sReadBuf, sizeof(sReadBuf));

			tmpData.time = i_time;
			tmpData.count = atoll(sReadBuf);
			sData.push_back(tmpData);
			
		}while(endTime !=startTime);

		json_response[REST_CODE] = REST_OK;
        json_response[REST_MSG] = REST_SUCCESS; 
		if(sData.size() == 0)
		{
			return;
		}
		for(int i = 0; i< (int)sData.size() - 1;i++)
		{
			statistic_add_json_response(REST_RESULT,sData[i].time,sData[i].count - sData[i+1].count,json_response);
		}
		if(sData[sData.size() - 1].time >= pquery_con->begin_time )
		{
			statistic_add_json_response(REST_RESULT,sData[sData.size() - 1].time,sData[sData.size() - 1].count,json_response);
		}          
        
	}
	catch (std::exception & ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query statistic error:%s\n", ex.what());
	}
}
void CRestQryStatistic::query_statistic_dayeach(Json::Value& json_response,PQUERY_STATISTIC_DAYEACH_CON pquery_con)
{
	try
	{       
		char *sReadBuf = (char*)malloc(READ_SIZE);
		if(sReadBuf == NULL )
			return;
		char sPathName[256] = {0};
		strcpy(sPathName, m_sStatisticPath);
		strcat(sPathName, "DayData/");
		int endTime = atoi(pquery_con->end_time.c_str());
		int startTime = atoi(pquery_con->begin_time.c_str());
		reduce_days2(startTime, 2);
		int nowDay = get_local_time_days();
		std::unordered_map<std::string,std::vector<STATISTIC_DATA>> sData;
		std::unordered_map<std::string,std::vector<STATISTIC_DATA>>::iterator dataItor;
		for (std::list<std::string>::iterator itor=pquery_con->key_lists.begin(); itor != pquery_con->key_lists.end(); ++itor)
		{
			std::vector<STATISTIC_DATA> vData;
			sData.insert(make_pair(*itor, vData));
		}
					
		do
		{
			//memset(sReadBuf, 0,READ_SIZE);
			char sFileName[256] = {0};
			strcpy(sFileName, sPathName);
			STATISTIC_DATA tmpData;
			char i_time[11];
			memset(i_time,0,11);
			sprintf(i_time,"%d",endTime);
			strcat(sFileName, i_time);

			if(nowDay == endTime)
			{
				std::unordered_map<std::string, unsigned long long>::iterator iterreal;
				for(dataItor = sData.begin();dataItor != sData.end();++dataItor)
				{
					if((iterreal= m_pEachTotalData->find(dataItor->first)) !=m_pEachTotalData->end() )
					{
						STATISTIC_DATA tmp;
						tmp.time = i_time;
						tmp.count= iterreal->second;
						dataItor->second.push_back(tmp);
					}
				}
				reduce_days2(endTime, 1);
				continue;
			}
			
			reduce_days2(endTime, 1);
			int fd = open(sFileName, O_RDONLY, 0666);
			if (fd < 0)
			{			
				continue;
			}
			read(fd, sReadBuf, READ_SIZE);
			for(dataItor = sData.begin();dataItor != sData.end();++dataItor)
			{
				char* pKey=strstr(sReadBuf,dataItor->first.c_str());
				if(pKey != NULL)
				{
					char sKey[10] = {0};
					STATISTIC_DATA tmp;
					tmp.time = i_time;
					sscanf(pKey, "%[^,],%llu", sKey, &tmp.count);
					dataItor->second.push_back(tmp);
				}
			}
			
		}while(endTime !=startTime);

		json_response[REST_CODE] = REST_OK;
        json_response[REST_MSG] = REST_SUCCESS; 

		for(dataItor = sData.begin();dataItor != sData.end();++dataItor)
		{
			if(dataItor->second.size() == 0)
			{
				continue;
			}
			for(int i = 0; i< (int)dataItor->second.size() - 1;i++)
			{
				statistic_add_json_response(dataItor->first,(dataItor->second)[i].time,dataItor->second[i].count - dataItor->second[i+1].count,json_response);
			}
			if(dataItor->second[dataItor->second.size() - 1].time >= pquery_con->begin_time )
			{
				statistic_add_json_response(dataItor->first,dataItor->second[dataItor->second.size() - 1].time,dataItor->second[dataItor->second.size() - 1].count,json_response);
			} 
		}
		if(sReadBuf != NULL )
			free(sReadBuf);
	}
	catch (std::exception & ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query statistic error:%s\n", ex.what());
	}
}

