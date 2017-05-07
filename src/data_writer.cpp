#include "data_writer.h"
#include <sstream>

extern bool g_isstart_pubsub;
extern bool g_isstart_statistic;
GPSINFO_S g_gpsInfoColumn;

CWriteData::CWriteData(int* pfd, TIDECLIENT_S* psClientInfo, unordered_map<string, \
						vector<GPSVALUE_S> >** ppvGpsInfo, char* psMaxTime, GpsidWriter* pGpsidWriter, hiredis_proxy* preids_proxy,CStatistic* pstatistic)
:m_pLogfd(pfd), m_pstClientInfo(psClientInfo), m_ppumGpsData(ppvGpsInfo),m_psMaxTime(psMaxTime),m_pGpsidWriter(pGpsidWriter),m_predis_proxy(preids_proxy),m_pstatistic(pstatistic)
{
}

CWriteData::~CWriteData()
{
}

int CWriteData::WriteDataCore()
{
	char sRetMsg[1024];
	memset(sRetMsg, 0, 1024);
	int iTotal = 0;
				
	iTotal = Write();
	if (iTotal < 0) {
		return -1;
	}
	strcpy(sRetMsg, "SUCC");
	if (Reply(iTotal, GPS_SUCCESS, sRetMsg) < 0) {
		return -1;
	}
	return 0;	
}

int CWriteData::WriteToMem(char* psValue, int iLength)
{
	GPSVALUE_S stValue;
	int iLen = 0;
	int iValLen = 0;
	int iWriteLen = 0;
	char sKey[32];
	char sVal[128];
	int iKeyLen = 0, iTimeLen = 0, iLatLen = 0, iLongLen = 0, iExpandLen = 0;
	memset(sVal, 0, sizeof(sVal));
	char* psVal = sVal;
	while (iLen < iLength) 
	{
		switch (*psValue) 
		{
			case TIDE_KEY:
				{
					iValLen = iKeyLen = ntohl(*(int*)(psValue + 1));
					memcpy(sKey, psValue + 5, iValLen);	
					sKey[iValLen] = 0;
				}
			break;
			case TIDE_TIME:
				{
					iValLen = iTimeLen = ntohl(*(int*)(psValue + 1));
					memcpy(stValue.m_sTime, psValue + 5, iValLen);	
					//get max time (year month day hour)
					if (strncmp(stValue.m_sTime, m_psMaxTime, 10) > 0) 
					{
						memcpy(m_psMaxTime, stValue.m_sTime, 10);
					}
				}
			break;
			case TIDE_LAT:
				{
					iValLen = iLatLen = ntohl(*(int*)(psValue + 1));
					if (iLatLen > 9) {
						iLatLen = 9;
					}
					memcpy(stValue.m_sLat, psValue + 5, iLatLen);	
				}
			break;
			case TIDE_LONG:
				{
					iValLen = iLongLen = ntohl(*(int*)(psValue + 1));
					if (iLongLen > 10) {
						iLongLen = 10;
					}
					memcpy(stValue.m_sLong, psValue + 5, iLongLen);	
				}
			break;
			case TIDE_EXPAND:
				{
					iValLen = iExpandLen = ntohl(*(int*)(psValue + 1));
					memcpy(stValue.m_sRemain, psValue + 5, iValLen);	
				}
			break;
			default:
			break;	
		}
		psValue += iValLen + 5;
		iLen += iValLen + 5;
	}
	memcpy(psVal, sKey, iKeyLen);	
	*(psVal + iKeyLen) = ',';	
	iWriteLen = iKeyLen + 1;
	psVal += iKeyLen + 1;
	
	memcpy(psVal, stValue.m_sTime, iTimeLen);		
	*(psVal + iTimeLen) = ',';	
	iWriteLen += iTimeLen + 1;
	psVal += iTimeLen + 1;
	
	memcpy(psVal, stValue.m_sLat, iLatLen);		
	*(psVal + iLatLen) = ',';	
	iWriteLen += iLatLen + 1;
	psVal += iLatLen + 1;
	
	memcpy(psVal, stValue.m_sLong, iLongLen);		
	*(psVal + iLongLen) = ',';	
	iWriteLen += iLongLen + 1;
	psVal += iLongLen + 1;
		
	memcpy(psVal, stValue.m_sRemain, iExpandLen);		
	*(psVal + iExpandLen) = '\n';	
	iWriteLen += iExpandLen + 1;
	
	write(*m_pLogfd, sVal, iWriteLen);	
	//write gpsid to mem map
	m_pGpsidWriter->InsertGpsIDMsg(sKey);
	//copy one data for publish to redis
	if(g_isstart_pubsub)
	{
		m_predis_proxy->m_pserver->push(sKey, stValue);
	}

	if(g_isstart_statistic)
	{
		m_pstatistic->write_data(sKey);
	}
	unordered_map<string, vector<GPSVALUE_S> >::iterator iterGpsInfo;
	if ((iterGpsInfo = (*m_ppumGpsData)->find(sKey)) != (*m_ppumGpsData)->end()) 
	{
        	(iterGpsInfo->second).push_back(stValue);
	}
	else 
	{
		vector<GPSVALUE_S> vecVal;
		vecVal.push_back(stValue);
		(*m_ppumGpsData)->insert(pair<string, vector<GPSVALUE_S> >(sKey, vecVal));
	}		
    return 0;
}

