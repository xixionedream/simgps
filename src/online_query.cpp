
#include "online_query.h"


CRestQryOnline::CRestQryOnline()
{

}

CRestQryOnline::~CRestQryOnline()
{

}

void CRestQryOnline::init(unordered_map<string, vector<GPSVALUE_S> >** ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_ONLINE_CON pquery_con,unordered_map<string,GPSVALUE_S>* plastdata)
{
	this->m_ppmem_tide = ppnum_tide;
	this->m_pmem_tide_data1 = pnum_tide_data1;
	this->m_pmem_tide_data2 = pnum_tide_data2;
	this->m_pquery_con = pquery_con;
	this->m_plastdata = plastdata;
}
void CRestQryOnline::query_mem(unordered_map<string, vector<GPSVALUE_S> >* pmem_tide,unordered_map<string, char>& unique_keymap,std::string judge_time,Json::Value& json_response)
{
	if( NULL == pmem_tide)
	{
		return;
	}
	unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	GPSVALUE_S* pgps_value;
	std::string data_time;
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
	time_t time_now;
	time(&time_now);
	for (itor_tide_data = (pmem_tide)->begin(); itor_tide_data != (pmem_tide)->end(); ++itor_tide_data)
    {
    	if(unique_keymap.find(itor_tide_data->first) != unique_keymap.end())
        {
            continue;
        }
        unique_keymap.insert(make_pair(itor_tide_data->first, 0x1));
        pgps_value = &(itor_tide_data->second.back());
   		data_time = pgps_value->m_sTime;
		sscanf(pgps_value->m_sTime,"%4d%2d%2d%2d%2d%2d",&year,&month,&day,&hour,&minute,&second);
		time_t time_value = GetTick(year,month,day,hour,minute,second);
		Json::Value key_value;
		key_value[REST_KEY] = itor_tide_data->first;
		key_value[REST_TIME] = (int)(time_now - time_value)/60;
		if(m_pquery_con->status == DEVICE_ALLSTATUS)
		{
			if(data_time > judge_time)
			{
				json_response[REST_ONLINE].append(key_value);
			}
			else
			{
				json_response[REST_OFFLINE].append(key_value);
			}
		}
		else if((m_pquery_con->status == DEVICE_ONLINE) && (data_time > judge_time))
		{
			json_response[REST_ONLINE].append(key_value);
		}
		else if((m_pquery_con->status == DEVICE_OFFLINE) && (data_time < judge_time))
		{
			json_response[REST_OFFLINE].append(key_value);
		}      
    }
}

void CRestQryOnline::query_online(Json::Value& json_response)
{
	try
	{		
		unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
        unordered_map<string, char> unique_keymap;
        unordered_map<string, GPSVALUE_S >::iterator itor_last_data;
        GPSVALUE_S* pgps_value;
        std::string data_time;

		int year = 0;
		int month = 0;
		int day = 0;
		int hour = 0;
		int minute = 0;
		int second = 0;
	
		time_t time_now;
		time(&time_now);
		time_t time_judge = time_now - m_pquery_con->judgetime*60;
		struct tm* tm_now;    
		tm_now = localtime(&time_judge);
		char i_time[15];
		memset(i_time,0,15);
		sprintf(i_time,"%04d%02d%02d%02d%02d%02d",tm_now->tm_year+1900,tm_now->tm_mon+1,\
		tm_now->tm_mday,tm_now->tm_hour,tm_now->tm_min,tm_now->tm_sec);
		std::string judge_time = i_time;    
        
        if ((*m_ppmem_tide) == m_pmem_tide_data1)
        {
            slave_mem = m_pmem_tide_data2;
        }		
        else
        {
            slave_mem = m_pmem_tide_data1;
        }
		
        query_mem(*m_ppmem_tide,unique_keymap,judge_time,json_response);
		query_mem(slave_mem,unique_keymap,judge_time,json_response);
             
        for (itor_last_data = m_plastdata->begin(); itor_last_data != m_plastdata->end(); ++itor_last_data)
        {
            if (unique_keymap.find(itor_last_data->first) != unique_keymap.end())
            {
                continue;
            }
            unique_keymap.insert(make_pair(itor_last_data->first, 0x1));
            pgps_value = &(itor_last_data->second);
            data_time = pgps_value->m_sTime;
			
			sscanf(pgps_value->m_sTime,"%4d%2d%2d%2d%2d%2d",&year,&month,&day,&hour,&minute,&second);
			time_t time_value = GetTick(year,month,day,hour,minute,second);
			Json::Value key_value;
			key_value[REST_KEY] = itor_last_data->first;
			key_value[REST_TIME] = (int)(time_now - time_value)/60;

			if(m_pquery_con->status == DEVICE_ALLSTATUS)
			{
				if(data_time > judge_time)
				{
					json_response[REST_ONLINE].append(key_value);
				}
				else
				{
					json_response[REST_OFFLINE].append(key_value);
				}
			}
			else if((m_pquery_con->status == DEVICE_ONLINE) && (data_time > judge_time))
			{
				json_response[REST_ONLINE].append(key_value);
			}
			else if((m_pquery_con->status == DEVICE_OFFLINE) && (data_time < judge_time))
			{
				json_response[REST_OFFLINE].append(key_value);
			}
            
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
