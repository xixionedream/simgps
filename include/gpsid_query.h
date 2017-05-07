/**
 *
 *Author:rjx
 *Nstruction: provide interface to query gpsid in sys. you can use JSON or WEB to do.\
	The query ways include (query all | query part | query by page).
 *Copyright 2016 netposa inc
 **/ 

#ifndef TIDE_GPSID_QUERY_H_
#define TIDE_GPSID_QUERY_H_

#include "utils.h"
#include "gpsid_writer.h"
#include "json/json.h"
#include "tide_server.h"
#include "tcp.h"
#include "cpprest_condition_define.h"


class CRestQryGpsid
{
public:
	CRestQryGpsid();
	~CRestQryGpsid();
	void Init(GpsidWriter* pgpsid_mem, PQUERY_GPSID_CON pquery_con);
	void QryQgpsid(Json::Value& json_response);

private:
	DISALLOW_COPY_AND_ASSIGN(CRestQryGpsid);

private:
	PQUERY_GPSID_CON m_pquery_con;
	GpsidWriter* m_pgpsid_writer;
};

#endif