int CWriteData::Write()
{
	char* psPacket = m_pstClientInfo->m_psPacket;
	if (*psPacket != 0x01) {
		return -1;
	}
	int iTotal = ntohl(*(int*)(psPacket + 1));
	psPacket += PRO_HEAD_LEN;
	char* psBegin = psPacket;
	int iNum = 0;
	
	while(iNum < iTotal) 
	{
		int iValLen = ntohl(*(int*)psBegin);
		psBegin += 4;
		int len = WriteToMem(psBegin, iValLen);
		if (len != 0)
		{
			Reply(iNum, GPS_FAILURE, psBegin + 4);
			return -1;
		};
		psBegin += iValLen;
		++iNum;
	}
	
	return iTotal;
}

int CWriteData::Reply(unsigned int uiTotal, char cResult, char* psRetMsg)
{
	char* psSendBuf = m_pstClientInfo->m_psPacket;
	char* psSend = m_pstClientInfo->m_psPacket;
	int iSendLen = PRO_HEAD_LEN;
	//type
	*psSendBuf = TIDE_INS;
	psSendBuf += 1;
	//total
	*(int*)psSendBuf = htonl(uiTotal);
	psSendBuf += 4;			
	//result
	*psSendBuf = cResult;
	psSendBuf += 1;
	//length
	*(int*)psSendBuf = htonl(strlen(psRetMsg));
	psSendBuf += 4;			
	//value	
	sprintf(psSendBuf, "%s", psRetMsg);
	iSendLen += strlen(psRetMsg);
	m_cSend.SetSocket(m_pstClientInfo->m_iSocket);
	if (m_cSend.TcpSend(psSend, iSendLen) < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "send err:%s", strerror(errno));
		return -1;
	}
	return 0;
}


rest_data_writer::rest_data_writer(unordered_map<string, vector<GPSVALUE_S> >**ppnum_tide, int* pfd, std::shared_ptr<hiredis_proxy> predis_proxy,GpsidWriter* pGpsidWriter,CStatistic* pstatistic)
{
	this->m_ppmem_tide = ppnum_tide;
	this->m_pfd = pfd;
	this->m_predis_proxy = predis_proxy;
	this->m_pGpsidWriter = pGpsidWriter;
	this->m_pstatistic = pstatistic;
}

rest_data_writer::~rest_data_writer()
{

}

