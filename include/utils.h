/**************************************************************************
Copyright (c) netposa
Author: mg
Update: rjx
Last Date:2016-6-29
Description:Provide basic methods or defination for server
**************************************************************************/
#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <iconv.h>
#include <signal.h>
#include <sstream>
#include <sys/sysinfo.h>
#include <assert.h>

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include "circular_queue.h"
#include "json/json.h"
#include "np_log.h"

#if 1 // for server
#define PRO_HEAD_LEN           10
#define FILE_PATH_LENGTH       128
#define READ_FILE_SIZE         2048

//write buffer
#define BUFFER_SIZE            (1024 * 1024 * 32)
#define MEM_SIZE         	   1024
#define MEM_BUFFER         	   (1024 * 64)

#define EPOLL_NUM              10000
#define WAIT_NUMS          	   1024

#define TIDE_INS               0x31
#define TIDE_QRY_REAL_POS      0x32
#define TIDE_QRY_HIS_LOCUS     0x33
#define TIDE_QRY_REAL_AROUND   0x34
#define TIDE_QRY_HIS_AROUND    0x35
#define TIDE_QRY_GPS_ID        0x36

#define TIDE_QRY_PAGE_SIZE     100

#define TIDE_KEY 	           0x01
#define TIDE_TIME 	           0x02
#define TIDE_LAT 	           0x03
#define TIDE_LONG 	           0x04
#define TIDE_EXPAND            0x05

#define GPS_SEP  	           "#"
#define GPS_VAL_SEP  	       ','
#define GPS_AROUND_SEP         "|"
#define GPSID_SPILT_CHR        "|"
#define GPSID_MAP_VALUE        0x0

#define GPS_SUCCESS            0x00
#define GPS_FAILURE            0x01

#define BINARY_SOCKET 	       0x01

#define DOUBLE_ZERO 0.0000001  //根据需要调整这个值
#define IS_DOUBLE_ZERO(d) (fabs(d) < DOUBLE_ZERO)


#define QRY_AROUND_CIRCLE 0x01
#define QRY_AROUND_RECTANGLE 0x02
#define QRY_AROUND_RECTANGLE_TWOPOINT 0x03
#define QRY_AROUND_POLYGON 0x04


#define DOUBLE_NULL            0.000000
#define HASHCODE_LENGTH        6
#define PRECISION              1000000

#define EARTH_RADIUS           6378.137
#define PI                     3.14159
#define MOD                    100

#define MAX_QUERY_FILEHOURS    100
#define MAX_QUERY_REDIUS       3000
#define MAX_QUERY_RESULTS      2000
#define MAX_QUERY_HISAROUND_RESULTS    4000

#define MAX_ERROR_TIMS         200
#define SLEEP_ONE_MIN          60
#define SLEEP_ONE_SECOND       1
#define CLEAN_TIME_HOUR        2
#define CLEAN_TIME_MIN         0
#define SAVE_FILE_DAYS         100

#define  KEY                 "key"
#define  TIME                "time"
#define  LATITUDE            "latitude"
#define  LONGITUDE           "longitude"
#define  EXPAND              "expand"

#define MAX_LENGTH_ONE_DATA 128
#define ADMINISTRATOR    "netposa_admin"
#define _LINE_LENGTH 30
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);	\
	void operator=(const TypeName&)
#endif

#if 1 //for redis pubsub service defination

#define  SUBUSER             "id"
#define  CUSTOMERS           "customers"
#define  CUSTOMER_ID         "id"
#define  CHANNEL             "channel"
#define  SUBTYPE             "type"
#define  SUBCONDITION        "condition"
#define  SUBKEYS             "keys"
#define  SUBKEY              "key"
#define  SUBREDIUS           "redius"
#define  LATITUDEONE         "latitude1"
#define  LATITUDETWO         "latitude2"
#define  LONGITUDEONE        "longitude1"
#define  LONGITUDETWO        "longitude2"

