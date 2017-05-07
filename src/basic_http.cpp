#include "basic_http.h"

extern bool g_isstart_pubsub;

basic_http::handler_map basic_http::_handlers;
basic_http::connection_map basic_http::_connections;
basic_http* basic_http::_pbasic_http = nullptr;
CGpsServer* basic_http::pgps_server;

void basic_http::init_http_server(uint32_t port)
{
    memset(_port, 0, sizeof(_port));
    snprintf(_port, sizeof(_port), "%u", port);
	basic_http::_pbasic_http = (basic_http*)this;

	//register work ways
	//register handler
	basic_http::uri_handler uri_handle_qrygpsid;
	uri_handle_qrygpsid.pbasic_server = this;
	uri_handle_qrygpsid.handle_func = handler_qry_gpsid;
	registe_handler(QRY_GPSID_REST, uri_handle_qrygpsid);
	//
	basic_http::uri_handler uri_handle_qryrealpos;
	uri_handle_qryrealpos.pbasic_server = this;
	uri_handle_qryrealpos.handle_func = handler_qry_realpos;
	registe_handler(QRY_REAL_POS_REST, uri_handle_qryrealpos);
	//
	basic_http::uri_handler uri_handle_qrylastpos;
	uri_handle_qrylastpos.pbasic_server = this;
	uri_handle_qrylastpos.handle_func = handler_qry_lastpos;
	registe_handler(QRY_LAST_POS_REST, uri_handle_qrylastpos);
	//
	basic_http::uri_handler uri_handle_qryrealaround;
	uri_handle_qryrealaround.pbasic_server = this;
	uri_handle_qryrealaround.handle_func = handler_qry_realaround;
	registe_handler(QRY_REAL_AROUND_REST, uri_handle_qryrealaround);
	//
	basic_http::uri_handler uri_handle_qryrealrectaround;
	uri_handle_qryrealrectaround.pbasic_server = this;
	uri_handle_qryrealrectaround.handle_func = handler_qry_realrectaround;
	registe_handler(QRY_REAL_RECTAROUND_REST, uri_handle_qryrealrectaround);
	//
	basic_http::uri_handler uri_handle_qryrealpolygonaround;
	uri_handle_qryrealpolygonaround.pbasic_server = this;
	uri_handle_qryrealpolygonaround.handle_func = handler_qry_realpolygonaround;
	registe_handler(QRY_REAL_POLYGONAROUND_REST, uri_handle_qryrealpolygonaround);
	//
	basic_http::uri_handler uri_handle_qryhislocus;
	uri_handle_qryhislocus.pbasic_server = this;
	uri_handle_qryhislocus.handle_func = handler_qry_hislocus;
	registe_handler(QRY_HIS_LOCUS_REST, uri_handle_qryhislocus);
	//
	basic_http::uri_handler uri_handle_qryhisaround;
	uri_handle_qryhisaround.pbasic_server = this;
	uri_handle_qryhisaround.handle_func = handler_qry_hisaround;
	registe_handler(QRY_HIS_AROUND_REST, uri_handle_qryhisaround);
	//
	basic_http::uri_handler uri_handle_qryhisrectaround;
	uri_handle_qryhisrectaround.pbasic_server = this;
	uri_handle_qryhisrectaround.handle_func = handler_qry_hisrectaround;
	registe_handler(QRY_HIS_RECTAROUND_REST, uri_handle_qryhisrectaround);
	//
	basic_http::uri_handler uri_handle_qryhispolygonaround;
	uri_handle_qryhispolygonaround.pbasic_server = this;
	uri_handle_qryhispolygonaround.handle_func = handler_qry_hispolygonaround;
	registe_handler(QRY_HIS_POLYGONAROUND_REST, uri_handle_qryhispolygonaround);
	//
	basic_http::uri_handler uri_handle_qryonline;
	uri_handle_qryonline.pbasic_server = this;
	uri_handle_qryonline.handle_func = handler_qry_online;
	registe_handler(QRY_ONLINE_REST, uri_handle_qryonline);
	//
	basic_http::uri_handler uri_handle_ping;
	uri_handle_ping.pbasic_server = this;
	uri_handle_ping.handle_func = handler_ping;
	registe_handler(PING_REST, uri_handle_ping);
	//
	basic_http::uri_handler uri_handle_writegps;
	uri_handle_writegps.pbasic_server = this;
	uri_handle_writegps.handle_func = handler_write_gps;
	registe_handler(WRITE_GPS_REST, uri_handle_writegps);
	//
	basic_http::uri_handler uri_handler_qry_version;
	uri_handler_qry_version.pbasic_server = this;
	uri_handler_qry_version.handle_func = handler_qry_version;
	registe_handler(SERVER_VERSION, uri_handler_qry_version);
	//
	basic_http::uri_handler uri_handler_qry_serverinfo;
	uri_handler_qry_serverinfo.pbasic_server = this;
	uri_handler_qry_serverinfo.handle_func = handler_qry_serverinfo;
	registe_handler(SERVER_INFO, uri_handler_qry_serverinfo);
	//
	basic_http::uri_handler uri_handler_qry_statisticrealeach;
	uri_handler_qry_statisticrealeach.pbasic_server = this;
	uri_handler_qry_statisticrealeach.handle_func = handler_qry_statisticrealeach;
	registe_handler(QRY_STATISTIC_REALEACH_REST, uri_handler_qry_statisticrealeach);
	//
	basic_http::uri_handler uri_handler_qry_statisticrealtotal;
	uri_handler_qry_statisticrealtotal.pbasic_server = this;
	uri_handler_qry_statisticrealtotal.handle_func = handler_qry_statisticrealtotal;
	registe_handler(QRY_STATISTIC_REALTOTAL_REST, uri_handler_qry_statisticrealtotal);
	//
	basic_http::uri_handler uri_handler_qry_statisdayeach;
	uri_handler_qry_statisdayeach.pbasic_server = this;
	uri_handler_qry_statisdayeach.handle_func = handler_qry_statisdayeach;
	registe_handler(QRY_STATISTIC_DAYEACH_REST, uri_handler_qry_statisdayeach);
	//
	basic_http::uri_handler uri_handler_qry_statisdaytotal;
	uri_handler_qry_statisdaytotal.pbasic_server = this;
	uri_handler_qry_statisdaytotal.handle_func = handler_qry_statisdaytotal;
	registe_handler(QRY_STATISTIC_DAYTOTAL_REST, uri_handler_qry_statisdaytotal);
	//
	basic_http::uri_handler uri_handler_subscribe_gps;
	uri_handler_subscribe_gps.pbasic_server = this;
	uri_handler_subscribe_gps.handle_func = handler_subscribe_gps;
	registe_handler(GPS_DATA_SUBSCRIBE, uri_handler_subscribe_gps);
	//
	basic_http::uri_handler uri_handler_qry_distancepoint;
	uri_handler_qry_distancepoint.pbasic_server = this;
	uri_handler_qry_distancepoint.handle_func = handler_qry_distancepoint;
	registe_handler(QRY_DISTANCE_POINT_REST, uri_handler_qry_distancepoint);
	//
	basic_http::uri_handler uri_handler_qry_distancedevice;
	uri_handler_qry_distancedevice.pbasic_server = this;
	uri_handler_qry_distancedevice.handle_func = handler_qry_distancedevice;
	registe_handler(QRY_DISTANCE_DEVICE_REST, uri_handler_qry_distancedevice);
	//
	basic_http::uri_handler uri_handle_writegpsinfo;
	uri_handle_writegpsinfo.pbasic_server = this;
	uri_handle_writegpsinfo.handle_func = handler_write_gpsinfo;
	registe_handler(WRITE_GPSINFO_REST, uri_handle_writegpsinfo);
	//
	basic_http::uri_handler uri_handle_qry_gpsinfo;
	uri_handle_qry_gpsinfo.pbasic_server = this;
	uri_handle_qry_gpsinfo.handle_func = handler_qry_gpsinfo;
	registe_handler(QRY_GPSINFO_REST, uri_handle_qry_gpsinfo);
}

