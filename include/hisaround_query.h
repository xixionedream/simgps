/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: wangzhen
Last Modify Date:2016-08-23
Description:Query historical GPS information around the device 
**************************************************************************/

#ifndef _HISAROUND_QUERY_H
#define _HISAROUND_QUERY_H
//
#include <unordered_map>
#include "utils.h"
#include "config.h"
#include "tide_server.h"
#include "geohash.h"
#include "circular_queue.h"
#include "json/json.h"

class CRestQryHisAround
{
public:
	CRestQryHisAround();
	~CRestQryHisAround();

	/*************************************************
		Function: init
		Description: Initialization query module
		Input: 		ppnum_tide: Pointer to the pointer of real-time memory data
			   pnum_tide_data1: Pointer to the first memory area 
			   pnum_tide_data2: Pointer to the second memory area
 				    pquery_con: Pointer to the query condition of the circle		     
		Return: void
	*************************************************/	
	void init(unordered_map<string, vector<GPSVALUE_S> >**ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_HIS_AROUND_CON pquery_con=NULL);

	/*************************************************
		Function: query_his_around
		Description: Query historical GPS information in a circle
		Input: NULL
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/	
	void query_his_around(Json::Value& json_response);

	/*************************************************
		Function: query_his_around
		Description: Query historical GPS information in a rectangle
		Input: pquery_con : Pointer to the query condition of the rectangle 
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/
	void query_his_rectaround(Json::Value& json_response,PQUERY_HIS_RECTAROUND_CON pquery_con);		
	void query_his_rectaround_twopoint(Json::Value& json_response,PQUERY_HIS_RECTAROUND_TWOPOINT_CON pquery_con);
	/*************************************************
		Function: query_his_around
		Description: Query historical GPS information in a polygon
		Input: pquery_con : Pointer to the query condition of the polygon 
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/
	void query_his_polygonaround(Json::Value& json_response,PQUERY_HIS_POLYGONAROUND_CON pquery_con); 
private:

	/*Area range of query condition*/
	typedef union{
 		double query_con_circle[3];	/*Circular area,query_con_circle[0]:longitude,query_con_circle[1]:latitude,query_con_circle[2]:radius*/
		double query_con_rect[4];	/*Rectangular area,query_con_rect[0]:eastLong, query_con_rect[1]:westLong, query_con_rect[2]:southLat,query_con_rect[3]:northLat)*/
		std::vector<QUERY_CON_POLYGONAROUND>* query_con_polygon;	/*Ordered vertices of an arbitrary polygon*/
	}judge_con;

	/*************************************************
		Function: judge_inrange
		Description: Judge whether the point is in designated area
		Input: 		type: Query shape(0x01:CIRCLE,0x02:RECTANGLE,0x03:RECTANGLE_TWOPOINT,0x04:POLYGON)
			   longitude: The longitude of the point
			    latitude: The latitude of the point			     
		Return: true: in
				false: not in
	*************************************************/
	bool judge_inrange(double longitude,double latitude,judge_con* con,int type);

	/*************************************************
		Function: query_mem
		Description: Query historical GPS information in memory data
		Input: 		type: Query shape(0x01:CIRCLE,0x02:RECTANGLE,0x03:RECTANGLE_TWOPOINT,0x04:POLYGON)
			  pquery_con: Pointer to the query condition
			  ppmem_tide: Pointer to the pointer of the real-time memory data
		   unique_keymap: Data repeat judgment map
		  query_res_size: Maximum number of query results
		   page_max_size: Maximum value of the query page
		   page_min_size: Minimum value of the query page
		      begin_time: Query start time
		        end_time: Query end time
		Output: json_response:The json string of query results	   			     
		Return: 0x1: OK
				0x0: Query results have reached the maximum number
	*************************************************/
	int  query_mem(int type,void* pquery_con,unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide,unordered_map<string, GPSVALUE_S>& umUnique,std::string begin_time,std::string end_time,Json::Value& json_response);

	/*************************************************
		Function: query_file
		Description: Query historical GPS information in file data
		Input: 		type: Query shape(0x01:CIRCLE,0x02:RECTANGLE,0x03:RECTANGLE_TWOPOINT,0x04:POLYGON)
			  pquery_con: Pointer to the query condition
		   unique_keymap: Data repeat judgment map
		  query_res_size: The number of query results
		   page_max_size: Maximum value of the query page
		   page_min_size: Minimum value of the query page
		      begin_time: Query start time
		        end_time: Query end time
		       sHashCode: Geohash value of the query point
		Output: json_response:The json string of query results	   			     
		Return: 0x1: OK
				0x0: Query results have reached the maximum number
	*************************************************/
	void query_file(Json::Value& json_response, unordered_map<string, GPSVALUE_S>& unique_keymap,std::string begin_time,std::string end_time,char* sHashCode,int type,void* pquery_con);
private:
	unordered_map<string, vector<GPSVALUE_S> >** m_ppmem_tide;		/*Pointer to the pointer of real-time memory data*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data1;	/*Pointer to the first memory area*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data2;	/*Pointer to the second memory area*/
	PQUERY_HIS_AROUND_CON m_pquery_con;	/*Pointer to the query condition of the circle*/
};

#endif //endif _HISAROUND_QUERY_H

