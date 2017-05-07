#ifndef _QRYGPSINFO_H
#define _QRYGPSINFO_H

#include <unordered_map>
#include "utils.h"
#include "tcp.h"
#include "circular_queue.h"
#include "tide_server.h"
#include "json/json.h"
#include "cpprest_condition_define.h"


class CRestQryGpsInfo
{
public:
	CRestQryGpsInfo();
	~CRestQryGpsInfo();
	
	void init(unordered_map<string, string>* m_pmem_gpsinfo);
	void query_gpsinfo(Json::Value& json_response,PQUERY_GPSINFO_CON pquery_con);	
	void add_json_response(Json::Value& json_response,unordered_map<string, string >::iterator iter,std::vector<std::string> output_column);
	int compare_column(std::vector<std::string>& columnName);
private:
	unordered_map<string, string >* m_pmem_gpsinfo;
	unsigned int m_resultNum;
};
#endif

