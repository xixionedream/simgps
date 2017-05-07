
#include "hislocus_query.h"

extern char g_sLocusPath[64];

static bool GetStartLine(char* psLine, unsigned int uiLineLength, const char* psBeginTime)
{
	int iLen = uiLineLength - 2;
	char* psEnd = psLine + iLen;
	long lBeginTime = Atol(psLine, 0, 14);
	long lEndTime = 0; 
	
	while (*psEnd-- != '|' && iLen-- > 0);
	psEnd += 2;
	char* psTime = strchr(psEnd, GPS_VAL_SEP);
	if (psTime) 
	{
		if (iLen > 0) 
		{
			lEndTime = lBeginTime + Atoi(psEnd, 0, psTime - psEnd);
		} else 
		{
			lEndTime = lBeginTime;
		}
	} 
	else 
	{
		return false;
	}	
	if (atol(psBeginTime) > lEndTime) 
	{
		return false;
	}
	return true;
}

CRestQryHisLocus::CRestQryHisLocus()
{

}

CRestQryHisLocus::~CRestQryHisLocus()
{

}

void CRestQryHisLocus::init(unordered_map<string, vector<GPSVALUE_S> >** ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_HIS_LOCUS_CON pquery_con)
{
	this->m_ppmem_tide = ppnum_tide;
	this->m_pmem_tide_data1 = pnum_tide_data1;
	this->m_pmem_tide_data2 = pnum_tide_data2;
	this->m_pquery_con = pquery_con;

	m_end_flag = false;
}
int  CRestQryHisLocus::parse_file_gpsdata(char* psLine, const char* psBeginTime, const char* psEndTime, const char* psKey, Json::Value& json_response, int& query_res_size,int page_max_size, int page_min_size)
{
	long lBeginTime = Atol(psBeginTime, 0, 14);
	long lEndTime = Atol(psEndTime, 0, 14);
	char* psBegin = psLine;
	char* psValuePos = NULL;
	char* psColumnPos = NULL;
	char sTime[15];
	char sLat[20];
	char sLong[20];

	long lTime = 0;
	int iAroundLen = strlen(GPS_AROUND_SEP);
	//first
	psValuePos = strstr(psBegin, GPS_AROUND_SEP);
	*psValuePos = 0;
	psColumnPos = strchr(psBegin, GPS_VAL_SEP);
	if (psColumnPos == NULL) 
	{
		return -1;
	}
	*psColumnPos = 0;
	long lBaseTime = atol(psBegin);

	psBegin = psColumnPos + 1;
	psColumnPos = strchr(psBegin, GPS_VAL_SEP);
	if (psColumnPos == NULL) 
	{
		return -1;
	}
	*psColumnPos = 0;
	long lBaseLat = atol(psBegin);

	psBegin = psColumnPos + 1;
	psColumnPos = strchr(psBegin, GPS_VAL_SEP);
	if (psColumnPos == NULL) 
	{
		return -1;
	}
	*psColumnPos = 0;
	long lBaseLong = atol(psBegin);

	psBegin = ++psColumnPos;
	
	if (lBaseTime >= lBeginTime && lBaseTime <= lEndTime)
	{
				
		ltos(lBaseTime,sTime);
		memset(sLat, 0, sizeof(sLat));
		sprintf(sLat, "%lf", (double)lBaseLat / PRECISION);
		memset(sLong, 0, sizeof(sLong));
		sprintf(sLong, "%lf", (double)lBaseLong / PRECISION);

		Json::Value gps_value;
		gps_value[REST_KEY] = psKey;
		gps_value[REST_TIME] = sTime;
		gps_value[REST_LATITUDE] = sLat;
		gps_value[REST_LONGTITUDE] = sLong;
		gps_value[REST_EXPAND] = psBegin;

		query_res_size++;
		if (query_res_size > page_min_size && query_res_size <= page_max_size)
		{
			json_response[REST_RESULT].append(gps_value);
		}
		if (query_res_size == page_max_size)
		{
			m_end_flag = true;
			return 2;
		}
						
	}

	psBegin = psValuePos + iAroundLen;

	while (1) 
	{
		if (*psBegin == 0) 
			break;

		//get erery gps info
		psValuePos = strstr(psBegin, GPS_AROUND_SEP);
		if (psValuePos == NULL) 
		{
			break;
		}
		*psValuePos = 0;
		//check time
		psColumnPos = strchr(psBegin, GPS_VAL_SEP);
		if (psColumnPos == NULL) 
		{
			psBegin = psValuePos + iAroundLen;
			continue;
		}
		*psColumnPos = 0;
		lTime = atol(psBegin) + lBaseTime;
		if (lTime < lBeginTime) 
		{
			psBegin = psValuePos + iAroundLen;
			continue;
		}

		if (lTime > lEndTime) 
		{
			m_end_flag = true;
			return 2;
		}
		psBegin = psColumnPos + 1;
		psColumnPos = strchr(psBegin, GPS_VAL_SEP);
		if (psColumnPos == NULL) 
		{
			psBegin = psValuePos + iAroundLen;
			continue;
		}
		else 
		{
			*psColumnPos = 0;
			memset(sLat, 0, 20);
			sprintf(sLat, "%lf", (double)(atol(psBegin) + lBaseLat) / PRECISION);
		}

		psBegin = psColumnPos + 1;
		psColumnPos = strchr(psBegin, GPS_VAL_SEP);
		if (psColumnPos == NULL) 
		{
			psBegin = psValuePos + iAroundLen;
			continue;
		}
		else 
		{
			*psColumnPos = 0;
			memset(sLong, 0, 20);
			sprintf(sLong, "%lf", (double)(atol(psBegin) + lBaseLong) / PRECISION);
		}
		psBegin = ++psColumnPos;

		ltos(lTime, sTime);
		Json::Value gps_value;
		gps_value[REST_KEY] = psKey;
		gps_value[REST_TIME] = sTime;
		gps_value[REST_LATITUDE] = sLat;
		gps_value[REST_LONGTITUDE] = sLong;
		gps_value[REST_EXPAND] = psBegin;
		
		if (psBegin)
		{
			gps_value[REST_EXPAND] = psBegin;
		}
		else
		{
			gps_value[REST_EXPAND] = "";
		}
		
		query_res_size++;
		if (query_res_size > page_min_size && query_res_size <= page_max_size)
		{
			json_response[REST_RESULT].append(gps_value);
		}
		if (query_res_size == page_max_size)
		{
			m_end_flag = true;
			return 2;
		}
			
		psBegin = psValuePos + iAroundLen;
	}
	return 0;
}

