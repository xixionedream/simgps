/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: wangzhen
Last Modify Date:2016-08-23
Description:Query real-time GPS information of the device 
**************************************************************************/

#ifndef _QRYREALPOS_H
#define _QRYREALPOS_H

#include <unordered_map>
#include "utils.h"
#include "tcp.h"
#include "circular_queue.h"
#include "tide_server.h"
#include "json/json.h"
#include "cpprest_condition_define.h"


class CRestQryRealPos
{
public:
	CRestQryRealPos();
	~CRestQryRealPos();
	/*************************************************
		Function: init
		Description: Initialization query module
		Input: 		ppnum_tide: Pointer to the pointer of real-time memory data
			   pnum_tide_data1: Pointer to the first memory area 
			   pnum_tide_data2: Pointer to the second memory area
 				    pquery_con: Pointer to the query condition
 				     plastdata: Pointer to the last data for all devices
		Return: void
	*************************************************/
	void init(unordered_map<string, vector<GPSVALUE_S> >** ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_REAL_POS_CON pquery_con,unordered_map<string,GPSVALUE_S>* plastdata);

	/*************************************************
		Function: query_real_pos
		Description: Query real-time GPS information of the specified devices
		Input: NULL
		Output: json_response:The json string of query results
		Return: void
	*************************************************/
	void query_real_pos(Json::Value& json_response);

	/*************************************************
		Function: query_real_all_pos
		Description: Query real-time GPS information of all devices
		Input: NULL
		Output: json_response:The json string of query results
		Return: void
	*************************************************/
	
	void query_real_device_distance(Json::Value& json_response,PQUERY_DISTINCE_DEVICE_CON pquery_con);
private:
	unordered_map<string, vector<GPSVALUE_S> >** m_ppmem_tide;		/*Pointer to the pointer of real-time memory data*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data1;  /*Pointer to the first memory area*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data2;  /*Pointer to the second memory area*/

	unordered_map<string,GPSVALUE_S>* m_plastdata;	/*Pointer to the last data for all devices*/
	PQUERY_REAL_POS_CON m_pquery_con;	/*Pointer to the query condition*/
};
#endif