#define UNSUB                 0
#define SUB_ALL               1
#define SUB_KEYS              2
#define SUB_CIRCLE            3
#define SUB_RECT              4
#define SUB_PLOYGON           5

typedef int SUB_TYPE;

typedef struct SUBSCRIBE_DEF_
{
	SUB_TYPE type;
	std::string description;
}SUBSCRIBE_DEF;

static const SUBSCRIBE_DEF SUB_DEFINATION[] =
{
	{ 0, "unsubscribe" },
	{ 1, "subscribe all"},
	{ 2, "subscribe gps info by key or keys" },
	{ 3, "subscribe circle" },
	{ 4, "subscribe rect" },
	{ 5, "subscribe plygon" }
};

#endif 

using namespace std;


	
// client socket package
typedef struct
{
	int m_iSocket;
	pthread_mutex_t* m_pSocketLock;
	char* m_psPacket;
}TIDECLIENT_S;

// mempool struct
typedef struct
{
	//repeat quene
	CCircularQueue<char*>* m_pcCircularQueue; 
	pthread_mutex_t* m_pMemPoolLock;
	pthread_cond_t* m_pMemPoolCond;
}MEMPOOL_S;

// gps data defination (not include gpsid)
typedef struct stGpsValue
{
	char m_sTime[15];
	char m_sLat[15];
	char m_sLong[15];
	char m_sRemain[256];
	stGpsValue() 
	{
		memset(m_sTime, 0, 15);
		memset(m_sLat, 0, 15);
		memset(m_sLong, 0, 15);
		memset(m_sRemain, 0, 256);
	}
}GPSVALUE_S,*PGPSVALUE_S;

typedef struct stGpsInfo
{
	int initFlag;
	std::vector<std::string> columnName;
	stGpsInfo() 
	{
		initFlag = 0;
	}
}GPSINFO_S,*PGPSINFO_S;

// gps data defination (include gpsid)
typedef struct stGpsData
{
	string m_sKey;
	char m_sTime[15];
	std::string m_sLat;
	std::string m_sLong;
	std::string m_sRemain;
	stGpsData() 
	{
		memset(m_sTime, 0, 15);
	}
}GPSDATA_S;

// openfile struct
typedef struct stFileInfo
{
	int m_fd;
	string m_sFileName;
	stFileInfo() 
	{
		m_fd = -1;
	}
}FILEINFO_S;

//locus gps data index file struct
typedef struct stLocusIndex
{
	unsigned long m_ulKey;
	unsigned int m_uiOffset;
	unsigned int m_uiLength;
}LOCUSINDEX_S;

/*************************************************
Function: GetLocalTimeHour
Description: get local time(accurate to hour)
Input: void
Return: a int number which accurate to hour by the lcoal time. expample: 2016062815
*************************************************/
int GetLocalTimeHour();

/*************************************************
Function: Trim
Description: put off blank space in str.
Input: char*, your input str.
Return: the result whcih have strike out space.
*************************************************/
char* Trim(char*);

/*************************************************
Function: spliterByStr
Description: According to the sub string segmentation
Input: char* psStr,spilt string;  int iLen,spilt string length; \
const char* psComm,sub string; vRet,your spit result.
Return: a int number. 0 : success. -1:failed.
*************************************************/
int spliterByStr(char* psStr, int iLen, const char* psComma, vector<char*>* vRet);

/*************************************************
Function: freeSpliter
Description: Release resources of vectors.
Input: vRet: the resources of which we need to free.
Return: void
*************************************************/
void freeSpliter(vector<char*>* vRet);

/*************************************************
Function: codeConvert
Description: Character transcoding.
Input: psFromCharset: source charset; psToCharset: destination charset. psInStr:charset string input.\
 psRst :charset string result.
Return: a int number. 0 : success. -1:failed.
*************************************************/
int codeConvert(const char* psFromCharset, const char* psToCharset, char* psInStr, char* psRst);