void CRestQryHisLocus::query_his_locus_infile(Json::Value& json_response, int& query_res_size,int page_max_size, int page_min_size)
{
	//query in file
	char sReadBuf[1024 * 360];
	bool bCheckFlag = true;
	unsigned long ulKey = Fnv64a(this->m_pquery_con->key.c_str(), this->m_pquery_con->key.size());
	int iIndexNum = 0;
	int iIndex = 0;
	char sLocusFileName[64], sLocusIndexFileName[64];
	LOCUSINDEX_S stLocusIndex;
	int iIndexSize = sizeof(stLocusIndex);
	LOCUSINDEX_S stLocusIndexs[256];
	int iReadLength = 0;

	int iBeginHours = Atoi(this->m_pquery_con->begin_time.c_str(), 0, 10);
	int iEndHours = Atoi(this->m_pquery_con->end_time.c_str(), 0, 10);
	while (iBeginHours <= iEndHours)
	{
		memset(sLocusIndexFileName, 0, sizeof(sLocusIndexFileName));
		memset(sLocusFileName, 0, sizeof(sLocusFileName));
		sprintf(sLocusFileName, "%s%d_%lu", g_sLocusPath, iBeginHours, ulKey % MOD);
		iBeginHours = GetDateByHour(iBeginHours);
		strcpy(sLocusIndexFileName, sLocusFileName);
		strcat(sLocusIndexFileName, "_index");
		//get index info
		int fdLocusIndex = open(sLocusIndexFileName, O_RDONLY, 0666);
		if (fdLocusIndex < 0)
		{
			continue;
		}
		iIndexNum = 0;
		memset(&stLocusIndex, 0, iIndexSize);
		while (read(fdLocusIndex, &stLocusIndex, iIndexSize))
		{
			if (stLocusIndex.m_ulKey == ulKey)
			{
				memcpy(&stLocusIndexs[iIndexNum++], &stLocusIndex, iIndexSize);
			}
			memset(&stLocusIndex, 0, iIndexSize);
		}
		close(fdLocusIndex);
		//get Tide Data	
		int fdLocus = open(sLocusFileName, O_RDONLY, 0666);
		if (fdLocus < 0)
		{
			break;
		}
		for (iIndex = 0; iIndex < iIndexNum; ++iIndex)
		{
			if (lseek(fdLocus, stLocusIndexs[iIndex].m_uiOffset, SEEK_SET) < 0)
			{
				NP_LOG(np_log::NP_ERROR, "lseek to file [%s]:[%u] err\n", sLocusFileName, stLocusIndexs[iIndex].m_uiOffset);
				return;
			}
			iReadLength = stLocusIndexs[iIndex].m_uiLength;
			memset(sReadBuf, 0, sizeof(sReadBuf));
			read(fdLocus, sReadBuf, iReadLength);
			//int iFilterLen = 0;
			if (bCheckFlag)
			{
				if (!GetStartLine(sReadBuf, iReadLength, this->m_pquery_con->begin_time.c_str()))
				{
					continue;
				}
				else
				{
					bCheckFlag = false;
				}
			}
			if (parse_file_gpsdata(sReadBuf, this->m_pquery_con->begin_time.c_str(), this->m_pquery_con->end_time.c_str(), this->m_pquery_con->key.c_str(), json_response, query_res_size, page_max_size, page_min_size) > 0)
			{
				break;
			}
		}

		close(fdLocus);
		if (m_end_flag)
		{
			return;
		}
	}
}

