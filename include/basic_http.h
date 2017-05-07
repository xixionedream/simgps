#ifndef _BASIC_HTTP_H
#define _BASIC_HTTP_H

#include "mongoose.h"
#include <map>
#include <functional>

#include "system_info_manager.h"
#include "url_encode.h"
#include "url_parse.h"

class basic_http
{
public:
    using handler = std::function<void(std::string, std::string, std::string&)>;
	typedef struct URI_HANDLER_
	{
		handler handle_func;
		basic_http* pbasic_server;
	}uri_handler;
public:
	virtual ~basic_http(){};
  
	void init_http_server(uint32_t port);
    bool start_server();
    bool close_server();
	bool registe_handler(std::string uri, uri_handler f);
    void unregiste_Handler(std::string uri);
    void loop(int milli);

    void send_reply(std::string uri, std::string reply);
    void send_error(std::string uri, int errcode, std::string reply);
	void reply(mg_connection* conn, std::string reply);

	//work func
	static void handler_qry_gpsid(std::string query, std::string body, std::string& response);
	static void handler_qry_realpos(std::string query, std::string body, std::string& response);
	static void handler_qry_lastpos(std::string query, std::string body, std::string& response);
	static void handler_qry_realaround(std::string query, std::string body, std::string& response);//在圆形范围内查找
	static void handler_qry_realrectaround(std::string query, std::string body, std::string& response);//在矩形范围内查找
	static void handler_qry_realpolygonaround(std::string query, std::string body, std::string& response);//在多边形范围内查找
	
	static void handler_qry_hislocus(std::string query, std::string body, std::string& response);
	static void handler_qry_hisaround(std::string query, std::string body, std::string& response);//在圆形范围内查找
	static void handler_qry_hisrectaround(std::string query, std::string body, std::string& response);//在矩形范围内查找
	static void handler_qry_hispolygonaround(std::string query, std::string body, std::string& response);//在多边形范围内查找
	static void handler_ping(std::string query, std::string body, std::string& response);
	static void handler_qry_version(std::string query, std::string body, std::string& response);
	static void handler_qry_serverinfo(std::string query, std::string body, std::string& response);
	static void handler_write_gps(std::string query, std::string body, std::string& response);
	static void handler_write_gpsinfo(std::string query, std::string body, std::string& response);
	static void handler_qry_online(std::string query, std::string body, std::string& response);
	static void handler_qry_statisticrealeach(std::string query, std::string body, std::string& response);
	static void handler_qry_statisticrealtotal(std::string query, std::string body, std::string& response);
	static void handler_qry_statisdayeach(std::string query, std::string body, std::string& response);
	static void handler_qry_statisdaytotal(std::string query, std::string body, std::string& response);
	static void handler_subscribe_gps(std::string query, std::string body, std::string& response);
	static void handler_qry_distancepoint(std::string query, std::string body, std::string& response);
	static void handler_qry_distancedevice(std::string query, std::string body, std::string& response);
	static void handler_qry_gpsinfo(std::string query, std::string body, std::string& response);
protected:
	using handler_map = std::map<std::string, uri_handler>;
    using connection_map = std::multimap<std::string, mg_connection*>;

private:
	static void ev_handler(struct mg_connection* nc, int ev, void* ev_data);
	static void handler_requst(struct mg_connection* nc, int ev, void *ev_data);
	static std::string pack_json_to_str(Json::Value* json_value);
public:
	static handler_map _handlers;
	static connection_map _connections;
	static basic_http* _pbasic_http;
	char _port[11];
	struct mg_mgr _mgr;

public:
	static CGpsServer* pgps_server;
};

#endif// _BASIC_HTTP_H
