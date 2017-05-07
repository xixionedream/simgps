#ifndef _GPS_AREA_SUBSCRIBE_CON_H
#define _GPS_AREA_SUBSCRIBE_CON_H

#include "utils.h"

class GPS_CIRCLE_AREA_SUBSCRIBE_CON
{
public:
	GPS_CIRCLE_AREA_SUBSCRIBE_CON(){};
	~GPS_CIRCLE_AREA_SUBSCRIBE_CON(){};
private:
	bool is_equal(double &double1, double &double2)
	{
		double tmp = double1 - double2;
		return tmp <= 0.000001 && tmp >= -0.000001;
	}
public:
	bool operator==(GPS_CIRCLE_AREA_SUBSCRIBE_CON& con)
	{
		if (con.redius != this->redius) return false;
		if (!is_equal(con.latitude, this->latitude)) return false;
		if (!is_equal(con.longitude, this->longitude)) return false;
		return true;
	}
	bool is_in_circle(double des_latitude, double des_longitude)
	{
		return (int)GetDistance(des_latitude, des_longitude, latitude, longitude) < redius;
	}
public:
	int redius;
	double latitude;
	double longitude;
};

class GPS_RECT_AREA_SUBSCRIBE_CON
{
public:
	GPS_RECT_AREA_SUBSCRIBE_CON(){};
	~GPS_RECT_AREA_SUBSCRIBE_CON(){};
private:
	bool is_equal_latlong(double &double1, double &double2)
	{
		double tmp = double1 - double2;
		return tmp <= 0.0000001 && tmp >= -0.0000001;
	}
public:
	bool operator==(GPS_RECT_AREA_SUBSCRIBE_CON& con)
	{
		if (!is_equal_latlong(con.latitude_one, this->latitude_one)) return false;
		if (!is_equal_latlong(con.latitude_two, this->latitude_two)) return false;
		if (!is_equal_latlong(con.longitude_one, this->longitude_one)) return false;
		if (!is_equal_latlong(con.longitude_two, this->longitude_two)) return false;
		return true;
	}
	bool is_in_rect(double des_latitude, double des_longitude)
	{
		if (latitude_one > latitude_two)
		{
			max_lat = latitude_one;
			min_lat = latitude_two;
		}
		else
		{
			max_lat = latitude_two;
			min_lat = latitude_one;
		}

		if (longitude_one > longitude_two)
		{
			max_long = longitude_one;
			min_long = longitude_two;
		}
		else
		{
			max_long = longitude_two;
			min_long = longitude_one;
		}

		if (des_latitude >= min_lat && des_latitude <= max_lat && des_longitude >= min_long && des_longitude <= max_long)
		{
			return true;
		}
		return false;
	}
private:
	double max_lat;
	double max_long;
	double min_lat;
	double min_long;
public:
	double latitude_one;
	double longitude_one;

	double latitude_two;
	double longitude_two;
};
#endif//_GPS_AREA_SUBSCRIBE_CON_H