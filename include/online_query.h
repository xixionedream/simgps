/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: wangzhen
Last Modify Date:2016-08-24
Description:Query online devices in the valid time
**************************************************************************/
#ifndef _QRYONLINE_H
#define _QRYONLINE_H

#include <unordered_map>
#include "utils.h"
#include "tcp.h"
#include "circular_queue.h"
#include "tide_server.h"
#include "json/json.h"
#include "cpprest_condition_define.h"


class CRestQryOnline
{
public:
	CRestQryOnline();
	~CRestQryOnline();

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
	void init(unordered_map<string, vector<GPSVALUE_S> >** ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_ONLINE_CON pquery_con,unordered_map<string,GPSVALUE_S>* plastdata);

	/*************************************************
		Function: query_online
		Description: Query online devices in the valid time
		Input: NULL
		Output: json_response:The json string of query results
		Return: void
	*************************************************/
	void query_online(Json::Value& json_response);

private:
	/*************************************************
		Function: query_mem
		Description: Query online devices in memory data
		Input: 	  pmem_tide: Pointer to the real-time memory data
		  	  unique_keymap: Data repeat judgment map
		  	     judge_time: Online judgment time
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/
	void query_mem(unordered_map<string, vector<GPSVALUE_S> >* pmem_tide,unordered_map<string, char>& unique_keymap,std::string judge_time,Json::Value& json_response);
private:
	unordered_map<string, vector<GPSVALUE_S> >** m_ppmem_tide;		/*Pointer to the pointer of real-time memory data*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data1;	/*Pointer to the first memory area*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data2;	/*Pointer to the second memory area*/

	unordered_map<string,GPSVALUE_S>* m_plastdata;	/*Pointer to the last data for all devices*/
	PQUERY_ONLINE_CON m_pquery_con;		/*Pointer to the query condition*/
};
#endif