bool basic_http::start_server()
{
    mg_mgr_init(&_mgr, NULL);
	auto nc = mg_bind(&_mgr, _port, ev_handler);

    if(nullptr == nc)
        return false;

    mg_set_protocol_http_websocket(nc);
    return true;
}

bool basic_http::close_server()
{
    mg_mgr_free(&_mgr);
    return true;
}

bool basic_http::registe_handler(std::string uri, uri_handler f)
{
    auto it = _handlers.find(uri);
	if (_handlers.end() != it)
	{
		return false;
	}
    return _handlers.emplace(uri, f).second;
}

void basic_http::unregiste_Handler(std::string uri)
{
    auto it = _handlers.find(uri);
    if(_handlers.end() != it)
        _handlers.erase(it);
}

void basic_http::loop(int milli)
{
    mg_mgr_poll(&_mgr, milli);
}

void basic_http::ev_handler(struct mg_connection* nc, int ev, void* ev_data)
{
   switch(ev)
   {
        case MG_EV_HTTP_REQUEST:
			handler_requst(nc, ev, ev_data);
            break;
        default:
           break;
   }
}

void basic_http::handler_requst(struct mg_connection *nc, int ev, void* ev_data)
{
    http_message* hm = (http_message*)ev_data;
    std::string uri(hm->uri.p, hm->uri.len);

    auto it = _handlers.find(uri);
	std::string response;
	if (_handlers.end() == it)
	{
		if (_pbasic_http != nullptr)
		{
			Json::Value json_response;
			json_response[REST_CODE] = RESPONSE_ARRAY[9].code;
			json_response[REST_MSG] = RESPONSE_ARRAY[9].description;		
			response = pack_json_to_str(&json_response);
			_pbasic_http->reply(nc, response);
		}
		return;
	}
    _connections.emplace(uri, nc);
    it->second.handle_func(std::string(hm->query_string.p, hm->query_string.len),std::string(hm->body.p, hm->body.len), response);
	//reponse client
	if (it->second.pbasic_server != nullptr)
	{
		it->second.pbasic_server->send_reply(uri, response);
	}
}

