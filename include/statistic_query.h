/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: wangzhen
Last Modify Date:2016-08-24
Description:Query statistics data
**************************************************************************/

#ifndef _STATISTIC_QUERY_H
#define _STATISTIC_QUERY_H
//
#include <list>
#include <unordered_map>
#include "utils.h"
#include "config.h"
#include "tide_server.h"
#include "geohash.h"
#include "circular_queue.h"
#include "json/json.h"

typedef enum {
  QUERY_CURRENT = 0,            /* Current total */
  QUERY_CUMULATIVE,       		/* Cumulative value in statistical time */
  QUERY_DIFFER,      		 	/* Difference value in statistical time*/
} E_STATISTIC_QUERY_TYPE;

class CRestQryStatistic
{
public:
	CRestQryStatistic(){};
	~CRestQryStatistic(){};

	/*************************************************
		Function: init
		Description: Initialization query module
		Input: 	pTimerEach: Pointer to the statistical data of the device
			   pTimerTotal: Pointer to the statistical total data of all devices
			   		 pReal: Pointer to the latest real-time statistics	
		Return:void
	*************************************************/		
	void init(std::unordered_map<std::string, std::list<STATISTIC_DATA>>* pTimerEach,std::list<STATISTIC_DATA>* pTimerTotal,std::unordered_map<std::string, unsigned long long>* pReal);

	/*************************************************
		Function: query_statistic_realeach
		Description: Query statistics data of the device
		Input: 	pquery_con: Pointer to the query condition
		Output: json_response:The json string of query results	   
		Return:void
	*************************************************/	
	void query_statistic_realeach(Json::Value& json_response,PQUERY_STATISTIC_REALEACH_CON pquery_con);

	/*************************************************
		Function: query_statistic_realtotal
		Description: Query statistics total data of all device
		Input: 	pquery_con: Pointer to the query condition
		Output: json_response:The json string of query results	   
		Return:void
	*************************************************/	
	void query_statistic_realtotal(Json::Value& json_response,PQUERY_STATISTIC_REALTOTAL_CON pquery_con);
	void query_statistic_daytotal(Json::Value& json_response,PQUERY_STATISTIC_DAYTOTAL_CON pquery_con);
	void query_statistic_dayeach(Json::Value& json_response,PQUERY_STATISTIC_DAYEACH_CON pquery_con);
private:
	std::unordered_map<std::string, std::list<STATISTIC_DATA>>* m_pTimerEachData;	/*Statistical data of the device*/
	std::list<STATISTIC_DATA>* m_pTimerTotalData;	/*Statistical total data of all devices*/
	std::unordered_map<std::string, unsigned long long>* m_pEachTotalData;	/*The latest real-time statistics*/

};

#endif //endif _STATISTIC_QUERY_H

