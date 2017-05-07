#include "gps_pubsub_manager.h"

gps_pubsub_manager::gps_pubsub_manager()
{
	this->max_buffer_size = 1000;
	this->m_pgps_list = std::shared_ptr<std::queue<GPS_DATA>>(new std::queue<GPS_DATA>());
}

gps_pubsub_manager::gps_pubsub_manager(int maxsize)
{
	this->max_buffer_size = maxsize;
	this->m_pgps_list = std::shared_ptr<std::queue<GPS_DATA>>(new std::queue<GPS_DATA>());
}

gps_pubsub_manager::~gps_pubsub_manager()
{

}

void gps_pubsub_manager::push(std::string key, GPSVALUE_S value)
{
	if (((int)m_pgps_list->size() < this->max_buffer_size) && ((int)this->m_subscribe_cons.size() > 0))
	{
		GPS_DATA gpsdata;
		gpsdata.key = key;
		gpsdata.laitude = value.m_sLat;
		gpsdata.longitude = value.m_sLong;
		gpsdata.time = value.m_sTime;
		gpsdata.expand = value.m_sRemain;
		m_pgps_list->push(gpsdata);
	}
}

void gps_pubsub_manager::push(GPS_DATA gps_value)
{
	if (((int)m_pgps_list->size() < this->max_buffer_size) && ((int)this->m_subscribe_cons.size() > 0))
	{
		m_pgps_list->push(gps_value);
	}
}

int gps_pubsub_manager::get_buffer_queue_size()
{
	return this->m_pgps_list->size();
}

int gps_pubsub_manager::get_sub_conditon_size()
{
	return this->m_subscribe_cons.size();
}

void gps_pubsub_manager::clear_sub_condition()
{
	this->m_subscribe_cons.clear();
}


std::string gps_pubsub_manager::convert_gps_to_jsonstr(GPS_DATA gpsdata)
{
	Json::Value json_gps;
	json_gps[KEY] = gpsdata.key;
	json_gps[TIME] = gpsdata.time;
	json_gps[LATITUDE] = gpsdata.laitude;
	json_gps[LONGITUDE] = gpsdata.longitude;
	json_gps[EXPAND] = gpsdata.expand;
	
	Json::FastWriter writer;
	return writer.write(json_gps);
}

std::string gps_pubsub_manager::produce_pub_result(GPS_DATA gpsdata, std::vector<int> ids)
{
	Json::Value gps_group;
	gps_group[KEY] = gpsdata.key;
	gps_group[TIME] = gpsdata.time;
	gps_group[LATITUDE] = gpsdata.laitude;
	gps_group[LONGITUDE] = gpsdata.longitude;
	gps_group[EXPAND] = gpsdata.expand;

	//gps_group[CUSTOMERS]
	std::for_each(ids.begin(), ids.end(), [&gps_group](int &val)\
	{Json::Value usr; gps_group[CUSTOMERS].append((usr[SUBUSER] = val)); });

	Json::FastWriter writer;
	return writer.write(gps_group);
}

bool gps_pubsub_manager::add_subscribe_by_keys(subscribe_conditon<void*>* psubs, int customer_id, Json::Value&  json_root)
{
	bool ret_res = true;
	GPS_KEYS_SUBSCRIBE_CON* psub_tmp = (GPS_KEYS_SUBSCRIBE_CON*)psubs->m_pcon;
	Json::Value json_keys = json_root[SUBKEY];
	for (unsigned int i = 0; i < json_keys.size(); i++)
	{
		psub_tmp->key_map.insert(pair<std::string, char>(json_keys[i].asString(), 0x0));
	}
	if (json_keys.size() <= 0)
	{
		ret_res = false;
	}
	return ret_res;
}

bool gps_pubsub_manager::add_subscribe_by_circle(subscribe_conditon<void*>* sub_list, int customer_id, Json::Value&  json_root)
{
	bool ret_res = true;
	GPS_CIRCLE_AREA_SUBSCRIBE_CON* pcon = (GPS_CIRCLE_AREA_SUBSCRIBE_CON*)sub_list->m_pcon;
	pcon->redius = json_root[SUBREDIUS].asInt();
	std::string lat = json_root[LATITUDE].asString();
	std::string longi = json_root[LONGITUDE].asString();
	pcon->latitude = atof(lat.c_str());
	pcon->longitude = atof(longi.c_str());
	//std::cout << lat << "   " << pcon->latitude << " " << longi << "  " << pcon->longitude << std::endl;
	if (pcon->redius <= 0 || pcon->latitude < -90 || pcon->latitude > 90 || pcon->longitude < -180 || pcon->longitude > 180)
	{
		ret_res = false;
	}
	return ret_res;
}

