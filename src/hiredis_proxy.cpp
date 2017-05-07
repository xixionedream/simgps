#include "hiredis_proxy.h"
#include "gps_pubsub_manager.h"
#include "libev.h"

gps_pubsub_manager *g_subscribe_server = nullptr;

hiredis_proxy::hiredis_proxy()
{
	this->m_redis_host = "127.0.0.1";
	this->m_redis_port = 6379;
	this->m_exception_times = 0;
	set_subscribe_channel();
	set_publish_channel();
	m_pserver = std::shared_ptr<gps_pubsub_manager>(new gps_pubsub_manager(1000));
}

hiredis_proxy::~hiredis_proxy()
{

}

void hiredis_proxy::set_manager(void* pman)
{
	g_subscribe_server = (gps_pubsub_manager*)(pman);
}

hiredis_proxy::hiredis_proxy(std::string pub_channel, std::string sub_channel, int maxsize, std::string host, int port)
{
	this->m_redis_host = host;
	this->m_redis_port = port;
	this->m_exception_times = 0;
	set_publish_channel(pub_channel);
	set_subscribe_channel(sub_channel);
	m_pserver = std::shared_ptr<gps_pubsub_manager>(new gps_pubsub_manager(maxsize));
}


void hiredis_proxy::set_subscribe_channel(std::string subscribe_name)
{
	this->m_subscribe_channel = subscribe_name;
}

void hiredis_proxy::set_publish_channel(std::string publish_name)
{
	this->m_publish_channel = publish_name;
}


void hiredis_proxy::connect_callback(const redisAsyncContext *pconnect, int status)
{
	if (status == REDIS_OK) 
	{
		NP_LOG(np_log::NP_INFO, "connect to redis server success.");
	}
	else
	{
		NP_LOG(np_log::NP_ERROR, "connect redis server error.msg.msg:%s", pconnect->errstr);
	}
}

void hiredis_proxy::disconnect_callback(const redisAsyncContext *pconnect, int status)
{
	if (status == REDIS_OK)
	{
		NP_LOG(np_log::NP_INFO, "disconnect to redis server success.");
	}
	else
	{
		NP_LOG(np_log::NP_ERROR, "disconnect redis server error.msg:%s", pconnect->errstr);
	}
}

void hiredis_proxy::get_callback(const redisAsyncContext *pconnect, void *preply_data, void *pprovide_data)
{
	redisReply *preply = (redisReply *)pconnect;
	if (preply != NULL)
	{
		//get data from redis db. [write your data processing logic]
	}
}

void hiredis_proxy::subcribe_callback(redisAsyncContext *pconnect, void *preply_data, void *provide_data)
{
	redisReply *preply = (redisReply *)preply_data;
	if (preply == NULL) return;
	if (preply->type == REDIS_REPLY_ARRAY && preply->elements == 3)
	{
		if(strcmp(preply->element[0]->str, "subscribe") != 0)
		{
			//std::cout << "[recv data from channel][" << preply->element[1]->str << "].data:" << preply->element[2]->str << std::endl;
			//subscribed gps info
			g_subscribe_server->parse_subscribe_conditon(preply->element[2]->str);
		}
	}
}

void hiredis_proxy::publish_callback(const redisAsyncContext *pconnect, void *preply_data, void *pprovide_data)
{
	redisReply *preply = (redisReply *)pconnect;
	if (preply != NULL)
	{
		//publish callback  [write your data processing logic]
	}
}
void hiredis_proxy::publish_msg(const char* pdata)
{
	redisCommand(this->m_pconnect_pub.get(), "PUBLISH %s %s", m_publish_channel.c_str(), pdata);
}

bool  hiredis_proxy::get_link_state()
{
	return this->m_link_state;
}

std::string hiredis_proxy::get_error()
{
	return this->m_pconnect_pub->errstr;
}

bool hiredis_proxy::connect_redis()
{
	try
	{
		m_pconnect_pub = std::shared_ptr<redisContext>(redisConnect(m_redis_host.c_str(), m_redis_port));
		if (m_pconnect_pub->err)
		{
			m_link_state = false;
			NP_LOG(np_log::NP_ERROR, "redis publish proxy connect error.");
			return false;
		}
		
		this->m_pconnect_sub = redisAsyncConnect(m_redis_host.c_str(), m_redis_port);
		if (m_pconnect_sub == nullptr || m_pconnect_sub->err)
		{
			m_link_state = false;
			NP_LOG(np_log::NP_ERROR, "redis subscribed proxy connect error.");
			return false;
		}
		m_link_state = true;
		np_log::NP_STDOUT("* connect to redis %s:%d.",m_redis_host.c_str(), m_redis_port);
	}
	catch (std::exception & ex)
	{
		NP_LOG(np_log::NP_ERROR, "redis connect error.msg:%s", ex.what());
	}
	return this->m_link_state;
}