void basic_http::reply(mg_connection* conn, std::string reply)
{
	mg_printf(conn, "HTTP/1.1 200 OK\r\niConnection: close\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s\r\n", (uint32_t)reply.length(), reply.c_str());
}

void basic_http::send_reply(std::string uri, std::string reply)
{
    auto range = _connections.equal_range(uri);
    if(range.first == range.second)
        return;

    auto it = range.first;
    mg_printf(it->second, "HTTP/1.1 200 OK\r\niConnection: close\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s\r\n", (uint32_t)reply.length(), reply.c_str());

    it->second->flags |= MG_F_SEND_AND_CLOSE;
    _connections.erase(it);
}

void basic_http::send_error(std::string uri, int errcode, std::string reply)
{
    auto range = _connections.equal_range(uri);
    if(range.first == range.second)
        return;

    auto it = range.first;
    mg_printf(it->second, "HTTP/1.1 %d %s\r\n", errcode, reply.c_str());

    it->second->flags |= MG_F_SEND_AND_CLOSE;
    _connections.erase(it);
}

std::string  basic_http::pack_json_to_str(Json::Value* pjson_value)
{
	Json::FastWriter fast_writer;
	return fast_writer.write(*pjson_value);
}
//
void basic_http::handler_qry_gpsid(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_GPSID_CON pquery_con = url_parse::parse_query_gpsid_str(query);
	CRestQryGpsid* pquery_gpsid_server = new CRestQryGpsid();
	Json::Value json_response;
	//copy writer mem address
	GpsidWriter* p_gpsid_query = &(pgps_server->m_gpsid_writer);
	//Init gpsid qry service
	pquery_gpsid_server->Init(p_gpsid_query, pquery_con);
	//qry gpsid
	pquery_gpsid_server->QryQgpsid(json_response);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_gpsid_server != NULL)
	{
		delete pquery_gpsid_server;
	}
}