/*************************************************
Function: Fnv64a
Description: fnv hash code algorithm. it would to code your input string by fnv hash.
Input: buf: your string which you want to code; len:your string len.
Return: your result after to use fnv code.
*************************************************/
uint64_t Fnv64a(const void *buf, size_t len);

/*************************************************
Function: GeohashEncode4
Description: Hashcode coding based on 16 bits.
Input: dLatitude: your latitude; dLongitude:your longitude; psGeoKey: the result of coding; \
 iPrecision:encoded bit length;
Return: your result after to use fnv code.
*************************************************/
void GeohashEncode4(double dLatitude, double dLongitude, char* psGeoKey, int iPrecision = 7);

/*************************************************
Function: GeohashEncode5
Description: Hashcode coding based on 32 bits.
Input: dLatitude: your latitude; dLongitude:your longitude; psGeoKey: the result of coding; \
 iPrecision:encoded bit length;
Return: your result after to use fnv code.
*************************************************/
void GeohashEncode5(double dLatitude, double dLongitude, char* psGeoKey, int iPrecision = 6);

/*************************************************
Function: Atoi
Description: string convert to int.
Input: psStr: your source string; string:convert begin seat; usLen: convert char length
Return: convert result.
*************************************************/
int Atoi(const char* psStr, unsigned short usBegin, unsigned short usLen);

/*************************************************
Function: Atol
Description: string convert to long.
Input: psStr: your source string; string:convert begin seat; usLen: convert char length
Return: convert result.
*************************************************/
long Atol(const char* psStr, unsigned short usBegin, unsigned short usLen);

/*************************************************
Function: GetRectangleEdge
Description: Calculate the rectangular region according to a point on the map
Input: dLong:the longitude on point ;
		dLat: the latitude on point ; \
  ewDistance: the distance from this point to the east and west ; \
  snDistance: the distance from this point to the south and north ;\
Output:
    eastLong: the longitude from this point to the east of the ewDistance meter ;\
  	westLong: the longitude from this point to the west of the ewDistance meter ;\
  	southLat: the longitude from this point to the south of the snDistance meter ;\
  	northLat: the longitude from this point to the north of the snDistance meter ;\
Return: NULL
*************************************************/
void GetRectangleEdge(double dLong, double dLat,double ewDistance,double snDistance, double& eastLong, double& westLong, double& southLat, double& northLat);

/*************************************************
Function: GetDistance
Description: Calculate the distance between two points on the map
Input: dLat1: the latitude on point one; dLong1:the longitude on point two;\
 dLat2: the latitude on point two; dLong2 the longitude on point2.
Return: the calculate result
*************************************************/
double GetDistance(double dLat1, double dLong1, double dLat2, double dLong2);

/*************************************************
Function: ltos
Description: convert long to string
Input: ltos: long number; psRst:the convert result
Return: a int number. 0 : success. -1:failed.
*************************************************/
int ltos(long lNumber, char* psRst);

/*************************************************
Function: GetNTUByDistance
Description: get the distance of NTU
Input: uiMile:the point to point. mils; dLat:laititude; dLong:longitude;\
 uiLat:out laitude;  uiLong:out longitude; 
Return: a int number. 0 : success. -1:failed.
*************************************************/
void GetNTUByDistance(unsigned int uiMile, double dLat, double dLong, unsigned int& uiLat, unsigned int& uiLong);

/*************************************************
Function: GetDateByHour
Description: Calculated by the hour difference
Input: iYearMonthDay:your cur time(2016050214); iNum:+ or - number;
Return: the result of calculate date.
*************************************************/
int GetDateByHour(int iYearMonthDay, int iNum = 1);

/*************************************************
Function: MakeDir
Description: create contens.
Input:psDirPath:your contens path; usDirPathLength: length of content path.
Return: a int number. 0 : success. -1:failed.
*************************************************/
int MakeDir(char* psDirPath, unsigned short usDirPathLength);