bool gps_pubsub_manager::add_subscribe_by_rect(subscribe_conditon<void*>* sub_list, int customer_id, Json::Value&  json_root)
{
	bool ret_res = true;
	GPS_RECT_AREA_SUBSCRIBE_CON* pcon = (GPS_RECT_AREA_SUBSCRIBE_CON*)sub_list->m_pcon;
	std::string lat1 = json_root[LATITUDEONE].asString();
	std::string longi1 = json_root[LONGITUDEONE].asString();
	std::string lat2 = json_root[LATITUDETWO].asString();
	std::string longi2 = json_root[LONGITUDETWO].asString();
	pcon->latitude_one = atof(lat1.c_str());
	pcon->longitude_one = atof(longi1.c_str());
	pcon->latitude_two = atof(lat2.c_str());
	pcon->longitude_two = atof(longi2.c_str());
	//std::cout << lat << "   " << pcon->latitude << " " << longi << "  " << pcon->longitude << std::endl;
	if (pcon->latitude_one < -90 || pcon->latitude_one > 90 || pcon->longitude_one < -180 || pcon->longitude_one > 180 || pcon->latitude_two < -90 || pcon->latitude_two > 90 || pcon->longitude_two < -180 || pcon->longitude_two > 180)
	{
		ret_res = false;
	}
	return ret_res;
}

int gps_pubsub_manager::parse_subscribe_conditon(std::string condition)
{
	Json::Reader json_reader;
	Json::Value  json_root;
	if (!json_reader.parse(condition.c_str(), json_root))
	{
		NP_LOG(np_log::NP_WARNING, "subscribed msg is not a json str");
		return RESPONSE_ARRAY[9].code;
	}
	
	//parse subscribe type
	int type = json_root[SUBTYPE].asInt();
	int customer_id = json_root[CUSTOMER_ID].asInt();

	if (m_subscribe_cons.find(customer_id) == m_subscribe_cons.end() && type == UNSUB)
	{
		return RESPONSE_ARRAY[13].code; // the user don't subscribe any data.
	}

	if (m_subscribe_cons.find(customer_id) != m_subscribe_cons.end() && type != UNSUB)
	{
		return RESPONSE_ARRAY[14].code; // the user has subscribed. it needs to unsubscribe firstly.
	}

	//unsubscribe
	std::map<int, subscribe_conditon<void*>*>::iterator it = m_subscribe_cons.find(customer_id);
	if (it != m_subscribe_cons.end() && type == UNSUB)
	{ 
		//delete
		if (it->second != nullptr && it->second->m_pcon != nullptr)
		{
			switch (it->second->type)
			{
			case SUB_ALL:
				break;
			case SUB_KEYS:
				delete (GPS_KEYS_SUBSCRIBE_CON*)it->second->m_pcon;
				break;
			case SUB_CIRCLE:
				delete (GPS_CIRCLE_AREA_SUBSCRIBE_CON*)it->second->m_pcon;
				break;
			case SUB_RECT:
				break;
			case SUB_PLOYGON:
				break;
			default:
				break;
			}
		}
		it->second->m_pcon = nullptr;
		delete it->second;
		m_subscribe_cons.erase(it);	
	}

	if (m_subscribe_cons.find(customer_id) == m_subscribe_cons.end() && type != UNSUB)
	{
		bool is_right_conditon = true;
		subscribe_conditon<void*>* pcon = new subscribe_conditon<void*>();
		//parse sunscribe conditon
		switch (type)
		{
		case SUB_ALL:
			{
				pcon->m_pcon = nullptr;
				pcon->type = SUB_ALL;
			}
			break;
		case SUB_KEYS:
			{
				pcon->m_pcon = new GPS_KEYS_SUBSCRIBE_CON();
				pcon->type = SUB_KEYS;
				if (!add_subscribe_by_keys(pcon, customer_id, json_root))
				{
					is_right_conditon = false;
					delete (GPS_KEYS_SUBSCRIBE_CON*)pcon->m_pcon;
				}
			}
			break;
		case SUB_CIRCLE:
			{
				pcon->m_pcon = new GPS_CIRCLE_AREA_SUBSCRIBE_CON();
				pcon->type = SUB_CIRCLE;
				if (!add_subscribe_by_circle(pcon, customer_id, json_root))
				{
					is_right_conditon = false;
					delete (GPS_CIRCLE_AREA_SUBSCRIBE_CON*)pcon->m_pcon;
				}
			}
			break;
		case SUB_RECT:
			{
				pcon->m_pcon = new GPS_RECT_AREA_SUBSCRIBE_CON();
				pcon->type = SUB_RECT;
				if (!add_subscribe_by_rect(pcon, customer_id, json_root))
				{
					is_right_conditon = false;
					delete (GPS_RECT_AREA_SUBSCRIBE_CON*)pcon->m_pcon;
				}
			}
			break;
		case SUB_PLOYGON:
			break;
		default:
			is_right_conditon = false;
			return RESPONSE_ARRAY[11].code; //subscribe type is not supported.
		}

		if (!is_right_conditon)
		{
			if (pcon)
			{
				delete pcon;
			}
			return RESPONSE_ARRAY[15].code;
		}

		m_subscribe_cons.insert(std::pair<int, subscribe_conditon<void*>*>(customer_id, pcon));
	}


	//persistence print
	persitence_subscribe_file();

	return RESPONSE_ARRAY[0].code; //subscribe success.
}