void rest_data_writer::write_gpsdata(std::string body, Json::Value& json_response)
{
	try
	{
		Json::Reader json_reader;
		Json::Value data_root;
		if (!json_reader.parse(body.c_str(), data_root))
		{
			//response
			json_response[REST_CODE] = RESPONSE_ARRAY[1].code;
			json_response[REST_MSG] = RESPONSE_ARRAY[1].description;
			return;
		}
		unordered_map<string, vector<GPSVALUE_S> >::iterator it_find;
		Json::Value write_array = data_root["write_data"];

		for (unsigned int i = 0; i < write_array.size(); i++)
		{
			std::string key = write_array[i][KEY].asString();
			
			GPSVALUE_S gps_data;
			int key_len = key.length();
			int time_len = write_array[i][TIME].asString().length();
			int longitude_len = write_array[i][LONGITUDE].asString().length();
			int latitude_len = write_array[i][LATITUDE].asString().length();
			int expand_len = write_array[i][EXPAND].asString().length();
						
			memcpy(gps_data.m_sTime, write_array[i][TIME].asString().c_str(), time_len);
			memcpy(gps_data.m_sLat, write_array[i][LATITUDE].asString().c_str(), latitude_len);
			memcpy(gps_data.m_sLong, write_array[i][LONGITUDE].asString().c_str(), longitude_len);
			memcpy(gps_data.m_sRemain, write_array[i][EXPAND].asString().c_str(), expand_len);

			char gps_filestr[MAX_LENGTH_ONE_DATA];
			memset(gps_filestr, 0, sizeof(gps_filestr));
			char* pfilestr = gps_filestr;

			int write_len = 0;
			memcpy(pfilestr, key.c_str(), key_len);
			*(pfilestr + key_len) = ',';
			write_len = key_len + 1;
			pfilestr += key_len + 1;

			memcpy(pfilestr, gps_data.m_sTime, time_len);
			*(pfilestr + time_len) = ',';
			write_len += time_len + 1;
			pfilestr += time_len + 1;

			memcpy(pfilestr, gps_data.m_sLat, latitude_len);
			*(pfilestr + latitude_len) = ',';
			write_len += latitude_len + 1;
			pfilestr += latitude_len + 1;

			memcpy(pfilestr, gps_data.m_sLong, longitude_len);
			*(pfilestr + longitude_len) = ',';
			write_len += longitude_len + 1;
			pfilestr += longitude_len + 1;

			memcpy(pfilestr, gps_data.m_sRemain, expand_len);
			*(pfilestr + expand_len) = '\n';
			write_len += expand_len + 1;

			m_pGpsidWriter->InsertGpsIDMsg(key);
			//
			if(g_isstart_pubsub)
			{
				m_predis_proxy->m_pserver->push(key, gps_data);
			}
			if(g_isstart_statistic)
			{
				m_pstatistic->write_data(key);
			}
			write(*m_pfd, gps_filestr, write_len);
			if ((it_find = (*m_ppmem_tide)->find(key)) != (*m_ppmem_tide)->end())
			{
				(it_find->second).push_back(gps_data);
			}
			else
			{
				vector<GPSVALUE_S> gps_vec;
				gps_vec.push_back(gps_data);
				(*m_ppmem_tide)->insert(pair<string, vector<GPSVALUE_S> >(key, gps_vec));
			}
		}
		json_response[REST_CODE] = RESPONSE_ARRAY[0].code;
		json_response[REST_MSG] = RESPONSE_ARRAY[0].description;
	}
	catch (std::exception& ex)
	{
		json_response[REST_CODE] = RESPONSE_ARRAY[2].code;
		json_response[REST_MSG] = RESPONSE_ARRAY[2].description;
		NP_LOG(np_log::NP_ERROR, "[rest]:write data error. msg:%s", ex.what());
	}
}



gps_info_writer::gps_info_writer(int* pfd,unordered_map<string,string>* pGps_info)
{
	this->m_pfd = pfd;
	this->m_pGpsInfo = pGps_info;
}

gps_info_writer::~gps_info_writer()
{

}

void gps_info_writer::write_gpsinfo(std::string body, Json::Value& json_response)
{
	try
	{
		Json::Reader json_reader;
		Json::Value data_root;
		if (!json_reader.parse(body.c_str(), data_root))
		{
			//response
			json_response[REST_CODE] = RESPONSE_ARRAY[1].code;
			json_response[REST_MSG] = RESPONSE_ARRAY[1].description;
			return;
		}
		Json::Value write_array = data_root["write_data"];
		unordered_map<string, string >::iterator it_find;
		char w_value[1024];
		int count = 0;
		for (unsigned int i = 0; i < write_array.size(); i++)
		{
			
			memset(w_value, 0, 1024);
			std::string id = write_array[i][GPS_ID].asString();
			std::string value = write_array[i][GPS_INFO_VALUE].asString();
			if(id.empty() || value.empty())
			{
				continue;
			}
			if ((it_find = m_pGpsInfo->find(id)) != m_pGpsInfo->end())
			{
				it_find->second = value;
			}
			else
			{
				m_pGpsInfo->insert(make_pair(id,value));
			}
			if(g_gpsInfoColumn.initFlag == 0)
			{
				int i = 0;
				char srcStr[512] = {0};
				char tmpStr[32][64] = {0};
				strcpy(srcStr,value.c_str());
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
			sprintf(w_value,"%s|%s\n",id.c_str(),value.c_str());
			write(*m_pfd, w_value, strlen(w_value));
			count++;
		}
		json_response[REST_CODE] = RESPONSE_ARRAY[0].code;
		json_response[REST_MSG] = RESPONSE_ARRAY[0].description;
		json_response[REST_NUM] = count;
	}
	catch (std::exception& ex)
	{
		json_response[REST_CODE] = RESPONSE_ARRAY[2].code;
		json_response[REST_MSG] = RESPONSE_ARRAY[2].description;
		NP_LOG(np_log::NP_ERROR, "[rest]:write data error. msg:%s", ex.what());
	}
}