/*************************************************
Function: ReduceHours
Description: Calculated by the hour difference
Input:time:your cur time(2016050214); iNum:- or + number;
Return: time. the result of calculator.
*************************************************/
void ReduceHours(int &time, int hours);

/*************************************************
Function: add_response
Description: combination query result of gpsdata to a string
Input:ptide_value:your input gpsdata stuct; response:the result of combinate;
Return: response. the result of combinate;
*************************************************/
void add_response(GPSVALUE_S* ptide_value, std::string& response);


/*************************************************
Function: add_json_response
Description: combination query result of gpsdata to a json string
Input:ptide_value:your input gpsdata stuct;key:combination key json_response:the result of combinate;
Return: json_response. the result of combinate;
*************************************************/
void add_json_response(GPSVALUE_S* ptide_value, std::string key, Json::Value& json_response);

/*************************************************
Function: add_json_response2
Description: combination query result of gpsdata to a json class.
Input:ptide_value:your input gpsdata stuct; json_response:the result of combinate;
Return: json_response. the result of combinate;
*************************************************/
void add_json_response2(GPSDATA_S* ptide_value, Json::Value& json_response);

/*************************************************
Function: get_local_time_days
Description: get local time.Accurate to daya.
Input:void
Return: the local time accurate to day.example:20160628
*************************************************/
int get_local_time_days();

/*************************************************
Function: reduce_days
Description: calculate time by days.
Input:day_time:your calculate days,it must contain days; days:- days.
Return: day_time:the result of calculator.
*************************************************/
void reduce_days(int& day_time, int days);
void reduce_days2(int& day_time,int days);

/*************************************************
Function: get_local_h_m
Description: get the currently hour and minutes.
Input:hour:the result of hour; min:the result of min.
Return: hour:the result of hour; min:the result of min.
*************************************************/
void get_local_h_m(int& hour, int &min);
int GetTimeZone();
time_t GetTick(int iY,int iM,int iD,int iH,int iMin,int iS);

/*************************************************
Function: IsInCircle
Description: Calculates whether the point is in a given circular range
Input: longitude: the longitude on point ;
		latitude: the latitude on point ; 
  	   query_con: Central point of judgment;query_con[0] is longitude, query_con[1] is latitude;
          redius: judge the distance ;
Return: true or false
*************************************************/
bool IsInCircle(double longitude,double latitude,double* query_con);

/*************************************************
Function: IsInRect
Description: Calculates whether the point is in a given rectangle range
Input: longitude: the longitude on point ;
		latitude: the latitude on point ; 
  	   query_con: rectangle points of judgment;query_con[0] is eastLong,
  	   			  query_con[1] is westLong, query_con[2] is southLat, query_con[3] is northLat;
          redius: judge the distance ;
Return: true or false
*************************************************/
bool IsInRect(double longitude,double latitude,double* query_con);

/*************************************************
Function: IsInPolygon
Description: Calculates whether the point is in a given polygon range(根据射线法)
Input: longitude: the longitude on point ;
		latitude: the latitude on point ; 
  	   query_con: Polygon points of judgment;
Return: true or false
*************************************************/
typedef struct QUERY_CON_POLYGONAROUND_
{
	double latitude;
	double longitude;
}QUERY_CON_POLYGONAROUND, *PQUERY_CON_POLYGONAROUND;
bool IsInPolygon(double longitude,double latitude, std::vector<QUERY_CON_POLYGONAROUND>& query_con);

/*************************************************
Function: get_exe_path
Description: Get the execution file path
Input: buf: result storage space ;
	 count: buf size; 
Return: address of buf or null
*************************************************/
char* get_exe_path( char * buf, int count);



/*************************************************
Function: value2string
Description: convert value to string.
Input:the value that you wanted to convert.
Return:the value string.
*************************************************/
template<class T>
string value2string(T value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

#ifdef __cplusplus
	extern "C" 
	{
	#endif /* __cplusplus */
	#ifdef __cplusplus
	}
#endif /* __cplusplus */

#endif /* _PUBFUNCTION_H_ */