void gps_pubsub_manager::persitence_subscribe_file()
{
	std::ofstream fs(m_file_path, std::ios::out | std::ios::trunc);
	if (!fs.is_open())
	{
		NP_LOG(np_log::NP_ERROR, "Open subscribe file write %s faild,err:%s", m_file_path.c_str(), strerror(errno));
		return;
	}
	//print
	Json::FastWriter fast_writer;
	for (auto mod : m_subscribe_cons)
	{
		Json::Value subscribes_line;
		std::string line;
		subscribes_line[CUSTOMER_ID] = mod.first;
		subscribes_line[SUBTYPE] = mod.second->type;
		bool is_need = false;
		switch (mod.second->type)
		{
		case SUB_ALL:
			{
				is_need = true;	
				NP_LOG(np_log::NP_INFO, "Subscribed user:%d.type:%d.Subscribe all.", mod.first, mod.second->type);
			}
			break;
		case SUB_KEYS:
			{
				GPS_KEYS_SUBSCRIBE_CON* pn = (GPS_KEYS_SUBSCRIBE_CON*)mod.second->m_pcon;
				std::string keys;
				for (auto pmod : pn->key_map)
				{
					Json::Value key_val;
					key_val[SUBKEY] = pmod.first;
					subscribes_line[SUBKEYS].append(key_val);
					keys.append(pmod.first);
					keys.append("|");
				}
				is_need = true;
				NP_LOG(np_log::NP_INFO, "Subscribed user:%d.type:%d.Subscribe id:%s.", mod.first, mod.second->type, keys.c_str());
			}
			break;
		case SUB_CIRCLE:
			{
				GPS_CIRCLE_AREA_SUBSCRIBE_CON* pcircle = (GPS_CIRCLE_AREA_SUBSCRIBE_CON*)mod.second->m_pcon;			
				subscribes_line[LATITUDE] = pcircle->latitude;
				subscribes_line[LONGITUDE] = pcircle->longitude;
				subscribes_line[SUBREDIUS] = pcircle->redius;			
				is_need = true;
				NP_LOG(np_log::NP_INFO, "Subscribed user:%d.type:%d.Subscribe redius:%d,latitude:%f.longitude:%f.", mod.first, mod.second->type, pcircle->redius, pcircle->latitude, pcircle->longitude);
			}
			break;
		case SUB_RECT:
			{
				GPS_RECT_AREA_SUBSCRIBE_CON* prect = (GPS_RECT_AREA_SUBSCRIBE_CON*)mod.second->m_pcon;
				subscribes_line[LATITUDEONE] = prect->latitude_one;
				subscribes_line[LATITUDETWO] = prect->latitude_two;
				subscribes_line[LONGITUDEONE] = prect->longitude_one;
				subscribes_line[LONGITUDETWO] = prect->longitude_two;
				is_need = true;
				NP_LOG(np_log::NP_INFO, "Subscribed user:%d.type:%d.Subscribe latitude1:%f.longitude1:%f.latitude2:%f.longitude2:%f.", mod.first, mod.second->type, prect->latitude_one, prect->longitude_one, prect->latitude_two, prect->longitude_two);
			}
			break;
		default:
			break;
		}

		if (is_need)
		{
			line = fast_writer.write(subscribes_line);
			fs << line;
		}
	}
	fs.close();
}