void basic_http::handler_qry_realpos(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	//key=123&expand=cartype:1$key=234&expand=cartype:2
	PQUERY_REAL_POS_CON pquery_con = url_parse::parse_query_realpos_str(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
    {
        json_response[REST_CODE] = pquery_con->flag;
        json_response[REST_MSG] = pquery_con->message;
        response = pack_json_to_str(&json_response);	
        delete pquery_con;
        return;
    }

	CRestQryRealPos* pquery_realpos_server = new CRestQryRealPos();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_realpos_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, pquery_con,&(pgps_server->m_lastdata));
	pquery_realpos_server->query_real_pos(json_response);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_realpos_server != NULL)
	{
		delete pquery_realpos_server;
	}
	
}
void basic_http::handler_qry_lastpos(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	//key=123&expand=cartype:1$key=234&expand=cartype:2
	PQUERY_LAST_POS_CON pquery_con = url_parse::parse_query_lastpos_str(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
    {
        json_response[REST_CODE] = pquery_con->flag;
        json_response[REST_MSG] = pquery_con->message;
        response = pack_json_to_str(&json_response);	
        delete pquery_con;
        return;
    }

	CRestQryLastPos* pquery_lastpos_server = new CRestQryLastPos();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_lastpos_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, pquery_con,&(pgps_server->m_lastdata));
	pquery_lastpos_server->query_last_pos(json_response);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_lastpos_server != NULL)
	{
		delete pquery_lastpos_server;
	}
	
}

void basic_http::handler_qry_hispolygonaround(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_HIS_POLYGONAROUND_CON pquery_con = url_parse::parse_query_hispolygonaround(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);	
		delete pquery_con;
		return;
	}

	CRestQryHisAround* pquery_hisaround_server = new CRestQryHisAround();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_hisaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2);
	pquery_hisaround_server->query_his_polygonaround(json_response,pquery_con);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_hisaround_server != NULL)
	{
		delete pquery_hisaround_server;
	}
}

