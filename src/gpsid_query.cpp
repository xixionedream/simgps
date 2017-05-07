#include "gpsid_query.h"
#include "reg_str.h"

//rest
CRestQryGpsid::CRestQryGpsid()
{

}

CRestQryGpsid::~CRestQryGpsid()
{

}

void CRestQryGpsid::Init(GpsidWriter* pgpsid_mem, PQUERY_GPSID_CON pquery_con)
{
	this->m_pgpsid_writer = pgpsid_mem;
	this->m_pquery_con = pquery_con;
}

void CRestQryGpsid::QryQgpsid(Json::Value& json_response)
{
	std::unordered_map<std::string, char>::iterator iter_gpsid;
	std::unordered_map<std::string, char>* pgpsid_map;
	try
	{
		if (m_pgpsid_writer != NULL)
		{
			//gpsid map
			pgpsid_map = m_pgpsid_writer->get_gpsid_map();
			//iterator get gpsid
			iter_gpsid = pgpsid_map->begin();
			//query all gpsid
			if (this->m_pquery_con->key == QUERY_ALL)
			{
				for (; iter_gpsid != pgpsid_map->end(); ++iter_gpsid)
				{
					Json::Value key_value;
					key_value[REST_KEY] = iter_gpsid->first;
					json_response[REST_RESULT].append(key_value);
				}
			}
			else //fuzzy query gpsid
			{
				for (; iter_gpsid != pgpsid_map->end(); ++iter_gpsid)
				{
					//match gpsid
					if (fnmatch_str(this->m_pquery_con->key.c_str(), iter_gpsid->first.c_str(), FNM_PATHNAME) != FNM_NOMATCH)
					{
						Json::Value key_value;
						key_value[REST_KEY] = iter_gpsid->first;
						json_response[REST_RESULT].append(key_value);
					}
				}
			}
			//
			json_response[REST_CODE] = REST_OK;
			json_response[REST_MSG] = REST_SUCCESS;
		}
		else
		{
			json_response[REST_CODE] = REST_UNEXPECTED;
			json_response[REST_MSG] = REST_EXCEPTION;
		}
	}
	catch (std::exception & ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsid error:%s", ex.what());
	}	
}
