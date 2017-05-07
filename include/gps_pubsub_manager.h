#ifndef _GPS_PUBSUB_MANAGER
#define _GPS_PUBSUB_MANAGER

#include <list>
#include <queue>
#include <memory>
#include <thread>
#include <algorithm>
#include <assert.h>

#include "gps_keys_subscribe_con.h"
#include "subscribe_conditon.h"
#include "gps_area_subscribe_con.h"
#include "cpprest_condition_define.h"
#include "json/json.h"

class gps_pubsub_manager
{
public:
	gps_pubsub_manager();
	gps_pubsub_manager(int maxsize = 1000);
	~gps_pubsub_manager();

public:
	typedef struct GPS_DATA_
	{
		std::string key;
		std::string time;
		std::string laitude;
		std::string longitude;
		std::string expand;
	}GPS_DATA,*PGPS_DATA;

#if 0
	gps_pubsub_manager(gps_pubsub_manager& gps_pubsub);
	void operator=(gps_pubsub_manager& gps_pubsub);
	void operator+=(gps_pubsub_manager& gps_pubsub);
#endif
public:
	int parse_subscribe_conditon(std::string condition);
	void push(std::string key, GPSVALUE_S value);
	void push(GPS_DATA gps_value);
	int get_buffer_queue_size();
	int get_sub_conditon_size();
	void clear_sub_condition();
	std::string produce_pub_result(GPS_DATA gpsdata, std::vector<int> ids);

public:
	void init_subscribe_file(std::string file_path);
	void read_subscribe_msg();
	void persitence_subscribe_file();

private:
	bool add_subscribe_by_keys(subscribe_conditon<void*>* psubs, int customer_id, Json::Value&  json_root);
	bool add_subscribe_by_circle(subscribe_conditon<void*> *sub_list, int customer_id, Json::Value&  json_root);
	bool add_subscribe_by_rect(subscribe_conditon<void*>* sub_list, int customer_id, Json::Value&  json_root);
	std::string convert_gps_to_jsonstr(GPS_DATA gpsdata);
private:
	int max_buffer_size;
	std::string m_file_path;
	std::string m_file_name;

public:
	std::shared_ptr<std::queue<GPS_DATA>> m_pgps_list;
	std::map<int, subscribe_conditon<void*>*> m_subscribe_cons;
};
#endif