#ifndef _HIREDIS_PROXY
#define _HIREDIS_PROXY

#include "async.h"
#include "hiredis.h"
#include <string>
#include <memory>
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "np_log.h"
#include "utils.h"
#include "gps_pubsub_manager.h"

class hiredis_proxy;

class  hiredis_proxy
{
public:
	hiredis_proxy();
	~hiredis_proxy();

	hiredis_proxy(std::string pub_channel,std::string sub_channel, int maxsize, std::string host = "127.0.0.1", int port = 6379);

private:
	static void connect_callback(const redisAsyncContext *pconnect, int status);
	static void disconnect_callback(const redisAsyncContext *pconnect, int status);
	static void get_callback(const redisAsyncContext *pconnect, void *preply_data, void *pprovide_data);
	static void subcribe_callback(redisAsyncContext *pconnect, void *preply_data, void *provide_data);
	static void publish_callback(const redisAsyncContext *pconnect, void *preply_data, void *pprovide_data);

public:
	void publish_msg(const char* pdata);
	void start_pubsub_service();
	bool connect_redis();
	void close_pubsub_service();
	static void set_manager(void* pman);
	void set_subscribe_channel(std::string subscribe_name = "gps_sub_channel");
	void set_publish_channel(std::string publish_name = "gps_publish_channel");
	bool  get_link_state();

	void start_service(std::string file_path);
	void stop_service();

	void publish_work_thread();
	void start_publish_work();
	
	std::string get_error();
private:
	std::shared_ptr<redisContext> m_pconnect_pub;
	//
	redisAsyncContext* m_pconnect_sub;
	//
	std::string m_redis_host;
	std::string m_subscribe_channel;
	std::string m_publish_channel;
	int m_redis_port;
	bool m_link_state;
	int m_exception_times;
public:
	std::shared_ptr<gps_pubsub_manager> m_pserver;
};


#endif