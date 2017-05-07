#include "url_parse.h"
#include <math.h>


bool url_parse::JudgeRange(double longitude, double latitude)
{
	if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0) 
	{
		return false;
	}
	return true;
}

int url_parse::JudgeTime(std::string begin_time,std::string end_time,int maxHours,std::string& outMessage)
{
	if (end_time <= begin_time)
	{
		outMessage = "query end time must greater than begin time";
		return REST_PARAMREEOR;
	}

	int i_begin_time = Atoi(begin_time.c_str(), 0, 10);
	int i_end_time = Atoi(end_time.c_str(), 0, 10);
	ReduceHours(i_end_time, maxHours);
	if (i_end_time > i_begin_time)
	{
		outMessage = "query file time must inside in 100 hours ";
		return REST_PARAMREEOR;
	}
	return REST_OK;
}

void url_parse::parse_url_by_spilt(std::string parse_str, std::vector<std::string>& tmp_vec)
{
	int flag = -1;
	std::string tmp;
	while((flag = parse_str.find(SPILT_CHR)) != -1)
	{
		tmp = parse_str.substr(0,flag);
		parse_str = parse_str.substr(flag + 1,parse_str.length() - flag);
		if( !tmp.empty())
		{
			tmp_vec.push_back(tmp);
		}		
	}
	if(!parse_str.empty())
	{
		tmp_vec.push_back(parse_str);
	}
}

bool url_parse::ParseToMap(std::string parseStr, std::map<std::string, std::string>& outMap)
{
	
	std::vector<std::string> tmpVec;
	std::vector<std::string>::iterator iter;
	std::string tmpStr;
	//parse to vec by split char
	parse_url_by_spilt(parseStr, tmpVec);
	for(iter = tmpVec.begin();iter!=tmpVec.end();++iter)
	{
		int flag = -1;
		if( (flag = iter->find(ASSIGN_CHR)) != -1)
		{
			tmpStr = iter->substr(0,flag);
			std::string vStr = iter->substr(flag + 1,iter->length() - flag);
			if(!tmpStr.empty() && !vStr.empty())
			{
				if (outMap.find(tmpStr)== outMap.end())
				{
					outMap.insert(make_pair(tmpStr,vStr));
				}
				else
				{
					return false;
				}
			}
		}
	}
	
	return true;
}

void url_parse::ParseToMapList(std::string parseStr, std::map<std::string, std::list<std::string>>& outMap)
{
	std::vector<std::string> tmpVec;
	std::vector<std::string>::iterator iter;
	std::map<std::string,std::list<std::string>>::iterator iterMap;
	std::string tmpStr;
	//parse to vec by split char
	parse_url_by_spilt(parseStr, tmpVec);
	for(iter = tmpVec.begin();iter!=tmpVec.end();++iter)
	{
		int flag = -1;
		if( (flag = iter->find(ASSIGN_CHR)) != -1)
		{
			tmpStr = iter->substr(0,flag);
			std::string vStr = iter->substr(flag + 1,iter->length() - flag);
			if(!tmpStr.empty() && !vStr.empty())
			{
				if ( (iterMap = outMap.find(tmpStr)) != outMap.end())
				{
					iterMap->second.push_back(vStr);
				}
				else
				{
					std::list<std::string> tlist;
					tlist.push_back(vStr);
					outMap.insert(make_pair(tmpStr,tlist));
				}
			}
		}
	}
}

//×Ö·û´®·Ö¸îº¯Êý
void url_parse::split(const std::string& str, std::vector<string>& tokens, const std::string& delimiters)
{
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos	  = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}


