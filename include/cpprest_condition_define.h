#ifndef _CPPREST_CONDITION_DEFINE_H
#define _CPPREST_CONDITION_DEFINE_H

#include <string>
#include <list>


#define QRY_GPSID_REST "/qrygpsid"
#define QRY_REAL_POS_REST "/qryrealpos"
#define QRY_LAST_POS_REST "/qrylastpos"
#define QRY_REAL_AROUND_REST "/qryrealaround"
#define QRY_REAL_RECTAROUND_REST "/qryrealrectaround"
#define QRY_REAL_POLYGONAROUND_REST "/qryrealpolygonaround"
#define QRY_HIS_LOCUS_REST "/qryhislocus"
#define QRY_HIS_AROUND_REST "/qryhisaround"
#define QRY_HIS_RECTAROUND_REST "/qryhisrectaround"
#define QRY_HIS_POLYGONAROUND_REST "/qryhispolygonaround"
#define QRY_ONLINE_REST "/qryonline"
#define WRITE_GPS_REST "/writegps"
#define WRITE_GPSINFO_REST "/writegpsinfo"
#define PING_REST "/ping"
#define SERVER_VERSION "/version"
#define SERVER_INFO "/serverinfo"
#define QRY_STATISTIC_REALEACH_REST "/qrystatisticrealeach"
#define QRY_STATISTIC_REALTOTAL_REST "/qrystatisticrealtotal"
#define QRY_STATISTIC_DAYEACH_REST "/qrystatisdayeach"
#define QRY_STATISTIC_DAYTOTAL_REST "/qrystatisdaytotal"
#define GPS_DATA_SUBSCRIBE "/gpssubscribe"
#define QRY_DISTANCE_POINT_REST "/qrydistpoint"
#define QRY_DISTANCE_DEVICE_REST "/qrydistdevice"
#define QRY_GPSINFO_REST "/qrygpsinfo"



#define QUERY_ALL "all"                     //query all
#define REST_KEY "key"                      //rest return code [key]
#define REST_TIME "time"                    //rest return code [time]
#define REST_LONGTITUDE "longitude"         //rest return code [longitude]
#define REST_EXPAND "expand"                //rest return code [expand]
#define REST_LATITUDE "latitude"            //rest return code [latitude]
#define REST_SUCCESS "success"              //rest return code [msg->OK]
#define REST_RESULT "result"                //rest return code [result]
#define REST_CODE "code"                    //rest return code [code]
#define REST_MSG "msg"                      //rest return code [msg]
#define REST_SEP "|"                        //rest return code [result->sep]
#define REST_DATA_SEP ";"                   //rest return code [result->data->sep]
#define REST_EXCEPTION "query exception"    //rest return code [msg->ERROR]
#define REST_STATE     "state"              //rest state
#define REST_NUM "num"                      //rest return code [num]
#define GPS_ID "gps_id"
#define GPS_INFO_VALUE "value"


#define MAX_QUERY_SIZE 100000

#define DEVICE_OFFLINE 0
#define DEVICE_ONLINE 1
#define DEVICE_ALLSTATUS 2

#define REST_ONLINE "online"
#define REST_OFFLINE "offline"


typedef enum {
  REST_OK = 0,           /* successful */
  REST_PARAMREEOR,       /* parameter error */
  REST_NOTINSCOPE,       /* not in scope*/
  REST_UNEXPECTED,		 /* an unexpected exception*/
  REST_LAST              /* never use */
} E_RESTERROR;

typedef struct INTERFACE_CALL_RESPONSE_
{
	int code;                // rest response code
	std::string description; // rest response code description
}INTERFACE_CALL_RESPONSE;

static const INTERFACE_CALL_RESPONSE  RESPONSE_ARRAY[] =
{
	//Rest interface error defination
	{  0, "SUCCESS" },
	{ -1, "Write data is not json." },
	{ -2, "System exception." },
	{ -3, "Query server info type not exist." },
	{ -4, "Please input right query condition." },
	{ -5, "Query server memeroy info exception." },
	{ -6, "Query server cpu info exception." },
	{ -7, "Query current procee cpu and memeroy info exception." },
	{ -8, "Query current procee detailed message exception." },
	{ -9, "The url is not effective." },
	{ -10, "Subscription conditions do not conform to specifications."},
	{ -11, "Subscription type does not exist."},
	{ -12, "Service don't start subscribe service." },
	{ -13, "The user don't subscribe any data."},
	{ -14, "The user has subscribed. it needs to unsubscribe firstly." },
	{ -15, "The subscribed condition is not right." }
};

typedef struct QUERY_GPSID_CON_
{
	std::string key;
}QUERY_GPSID_CON, *PQUERY_GPSID_CON;

typedef struct QUERY_CON_REAL_POS_
{
	std::string key;
}QUERY_CON_REAL_POS, *PQUERY_CON_REAL_POS;

typedef struct QUERY_REAL_POS_CON_
{
	int is_all;
	std::list<QUERY_CON_REAL_POS>  key_lists;

	int flag;
	std::string message;
}QUERY_REAL_POS_CON, *PQUERY_REAL_POS_CON;

typedef struct QUERY_LAST_POS_CON_
{
	std::list<QUERY_CON_REAL_POS>  key_lists;

	int flag;
	std::string message;
}QUERY_LAST_POS_CON, *PQUERY_LAST_POS_CON;



