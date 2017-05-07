
#include "realpos_query.h"
#include "reg_str.h"


CRestQryRealPos::CRestQryRealPos()
{

}

CRestQryRealPos::~CRestQryRealPos()
{

}

void CRestQryRealPos::init(unordered_map<string, vector<GPSVALUE_S> >** ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_REAL_POS_CON pquery_con,unordered_map<string,GPSVALUE_S>* plastdata)
{
	this->m_ppmem_tide = ppnum_tide;
	this->m_pmem_tide_data1 = pnum_tide_data1;
	this->m_pmem_tide_data2 = pnum_tide_data2;
	this->m_pquery_con = pquery_con;
	this->m_plastdata = plastdata;
}
/*
void CRestQryRealPos::query_real_all_pos(Json::Value& json_response)
{
	unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
	unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	unordered_map<string, char> unique_keymap;
	unordered_map<string, GPSVALUE_S >::iterator itor_last_data;
	GPSVALUE_S* pgps_value;
	
	for (itor_tide_data = (*m_ppmem_tide)->begin(); itor_tide_data != (*m_ppmem_tide)->end(); ++itor_tide_data)
	{
		unique_keymap.insert(make_pair(itor_tide_data->first, 0x1));
		pgps_value = &(itor_tide_data->second.back());
	
		add_json_response(pgps_value, itor_tide_data->first, json_response);
	}

	if ((*m_ppmem_tide) == m_pmem_tide_data1)
	{
		slave_mem = m_pmem_tide_data2;
	}		
	else
	{
		slave_mem = m_pmem_tide_data1;
	}
		
	for (itor_tide_data = slave_mem->begin(); itor_tide_data != slave_mem->end(); ++itor_tide_data)
	{
		if (unique_keymap.find(itor_tide_data->first) != unique_keymap.end())
		{
			continue;
		}
		unique_keymap.insert(make_pair(itor_tide_data->first, 0x1));
		pgps_value = &(itor_tide_data->second.back());
		
		add_json_response(pgps_value, itor_tide_data->first, json_response);
	}

	for (itor_last_data = m_plastdata->begin(); itor_last_data != m_plastdata->end(); ++itor_last_data)
	{
		if (unique_keymap.find(itor_last_data->first) != unique_keymap.end())
		{
			continue;
		}
		unique_keymap.insert(make_pair(itor_last_data->first, 0x1));
		pgps_value = &(itor_last_data->second);
		
		add_json_response(pgps_value, itor_last_data->first, json_response);
	}
	
}
*/
void CRestQryRealPos::query_real_pos(Json::Value& json_response)
{
	try
	{
		
		std::list<QUERY_CON_REAL_POS>::iterator itor;
		unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
		unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
		unordered_map<string, char> unique_keymap;
		GPSVALUE_S* pgps_value;
		int query_res_size = 0;
		int find_flag;

/*
		if( m_pquery_con->is_all == 1)
		{
			query_real_all_pos(json_response);
			json_response[REST_CODE] = REST_OK;
			json_response[REST_MSG] = REST_SUCCESS;
			return;
		}
*/
			
		for (itor_tide_data = (*m_ppmem_tide)->begin(); itor_tide_data != (*m_ppmem_tide)->end(); ++itor_tide_data)
		{
			find_flag = 0;
			for (itor = m_pquery_con->key_lists.begin(); itor != m_pquery_con->key_lists.end(); ++itor)
			{
				if (fnmatch_str(itor->key.c_str(), itor_tide_data->first.c_str(), FNM_PATHNAME) != FNM_NOMATCH)
				{
					find_flag = 1;
					break;
				}
			}
		
			if( 0 == find_flag )
			{
				continue;
			}
			unique_keymap.insert(make_pair(itor_tide_data->first, 0x1));
			pgps_value = &(itor_tide_data->second.back());
	
			query_res_size++;
			if (query_res_size > MAX_QUERY_RESULTS)
			{
				return;
			}
			add_json_response(pgps_value, itor_tide_data->first, json_response);
		}

		if ((*m_ppmem_tide) == m_pmem_tide_data1)
		{
			slave_mem = m_pmem_tide_data2;
		}		
		else
		{
			slave_mem = m_pmem_tide_data1;
		}
		
		for (itor_tide_data = slave_mem->begin(); itor_tide_data != slave_mem->end(); ++itor_tide_data)
		{
			find_flag = 0;
			for (itor = m_pquery_con->key_lists.begin(); itor != m_pquery_con->key_lists.end(); ++itor)
			{
				if (fnmatch_str(itor->key.c_str(), itor_tide_data->first.c_str(), FNM_PATHNAME) != FNM_NOMATCH)
				{
					find_flag = 1;
					break;
				}			
			}
			if( 0 == find_flag )
			{
				continue;
			}
			if (unique_keymap.find(itor_tide_data->first) != unique_keymap.end())
			{
				continue;
			}
			unique_keymap.insert(make_pair(itor_tide_data->first, 0x1));
			pgps_value = &(itor_tide_data->second.back());
		
			query_res_size++;
			if (query_res_size > MAX_QUERY_RESULTS)
			{
				return;
			}
			add_json_response(pgps_value, itor_tide_data->first, json_response);
		}

		json_response[REST_CODE] = REST_OK;
		json_response[REST_MSG] = REST_SUCCESS;
	}
	catch (std::exception & ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsid error:%s\n", ex.what());
	}
}
void CRestQryRealPos::query_real_device_distance(Json::Value& json_response,PQUERY_DISTINCE_DEVICE_CON pquery_con)
{
	try
	{
		unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
		unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	
		GPSVALUE_S pgps_value1;
		GPSVALUE_S pgps_value2;
		int find_flag1 = 0;
		int find_flag2 = 0;

		if ((*m_ppmem_tide) == m_pmem_tide_data1)
		{
			slave_mem = m_pmem_tide_data2;
		}		
		else
		{
			slave_mem = m_pmem_tide_data1;
		}
		
		if ( (itor_tide_data = (*m_ppmem_tide)->find(pquery_con->key1)) != (*m_ppmem_tide)->end())
		{
			find_flag1 = 1;
			pgps_value1 = itor_tide_data->second.back();
		}
		else if((itor_tide_data = slave_mem->find(pquery_con->key1)) != slave_mem->end())
		{
			find_flag1 = 1;
			pgps_value1 = itor_tide_data->second.back();
		}
		
		if ( (itor_tide_data = (*m_ppmem_tide)->find(pquery_con->key2)) != (*m_ppmem_tide)->end())
		{
			find_flag2 = 1;
			pgps_value2 = itor_tide_data->second.back();
		}
		else if((itor_tide_data = slave_mem->find(pquery_con->key2)) != slave_mem->end())
		{
			find_flag2 = 1;
			pgps_value2 = itor_tide_data->second.back();
		}

		json_response[REST_CODE] = REST_OK;
		json_response[REST_MSG] = REST_SUCCESS;
		if((find_flag1 == 1) && (find_flag2 == 1) )
		{
			double distance = GetDistance(atof(pgps_value2.m_sLat), atof(pgps_value2.m_sLong),atof(pgps_value1.m_sLat), atof(pgps_value1.m_sLong));		
			json_response["distance"] = distance;
		}
			
	}
	catch (std::exception & ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsid error:%s\n", ex.what());
	}
}

