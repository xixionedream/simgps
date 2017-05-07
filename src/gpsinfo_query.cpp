#include "gpsinfo_query.h"


extern GPSINFO_S g_gpsInfoColumn;
CRestQryGpsInfo::CRestQryGpsInfo()
{

}

CRestQryGpsInfo::~CRestQryGpsInfo()
{

}

void CRestQryGpsInfo::init(unordered_map<string, string>* m_pmem_gpsinfo)
{
	this->m_pmem_gpsinfo= m_pmem_gpsinfo;
	this->m_resultNum = 0;
}

void CRestQryGpsInfo::query_gpsinfo(Json::Value& json_response,PQUERY_GPSINFO_CON pquery_con)
{
	try
	{

		if(compare_column(pquery_con->output_column) == 0 )
		{
			if( !(pquery_con->output_column.size() == 1 && pquery_con->output_column[0]=="*"))
			{
				json_response[REST_CODE] = REST_PARAMREEOR;
				json_response[REST_MSG] = "column name is error";
				return;
			}	
		}
		unordered_map<string, string >::iterator iter;
		if(!pquery_con->gps_id.empty())
		{
			for(unsigned int i =0; i< pquery_con->gps_id.size();++i)
			{
				if((iter = m_pmem_gpsinfo->find(pquery_con->gps_id[i])) != m_pmem_gpsinfo->end())
				{
					unsigned int flag = 0;
					int find_flag = -1;
					for(std::map<std::string,std::vector<std::string>>::iterator iterM = pquery_con->query_conditions.begin();iterM != pquery_con->query_conditions.end();++iterM)
					{			
						for(unsigned int i =0; i< iterM->second.size();++i)
						{
							string tmp = iterM->second[i] + ",";
							if( (find_flag=iter->second.find(tmp)) != -1)
							{
								flag++;
								break;
							}
							else if( (find_flag=iter->second.find(iterM->second[i])) != -1)
							{
								if( (find_flag+ iterM->second[i].length()) == iter->second.length() )
								{
									flag++;
									break;
								}
							}
						}
			
					}
					if(flag == pquery_con->query_conditions.size())
					{
						add_json_response(json_response,iter,pquery_con->output_column);
					}
				}
			}
			json_response[REST_CODE] = REST_OK;
			json_response[REST_MSG] = REST_SUCCESS;
			json_response[REST_NUM] = m_resultNum;
			return;
		}
		if(pquery_con->query_conditions.empty())
		{
			for(iter =m_pmem_gpsinfo->begin();iter != m_pmem_gpsinfo->end();++iter )
			{
				add_json_response(json_response,iter,pquery_con->output_column);
			}
			json_response[REST_CODE] = REST_OK;
			json_response[REST_MSG] = REST_SUCCESS;
			json_response[REST_NUM] = m_resultNum;
			return;
		}
		for(iter =m_pmem_gpsinfo->begin();iter != m_pmem_gpsinfo->end();++iter )
		{
			unsigned int flag = 0;
			int find_flag = -1;
			for(std::map<std::string,std::vector<std::string>>::iterator iterM = pquery_con->query_conditions.begin();iterM != pquery_con->query_conditions.end();++iterM)
			{			
				for(unsigned int i =0; i< iterM->second.size();++i)
				{
					string tmp = iterM->second[i] + ",";
					if( (find_flag=iter->second.find(tmp)) != -1)
					{
						flag++;
						break;
					}
					else if( (find_flag=iter->second.find(iterM->second[i])) != -1)
					{
						if( (find_flag+ iterM->second[i].length()) == iter->second.length() )
						{
							flag++;
							break;
						}
					}
				}
			}
			if(flag == pquery_con->query_conditions.size())
			{
				add_json_response(json_response,iter,pquery_con->output_column);
			}
		}
		json_response[REST_CODE] = REST_OK;
		json_response[REST_MSG] = REST_SUCCESS;
		json_response[REST_NUM] = m_resultNum;
	}
	catch (std::exception & ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsinfo error:%s\n", ex.what());
	}
}
void CRestQryGpsInfo::add_json_response(Json::Value& json_response,unordered_map<string, string >::iterator iter,std::vector<std::string> output_column)
{
	Json::Value key_value;
	if(output_column.size() == 1 && (output_column[0] == "*"))
	{
		output_column = g_gpsInfoColumn.columnName;
	}
	for(unsigned int i =0;i < output_column.size();++i)
	{
		if(output_column[i] == GPS_ID)
		{
			key_value[GPS_ID] = iter->first.c_str();
		}
		else
		{
			string ksy;
			string value;
			int symbol_seat = iter->second.find(output_column[i]);
			if(symbol_seat != -1)
			{
				int e_seat = iter->second.find("=",symbol_seat);
				if(e_seat != -1)
				{
					ksy = iter->second.substr(symbol_seat,e_seat - symbol_seat);
				}
				else
				{
					continue;
				}

				int d_seat = iter->second.find(",",e_seat);
				if(d_seat == -1)
				{
					value = iter->second.substr(e_seat+1,iter->second.length()-e_seat-1);
				}
				else
				{
					value = iter->second.substr(e_seat+1,d_seat - e_seat-1);
				}

				key_value[ksy] = value;
			}					
		}		
	}
	if(!key_value.empty())
	{
		json_response[REST_RESULT].append(key_value);
		m_resultNum++;
	}
		
}
int CRestQryGpsInfo::compare_column(std::vector<std::string>& columnName)
{
	unsigned int flag = 0;
	for(unsigned int i =0;i<columnName.size();++i)
	{
		for(unsigned int j =0;j<g_gpsInfoColumn.columnName.size();++j)
		{
			if(columnName[i] == g_gpsInfoColumn.columnName[j])
			{
				flag++;
				break;
			}
		}
	}
	if(flag == columnName.size())
	{
		return -1;
	}
	return 0;
}


