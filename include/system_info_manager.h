#ifndef _SYSTEM_INFO_MANAGER_H
#define _SYSTEM_INFO_MANAGER_H

#include "json/json.h"
#include "proc_stat.h"
#include "cpprest_condition_define.h"
#include "tide_server.h"

class system_info_manager
{
public:
	system_info_manager();
	~system_info_manager();
	
public:
	static void qry_server_info(Json::Value &json_value, int type);
};

#endif//_SYSTEM_INFO_MANAGER_H