typedef struct QUERY_REAL_POLYGONAROUND_CON_
{
	std::vector<QUERY_CON_POLYGONAROUND>  point_array;
	std::string expand;
	
	int flag;
	std::string message;
}QUERY_REAL_POLYGONAROUND_CON, *PQUERY_REAL_POLYGONAROUND_CON;



typedef struct QUERY_REAL_AROUND_CON_
{
	double latitude;
	double longitude;
	int redius;
	std::string expand;
	std::string key;
	
	int flag;
	std::string message;
}QUERY_REAL_AROUND_CON, *PQUERY_REAL_AROUND_CON;

typedef struct QUERY_REAL_RECTAROUND_CON_
{
	double latitude;
	double longitude;
	int ew_len;
	int sn_len;
	std::string expand;
	std::string key;

	int flag;
	std::string message;
}QUERY_REAL_RECTAROUND_CON, *PQUERY_REAL_RECTAROUND_CON;

typedef struct QUERY_REAL_RECTAROUND_TWOPOINT_CON_
{
	double firstlatitude;
	double firstlongitude;
	double secondlatitude;
	double secondlongitude;
	std::string expand;

	int flag;
	std::string message;
}QUERY_REAL_RECTAROUND_TWOPOINT_CON, *PQUERY_REAL_RECTAROUND_TWOPOINT_CON;


typedef struct QUERY_HIS_LOCUS_CON_
{
	std::string begin_time;
	std::string end_time;
	std::string key;
	std::string expand;
	int cur_page;
	int page_size;

	int flag;
	std::string message;
}QUERY_HIS_LOCUS_CON, *PQUERY_HIS_LOCUS_CON;

typedef struct QUERY_HIS_AROUND_CON_
{
	std::string begin_time;
	std::string end_time;
	double latitude;
	double longitude;
	int redius;
	std::string expand;
	int cur_page;
	int page_size;

	int flag;
	std::string message;
}QUERY_HIS_AROUND_CON, *PQUERY_HIS_AROUND_CON;

typedef struct QUERY_HIS_RECTAROUND_CON_
{
	std::string begin_time;
	std::string end_time;
	double latitude;
	double longitude;
	int ew_len;
	int sn_len;
	int cur_page;
	int page_size;
	
	int flag;
	std::string message;
}QUERY_HIS_RECTAROUND_CON, *PQUERY_HIS_RECTAROUND_CON;

typedef struct QUERY_HIS_RECTAROUND_TWOPOINT_CON_
{
	std::string begin_time;
	std::string end_time;
	double firstlatitude;
	double firstlongitude;
	double secondlatitude;
	double secondlongitude;
	int cur_page;
	int page_size;
	
	int flag;
	std::string message;
}QUERY_HIS_RECTAROUND_TWOPOINT_CON, *PQUERY_HIS_RECTAROUND_TWOPOINT_CON;


typedef struct QUERY_HIS_POLYGONAROUND_CON_
{
	std::string begin_time;
	std::string end_time;
	std::vector<QUERY_CON_POLYGONAROUND>  point_array;
	int cur_page;
	int page_size;
	
	int flag;
	std::string message;
}QUERY_HIS_POLYGONAROUND_CON, *PQUERY_HIS_POLYGONAROUND_CON;

typedef struct QUERY_ONLINE_CON_
{
	int status;
	int judgetime;
	
	int flag;
	std::string message;
}QUERY_ONLINE_CON, *PQUERY_ONLINE_CON;

typedef struct QUERY_STATISTIC_REALEACH_CON_
{
	int type;
	std::list<std::string>  key_lists;

	int flag;
	std::string message;
}QUERY_STATISTIC_REALEACH_CON, *PQUERY_STATISTIC_REALEACH_CON;

typedef struct QUERY_STATISTIC_REALTOTAL_CON_
{
	int type;

	int flag;
	std::string message;
}QUERY_STATISTIC_REALTOTAL_CON, *PQUERY_STATISTIC_REALTOTAL_CON;

typedef struct QUERY_STATISTIC_DAYEACH_CON_
{
	std::string begin_time;
	std::string end_time;
	std::list<std::string>  key_lists;

	int flag;
	std::string message;
}QUERY_STATISTIC_DAYEACH_CON, *PQUERY_STATISTIC_DAYEACH_CON;

typedef struct QUERY_STATISTIC_DAYTOTAL_CON_
{
	std::string begin_time;
	std::string end_time;

	int flag;
	std::string message;
}QUERY_STATISTIC_DAYTOTAL_CON, *PQUERY_STATISTIC_DAYTOTAL_CON;

typedef struct QUERY_DISTINCE_DEVICE_CON_
{
	std::string key1;
	std::string key2;

	int flag;
	std::string message;
}QUERY_DISTINCE_DEVICE_CON, *PQUERY_DISTINCE_DEVICE_CON;

typedef struct QUERY_GPSINFO_CON_
{
	std::vector<std::string>  gps_id;
	std::map<std::string,std::vector<std::string>>  query_conditions;
	std::vector<std::string>  output_column;
	
	int flag;
	std::string message;
}QUERY_GPSINFO_CON, *PQUERY_GPSINFO_CON;

#endif //_CPPREST_CONDITION_DEFINE_H
