
#include "hisaround_query.h"

extern char g_sAroundPath[64];


int GetGpsData(char* psLine, GPSDATA_S* stGpsData)
{
	char* psBegin = psLine;
	//key
	char* psSep = strchr(psBegin, GPS_VAL_SEP);
	if (psSep == NULL) 
	{
		return -1;
	}
	*psSep = 0;	
	stGpsData->m_sKey = psBegin;
	psBegin = psSep + 1;
	//time	
	psSep = strchr(psBegin, GPS_VAL_SEP);
	if (psSep == NULL) 
	{
		return -1;
	}
	*psSep = 0;	
	memcpy(stGpsData->m_sTime, psBegin, strlen(psBegin));
	psBegin = psSep + 1;
	//latitude	
	psSep = strchr(psBegin, GPS_VAL_SEP);
	if (psSep == NULL) 
	{
		return -1;
	}
	*psSep = 0;	
	stGpsData->m_sLat = psBegin;
	psBegin = psSep + 1;
	//longitude
	psSep = strchr(psBegin, GPS_VAL_SEP);
	if (psSep == NULL) 
	{
		return -1;
	}
	*psSep = 0;	
	stGpsData->m_sLong = psBegin;
	psBegin = ++psSep;
	if (psBegin) 
	{
		stGpsData->m_sRemain = psBegin;
	}	
	return 0;
}

CRestQryHisAround::CRestQryHisAround()
{

}

CRestQryHisAround::~CRestQryHisAround()
{

}

void CRestQryHisAround::init(unordered_map<string, vector<GPSVALUE_S> >**ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_HIS_AROUND_CON pquery_con)
{
	this->m_ppmem_tide = ppnum_tide;
	this->m_pmem_tide_data1 = pnum_tide_data1;
	this->m_pmem_tide_data2 = pnum_tide_data2;
	this->m_pquery_con = pquery_con;
}
bool CRestQryHisAround::judge_inrange(double longitude,double latitude,judge_con* con,int type)
{
	bool ret = false;
	switch(type)
	{
		case QRY_AROUND_CIRCLE://circle
			{			
				ret = IsInCircle(longitude,latitude,(con->query_con_circle));
			}
			break;
		case QRY_AROUND_RECTANGLE://rect
		case QRY_AROUND_RECTANGLE_TWOPOINT://rect
			{
				ret = IsInRect(longitude,latitude,(con->query_con_rect));
			}
			break;
		case QRY_AROUND_POLYGON://polygon
			{
				ret = IsInPolygon(longitude,latitude,*(con->query_con_polygon));
			}
			break;
		default:
			break;
	}

	return ret;
}
int CRestQryHisAround::query_mem(int type,void* pquery_con,unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide,unordered_map<string, GPSVALUE_S>& umUnique,std::string begin_time,std::string end_time,Json::Value& json_response)
{
	unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	vector<GPSVALUE_S>::reverse_iterator itor_tide_vec;
	std::string data_time;
	judge_con con;
	switch(type)
	{
		case QRY_AROUND_CIRCLE://circle
			{
				PQUERY_HIS_AROUND_CON p = (PQUERY_HIS_AROUND_CON)pquery_con;				
				con.query_con_circle[0]=p->longitude;
				con.query_con_circle[1]=p->latitude;
				con.query_con_circle[2]=p->redius;
			}
			break;
		case QRY_AROUND_RECTANGLE://rect
			{
				PQUERY_HIS_RECTAROUND_CON p = (PQUERY_HIS_RECTAROUND_CON)pquery_con;
				GetRectangleEdge(p->longitude,p->latitude,p->ew_len,p->sn_len,con.query_con_rect[0],con.query_con_rect[1],con.query_con_rect[2],con.query_con_rect[3]);		
			}
			break;
		case QRY_AROUND_RECTANGLE_TWOPOINT://rect of two points
			{
				PQUERY_HIS_RECTAROUND_TWOPOINT_CON p = (PQUERY_HIS_RECTAROUND_TWOPOINT_CON)pquery_con;	
				con.query_con_rect[0] = p->firstlongitude > p->secondlongitude ? p->firstlongitude:p->secondlongitude;
				con.query_con_rect[1] = p->firstlongitude + p->secondlongitude - con.query_con_rect[0];
				con.query_con_rect[3] = p->firstlatitude > p->secondlatitude ? p->firstlatitude:p->secondlatitude;
				con.query_con_rect[2] = p->firstlatitude + p->secondlatitude - con.query_con_rect[3];
			}
			break;
		case QRY_AROUND_POLYGON://polygon
			{
				PQUERY_HIS_POLYGONAROUND_CON p = (PQUERY_HIS_POLYGONAROUND_CON)pquery_con;
				con.query_con_polygon = &(p->point_array);
			}
			break;
		default:
			return 0;
	}
	
	for (itor_tide_data = (*ppmem_tide)->begin(); itor_tide_data != (*ppmem_tide)->end(); ++itor_tide_data)
	{
		itor_tide_vec = itor_tide_data->second.rbegin();
		for (; itor_tide_vec != itor_tide_data->second.rend(); ++itor_tide_vec)
		{
			data_time = itor_tide_vec->m_sTime;
			if (data_time > end_time)
			{
				continue;
			}
			else if (data_time >= begin_time)
			{
				if (judge_inrange(atof(itor_tide_vec->m_sLong),atof(itor_tide_vec->m_sLat),&con,type))
				{
					if (umUnique.find(itor_tide_data->first) == umUnique.end()) 
					{
						umUnique.insert(make_pair(itor_tide_data->first, *itor_tide_vec));
					} 
					else 
					{
						break;
					}	
				}
			}
			else
			{
				break;
			}
		}
	}
	return 1;
}