bool url_parse::parse_subscribe_gps(std::string query, Json::Value& value, Json::Value& response)
{
	std::map<std::string, std::list<std::string>> subcon_map;
	//parse subscribe conditon
	ParseToMapList(query, subcon_map);
	//init subscribe
	std::map<std::string, std::list<std::string>>::iterator it_find = subcon_map.end();

	response[REST_CODE] = RESPONSE_ARRAY[10].code;
	response[REST_MSG] = RESPONSE_ARRAY[10].description;
	//set type
	if ((it_find = subcon_map.find(SUBTYPE)) == subcon_map.end())
	{
		return false;
	}
	std::string type = *(it_find->second.begin());
	value[SUBTYPE] = atoi(type.c_str());
	
	//set id
	if ((it_find = subcon_map.find(SUBUSER)) == subcon_map.end())
	{
		return false;
	}
	std::string id = *(it_find->second.begin());
	value[SUBUSER] = atoi(id.c_str());

	//
	int sub_type = atoi(type.c_str());

	switch (sub_type)
	{
	case UNSUB:
	case SUB_ALL:
		break;
	case SUB_KEYS:
		{
		if ((it_find = subcon_map.find(SUBKEY)) == subcon_map.end())
			{
				return false;
			}
			for (auto key : it_find->second)
			{
				value[SUBKEY].append(key);
			}
		}
		break;
	case SUB_CIRCLE:
		{
			if ((it_find = subcon_map.find(LATITUDE)) == subcon_map.end())
			{
				return false;
			}
			std::string latitude = *it_find->second.begin();
			value[LATITUDE] = latitude;


			if ((it_find = subcon_map.find(LONGITUDE)) == subcon_map.end())
			{
				return false;
			}
			std::string longitude = *it_find->second.begin();
			value[LONGITUDE] = longitude;

			if ((it_find = subcon_map.find(SUBREDIUS)) == subcon_map.end())
			{
				return false;
			}
			std::string redius = *it_find->second.begin();
			value[SUBREDIUS] = atof(redius.c_str());
		}
		break;
	case SUB_RECT:
		{
			if ((it_find = subcon_map.find(LATITUDEONE)) == subcon_map.end())
			{
				return false;
			}
			std::string latitude1 = *it_find->second.begin();
			value[LATITUDEONE] = latitude1;


			if ((it_find = subcon_map.find(LATITUDETWO)) == subcon_map.end())
			{
				return false;
			}
			std::string latitude2 = *it_find->second.begin();
			value[LATITUDETWO] = latitude2;

			if ((it_find = subcon_map.find(LONGITUDEONE)) == subcon_map.end())
			{
				return false;
			}
			std::string longitude1 = *it_find->second.begin();
			value[LONGITUDEONE] = longitude1;

			if ((it_find = subcon_map.find(LONGITUDETWO)) == subcon_map.end())
			{
				return false;
			}
			std::string longitude2 = *it_find->second.begin();
			value[LONGITUDETWO] = longitude2;
		}
		break;
	default:
		response[REST_CODE] = RESPONSE_ARRAY[11].code;
		response[REST_MSG] = RESPONSE_ARRAY[11].description;
		return false;
	}

	response[REST_CODE] = RESPONSE_ARRAY[0].code;
	response[REST_MSG] = RESPONSE_ARRAY[0].description;
	return true;
}

PQUERY_GPSID_CON url_parse::parse_query_gpsid_str(std::string query_con)
{
	PQUERY_GPSID_CON pquery_con = new QUERY_GPSID_CON();
	try
	{
		int size_seat = query_con.find("key=");
		if (size_seat == -1)
		{
			pquery_con->key = QUERY_ALL;
		}		
		else
		{
			pquery_con->key = query_con.substr(size_seat + 4);
		}
	}
	catch (...)
	{
		pquery_con->key = QUERY_ALL;
	}
	return pquery_con;
}