void basic_http::handler_qry_hisrectaround(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);

	
	if (query.find("firstlongitude=") != string::npos)
	{
		PQUERY_HIS_RECTAROUND_TWOPOINT_CON pquery_con = url_parse::parse_query_hisrectaround_twopoint(query);
		Json::Value json_response;
		if (pquery_con->flag != REST_OK)
		{
			json_response[REST_CODE] = pquery_con->flag;
			json_response[REST_MSG] = pquery_con->message;
			response = pack_json_to_str(&json_response);
			delete pquery_con;
			return;
		}

		CRestQryHisAround* pquery_hisaround_server = new CRestQryHisAround();
		//copy save gps mem address
		unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
		unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
		unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
		//
		pquery_hisaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2);
		pquery_hisaround_server->query_his_rectaround_twopoint(json_response,pquery_con);
		response = pack_json_to_str(&json_response);
		//cout<<"response:"<<response<<endl;
		//clean
		if (pquery_con != NULL)
		{
			delete pquery_con;
		}
		if (pquery_hisaround_server != NULL)
		{
			delete pquery_hisaround_server;
		}
	}
	else
	{
		PQUERY_HIS_RECTAROUND_CON pquery_con = url_parse::parse_query_hisrectaround(query);
		Json::Value json_response;
		if (pquery_con->flag != REST_OK)
		{
			json_response[REST_CODE] = pquery_con->flag;
			json_response[REST_MSG] = pquery_con->message;
			response = pack_json_to_str(&json_response);
			delete pquery_con;
			return;
		}

		CRestQryHisAround* pquery_hisaround_server = new CRestQryHisAround();
		//copy save gps mem address
		unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
		unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
		unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
		//
		pquery_hisaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2);
		pquery_hisaround_server->query_his_rectaround(json_response,pquery_con);
		response = pack_json_to_str(&json_response);
		//cout<<"response:"<<response<<endl;
		//clean
		if (pquery_con != NULL)
		{
			delete pquery_con;
		}
		if (pquery_hisaround_server != NULL)
		{
			delete pquery_hisaround_server;
		}
	}
	
}
void basic_http::handler_qry_realrectaround(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);

	if (query.find("firstlongitude=") != string::npos)
	{
		PQUERY_REAL_RECTAROUND_TWOPOINT_CON pquery_con = url_parse::parse_query_realrectaround_twopoint(query);	
        Json::Value json_response;
        if (pquery_con->flag != REST_OK)
        {
            json_response[REST_CODE] = pquery_con->flag;
            json_response[REST_MSG] = pquery_con->message;
            response = pack_json_to_str(&json_response);	
            delete pquery_con;
            return;
        }
        
        CRestQryRealAround* pquery_realaround_server = new CRestQryRealAround();
        //copy save gps mem address
        unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
        unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
        unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
        //
        pquery_realaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, NULL,NULL,pquery_con);
        pquery_realaround_server->query_real_around(json_response,QRY_AROUND_RECTANGLE_TWOPOINT);
        response = pack_json_to_str(&json_response);
        //cout<<"response:"<<response<<endl;
        if (pquery_con != NULL)
        {
            delete pquery_con;
        }
        if (pquery_realaround_server != NULL)
        {
            delete pquery_realaround_server;
        }   
	}
	else
	{
		PQUERY_REAL_RECTAROUND_CON pquery_con = url_parse::parse_query_realrectaround(query);
        Json::Value json_response;
        if (pquery_con->flag != REST_OK)
        {
            json_response[REST_CODE] = pquery_con->flag;
            json_response[REST_MSG] = pquery_con->message;
            response = pack_json_to_str(&json_response);	
            delete pquery_con;
            return;
        }
        
        CRestQryRealAround* pquery_realaround_server = new CRestQryRealAround();
        //copy save gps mem address
        unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
        unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
        unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
        //
        pquery_realaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, NULL,pquery_con);
		if(!pquery_con->key.empty())
			pquery_realaround_server->query_real_around_bykey(json_response,QRY_AROUND_RECTANGLE);
		else
       		pquery_realaround_server->query_real_around(json_response,QRY_AROUND_RECTANGLE);
        response = pack_json_to_str(&json_response);
        //cout<<"response:"<<response<<endl;
        if (pquery_con != NULL)
        {
            delete pquery_con;
        }
        if (pquery_realaround_server != NULL)
        {
            delete pquery_realaround_server;
        }   
	}
	
	
}

void basic_http::handler_qry_realaround(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_REAL_AROUND_CON pquery_con = url_parse::parse_query_realaround(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);	
		delete pquery_con;
		return;
	}

	CRestQryRealAround* pquery_realaround_server = new CRestQryRealAround();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_realaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, pquery_con);
	if(!pquery_con->key.empty())
		pquery_realaround_server->query_real_around_bykey(json_response,QRY_AROUND_CIRCLE);
	else
		pquery_realaround_server->query_real_around(json_response,QRY_AROUND_CIRCLE);
	
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_realaround_server != NULL)
	{
		delete pquery_realaround_server;
	}
}

void basic_http::handler_qry_realpolygonaround(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	Json::Value json_response;
	if (query.find("type=2") != string::npos)
	{
		CRestQryRealAround* pquery_realaround_server = new CRestQryRealAround();
		//copy save gps mem address
		unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
		unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
		unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
		//
		pquery_realaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, NULL);
		pquery_realaround_server->query_real_around_wktpolygon(json_response,body);
		response = pack_json_to_str(&json_response);
		return;
	}
	PQUERY_REAL_POLYGONAROUND_CON pquery_con = url_parse::parse_query_realpolygonaround(query);
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);	
		delete pquery_con;
		return;
	}

	CRestQryRealAround* pquery_realaround_server = new CRestQryRealAround();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_realaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, NULL);
	pquery_realaround_server->query_real_around_polygon(json_response,pquery_con);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_realaround_server != NULL)
	{
		delete pquery_realaround_server;
	}
}

void basic_http::handler_qry_hislocus(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_HIS_LOCUS_CON pquery_con = url_parse::parse_query_hislocus(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);
		delete pquery_con;
		return;
	}

	CRestQryHisLocus* pquery_hislocus_server = new CRestQryHisLocus();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_hislocus_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, pquery_con);
	pquery_hislocus_server->query_his_locus(json_response);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_hislocus_server != NULL)
	{
		delete pquery_hislocus_server;
	}
}