void hiredis_proxy::start_pubsub_service()
{
	try
	{
		while(true)
		{
			//
			if (!this->m_link_state)
			{
				while(!connect_redis())
				{
					sleep(10);
				}
			}
			//
			redisLibevAttach(EV_DEFAULT_ this->m_pconnect_sub);
			//register callback
			redisAsyncSetConnectCallback(this->m_pconnect_sub, connect_callback);
			redisAsyncSetDisconnectCallback(this->m_pconnect_sub, disconnect_callback);
			//std::cout<<"subscribed channel:"<< m_subscribe_channel<<std::endl;
			//start a thread to publish subscribed gps data.
			start_publish_work();
			//subscribe channel this->m_subscribe_channel.c_str() //this->m_subscribe_channel.c_str()
			redisAsyncCommand(this->m_pconnect_sub, subcribe_callback, NULL, "SUBSCRIBE %s", m_subscribe_channel.c_str());
			//loop
			ev_loop(EV_DEFAULT_ 0);
			//
			NP_LOG(np_log::NP_ERROR, "pub sub service exception.it's the %d times exit.", ++m_exception_times);
			this->m_link_state = false;
			sleep(3);
		}
	}
	catch (std::exception & ex)
	{
		m_link_state = false;
		NP_LOG(np_log::NP_ERROR, "start pubsub service[redis] error.msg:%s", ex.what());
	}
}

void hiredis_proxy::close_pubsub_service()
{
	if (m_link_state)
	{
		redisAsyncDisconnect(m_pconnect_sub);
		redisAsyncFree(m_pconnect_sub);
		m_link_state = false;
	}
}

void hiredis_proxy::publish_work_thread()
{
	np_log::NP_STDOUT("* start gps data publish work service.");
	while (this->m_link_state)
	{
		if (this->m_pserver->get_buffer_queue_size() <= 0)
		{
			//usleep(500*1000);
			sleep(1);
			continue;
		}
		gps_pubsub_manager::GPS_DATA gpsdata = m_pserver->m_pgps_list->front();
		this->m_pserver->m_pgps_list->pop();
		std::vector<int> ids;
		for (auto &cond : this->m_pserver->m_subscribe_cons)
		{
			if (cond.second->type == SUB_ALL)
			{
				ids.push_back(cond.first);
			}
			else if (cond.second->type == SUB_KEYS)
			{
				GPS_KEYS_SUBSCRIBE_CON* pcon = (GPS_KEYS_SUBSCRIBE_CON*)cond.second->m_pcon;
				if (pcon->key_map.find(gpsdata.key) != pcon->key_map.end())
				{
					ids.push_back(cond.first);
				}
			}
			else if (cond.second->type == SUB_CIRCLE)
			{
				//conditon array
				GPS_CIRCLE_AREA_SUBSCRIBE_CON* pcon = (GPS_CIRCLE_AREA_SUBSCRIBE_CON*)cond.second->m_pcon;
				if (pcon->is_in_circle(atof(gpsdata.laitude.c_str()), atof(gpsdata.longitude.c_str())))
				{
					ids.push_back(cond.first);
				}
			}
			else if (cond.second->type == SUB_RECT)
			{
				//conditon array
				GPS_RECT_AREA_SUBSCRIBE_CON* prect = (GPS_RECT_AREA_SUBSCRIBE_CON*)cond.second->m_pcon;
				if (prect->is_in_rect(atof(gpsdata.laitude.c_str()), atof(gpsdata.longitude.c_str())))
				{
					ids.push_back(cond.first);
				}
			}
		}
		if (ids.size() > 0)
		{
			publish_msg(m_pserver->produce_pub_result(gpsdata, ids).c_str());
		}
	}

	np_log::NP_STDOUT("* exit publish wrok service ...");
}

void hiredis_proxy::start_publish_work()
{
	if (get_link_state())
	{
		std::thread publish_thread = std::thread(&hiredis_proxy::publish_work_thread, this);
		publish_thread.detach();
	}
	else
	{
		NP_LOG(np_log::NP_ERROR, "redis service link is not success. so start publish work failed!");
	}
}


void hiredis_proxy::start_service(std::string file_path)
{
	m_pserver->init_subscribe_file(file_path);

	g_subscribe_server = this->m_pserver.get();
	//assert(hiredis_proxy::set_gpsmanager != nullptr);
	assert(this != nullptr);
	//connect redis
	while(!connect_redis())
	{
		sleep(15);
	}

	//start pub service(register callback and statr event mechanism)
	start_pubsub_service();
}

void hiredis_proxy::stop_service()
{
	close_pubsub_service();
	m_pserver->clear_sub_condition();
}