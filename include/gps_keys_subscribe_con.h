#ifndef _GPS_KEYS_SUBSCRIBE_CON_H
#define _GPS_KEYS_SUBSCRIBE_CON_H

#include <unordered_map>

class GPS_KEYS_SUBSCRIBE_CON
{
public:
	GPS_KEYS_SUBSCRIBE_CON(){};
	~GPS_KEYS_SUBSCRIBE_CON(){};
public:
	bool operator==(GPS_KEYS_SUBSCRIBE_CON& con)
	{
		if (con.key_map.size() != key_map.size()) return false;
		for (auto key : con.key_map)
		{
			if(key_map.find(key.first) == key_map.end())
			{
				return false;
			}
		}
		return true;
	}
public:
	std::unordered_map<std::string, char> key_map;
};

#endif //_GPS_KEYS_SUBSCRIBE_CON_H