void CRestQryHisAround::query_his_around(Json::Value& json_response)
{
	unordered_map<string, GPSVALUE_S> umUnique;
	unordered_map<string, GPSVALUE_S>::iterator unique_iter;
	int count = 0;
	int page_min_size = m_pquery_con->cur_page*m_pquery_con->page_size - m_pquery_con->page_size;
	int page_max_size = m_pquery_con->cur_page*m_pquery_con->page_size;
	std::string begin_time=m_pquery_con->begin_time;
	std::string end_time=m_pquery_con->end_time;
	
	try
	{
		
		int ret = query_mem(QRY_AROUND_CIRCLE,m_pquery_con,m_ppmem_tide,umUnique,begin_time,end_time,json_response);
		if(ret == 0 )
		{				
			return;
		}

		//qry in file
		char sHashCode[HASHCODE_LENGTH];
		if (geohash_encode(this->m_pquery_con->latitude, this->m_pquery_con->longitude, sHashCode, HASHCODE_LENGTH) != 0)
		{
			NP_LOG(np_log::NP_ERROR, "Get hashcode err\n");
		}
		else
		{
			query_file(json_response, umUnique,begin_time,end_time,sHashCode,QRY_AROUND_CIRCLE,m_pquery_con);
		}

		for(unique_iter = umUnique.begin(); unique_iter !=umUnique.end(); ++unique_iter)
		{
			count++;
			if (count > page_min_size && count <= page_max_size)
			{
				add_json_response(&(unique_iter->second), unique_iter->first, json_response);
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

void CRestQryHisAround::query_file(Json::Value& json_response, unordered_map<string, GPSVALUE_S>& unique_keymap,std::string begin_time,std::string end_time,char* sHashCode,int type,void* pquery_con)
{
	char sHashStr[64];
	memset(sHashStr, 0, 64);
	int iHashCnt = 0;
	unordered_map<string, GPSVALUE_S> fileResult;
	unordered_map<string, GPSVALUE_S>::iterator fileResult_iter;
	if (geo_neighbors(sHashCode, sHashStr, 1, &iHashCnt) != 0) 
	{
		NP_LOG(np_log::NP_ERROR, "Get neighbors err\n");
		return;
	}

	char* psHashNeighbor[iHashCnt + 1];
	int iHashNum = 0;
	while (iHashNum < iHashCnt) 
	{
		psHashNeighbor[iHashNum] = &sHashStr[iHashNum * HASHCODE_LENGTH];
		++iHashNum;
	}
	psHashNeighbor[iHashNum] = sHashCode;

	int iOffsets[iHashCnt];
	int iNum = 0;
	int iCurHour = 0;
	int iBeginHours = Atoi(begin_time.c_str(), 0, 10);
	int iEndHours = Atoi(end_time.c_str(), 0, 10);

	
	char sAroundFileName[64];
	char sReadBuf[256];
	int iReadLength = 0;

	judge_con con;
	switch(type)
	{
		case QRY_AROUND_CIRCLE://circle
			{
				PQUERY_HIS_AROUND_CON p = (PQUERY_HIS_AROUND_CON)pquery_con;				
				con.query_con_circle[0]=p->longitude;
				con.query_con_circle[1]=p->latitude;
				con.query_con_circle[2]=p->redius;
			}
			break;
		case QRY_AROUND_RECTANGLE://rect
			{
				PQUERY_HIS_RECTAROUND_CON p = (PQUERY_HIS_RECTAROUND_CON)pquery_con;
				GetRectangleEdge(p->longitude,p->latitude,p->ew_len,p->sn_len,con.query_con_rect[0],con.query_con_rect[1],con.query_con_rect[2],con.query_con_rect[3]);		
			}
			break;
		case QRY_AROUND_RECTANGLE_TWOPOINT://rect of two points
			{
				PQUERY_HIS_RECTAROUND_TWOPOINT_CON p = (PQUERY_HIS_RECTAROUND_TWOPOINT_CON)pquery_con;	
				con.query_con_rect[0] = p->firstlongitude > p->secondlongitude ? p->firstlongitude:p->secondlongitude;
				con.query_con_rect[1] = p->firstlongitude + p->secondlongitude - con.query_con_rect[0];
				con.query_con_rect[3] = p->firstlatitude > p->secondlatitude ? p->firstlatitude:p->secondlatitude;
				con.query_con_rect[2] = p->firstlatitude + p->secondlatitude - con.query_con_rect[3];
			}
			break;
		case QRY_AROUND_POLYGON://polygon
			{
				PQUERY_HIS_POLYGONAROUND_CON p = (PQUERY_HIS_POLYGONAROUND_CON)pquery_con;
				con.query_con_polygon = &(p->point_array);
			}
			break;
		default:
			return;
	}
	iCurHour = iEndHours;
	while (iCurHour >= iBeginHours)
	{		
		iCurHour = GetDateByHour(iEndHours, iNum--);
		for (iHashNum = iHashCnt; iHashNum >= 0; --iHashNum) 
		{
			memset(sAroundFileName, 0, sizeof(sAroundFileName));
			sprintf(sAroundFileName, "%s%d_%s", g_sAroundPath, iCurHour, psHashNeighbor[iHashNum]);
			FILE* fpHisAround = fopen(sAroundFileName, "r");
			if (fpHisAround == NULL) 
			{
				continue;
			}
			//iLastFileHour = iCurHour;
			iOffsets[iHashNum] = 0;
			memset(sReadBuf, 0, 256);
			while (fgets(sReadBuf, 256, fpHisAround)) 
			{
				iReadLength = strlen(sReadBuf);
				iOffsets[iHashNum] += iReadLength;
				sReadBuf[iReadLength - 1] = 0;
				//check time
				GPSDATA_S stGpsData;
				if (GetGpsData(sReadBuf, &stGpsData) != 0) 
				{
					continue;
				}
				if (strncmp(stGpsData.m_sTime,begin_time.c_str(), 14) < 0) 
				{
					continue;
				}
				if (strncmp(stGpsData.m_sTime,end_time.c_str(), 14) > 0) 
				{	
					continue;
				}
				if (!judge_inrange(atof(stGpsData.m_sLong.c_str()),atof(stGpsData.m_sLat.c_str()),&con,type))
				{
					continue;
				}
				if (unique_keymap.find(stGpsData.m_sKey) == unique_keymap.end())
				{					
					GPSVALUE_S tmpValue;
					strcpy(tmpValue.m_sLat,stGpsData.m_sLat.c_str());
					strcpy(tmpValue.m_sLong,stGpsData.m_sLong.c_str());
					strcpy(tmpValue.m_sTime,stGpsData.m_sTime);
					strcpy(tmpValue.m_sRemain,stGpsData.m_sRemain.c_str());
					if ((fileResult_iter = fileResult.find(stGpsData.m_sKey)) == fileResult.end()) 
					{
						fileResult.insert(make_pair(stGpsData.m_sKey, tmpValue));
					} 
					else 
					{
						if (strncmp(stGpsData.m_sTime,fileResult_iter->second.m_sTime, 14) < 0) 
						{	
							continue;
						}
						fileResult_iter->second = tmpValue;
					}
				}
				else
				{
					continue;
				}
				
			}
			fclose(fpHisAround);
			
		}
		for(fileResult_iter = fileResult.begin();fileResult_iter != fileResult.end();++fileResult_iter)
		{
			if (unique_keymap.find(fileResult_iter->first) == unique_keymap.end()) 
			{
				unique_keymap.insert(make_pair(fileResult_iter->first, fileResult_iter->second));
			} 
		}
		fileResult.clear();
	}

	
}
void CRestQryHisAround::query_his_rectaround(Json::Value& json_response,PQUERY_HIS_RECTAROUND_CON pquery_con)
{
	unordered_map<string, GPSVALUE_S> umUnique;	
	unordered_map<string, GPSVALUE_S>::iterator unique_iter;
	int count = 0;
	int page_min_size = pquery_con->cur_page*pquery_con->page_size - pquery_con->page_size;
	int page_max_size = pquery_con->cur_page*pquery_con->page_size;
	std::string begin_time=pquery_con->begin_time;
	std::string end_time=pquery_con->end_time;
	try
	{
		int ret = query_mem(QRY_AROUND_RECTANGLE,pquery_con,m_ppmem_tide,umUnique,begin_time,end_time,json_response);
		if(ret == 0 )
		{
			return;
		}

		char sHashCode[HASHCODE_LENGTH];
		if (geohash_encode(pquery_con->latitude, pquery_con->longitude, sHashCode, HASHCODE_LENGTH) != 0)
		{
			NP_LOG(np_log::NP_ERROR, "Get hashcode err\n");
		}
		else
		{
			query_file(json_response, umUnique,begin_time,end_time,sHashCode,QRY_AROUND_RECTANGLE,pquery_con);
		}
		for(unique_iter = umUnique.begin(); unique_iter !=umUnique.end(); ++unique_iter)
		{
			count++;
			if (count > page_min_size && count <= page_max_size)
			{
				add_json_response(&(unique_iter->second), unique_iter->first, json_response);
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
void CRestQryHisAround::query_his_rectaround_twopoint(Json::Value& json_response,PQUERY_HIS_RECTAROUND_TWOPOINT_CON pquery_con)
{
	unordered_map<string, GPSVALUE_S> umUnique;	
	unordered_map<string, GPSVALUE_S>::iterator unique_iter;
	int count = 0;
	int page_min_size = pquery_con->cur_page*pquery_con->page_size - pquery_con->page_size;
	int page_max_size = pquery_con->cur_page*pquery_con->page_size;
	std::string begin_time=pquery_con->begin_time;
	std::string end_time=pquery_con->end_time;
	try
	{
		int ret = query_mem(QRY_AROUND_RECTANGLE_TWOPOINT,pquery_con,m_ppmem_tide,umUnique,begin_time,end_time,json_response);
		if(ret == 0 )
		{
			return;
		}

		char sHashCode[HASHCODE_LENGTH];
		if (geohash_encode((pquery_con->firstlatitude + pquery_con->secondlatitude)/2, (pquery_con->firstlongitude +pquery_con->secondlongitude )/2, sHashCode, HASHCODE_LENGTH) != 0)
		{
			NP_LOG(np_log::NP_ERROR, "Get hashcode err\n");
		}
		else
		{
			query_file(json_response, umUnique,begin_time,end_time,sHashCode,QRY_AROUND_RECTANGLE_TWOPOINT,pquery_con);
		}
		for(unique_iter = umUnique.begin(); unique_iter !=umUnique.end(); ++unique_iter)
		{
			count++;
			if (count > page_min_size && count <= page_max_size)
			{
				add_json_response(&(unique_iter->second), unique_iter->first, json_response);
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


void CRestQryHisAround::query_his_polygonaround(Json::Value& json_response,PQUERY_HIS_POLYGONAROUND_CON pquery_con)
{
	unordered_map<string, GPSVALUE_S> umUnique;
	unordered_map<string, GPSVALUE_S>::iterator unique_iter;
	int count = 0;
	int page_min_size = pquery_con->cur_page*pquery_con->page_size - pquery_con->page_size;
	int page_max_size = pquery_con->cur_page*pquery_con->page_size;
	QUERY_CON_POLYGONAROUND center_point;
	std::string begin_time=pquery_con->begin_time;
	std::string end_time=pquery_con->end_time;
	
	try
	{
		double maxlat =pquery_con->point_array[0].latitude;
		double minlat =pquery_con->point_array[0].latitude;
		double maxlong =pquery_con->point_array[0].longitude;
		double minlong =pquery_con->point_array[0].longitude;
	
		for(unsigned int i =0;i<pquery_con->point_array.size();i++)
		{
			if(pquery_con->point_array[i].latitude<minlat){  
				minlat=pquery_con->point_array[i].latitude; 
			}
			if(pquery_con->point_array[i].latitude>maxlat){  
				maxlat=pquery_con->point_array[i].latitude; 
			}
			if(pquery_con->point_array[i].longitude<minlong){  
				minlong=pquery_con->point_array[i].longitude; 
			}
			if(pquery_con->point_array[i].longitude>maxlong){  
				maxlong=pquery_con->point_array[i].longitude; 
			}
		}
		center_point.latitude = (maxlat + minlat)/2;
		center_point.longitude = (maxlong + minlong)/2;

		if(GetDistance(minlat, minlong, minlat, maxlong) > 6000)
		{
			json_response[REST_CODE] = REST_NOTINSCOPE;
			json_response[REST_MSG] = "The maximum length of the outer rectangle of the polygon must be less than 6000m";
			return;
		}
		if(GetDistance(minlat, minlong, maxlat, minlong) > 6000)
		{
			json_response[REST_CODE] = REST_NOTINSCOPE;
			json_response[REST_MSG] = "The maximum length of the outer rectangle of the polygon must be less than 6000m";
			return;
		}
		int ret = query_mem(QRY_AROUND_POLYGON,pquery_con,m_ppmem_tide,umUnique,begin_time,end_time,json_response);
		if(ret == 0 )
		{
			return;
		}
		//qry in file
		char sHashCode[HASHCODE_LENGTH];
		if (geohash_encode(center_point.latitude, center_point.longitude, sHashCode, HASHCODE_LENGTH) != 0)
		{
			NP_LOG(np_log::NP_ERROR, "Get hashcode err\n");
		}
		else
		{
			query_file(json_response, umUnique,begin_time,end_time,sHashCode,QRY_AROUND_POLYGON,pquery_con);
		}
		for(unique_iter = umUnique.begin(); unique_iter !=umUnique.end(); ++unique_iter)
		{
			count++;
			if (count > page_min_size && count <= page_max_size)
			{
				add_json_response(&(unique_iter->second), unique_iter->first, json_response);
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