void gps_pubsub_manager::init_subscribe_file(std::string file_path)
{
	if (file_path.length() <= 0)
	{
		NP_LOG(np_log::NP_ERROR, "Init Subscribed Persisitence File error.");
		return;
	}
	m_file_name = "subscribe";
	char filepath[FILE_PATH_LENGTH];
	memset(filepath, 0, FILE_PATH_LENGTH);
	memcpy(filepath, file_path.c_str(), (size_t)file_path.length());
	strcat(filepath, m_file_name.c_str());
	this->m_file_path = filepath;
	//read subscribe msg to 
	read_subscribe_msg();
}

void gps_pubsub_manager::read_subscribe_msg()
{
	std::ifstream fs(m_file_path, std::ios::in);
	if (!fs.is_open())
	{
		NP_LOG(np_log::NP_ERROR, "Open subscribe file read %s faild,err:%s", m_file_path.c_str(), strerror(errno));
		return;
	}

	char tmp[4096];
	while (!fs.eof())
	{
		memset(tmp, 0, sizeof(tmp));
		fs.getline(tmp, 4096);

		Json::Reader json_reader;
		Json::Value  json_root;
		//read end
		if (tmp[0] == 0)
		{
			break;
		}
		if (!json_reader.parse(tmp, json_root))
		{
			NP_LOG(np_log::NP_ERROR, "Read subscribed msg from file error.");
			return;
		}

		int type = json_root[SUBTYPE].asInt();
		int id = json_root[CUSTOMER_ID].asInt();
		subscribe_conditon<void*>* pcon = new subscribe_conditon<void*>();

		bool is_need = false;
		switch (type)
		{
		case SUB_ALL:
		{
			subscribe_conditon<void*>* pcon = new subscribe_conditon<void*>();
			pcon->m_pcon = nullptr;
			pcon->type = SUB_ALL;
			is_need = true;
			m_subscribe_cons.insert(std::pair<int, subscribe_conditon<void*>*>(id, pcon));
		}
		break;
		case SUB_KEYS:
		{
			pcon->m_pcon = new GPS_KEYS_SUBSCRIBE_CON();
			pcon->type = SUB_KEYS;
			Json::Value json_keys = json_root[SUBKEYS];
			GPS_KEYS_SUBSCRIBE_CON* ptmp = (GPS_KEYS_SUBSCRIBE_CON*)pcon->m_pcon;
			for (unsigned int i = 0; i < json_keys.size(); i++)
			{
				ptmp->key_map.insert(std::pair<string, char>(json_keys[i][SUBKEY].asString(), 0x0));
			}
			is_need = true;
			m_subscribe_cons.insert(std::pair<int, subscribe_conditon<void*>*>(id, pcon));
		}
		break;
		case SUB_CIRCLE:
		{
			pcon->m_pcon = new GPS_CIRCLE_AREA_SUBSCRIBE_CON();
			pcon->type = SUB_CIRCLE;
			GPS_CIRCLE_AREA_SUBSCRIBE_CON* ptmp = (GPS_CIRCLE_AREA_SUBSCRIBE_CON*)pcon->m_pcon;
			ptmp->latitude = json_root[LATITUDE].asDouble();
			ptmp->longitude = json_root[LONGITUDE].asDouble();
			ptmp->redius = json_root[SUBREDIUS].asDouble();
			is_need = true;
		}
			break;
		case SUB_RECT:
		{
			pcon->m_pcon = new GPS_CIRCLE_AREA_SUBSCRIBE_CON();
			pcon->type = SUB_RECT;
			GPS_RECT_AREA_SUBSCRIBE_CON* prect = (GPS_RECT_AREA_SUBSCRIBE_CON*)pcon->m_pcon;
			prect->latitude_one = json_root[LATITUDEONE].asDouble();
			prect->latitude_two = json_root[LATITUDETWO].asDouble();
			prect->longitude_one = json_root[LONGITUDEONE].asDouble();
			prect->longitude_two = json_root[LONGITUDETWO].asDouble();
			is_need = true;
		}
			break;
		default:
			break;
		}

		if (is_need)
		{
			m_subscribe_cons.insert(std::pair<int, subscribe_conditon<void*>*>(id, pcon));
		}
	}
	fs.close();
}

