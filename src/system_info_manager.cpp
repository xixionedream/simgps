
#include "system_info_manager.h"

system_info_manager::system_info_manager()
{
	
}

system_info_manager::~system_info_manager()
{
	
}

void system_info_manager::qry_server_info(Json::Value &json_value, int type)
{
	try
	{
		bool query_flag = true;
		switch(type)
		{
		case -1:
			{
				query_flag = false;
				json_value[REST_CODE] = RESPONSE_ARRAY[4].code;
				json_value[REST_MSG] = RESPONSE_ARRAY[4].description;
			}
			break;
		case 0:
			for(int i=1; i<6; i++)
			{
				qry_server_info(json_value,i);
			}
			break;
		case 1:
			{
				//server information
				json_value["version"] = CGpsServer::m_pbasic_info->version;
				json_value["company"] = CGpsServer::m_pbasic_info->company;
				json_value["server_name"] = CGpsServer::m_pbasic_info->server_name;
				json_value["server_addr"] = CGpsServer::m_pbasic_info->server_addr;
			}
			break;
		case 2:
			{
				//mem information
				std::string mem_remain_size;
				std::string mem_use_range;
				std::string procee_num;
				if(get_mem_process_info(mem_remain_size, mem_use_range, procee_num))
				{
					json_value["mem_remain_size"] = mem_remain_size;
					json_value["procee_num"] = procee_num;
				}
				else
				{
					query_flag = false;
					json_value[REST_CODE] = RESPONSE_ARRAY[5].code;
					json_value[REST_MSG] = RESPONSE_ARRAY[5].description;
				}
			}
			break;
		case 3:
			{
				//cpu information
				float cpu = 0.00;
				float mem = 0.00;
				if(get_system_usage(cpu,mem))
				{
					json_value["cpu_usage_range"] = value2string(cpu);
					json_value["mem_usage_range"] = value2string(mem);
				}
				else
				{
					query_flag = false;
					json_value[REST_CODE] = RESPONSE_ARRAY[6].code;
					json_value[REST_MSG] = RESPONSE_ARRAY[6].description;
				}
			}
			break;
		case 4:
			{
				//cur process cpu and mem usage information
				float cpu = 0.00;
				float mem = 0.00;
				unsigned long long uptime = 0;
				if(get_process_usage(getpid(),cpu,mem,uptime))
				{
					json_value["cur_process_cpu_usage_range"] = value2string(cpu);
					json_value["cur_process_mem_usage_range"] = value2string(mem);
				}
				else
				{
					query_flag =false;
					json_value[REST_CODE] = RESPONSE_ARRAY[7].code;
					json_value[REST_MSG] = RESPONSE_ARRAY[7].description;
				}
			}
			break;
		case 5:
			{
				//disk usage
				size_t total_size;
				size_t avalible_size;
				size_t free_size;
				if(get_disk_usage(total_size,avalible_size,free_size))
				{
					json_value["disk_total_size"] = value2string(total_size);
					json_value["disk_avalible_size"] = value2string(avalible_size);
					json_value["disk_free_size"] = value2string(free_size);
				}
			}
			break;
		case 6:
			{
				//get cur prcocess detailed message
				process_stat cur_pid_stat;
				int pid = getpid();
				if(get_process_stat(pid,cur_pid_stat))
				{
					json_value["process_name"] = value2string(cur_pid_stat.name);
					json_value["process_pid"] = value2string(pid);
					json_value["process_state"] = value2string(cur_pid_stat.state);
					json_value["process_ppid"] = value2string(cur_pid_stat.ppid);
					json_value["process_pgrp"] = value2string(cur_pid_stat.pgrp);
					json_value["process_session"] = value2string(cur_pid_stat.session);
					json_value["process_tty_nr"] = value2string(cur_pid_stat.tty_nr);
					json_value["process_tpgid"] = value2string(cur_pid_stat.tpgid);
					json_value["process_flags"] = value2string(cur_pid_stat.flags);
					json_value["process_minflt"] = value2string(cur_pid_stat.minflt);
					json_value["process_cminflt"] = value2string(cur_pid_stat.cminflt);
					json_value["process_majflt"] = value2string(cur_pid_stat.majflt);
					json_value["process_cmajflt"] = value2string(cur_pid_stat.cmajflt);
					json_value["process_utime"] = value2string(cur_pid_stat.utime);
					json_value["process_stime"] = value2string(cur_pid_stat.stime);
					json_value["process_cutime"] = value2string(cur_pid_stat.cutime);
					json_value["process_cstime"] = value2string(cur_pid_stat.cstime);
					json_value["process_priority"] = value2string(cur_pid_stat.priority);
					json_value["process_nice"] = value2string(cur_pid_stat.nice);
					json_value["process_threads"] = value2string(cur_pid_stat.threads);
					json_value["process_iterealvalue"] = value2string(cur_pid_stat.iterealvalue);
					json_value["process_starttime"] = value2string(cur_pid_stat.starttime);
					json_value["process_vsize"] = value2string(cur_pid_stat.vsize);
					json_value["process_rss"] = value2string(cur_pid_stat.rss);
				}
				else
				{
					query_flag =false;
					json_value[REST_CODE] = RESPONSE_ARRAY[8].code;
					json_value[REST_MSG] = RESPONSE_ARRAY[8].description;
				}
			}
			break;
		default:
			query_flag = false;
			json_value[REST_CODE] = RESPONSE_ARRAY[3].code;
			json_value[REST_MSG] = RESPONSE_ARRAY[3].description;
			break;
		}
		
		if(query_flag)
		{
			json_value[REST_CODE] = RESPONSE_ARRAY[0].code;
			json_value[REST_MSG] = RESPONSE_ARRAY[0].description;
		}
	}
	catch(...)
	{
		np_log::NP_LOG(np_log::NP_ERROR,"query server type msg exception.");
	}
}