void basic_http::handler_qry_online(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_ONLINE_CON pquery_con = url_parse::parse_query_online(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);
		delete pquery_con;
		return;
	}

	CRestQryOnline* pquery_online_server = new CRestQryOnline();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_online_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, pquery_con,&(pgps_server->m_lastdata));
	pquery_online_server->query_online(json_response);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_online_server != NULL)
	{
		delete pquery_online_server;
	}
}

void basic_http::handler_ping(std::string query, std::string body, std::string& response)
{
	Json::Value json_response;

	json_response[REST_CODE] = RESPONSE_ARRAY[0].code;
	json_response[REST_MSG] = RESPONSE_ARRAY[0].description;
	json_response[REST_STATE] = "OK";
	response = pack_json_to_str(&json_response);
}

void basic_http::handler_qry_hisaround(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_HIS_AROUND_CON pquery_con = url_parse::parse_query_hisaround(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);
		delete pquery_con;
		return;
	}

	CRestQryHisAround* pquery_hisaround_server = new CRestQryHisAround();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_hisaround_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, pquery_con);
	pquery_hisaround_server->query_his_around(json_response);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_hisaround_server != NULL)
	{
		delete pquery_hisaround_server;
	}
}

void basic_http::handler_qry_statisticrealeach(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_STATISTIC_REALEACH_CON pquery_con = url_parse::parse_query_statistic_realeach(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);
		delete pquery_con;
		return;
	}

	CRestQryStatistic* pquery_statistic_server = new CRestQryStatistic();

	std::unordered_map<std::string, std::list<STATISTIC_DATA>>* pTimerEachData = &(pgps_server->m_pstatistic->m_TimerEachData);
	std::list<STATISTIC_DATA>* pTimerTotalData  = &(pgps_server->m_pstatistic->m_TimerTotalData);
	std::unordered_map<std::string, unsigned long long>* pEachTotalData = &(pgps_server->m_pstatistic->m_EachTotalData);

	//
	pquery_statistic_server->init(pTimerEachData,pTimerTotalData,pEachTotalData);
	pquery_statistic_server->query_statistic_realeach(json_response,pquery_con);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_statistic_server != NULL)
	{
		delete pquery_statistic_server;
	}
}
void basic_http::handler_qry_statisticrealtotal(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);

	PQUERY_STATISTIC_REALTOTAL_CON pquery_con = url_parse::parse_query_statistic_realtotal(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);
		delete pquery_con;
		return;
	}
	
	CRestQryStatistic* pquery_statistic_server = new CRestQryStatistic();
	std::unordered_map<std::string, std::list<STATISTIC_DATA>>* pTimerEachData = &(pgps_server->m_pstatistic->m_TimerEachData);
	std::list<STATISTIC_DATA>* pTimerTotalData  = &(pgps_server->m_pstatistic->m_TimerTotalData);
	std::unordered_map<std::string, unsigned long long>* pEachTotalData = &(pgps_server->m_pstatistic->m_EachTotalData);

	//
	pquery_statistic_server->init(pTimerEachData,pTimerTotalData,pEachTotalData);
	pquery_statistic_server->query_statistic_realtotal(json_response,pquery_con);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_statistic_server != NULL)
	{
		delete pquery_statistic_server;
	}
}