//key=123&expand=cartype:1$key=234&expand=cartype:2
PQUERY_REAL_POS_CON url_parse::parse_query_realpos_str(std::string query_con)
{
	PQUERY_REAL_POS_CON pquery_con = new QUERY_REAL_POS_CON();
	try
	{
		pquery_con->is_all = 0;
/*		
		if(query_con.find("key=all") != std::string::npos)
		{
			pquery_con->is_all = 1;
			pquery_con->flag = REST_OK;
			return pquery_con;
		}
*/
		std::map<std::string,std::list<std::string>> resultMap;
		std::map<std::string,std::list<std::string>>::iterator itermap;
		ParseToMapList(query_con,resultMap);
		if((itermap=resultMap.find("key"))!= resultMap.end())
		{
			std::list<std::string>::iterator iter;
			for(iter = itermap->second.begin();iter != itermap->second.end();++iter)
			{ 
				QUERY_CON_REAL_POS query_con_obj;
				if( *iter == "all")
				{
					query_con_obj.key = "*";
				}
				else
				{
					query_con_obj.key = *iter;
				}			
				pquery_con->key_lists.push_back(query_con_obj);
			}
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}

		if(pquery_con->key_lists.size() == 0 || pquery_con->key_lists.size() >100)
		{
			pquery_con->key_lists.clear();
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "query conditon the number of key must be less than or equal to 100";
			return pquery_con;
		}
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->key_lists.clear();
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_LAST_POS_CON url_parse::parse_query_lastpos_str(std::string query_con)
{
	PQUERY_LAST_POS_CON pquery_con = new QUERY_LAST_POS_CON();
	try
	{

		std::map<std::string,std::list<std::string>> resultMap;
		std::map<std::string,std::list<std::string>>::iterator itermap;
		ParseToMapList(query_con,resultMap);
		if((itermap=resultMap.find("key"))!= resultMap.end())
		{
			std::list<std::string>::iterator iter;
			for(iter = itermap->second.begin();iter != itermap->second.end();++iter)
			{ 
				QUERY_CON_REAL_POS query_con_obj;
				query_con_obj.key = *iter;
				pquery_con->key_lists.push_back(query_con_obj);
			}
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}

		if(pquery_con->key_lists.size() == 0 || pquery_con->key_lists.size() >100)
		{
			pquery_con->key_lists.clear();
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "query conditon the number of key must be less than or equal to 100";
			return pquery_con;
		}
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->key_lists.clear();
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_STATISTIC_REALTOTAL_CON url_parse::parse_query_statistic_realtotal(std::string query_con)
{
	PQUERY_STATISTIC_REALTOTAL_CON pquery_con = new QUERY_STATISTIC_REALTOTAL_CON();
	try
	{
		int size_seat = query_con.find("type=");
		if (size_seat == -1)
		{
			pquery_con->type = 0;
		}		
		else
		{
			pquery_con->type = atoi((query_con.substr(size_seat + 5)).c_str());
		}
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_STATISTIC_REALEACH_CON url_parse::parse_query_statistic_realeach(std::string query_con)
{
	PQUERY_STATISTIC_REALEACH_CON pquery_con = new QUERY_STATISTIC_REALEACH_CON();
	try
	{
		std::map<std::string,std::list<std::string>> resultMap;
		std::map<std::string,std::list<std::string>>::iterator itermap;
		ParseToMapList(query_con,resultMap);
		if((itermap=resultMap.find("key"))!= resultMap.end())
		{
			std::list<std::string>::iterator iter;
			for(iter = itermap->second.begin();iter != itermap->second.end();++iter)
			{ 
				pquery_con->key_lists.push_back(*iter);
			}
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}

		if( (itermap=resultMap.find("type"))!= resultMap.end())
		{
			pquery_con->type = atoi((*(itermap->second.begin())).c_str());
		}
		else
		{
			pquery_con->type = 0;
		}
		
		if(pquery_con->key_lists.size() == 0 || pquery_con->key_lists.size() >100)
		{
			pquery_con->key_lists.clear();
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "query conditon the number of key must be less than or equal to 100";
			return pquery_con;
		}
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->key_lists.clear();
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}
PQUERY_STATISTIC_DAYEACH_CON url_parse::parse_query_statistic_dayeach(std::string query_con)
{
	PQUERY_STATISTIC_DAYEACH_CON pquery_con = new QUERY_STATISTIC_DAYEACH_CON();
	try
	{
		std::map<std::string,std::list<std::string>> resultMap;
		std::map<std::string,std::list<std::string>>::iterator itermap,itermap2;
		ParseToMapList(query_con,resultMap);
		if((itermap=resultMap.find("key"))!= resultMap.end())
		{
			std::list<std::string>::iterator iter;
			for(iter = itermap->second.begin();iter != itermap->second.end();++iter)
			{ 
				pquery_con->key_lists.push_back(*iter);
			}
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}

		if( (itermap=resultMap.find("begin"))!= resultMap.end() && (itermap2=resultMap.find("end"))!= resultMap.end() )
		{
			pquery_con->begin_time = *(itermap->second.begin());
			pquery_con->end_time = *(itermap2->second.begin());

			if( (pquery_con->begin_time.length() != 8) || (pquery_con->end_time.length() != 8))
			{
				pquery_con->flag = REST_PARAMREEOR;
				pquery_con->message = "time of query conditon is error";
				return pquery_con;
			}
		}
		else if( (itermap=resultMap.find("begin"))== resultMap.end() && (itermap2=resultMap.find("end"))== resultMap.end() )
		{
			time_t time_now;
			time(&time_now);
			struct tm* tm_now;    
			tm_now = localtime(&time_now);
			char i_time[11];
			memset(i_time,0,11);
			sprintf(i_time,"%04d%02d%02d",tm_now->tm_year+1900,tm_now->tm_mon+1,tm_now->tm_mday);
			pquery_con->begin_time = i_time;
			pquery_con->end_time = i_time;
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}
	
		if(pquery_con->begin_time > pquery_con->end_time)
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query end time must greater than begin time";
			return pquery_con;
		}
		if(pquery_con->key_lists.size() == 0 || pquery_con->key_lists.size() >100)
		{
			pquery_con->key_lists.clear();
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "query conditon the number of key must be less than or equal to 100";
			return pquery_con;
		}
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->key_lists.clear();
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}
PQUERY_STATISTIC_DAYTOTAL_CON url_parse::parse_query_statistic_daytotal(std::string query_con)
{
	PQUERY_STATISTIC_DAYTOTAL_CON pquery_con = new QUERY_STATISTIC_DAYTOTAL_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if(resultMap.find("begin") != resultMap.end() &&\
			resultMap.find("end") != resultMap.end())
		{
			
			pquery_con->begin_time = (*resultMap.find("begin")).second;
			pquery_con->end_time = (*resultMap.find("end")).second;

			if( (pquery_con->begin_time.length() != 8) || (pquery_con->end_time.length() != 8))
			{
				pquery_con->flag = REST_PARAMREEOR;
				pquery_con->message = "time of query conditon is error";
				return pquery_con;
			}
		}
		else if(resultMap.find("begin") == resultMap.end() && resultMap.find("end") == resultMap.end())
		{
			time_t time_now;
			time(&time_now);
			struct tm* tm_now;    
			tm_now = localtime(&time_now);
			char i_time[11];
			memset(i_time,0,11);
			sprintf(i_time,"%04d%02d%02d",tm_now->tm_year+1900,tm_now->tm_mon+1,tm_now->tm_mday);
			pquery_con->begin_time = i_time;
			pquery_con->end_time = i_time;
		}	
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}
		
		if(pquery_con->begin_time > pquery_con->end_time)
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query end time must greater than begin time";
			return pquery_con;
		}
		
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

//longitude=32.231234&latitude=138.1235124&redius=1000&expand=cartype:1
PQUERY_REAL_AROUND_CON url_parse::parse_query_realaround(std::string query_con)
{
	PQUERY_REAL_AROUND_CON pquery_con = new QUERY_REAL_AROUND_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		if(resultMap.find("key")!= resultMap.end())
		{
			pquery_con->key = (*resultMap.find("key")).second;
			if(resultMap.find("redius") != resultMap.end())
			{
				pquery_con->redius = atoi((*resultMap.find("redius")).second.c_str());
			}
			else
			{
				pquery_con->flag = REST_PARAMREEOR;
				pquery_con->message = "query conditon must have redius";
				return pquery_con;
			}
			
		}
		else
		{
			if(resultMap.find("longitude") != resultMap.end() &&\
				resultMap.find("latitude") != resultMap.end()&&\
				resultMap.find("redius") != resultMap.end())
			{
				pquery_con->longitude = atof((*resultMap.find("longitude")).second.c_str());
				pquery_con->latitude = atof((*resultMap.find("latitude")).second.c_str());
				pquery_con->redius = atoi((*resultMap.find("redius")).second.c_str());
			}
			else
			{
				pquery_con->flag = REST_PARAMREEOR;
				pquery_con->message = "query conditon is missing";
				return pquery_con;
			}
		}

		if(false ==JudgeRange(pquery_con->longitude,pquery_con->latitude))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "longitude or latitude is not in scope";
			return pquery_con;
		}
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_REAL_POLYGONAROUND_CON url_parse::parse_query_realpolygonaround(std::string query_con)
{
	PQUERY_REAL_POLYGONAROUND_CON pquery_con = new QUERY_REAL_POLYGONAROUND_CON();
	try
	{
		std::map<std::string,std::list<std::string>> resultMap;
		std::map<std::string,std::list<std::string>>::iterator itermap;
		std::list<std::string> query_list;
		std::string typeFlag = "0";
		ParseToMapList(query_con,resultMap);
		if((itermap=resultMap.find("type"))!= resultMap.end())
		{
			typeFlag = *(itermap->second.begin());
			if( typeFlag== "1")
			{
				if((itermap=resultMap.find("point"))!= resultMap.end())
				{
					std::vector<std::string> point;
					std::vector<std::string>::iterator iter;
					split(*(itermap->second.begin()),point,";");
					for(iter= point.begin();iter!=point.end();++iter)
					{
						int ll_seat = (*iter).find(',');
						if(ll_seat <= -1)
						{
							pquery_con->flag = REST_PARAMREEOR;
							pquery_con->message = "point must be separated by a ','.";
							return pquery_con;
						}

						QUERY_CON_POLYGONAROUND query_con_obj;
						query_con_obj.longitude = atof(((*iter).substr(0,ll_seat)).c_str());
						query_con_obj.latitude = atof(((*iter).substr(ll_seat+1,(*iter).length()- ll_seat -1)).c_str());
						pquery_con->point_array.push_back(query_con_obj);
					}
				
				}
				else
				{
					pquery_con->flag = REST_PARAMREEOR;
					pquery_con->message = "query conditon is missing";
					return pquery_con;
				}
			}
		}
		if(typeFlag == "0")
		{
			if((itermap=resultMap.find("point"))!= resultMap.end())
            {
                std::list<std::string>::iterator iter;
                for(iter = itermap->second.begin();iter != itermap->second.end();++iter)
                {
                    QUERY_CON_POLYGONAROUND query_con_obj;
                    
                    int ll_seat = (*iter).find(',');
                    if(ll_seat <= -1)
                    {
                        pquery_con->flag = REST_PARAMREEOR;
                        pquery_con->message = "point must be separated by a ','.";
                        return pquery_con;
                    }	
                    query_con_obj.longitude = atof(((*iter).substr(0,ll_seat)).c_str());
                    query_con_obj.latitude = atof(((*iter).substr(ll_seat+1,(*iter).length()- ll_seat -1)).c_str());
                    
                    if(false ==JudgeRange(query_con_obj.longitude,query_con_obj.latitude))
                    {
                        pquery_con->flag = REST_PARAMREEOR;
                        pquery_con->message = "longitude or latitude is not in scope";
                        return pquery_con;
                    }
                    pquery_con->point_array.push_back(query_con_obj);
                }
            }
            else
            {
                pquery_con->flag = REST_PARAMREEOR;
                pquery_con->message = "query conditon is missing";
                return pquery_con;
            }
		}
		else if(typeFlag != "1")
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "value of type is error";
			return pquery_con;
		}
		
		
		if( pquery_con->point_array.size() < 3 || pquery_con->point_array.size() > 1000)
		{
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "the number of points must be greater than or equal to 3 and less than 1000.";
			return pquery_con;
		}	
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->point_array.clear();
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_REAL_RECTAROUND_CON url_parse::parse_query_realrectaround(std::string query_con)
{
	PQUERY_REAL_RECTAROUND_CON pquery_con = new QUERY_REAL_RECTAROUND_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		if(resultMap.find("key")!= resultMap.end())
		{
			pquery_con->key = (*resultMap.find("key")).second;
			if(resultMap.find("ewlen") != resultMap.end() &&\
				resultMap.find("snlen") != resultMap.end())
			{
				pquery_con->ew_len = atoi((*resultMap.find("ewlen")).second.c_str());
				pquery_con->sn_len = atoi((*resultMap.find("snlen")).second.c_str());
			}
			else
			{
				pquery_con->flag = REST_PARAMREEOR;
				pquery_con->message = "query conditon must have redius";
				return pquery_con;
			}
			
		}
		else
		{
			if(resultMap.find("longitude") != resultMap.end() &&\
				resultMap.find("latitude") != resultMap.end() &&\
				resultMap.find("ewlen") != resultMap.end() &&\
				resultMap.find("snlen") != resultMap.end())
			{
				pquery_con->longitude = atof((*resultMap.find("longitude")).second.c_str());
				pquery_con->latitude = atof((*resultMap.find("latitude")).second.c_str());
				pquery_con->ew_len = atoi((*resultMap.find("ewlen")).second.c_str());
				pquery_con->sn_len = atoi((*resultMap.find("snlen")).second.c_str());
			}
			else
			{
				pquery_con->flag = REST_PARAMREEOR;
				pquery_con->message = "query conditon is missing";
				return pquery_con;
			}
			if(false ==JudgeRange(pquery_con->longitude,pquery_con->latitude))
			{
				pquery_con->flag = REST_PARAMREEOR;
				pquery_con->message = "longitude or latitude is not in scope";
				return pquery_con;
			}
		}	
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_REAL_RECTAROUND_TWOPOINT_CON url_parse::parse_query_realrectaround_twopoint(std::string query_con)
{
	PQUERY_REAL_RECTAROUND_TWOPOINT_CON pquery_con = new QUERY_REAL_RECTAROUND_TWOPOINT_CON();
	try
	{

		std::map<std::string,std::string> resultMap;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if(resultMap.find("firstlongitude") != resultMap.end() &&\
			resultMap.find("firstlatitude") != resultMap.end() &&\
			resultMap.find("secondlongitude") != resultMap.end() &&\
			resultMap.find("secondlatitude") != resultMap.end())
		{
			pquery_con->firstlongitude = atof((*resultMap.find("firstlongitude")).second.c_str());
			pquery_con->firstlatitude =  atof((*resultMap.find("firstlatitude")).second.c_str());
			pquery_con->secondlongitude = atof((*resultMap.find("secondlongitude")).second.c_str());
			pquery_con->secondlatitude =  atof((*resultMap.find("secondlatitude")).second.c_str());
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}
		if(false ==JudgeRange(pquery_con->firstlongitude,pquery_con->firstlatitude) ||\
			false ==JudgeRange(pquery_con->secondlongitude,pquery_con->secondlatitude))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "longitude or latitude is not in scope";
			return pquery_con;
		}
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}
PQUERY_HIS_RECTAROUND_TWOPOINT_CON url_parse::parse_query_hisrectaround_twopoint(std::string query_con)
{
	PQUERY_HIS_RECTAROUND_TWOPOINT_CON pquery_con = new QUERY_HIS_RECTAROUND_TWOPOINT_CON();
	try
	{

		std::map<std::string,std::string> resultMap;
		std::map<std::string,std::string>::iterator iter;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if( resultMap.find("begin") != resultMap.end() &&\
			resultMap.find("end") != resultMap.end() &&\
			resultMap.find("firstlongitude") != resultMap.end() &&\
			resultMap.find("firstlatitude") != resultMap.end() &&\
			resultMap.find("secondlongitude") != resultMap.end() &&\
			resultMap.find("secondlatitude") != resultMap.end())
		{
			pquery_con->begin_time = (*resultMap.find("begin")).second;
			pquery_con->end_time = (*resultMap.find("end")).second;
			pquery_con->firstlongitude = atof((*resultMap.find("firstlongitude")).second.c_str());
			pquery_con->firstlatitude =  atof((*resultMap.find("firstlatitude")).second.c_str());
			pquery_con->secondlongitude = atof((*resultMap.find("secondlongitude")).second.c_str());
			pquery_con->secondlatitude =  atof((*resultMap.find("secondlatitude")).second.c_str());
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}
		if(false ==JudgeRange(pquery_con->firstlongitude,pquery_con->firstlatitude) ||\
			false ==JudgeRange(pquery_con->secondlongitude,pquery_con->secondlatitude))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "longitude or latitude is not in scope";
			return pquery_con;
		}
		
		if (GetDistance(pquery_con->firstlatitude, pquery_con->firstlongitude,pquery_con->secondlatitude, pquery_con->secondlongitude) > sqrt((MAX_QUERY_REDIUS*2)*(MAX_QUERY_REDIUS*2)*2))
		{
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "query the length of the rectangle must inside in 6000m";
			return pquery_con;
		}
		
		if((iter=resultMap.find("curpage")) != resultMap.end())
		{
			pquery_con->cur_page = atoi((*iter).second.c_str());
		}
		else
		{
			pquery_con->cur_page = 1;
		}
		if((iter=resultMap.find("pagesize")) != resultMap.end())
		{
			pquery_con->page_size = atoi((*iter).second.c_str());
		}
		else
		{
			pquery_con->page_size = MAX_QUERY_SIZE;
		}
		
		//compare time
		pquery_con->flag = JudgeTime( pquery_con->begin_time,pquery_con->end_time,MAX_QUERY_FILEHOURS,pquery_con->message);
	
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

//begin=201606040321&end=201606040321&longitude=32.231234&latitude=138.1235124&redius=1000&expand=cartype:1&curpage=1&pagesize=20
PQUERY_HIS_AROUND_CON url_parse::parse_query_hisaround(std::string query_con)
{
	PQUERY_HIS_AROUND_CON pquery_con = new QUERY_HIS_AROUND_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		std::map<std::string,std::string>::iterator iter;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if(resultMap.find("begin") != resultMap.end() &&\
			resultMap.find("end") != resultMap.end() &&\
			resultMap.find("longitude") != resultMap.end() &&\
			resultMap.find("latitude") != resultMap.end() &&\
			resultMap.find("redius") != resultMap.end())
		{
			pquery_con->begin_time = (*resultMap.find("begin")).second;
			pquery_con->end_time = (*resultMap.find("end")).second;
			pquery_con->longitude = atof((*resultMap.find("longitude")).second.c_str());
			pquery_con->latitude =  atof((*resultMap.find("latitude")).second.c_str());
			pquery_con->redius = atoi((*resultMap.find("redius")).second.c_str());
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}

		if(false ==JudgeRange(pquery_con->longitude,pquery_con->latitude))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "longitude or latitude is not in scope";
			return pquery_con;
		}
				
		if (pquery_con->redius > MAX_QUERY_REDIUS)
		{
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "query redius must inside in 3000";
			return pquery_con;
		}

		if((iter=resultMap.find("curpage")) != resultMap.end())
		{
			pquery_con->cur_page = atoi((*iter).second.c_str());
		}
		else
		{
			pquery_con->cur_page = 1;
		}
		if((iter=resultMap.find("pagesize")) != resultMap.end())
		{
			pquery_con->page_size = atoi((*iter).second.c_str());
		}
		else
		{
			pquery_con->page_size = MAX_QUERY_SIZE;
		}
		
		//compare time
		pquery_con->flag = JudgeTime( pquery_con->begin_time,pquery_con->end_time,MAX_QUERY_FILEHOURS,pquery_con->message);
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_HIS_RECTAROUND_CON url_parse::parse_query_hisrectaround(std::string query_con)
{
	PQUERY_HIS_RECTAROUND_CON pquery_con = new QUERY_HIS_RECTAROUND_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		std::map<std::string,std::string>::iterator iter;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if(resultMap.find("begin") != resultMap.end() &&\
			resultMap.find("end") != resultMap.end() &&\
			resultMap.find("longitude") != resultMap.end() &&\
			resultMap.find("latitude") != resultMap.end() &&\
			resultMap.find("ewlen") != resultMap.end() &&\
			resultMap.find("snlen") != resultMap.end())
		{
			pquery_con->begin_time = (*resultMap.find("begin")).second;
			pquery_con->end_time = (*resultMap.find("end")).second;
			pquery_con->longitude = atof((*resultMap.find("longitude")).second.c_str());
			pquery_con->latitude =  atof((*resultMap.find("latitude")).second.c_str());
			pquery_con->ew_len = atoi((*resultMap.find("ewlen")).second.c_str());
			pquery_con->sn_len = atoi((*resultMap.find("snlen")).second.c_str());
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}
		
		if(false ==JudgeRange(pquery_con->longitude,pquery_con->latitude))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "longitude or latitude is not in scope";
			return pquery_con;
		}
	
		if (pquery_con->ew_len > MAX_QUERY_REDIUS)
		{
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "query ew_len must inside in 3000m";
			return pquery_con;
		}
		
		if (pquery_con->sn_len > MAX_QUERY_REDIUS)
		{
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "query sn_len must inside in 3000m";
			return pquery_con;
		}

		if((iter=resultMap.find("curpage")) != resultMap.end())
		{
			pquery_con->cur_page = atoi((*iter).second.c_str());
		}
		else
		{
			pquery_con->cur_page = 1;
		}
		if((iter=resultMap.find("pagesize")) != resultMap.end())
		{
			pquery_con->page_size = atoi((*iter).second.c_str());
		}
		else
		{
			pquery_con->page_size = MAX_QUERY_RESULTS;
		}

		//compare time		
		pquery_con->flag = JudgeTime( pquery_con->begin_time,pquery_con->end_time,MAX_QUERY_FILEHOURS,pquery_con->message);
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_HIS_POLYGONAROUND_CON url_parse::parse_query_hispolygonaround(std::string query_con)
{
	PQUERY_HIS_POLYGONAROUND_CON pquery_con = new QUERY_HIS_POLYGONAROUND_CON();
	try
	{
		std::map<std::string,std::list<std::string>> resultMap;
		std::map<std::string,std::list<std::string>>::iterator itermap;
		ParseToMapList(query_con,resultMap);
		if(resultMap.find("begin")!= resultMap.end() &&\
			resultMap.find("end")!= resultMap.end() &&\
			resultMap.find("point")!= resultMap.end())
		{
			std::string typeFlag = "0";
			if((itermap=resultMap.find("type"))!= resultMap.end())
			{
				typeFlag = *(itermap->second.begin());
				if( typeFlag== "1")
				{
					itermap = resultMap.find("point");
					std::vector<std::string> point;
					std::vector<std::string>::iterator iter;
					split(*(itermap->second.begin()),point,";");
					for(iter= point.begin();iter!=point.end();++iter)
					{
						int ll_seat = (*iter).find(',');
						if(ll_seat <= -1)
						{
							pquery_con->flag = REST_PARAMREEOR;
							pquery_con->message = "point must be separated by a ','.";
							return pquery_con;
						}

						QUERY_CON_POLYGONAROUND query_con_obj;
						query_con_obj.longitude = atof(((*iter).substr(0,ll_seat)).c_str());
						query_con_obj.latitude = atof(((*iter).substr(ll_seat+1,(*iter).length()- ll_seat -1)).c_str());
						pquery_con->point_array.push_back(query_con_obj);
					}					
				}
			}
			if(typeFlag == "0")
			{
				itermap = resultMap.find("point");
				std::list<std::string>::iterator iter;
				for(iter = itermap->second.begin();iter != itermap->second.end();++iter)
				{
					QUERY_CON_POLYGONAROUND query_con_obj;
					int ll_seat = (*iter).find(',');
					if(ll_seat <= -1)
					{
						pquery_con->flag = REST_PARAMREEOR;
						pquery_con->message = "point must be separated by a ','.";
						return pquery_con;
					}	
					query_con_obj.longitude = atof(((*iter).substr(0,ll_seat)).c_str());
					query_con_obj.latitude = atof(((*iter).substr(ll_seat+1,(*iter).length()- ll_seat -1)).c_str());
					if(false ==JudgeRange(query_con_obj.longitude,query_con_obj.latitude))
					{
						pquery_con->flag = REST_PARAMREEOR;
						pquery_con->message = "longitude or latitude is not in scope";
						return pquery_con;
					}
					pquery_con->point_array.push_back(query_con_obj);
				}
			}
			else if(typeFlag != "1")
			{
				pquery_con->flag = REST_PARAMREEOR;
				pquery_con->message = "value of type is error";
				return pquery_con;
			}
			
			itermap = resultMap.find("begin");
			pquery_con->begin_time = *itermap->second.begin();
			itermap = resultMap.find("end");
			pquery_con->end_time = *itermap->second.begin();
			
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}
		
		if( pquery_con->point_array.size() < 3 || pquery_con->point_array.size() > 1000)
		{
			pquery_con->flag = REST_NOTINSCOPE;
			pquery_con->message = "the number of points must be greater than or equal to 3 and less than 1000.";
			return pquery_con;
		}	

		if( (itermap=resultMap.find("curpage"))!= resultMap.end())
		{
			pquery_con->cur_page = atoi((*(itermap->second.begin())).c_str());
		}
		else
		{
			pquery_con->cur_page = 1;
		}

		if( (itermap=resultMap.find("pagesize"))!= resultMap.end())
		{
			pquery_con->page_size = atoi((*(itermap->second.begin())).c_str());
		}
		else
		{
			pquery_con->page_size = MAX_QUERY_SIZE;
		}

		//compare time	
		pquery_con->flag = JudgeTime( pquery_con->begin_time,pquery_con->end_time,MAX_QUERY_FILEHOURS,pquery_con->message);
	}
	catch (std::exception & ex)
	{
		pquery_con->point_array.clear();
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

//begin=201606040321&end=201606040321&key=1000&expand=cartype:1&curpage=1&pagesize=20
PQUERY_HIS_LOCUS_CON url_parse::parse_query_hislocus(std::string query_con)
{
	PQUERY_HIS_LOCUS_CON pquery_con = new QUERY_HIS_LOCUS_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		std::map<std::string,std::string>::iterator iter;
		
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if(resultMap.find("begin") != resultMap.end() &&\
			resultMap.find("end") != resultMap.end() &&\
			resultMap.find("key") != resultMap.end())
		{
			pquery_con->begin_time = (*resultMap.find("begin")).second;
			pquery_con->end_time = (*resultMap.find("end")).second;
			pquery_con->key = (*resultMap.find("key")).second;
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}

		if((iter=resultMap.find("curpage")) != resultMap.end())
		{
			pquery_con->cur_page = atoi((*iter).second.c_str());
		}
		else
		{
			pquery_con->cur_page = 1;
		}
		if((iter=resultMap.find("pagesize")) != resultMap.end())
		{
			pquery_con->page_size = atoi((*iter).second.c_str());
		}
		else
		{
			pquery_con->page_size = MAX_QUERY_SIZE;
		}
		//compare time	
		pquery_con->flag = JudgeTime( pquery_con->begin_time,pquery_con->end_time,MAX_QUERY_FILEHOURS,pquery_con->message);
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}

PQUERY_ONLINE_CON url_parse::parse_query_online(std::string query_con)
{
	PQUERY_ONLINE_CON pquery_con = new QUERY_ONLINE_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if(resultMap.find("status") != resultMap.end() &&\
			resultMap.find("judgetime") != resultMap.end())
		{
			pquery_con->status = atoi((*resultMap.find("status")).second.c_str());
			pquery_con->judgetime = atoi((*resultMap.find("judgetime")).second.c_str());
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}
PQUERY_DISTINCE_DEVICE_CON url_parse::parse_query_devicedistance(std::string query_con)
{
	PQUERY_DISTINCE_DEVICE_CON pquery_con = new QUERY_DISTINCE_DEVICE_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if(resultMap.find("key1") != resultMap.end() &&\
			resultMap.find("key2") != resultMap.end())
		{
			
			pquery_con->key1= (*resultMap.find("key1")).second;
			pquery_con->key2= (*resultMap.find("key2")).second;

		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}
		
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}
PQUERY_GPSINFO_CON url_parse::parse_query_gpsinfo(std::string query_con)
{
	PQUERY_GPSINFO_CON pquery_con = new QUERY_GPSINFO_CON();
	try
	{
		std::map<std::string,std::string> resultMap;
		std::map<std::string,std::string>::iterator iter;
		if(false == ParseToMap(query_con,resultMap))
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is repeated";
			return pquery_con;
		}
		
		if(resultMap.find("output") != resultMap.end())
		{
			char sOutPut[512]={0};
			strcpy(sOutPut,resultMap.find("output")->second.c_str());
			char* token = strtok( sOutPut, ",");
    		while( token != NULL )
    		{
        		/* While there are tokens in "string" */
				pquery_con->output_column.push_back(token);
        		/* Get next token: */
        		token = strtok( NULL, ",");
    		}
		}
		else
		{
			pquery_con->flag = REST_PARAMREEOR;
			pquery_con->message = "query conditon is missing";
			return pquery_con;
		}

		if(resultMap.find(GPS_ID) != resultMap.end())
		{
			std::vector<std::string> keyid;
			split(resultMap.find(GPS_ID)->second,keyid,",");
			pquery_con->gps_id = keyid;
		}
		for(iter = resultMap.begin();iter != resultMap.end();++iter)
		{
			if(iter->first != "output" && iter->first != GPS_ID)
			{
				std::vector<std::string> con;
				char tmpstr[64] = {0};
				strcpy(tmpstr,iter->first.c_str());				
				strcat(tmpstr,"=");

				split(iter->second,con,",");
				for(std::vector<std::string>::iterator iter = con.begin();iter != con.end();++iter)
				{
					*iter =tmpstr + *iter ;
				}
				pquery_con->query_conditions.insert(make_pair(iter->first,con));
			}
		}
		
		pquery_con->flag = REST_OK;
	}
	catch (std::exception & ex)
	{
		pquery_con->flag = REST_UNEXPECTED;
		np_log::NP_LOG(np_log::NP_ERROR, "parse parameter error:%s", ex.what());
	}
	return pquery_con;
}





