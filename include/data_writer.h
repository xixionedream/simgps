#ifndef _WRITEDATA_H
#define _WRITEDATA_H

#include <unordered_map>
#include "tcp.h"
#include "gpsid_writer.h"
#include "json/json.h"
#include "cpprest_condition_define.h"
#include "hiredis_proxy.h"
#include "statistic.h"
class CWriteData
{
public:
	CWriteData(int*, TIDECLIENT_S*, unordered_map<string, vector<GPSVALUE_S> >**, char*, GpsidWriter*, hiredis_proxy*,CStatistic*);
	~CWriteData();
	
	int WriteDataCore();
private:	
	int	Write();
	int WriteToMem(char*, int);	 
	//int WriteToMem(char*);	 
	int GpsParse();
	int Reply(unsigned int, char, char*);

	int* m_pLogfd;
	TIDECLIENT_S* m_pstClientInfo;
	unordered_map<string, vector<GPSVALUE_S> >** m_ppumGpsData;
	CTcp m_cSend;
	vector<char*> m_vGpsDataArry;
	char* m_psMaxTime;
	GpsidWriter* m_pGpsidWriter;
	hiredis_proxy* m_predis_proxy;
	CStatistic* m_pstatistic;
};

class rest_data_writer
{
public:
	rest_data_writer(unordered_map<string, vector<GPSVALUE_S> >**, int* m_pfd, std::shared_ptr<hiredis_proxy> predis_proxy,GpsidWriter* pGpsidWriter,CStatistic*);
	~rest_data_writer();

public:
	void write_gpsdata(std::string body, Json::Value& json_response);

private:
	unordered_map<string, vector<GPSVALUE_S> >** m_ppmem_tide;
	std::shared_ptr<hiredis_proxy> m_predis_proxy;
	int* m_pfd;
	GpsidWriter* m_pGpsidWriter;
	CStatistic* m_pstatistic;
};

class gps_info_writer
{
public:
	gps_info_writer(int* m_pfd,unordered_map<string,string>* pGps_info);
	~gps_info_writer();
public:
	void write_gpsinfo(std::string body, Json::Value& json_response);
private:
	int* m_pfd;
	unordered_map<string,string>* m_pGpsInfo;
};

#endif // _WRITEDATA_H

