/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: wangzhen
Last Modify Date:2016-08-24
Description:Query historical locus information for the device 
**************************************************************************/

#ifndef _HISLOCUS_QUERY_H
#define _HISLOCUS_QUERY_H
//
#include <unordered_map>
#include "utils.h"
#include "config.h"
#include "circular_queue.h"
#include "tide_server.h"
#include "json/json.h"

#define MAX_QRY_HOUR 24

class CRestQryHisLocus
{
public:
	CRestQryHisLocus();
	~CRestQryHisLocus();

	/*************************************************
		Function: init
		Description: Initialization query module
		Input: 		ppnum_tide: Pointer to the pointer of real-time memory data
			   pnum_tide_data1: Pointer to the first memory area 
			   pnum_tide_data2: Pointer to the second memory area
 				    pquery_con: Pointer to the query condition		     
		Return: void
	*************************************************/	
	void init(unordered_map<string, vector<GPSVALUE_S> >** ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_HIS_LOCUS_CON pquery_con);

	/*************************************************
		Function: query_his_locus
		Description: Query historical locus information for the device
		Input: NULL
		Output: response:The json string of query results	   			     
		Return: void
	*************************************************/	
	void query_his_locus(Json::Value& response);

	/*************************************************
		Function: query_his_locus_infile
		Description: Query historical locus information in the history file
		Input: query_res_size: The number of query results
		Output: response:The json string of query results	   			     
		Return: void
	*************************************************/	
	void query_his_locus_infile(Json::Value& response, int& query_res_size, int page_max_size, int page_min_size);

	/*************************************************
		Function: parse_file_gpsdata
		Description: Parse the buffer from reading the file, judge whether to meet the conditions of the query
		Input: 		psLine: Pointer to a row of data from a file
			   psBeginTime: Pointer to the start time of query condition
				 psEndTime: Pointer to the end time of query condition
					 psKey: Pointer to the key of query condition
			query_res_size: The number of query results
		Output: response:The json string of query results	   			     
		Return: void
	*************************************************/	
	int  parse_file_gpsdata(char* psLine, const char* psBeginTime, const char* psEndTime, const char* psKey, Json::Value& response, int& query_res_size, int page_max_size, int page_min_size);
private:
	unordered_map<string, vector<GPSVALUE_S> >** m_ppmem_tide;		/*Pointer to the pointer of real-time memory data*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data1;	/*Pointer to the first memory area*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data2;	/*Pointer to the second memory area*/
	bool m_end_flag;	/*The flag of end query*/
	PQUERY_HIS_LOCUS_CON m_pquery_con;	/*Pointer to the query condition*/
};
#endif //endif _HISLOCUS_QUERY_H