void basic_http::handler_qry_statisdayeach(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_STATISTIC_DAYEACH_CON pquery_con = url_parse::parse_query_statistic_dayeach(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);
		delete pquery_con;
		return;
	}

	CRestQryStatistic* pquery_statistic_server = new CRestQryStatistic();

	std::unordered_map<std::string, std::list<STATISTIC_DATA>>* pTimerEachData = &(pgps_server->m_pstatistic->m_TimerEachData);
	std::list<STATISTIC_DATA>* pTimerTotalData  = &(pgps_server->m_pstatistic->m_TimerTotalData);
	std::unordered_map<std::string, unsigned long long>* pEachTotalData = &(pgps_server->m_pstatistic->m_EachTotalData);

	//
	pquery_statistic_server->init(pTimerEachData,pTimerTotalData,pEachTotalData);
	pquery_statistic_server->query_statistic_dayeach(json_response,pquery_con);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_statistic_server != NULL)
	{
		delete pquery_statistic_server;
	}
}
void basic_http::handler_qry_statisdaytotal(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);

	PQUERY_STATISTIC_DAYTOTAL_CON pquery_con = url_parse::parse_query_statistic_daytotal(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);
		delete pquery_con;
		return;
	}
	
	CRestQryStatistic* pquery_statistic_server = new CRestQryStatistic();
	std::unordered_map<std::string, std::list<STATISTIC_DATA>>* pTimerEachData = &(pgps_server->m_pstatistic->m_TimerEachData);
	std::list<STATISTIC_DATA>* pTimerTotalData  = &(pgps_server->m_pstatistic->m_TimerTotalData);
	std::unordered_map<std::string, unsigned long long>* pEachTotalData = &(pgps_server->m_pstatistic->m_EachTotalData);

	//
	pquery_statistic_server->init(pTimerEachData,pTimerTotalData,pEachTotalData);
	pquery_statistic_server->query_statistic_daytotal(json_response,pquery_con);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_statistic_server != NULL)
	{
		delete pquery_statistic_server;
	}
}

void basic_http::handler_write_gps(std::string query, std::string body, std::string& response)
{
	Json::Value json_response;
	rest_data_writer gps_writer(&(pgps_server->m_pumGpsInfo), &(pgps_server->m_pLogfd), pgps_server->m_predis_proxy,&(pgps_server->m_gpsid_writer),pgps_server->m_pstatistic);
	gps_writer.write_gpsdata(body, json_response);
	//response
	response = pack_json_to_str(&json_response);
}
void basic_http::handler_write_gpsinfo(std::string query, std::string body, std::string& response)
{
	Json::Value json_response;
	gps_info_writer gpsinfo_writer(&(pgps_server->m_gpsInfoFile.m_fd), &(pgps_server->m_gpsinfodata));
	gpsinfo_writer.write_gpsinfo(body, json_response);
	//response
	response = pack_json_to_str(&json_response);
}

void basic_http::handler_qry_version(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	Json::Value json_response;
	json_response[REST_CODE] = RESPONSE_ARRAY[0].code;
	json_response[REST_MSG] = RESPONSE_ARRAY[0].description;
	json_response["version"] = CGpsServer::m_pbasic_info->version;
	response = pack_json_to_str(&json_response);
}

void basic_http::handler_qry_serverinfo(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	Json::Value json_response;
	int query_info_type = -1;
	int query_seat = query.find("type=");
	if(query_seat != -1)
	{
		std::string type = query.substr(query_seat + 5,query.length() - 5 - query_seat);
		query_info_type = atoi(type.c_str());
	}
	system_info_manager::qry_server_info(json_response, query_info_type);
	response = pack_json_to_str(&json_response);
}


