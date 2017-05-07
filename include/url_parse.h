
#ifndef _URL_PARSE_H
#define _URL_PARSE_H

#include "utils.h"
#include "cpprest_condition_define.h"
#include "np_log.h"

#define SPILT_CHR '&'
#define ASSIGN_CHR '='

class url_parse
{
public:
	url_parse(){}
	~url_parse(){}
	
public:
	static bool JudgeRange(double longitude, double latitude);
	static int  JudgeTime(std::string begin_time,std::string end_time,int maxHours,std::string& outMessage);
	static bool ParseToMap(std::string parseStr, std::map<std::string, std::string>& outMap);
	static void ParseToMapList(std::string parseStr, std::map<std::string, std::list<std::string>>& outMap);
	static void parse_url_by_spilt(std::string parse_str, std::vector<std::string>& tmp_vec);
	static void split(const std::string& str, std::vector<string>& tokens, const std::string& delimiters);
		
	static bool parse_subscribe_gps(std::string query,Json::Value& value, Json::Value& response);
    static PQUERY_GPSID_CON parse_query_gpsid_str(std::string query_con);
	static PQUERY_REAL_POS_CON parse_query_realpos_str(std::string query_con);
	static PQUERY_LAST_POS_CON parse_query_lastpos_str(std::string query_con);
	static PQUERY_REAL_AROUND_CON parse_query_realaround(std::string query_con);
	static PQUERY_REAL_POLYGONAROUND_CON parse_query_realpolygonaround(std::string query_con);	
	static PQUERY_REAL_RECTAROUND_CON parse_query_realrectaround(std::string query_con);
	static PQUERY_REAL_RECTAROUND_TWOPOINT_CON parse_query_realrectaround_twopoint(std::string query_con);
	static PQUERY_HIS_AROUND_CON parse_query_hisaround(std::string query_con);
	static PQUERY_HIS_RECTAROUND_CON parse_query_hisrectaround(std::string query_con);
	static PQUERY_HIS_POLYGONAROUND_CON parse_query_hispolygonaround(std::string query_con);
	static PQUERY_HIS_RECTAROUND_TWOPOINT_CON parse_query_hisrectaround_twopoint(std::string query_con);
	static PQUERY_HIS_LOCUS_CON parse_query_hislocus(std::string query_con);	
	static PQUERY_ONLINE_CON parse_query_online(std::string query_con);
	static PQUERY_STATISTIC_REALEACH_CON parse_query_statistic_realeach(std::string query_con);
	static PQUERY_STATISTIC_REALTOTAL_CON parse_query_statistic_realtotal(std::string query_con);
	static PQUERY_STATISTIC_DAYEACH_CON parse_query_statistic_dayeach(std::string query_con);
	static PQUERY_STATISTIC_DAYTOTAL_CON parse_query_statistic_daytotal(std::string query_con);
	static PQUERY_DISTINCE_DEVICE_CON parse_query_devicedistance(std::string query_con);
	static PQUERY_GPSINFO_CON parse_query_gpsinfo(std::string query_con);
};

#endif