void CRestQryHisLocus::query_his_locus(Json::Value& json_response)
{
	// first query mem which is inside query time
	unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	vector<GPSVALUE_S>::iterator itor_tide_vec;
	int page_min_size = m_pquery_con->cur_page*m_pquery_con->page_size - m_pquery_con->page_size;
	int page_max_size = m_pquery_con->cur_page*m_pquery_con->page_size;
	
	try
	{
		int query_res_size = 0;
		
		//qry in file(begin_time - end_time)
		query_his_locus_infile(json_response, query_res_size,page_max_size,page_min_size);
		
		//query mem1
		if( !m_end_flag)
		{
			itor_tide_data = (*m_ppmem_tide)->find(m_pquery_con->key);
			if (itor_tide_data != (*m_ppmem_tide)->end())
			{
				itor_tide_vec = itor_tide_data->second.begin();
				std::string data_time;
				for (; itor_tide_vec != itor_tide_data->second.end(); ++itor_tide_vec)
				{
					data_time = itor_tide_vec->m_sTime;
					if (data_time > this->m_pquery_con->end_time)
					{
						break;
					}
					else if (data_time >= this->m_pquery_con->begin_time)
					{

						query_res_size++;
						if (query_res_size > page_min_size && query_res_size <= page_max_size)
						{
							add_json_response(&(*itor_tide_vec), itor_tide_data->first, json_response);
						}
						if (query_res_size == page_max_size)
						{
							json_response[REST_CODE] = REST_OK;
							json_response[REST_MSG] = REST_SUCCESS;
							return;
						}	
						
					}
					else
					{
						continue;
					}
				}
			}
		}
		
		json_response[REST_CODE] = REST_OK;
		json_response[REST_MSG] = REST_SUCCESS;
	}
	catch (std::exception& ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsid error:%s\n", ex.what());
	}
}
