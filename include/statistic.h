/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: wangzhen
Last Modify Date:2016-08-24
Description:Statistics the number of GPS data for the device 
**************************************************************************/

#ifndef _STATISTIC_H
#define _STATISTIC_H
#include <list>
#include <unordered_map>
#include <string>
#include "json/json.h"
#include "utils.h"

/*Struct for storing statistical data*/
typedef struct STATISTIC_DATA_
{
	std::string  time;		/*Statistical time*/
	unsigned long long count;	/*Statistical data*/
}STATISTIC_DATA, *PSTATISTIC_DATA_;

class CStatistic
{
public:
	CStatistic(){};
	virtual ~CStatistic();

	/*************************************************
		Function: init_server
		Description: Initialization statistic module
		Input: 	intervalTime: Statistical interval time
			   		cycleNum: Statistical number of cycles 
			   statisticData: Statistical data on the last persistence
 				     bakData: No statistical data on the last persistence	
		Return: tuue:Success
				false:Fail
	*************************************************/	
	bool init_server(unsigned int intervalTime,unsigned int cycleNum,std::unordered_map<std::string, unsigned long long>* statisticData,std::unordered_map<std::string, unsigned int>* bakData);	

	/*************************************************
		Function: start_server
		Description: Start statistics
		Input: NULL	
		Return: true:Success
				false:Fail
	*************************************************/	
	bool start_server();

	/*************************************************
		Function: write_data
		Description: Access data for statistic
		Input: key:	Key for statistic
		Return: void
	*************************************************/
	void write_data(std::string key);
	
private:

	/*************************************************
		Function: do_statistic
		Description: Do statistical work
		Input: time:The time of this statistic
		Return: void
	*************************************************/
	void do_statistic(time_t time);	
	void do_statistic_day(time_t time);	
	/*************************************************
		Function: timer
		Description: judge the time whether or not to statistic
		Input: time:The time of judging
		Return: true: It is time for statistic
				false: It is not time for statistic
	*************************************************/
	bool timer(time_t time);
	bool timer_day(time_t time);
	
public:
	std::unordered_map<std::string, std::list<STATISTIC_DATA>> m_TimerEachData;	/*Statistical data of the device*/
	std::list<STATISTIC_DATA> m_TimerTotalData;	/*Statistical total data of all devices*/
	std::unordered_map<std::string, unsigned long long> m_EachTotalData; /*Access data for statistic*/

	unsigned int m_intervalTime;	/*Statistical interval time*/
	unsigned int m_cycleNum;	/*Statistical number of cycles*/
	time_t m_nextTime;		/*Next time of statistics*/
	int m_timezone;
};

#endif// _STATISTIC_H

