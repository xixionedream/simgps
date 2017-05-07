#ifndef _NP_LOG_H
#define _NP_LOG_H

#include <stdarg.h>
#include <iostream>
#include <string>
#include <fstream>

#include "utils.h"

class np_log
{
public:
	np_log();
	~np_log();
public:
	//info log
	static void NPLOG_INFO(std::string log);
	// error log
	static void NPLOG_ERROR(std::string log);
	//warn log
	static void NPLOG_WARNING(std::string log);
	//faltal
	static void NPLOG_FALTAL(std::string log);
	
	static void NP_STDOUT(const char* msg, ...);

	static void NPLOG(const char* filename, const int line, const int log_type, const char* log, ...);

	static void init_nplog(const char*);
	
	static void clean_nplog( );
private:
	static void write_nplog(std::string file, std::string content);
	static std::string  get_filename(const int type);
public:
	static const int NP_INFO;
	static const int NP_ERROR;
	static const int NP_WARNING;
	static const int NP_FATAL;
	static int BUF_MAX_SIZE;
	
	static int sys_pid;
private:
    static std::string m_filepath;
	static pthread_mutex_t m_lock;
};

#define NP_LOG(log_type, log, args...)  np_log::NPLOG(__FILE__, __LINE__, log_type, log, ##args)

#endif