void basic_http::handler_subscribe_gps(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	Json::Value json_response;
	if (g_isstart_pubsub)
	{
		Json::Value query_condtion;
		if (url_parse::parse_subscribe_gps(query, query_condtion, json_response))
		{
			int code = pgps_server->m_predis_proxy->m_pserver->parse_subscribe_conditon(pack_json_to_str(&query_condtion))*(-1);
			json_response[REST_CODE] = RESPONSE_ARRAY[code].code;
			json_response[REST_MSG] = RESPONSE_ARRAY[code].description;
		}
		
	}
	else
	{
		json_response[REST_CODE] = RESPONSE_ARRAY[12].code;
		json_response[REST_MSG] = RESPONSE_ARRAY[12].description;
	}
	response = pack_json_to_str(&json_response);
}
void basic_http::handler_qry_distancepoint(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	Json::Value json_response;
	std::map<std::string,std::string> resultMap;
	std::map<std::string,std::string>::iterator itermap1,itermap2;
	if(false == url_parse::ParseToMap(query,resultMap))
	{
		json_response[REST_CODE] = REST_PARAMREEOR;
		json_response[REST_MSG] = "query conditon is missing";
	}
	else if( (itermap1=resultMap.find("point1")) != resultMap.end() && (itermap2=resultMap.find("point2")) != resultMap.end())
	{
		int seat1 = (*itermap1).second.find(',');
		int seat2 = (*itermap2).second.find(',');
		if(seat1 <= -1 || seat2 <= -1)
		{
			json_response[REST_CODE] = REST_PARAMREEOR;
			json_response[REST_MSG] = "query conditon is error";
		}
		else
		{
			double longitude1 = atof(((*itermap1).second.substr(0,seat1)).c_str());
			double latitude1 = atof(((*itermap1).second.substr(seat1+1,(*itermap1).second.length()- seat1 -1)).c_str());

			double longitude2 = atof(((*itermap2).second.substr(0,seat2)).c_str());
			double latitude2 = atof(((*itermap2).second.substr(seat2+1,(*itermap2).second.length()- seat2 -1)).c_str());
		
			double distance = GetDistance(latitude1,longitude1,latitude2, longitude2);
			json_response[REST_CODE] = REST_OK;
			json_response[REST_MSG] = REST_SUCCESS;
			json_response["distance"] = distance;
		}
		
	}
	else
	{
		json_response[REST_CODE] = REST_PARAMREEOR;
		json_response[REST_MSG] = "query conditon is missing";
	}
	response = pack_json_to_str(&json_response);
}
void basic_http::handler_qry_distancedevice(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	//key=123&expand=cartype:1$key=234&expand=cartype:2
	PQUERY_DISTINCE_DEVICE_CON pquery_con = url_parse::parse_query_devicedistance(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
    {
        json_response[REST_CODE] = pquery_con->flag;
        json_response[REST_MSG] = pquery_con->message;
        response = pack_json_to_str(&json_response);	
        delete pquery_con;
        return;
    }

	CRestQryRealPos* pquery_realpos_server = new CRestQryRealPos();
	//copy save gps mem address
	unordered_map<string, vector<GPSVALUE_S> >** ppmem_tide = &(pgps_server->m_pumGpsInfo);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data1 = &(pgps_server->m_umGpsInfo1);
	unordered_map<string, vector<GPSVALUE_S> >* pmem_tide_data2 = &(pgps_server->m_umGpsInfo2);
	//
	pquery_realpos_server->init(ppmem_tide, pmem_tide_data1, pmem_tide_data2, NULL,&(pgps_server->m_lastdata));
	pquery_realpos_server->query_real_device_distance(json_response,pquery_con);
	response = pack_json_to_str(&json_response);
	//cout<<"response:"<<response<<endl;
	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_realpos_server != NULL)
	{
		delete pquery_realpos_server;
	}
	
}

void basic_http::handler_qry_gpsinfo(std::string query, std::string body, std::string& response)
{
	query = url_encode::decode(query);
	PQUERY_GPSINFO_CON pquery_con = url_parse::parse_query_gpsinfo(query);
	Json::Value json_response;
	if (pquery_con->flag != REST_OK)
	{
		json_response[REST_CODE] = pquery_con->flag;
		json_response[REST_MSG] = pquery_con->message;
		response = pack_json_to_str(&json_response);
		delete pquery_con;
		return;
	}

	CRestQryGpsInfo* pquery_gpsinfo_server = new CRestQryGpsInfo();
	//copy save gps mem address
	unordered_map<string, string>* pmem_gpsinfo = &(pgps_server->m_gpsinfodata);
	//
	pquery_gpsinfo_server->init(pmem_gpsinfo);
	pquery_gpsinfo_server->query_gpsinfo(json_response,pquery_con);
	response = pack_json_to_str(&json_response);

	//clean
	if (pquery_con != NULL)
	{
		delete pquery_con;
	}
	if (pquery_gpsinfo_server != NULL)
	{
		delete pquery_gpsinfo_server;
	}
}

