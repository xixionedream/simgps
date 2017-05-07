/**************************************************************************
Copyright (c) netposa
Author: mg
Update: rjx
Last Date:2016-6-29
Description:tidedb soap service. Defines all entities that are used by the SOAP service
**************************************************************************/

//gsoap ns service style: rpc
//gsoap ns service encoding: encoded
//gsoap ns service name: GPS
#include <string>

//query colum strcut
struct QRYCONDITION_S
{
	std::string Column;
	std::string ColumnValue;
};

//query condition
struct CONDITION_S
{
	struct QRYCONDITION_S** __ptr;
	int __size;
};

//query condition struct
struct GPSQRY_S
{
	std::string Key;
	std::string Lat;
	std::string Long;
	int Redius;
	std::string BeginTime;
	std::string EndTime;
	struct CONDITION_S QueryConditions;	
	int PageNum;
	int PageSize;
};

//gps data struct
struct GPSWRITE_S
{
	std::string Key;
	std::string Time;
	std::string Lat;
	std::string Long;
	std::string Value;
};

//gps data coordinate struct
struct COORDINATE_S
{
	int pos;
	std::string latitude;	
	std::string longitude;	
};

struct COORDINATES_S
{
	struct COORDINATE_S** __ptr;
	int __size;
};

struct TIDEQRY_REGION_S
{
	int PageNum;
	int PageSize;
	struct COORDINATES_S coordinates;	
	struct CONDITION_S QueryConditions;	
};

struct TIDEQRY_S
{
	struct GPSQRY_S** __ptr;
	int __size;
};

struct GPS_S
{
	struct GPSWRITE_S** __ptr;
	int __size;
};

struct GPSRESULT_S
{
	int ResultCode;
	std::string ResultMsg;
};

struct GPSQRST_S
{
	struct GPS_S* QryResult;
	struct GPSRESULT_S* Result;
};

struct GPSID_QUERY_CON
{
	int Size;
	int Query_page;
	int Page_size;
};

struct GPSID
{
	std::string Key;
};

struct QUERY_GPS_RES
{
	struct GPSID** __ptr;
	int __size;
};

struct GPSID_RESULT
{
	struct QUERY_GPS_RES* QryResult;
	struct GPSRESULT_S* Result;
};
