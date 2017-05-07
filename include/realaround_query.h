/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: wangzhen
Last Modify Date:2016-08-23
Description:Query real-time GPS information around the device 
**************************************************************************/
#ifndef _QRYREALAROUND_H
#define _QRYREALAROUND_H
//Qry Real Around .h
#include <math.h>
#include <unordered_map>
#include "utils.h"
#include "circular_queue.h"
#include "tide_server.h"
#include "json/json.h"
#include "cpprest_condition_define.h"
#include <stdlib.h>


class CRestQryRealAround
{
public:
	CRestQryRealAround();
	~CRestQryRealAround();
	/*************************************************
		Function: init
		Description: Initialization query module
		Input: 		ppnum_tide: Pointer to the pointer of real-time memory data
			   pnum_tide_data1: Pointer to the first memory area 
			   pnum_tide_data2: Pointer to the second memory area
 				    pquery_con: Pointer to the query condition of a circle
 				pqueryrect_con: Pointer to the query condition of a rectangle with length of two edges
 	   pqueryrect_twopoint_con: Pointer to the query condition of a rectangle with diagonal points				     
		Return: void
	*************************************************/	
	void init(unordered_map<string, vector<GPSVALUE_S> >** ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_REAL_AROUND_CON pquery_con,PQUERY_REAL_RECTAROUND_CON pqueryrect_con=NULL,PQUERY_REAL_RECTAROUND_TWOPOINT_CON pqueryrect_twopoint_con=NULL);

	/*************************************************
		Function: query_real_around
		Description: Query real-time GPS information around the device,through the latitude and longitude 
		Input: 	range_shape: Query shape(0x01:CIRCLE,0x02:RECTANGLE,0x03:RECTANGLE_TWOPOINT,0x04:POLYGON)
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/	
	void query_real_around(Json::Value& json_response,int range_shape);

	/*************************************************
		Function: query_real_around_bykey
		Description: Query real-time GPS information around the device,through the device GPSID 
		Input: 	type: Query shape(0x01:CIRCLE,0x02:RECTANGLE,0x03:RECTANGLE_TWOPOINT,0x04:POLYGON)
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/	
	void query_real_around_bykey(Json::Value& json_response,int type);

	/*************************************************
		Function: query_real_around_bykey_inrectangle
		Description: Query real-time GPS information in a rectangle,through the device GPSID 
		Input: NULL
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/	
	void query_real_around_bykey_inrectangle(Json::Value& json_response);

	/*************************************************
		Function: query_real_around_bykey_incircular
		Description: Query real-time GPS information in a circle,through the device GPSID 
		Input: NULL
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/
	void query_real_around_bykey_incircular(Json::Value& json_response);

	/*************************************************
		Function: query_real_around_polygon
		Description: Query real-time GPS information in a polygon,through the latitude and longitude of many points 
		Input: NULL
		Output: json_response:The json string of query results	   			     
		Return: void
	*************************************************/
	void query_real_around_polygon(Json::Value& json_response,PQUERY_REAL_POLYGONAROUND_CON polygon_con);
	void query_real_around_wktpolygon(Json::Value& json_response,std::string& polygon_con);
private:
	/*Area range of query condition*/
	typedef union{
 		double query_con_circle[3];	/*Circular area,query_con_circle[0]:longitude,query_con_circle[1]:latitude,query_con_circle[2]:radius*/
		double query_con_rect[4];	/*Rectangular area,query_con_rect[0]:eastLong, query_con_rect[1]:westLong, query_con_rect[2]:southLat,query_con_rect[3]:northLat)*/
		std::vector<QUERY_CON_POLYGONAROUND>* query_con_polygon;	/*Ordered vertices of an arbitrary polygon*/
	}judge_con;

	/*************************************************
		Function: query_mem
		Description: Query real-time GPS information in designated area
		Input: 		type: Query shape(0x01:CIRCLE,0x02:RECTANGLE,0x03:RECTANGLE_TWOPOINT,0x04:POLYGON)
			  pquery_con: Pointer to the query condition
			  ppmem_tide: Pointer to the real-time memory data
		   unique_keymap: Data repeat judgment map
		  query_res_size: Maximum number of query results
		Output: json_response:The json string of query results	   			     
		Return: 0x1: OK
				0x0: Query results have reached the maximum number
	*************************************************/
	int query_mem(int type,void* pquery_con,unordered_map<string, vector<GPSVALUE_S> >* ppmem_tide,unordered_map<string, int>& unique_keymap,int& query_res_size,Json::Value& json_response);

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
private:
	unordered_map<string, vector<GPSVALUE_S> >** m_ppmem_tide;		/*Pointer to the pointer of real-time memory data*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data1;	/*Pointer to the first memory area*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pmem_tide_data2;	/*Pointer to the second memory area*/

	PQUERY_REAL_AROUND_CON m_pquery_con;	/*Pointer to the query condition of a circle*/
	PQUERY_REAL_RECTAROUND_CON m_pqueryrect_con;	/*Pointer to the query condition of a rectangle with length of two edges*/
	PQUERY_REAL_RECTAROUND_TWOPOINT_CON m_pqueryrect_twopoint_con;	/*Pointer to the query condition of a rectangle with diagonal points*/
};

#endif //endif _QRYREALAROUND_H

