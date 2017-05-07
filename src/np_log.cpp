#include "np_log.h"


const int np_log::NP_INFO = 0;
const int np_log::NP_ERROR = 2;
const int np_log::NP_WARNING = 1;
const int np_log::NP_FATAL = 3;

int np_log::BUF_MAX_SIZE = 512;
std::string np_log::m_filepath;
int np_log::sys_pid;
pthread_mutex_t np_log::m_lock;

np_log::np_log()
{
}

np_log::~np_log()
{

}

void np_log::write_nplog(std::string file, std::string content)
{
	content.append("\n");
	pthread_mutex_lock(&m_lock);
	std::ofstream fs(file,std::ios::app);
    fs<<content;
    fs.close();
	pthread_mutex_unlock(&m_lock);	
	std::cout<<content;
}

void np_log::NPLOG_INFO(std::string log)
{
	std::string filename = get_filename(NP_INFO);
	write_nplog(filename,log);
}

void np_log::NPLOG_ERROR(std::string log)
{
	std::string filename = get_filename(NP_ERROR);
	write_nplog(filename,log);
}

void np_log::NPLOG_WARNING(std::string log)
{
	std::string filename = get_filename(NP_WARNING);
	write_nplog(filename,log);
}

void np_log::NPLOG_FALTAL(std::string log)
{
	std::string filename = get_filename(NP_FATAL);
	write_nplog(filename,log);
	//
	kill(getpid(), SIGKILL);
}

void np_log::NP_STDOUT(const char* msg, ...)
{
	va_list va_log_list;
	va_start(va_log_list, msg);
	char buf[BUF_MAX_SIZE];
	memset(buf, 0, BUF_MAX_SIZE);
	(void)vsprintf(buf, msg, va_log_list);
	va_end(va_log_list);
	
	time_t st_time = time(NULL);							\
	struct tm* pst_tm = localtime(&st_time);				\
	fprintf(stdout, "[%d] %4d-%02d-%02d %02d:%02d:%02d ",getpid(), 1900 + pst_tm->tm_year, \
	1 + pst_tm->tm_mon, pst_tm->tm_mday,  pst_tm->tm_hour, pst_tm->tm_min, pst_tm->tm_sec);  \
	fprintf(stdout, buf);
	fprintf(stdout, "\r\n");
}

void np_log::NPLOG(const char* filename, const int line, const int log_type, const char* log, ...)
{
	va_list va_log_list;
	va_start(va_log_list, log);
	char buf[BUF_MAX_SIZE];
	memset(buf, 0, BUF_MAX_SIZE);
	(void)vsprintf(buf, log, va_log_list);
	va_end(va_log_list);
	char content[512];
	time_t ltime = time(NULL);
	struct tm* p_time = localtime(&ltime);
	
	switch (log_type)
	{
	case NP_INFO:
	    sprintf(content, "[%d][INFO %02d:%02d:%02d.%ld %s:%d]:%s",sys_pid, p_time->tm_hour, p_time->tm_min, p_time->tm_sec, (long)ltime, (char*)filename, (int)line, buf);	
		NPLOG_INFO(content);
		break;
	case NP_WARNING:
		sprintf(content, "[%d][WARN %02d:%02d:%02d.%ld %s:%d]:%s",sys_pid, p_time->tm_hour, p_time->tm_min, p_time->tm_sec, (long)ltime, (char*)filename, (int)line, buf);				
		NPLOG_WARNING(content);
		break;
	case NP_ERROR:
		sprintf(content, "[%d][ERROR %02d:%02d:%02d.%ld %s:%d]:%s",sys_pid, p_time->tm_hour, p_time->tm_min, p_time->tm_sec, (long)ltime, (char*)filename, (int)line, buf);				
		NPLOG_ERROR(content);
		break;
	case NP_FATAL:
		sprintf(content, "[%d][FALTA %02d:%02d:%02d.%ld %s:%d]:%s",sys_pid, p_time->tm_hour, p_time->tm_min, p_time->tm_sec, (long)ltime, (char*)filename, (int)line, buf);				
		NPLOG_FALTAL(content);
		break;
	default:
		std::cout << "write log to file error...." << std::endl;
		break;
	}
}


std::string  np_log::get_filename(const int type)
{
	time_t ltime = time(NULL);
	struct tm* p_time = localtime(&ltime);
	char filename[64];
	memset(filename, 0, sizeof(filename));
	switch (type)
	{
	case NP_INFO:
		sprintf(filename, "%s/INFO_%04d%02d%02d_%d", m_filepath.c_str(), p_time->tm_year + 1900, p_time->tm_mon + 1, p_time->tm_mday, sys_pid);
		break;
	case NP_WARNING:
		sprintf(filename, "%s/WARN_%04d%02d%02d_%d", m_filepath.c_str(), p_time->tm_year + 1900, p_time->tm_mon + 1, p_time->tm_mday, sys_pid);
		break;
	case NP_ERROR:
		sprintf(filename, "%s/ERROR_%04d%02d%02d_%d", m_filepath.c_str(), p_time->tm_year + 1900, p_time->tm_mon + 1, p_time->tm_mday, sys_pid);
		break;
	case NP_FATAL:
		sprintf(filename, "%s/FATAL_%04d%02d%02d_%d", m_filepath.c_str(), p_time->tm_year + 1900, p_time->tm_mon + 1, p_time->tm_mday, sys_pid);
		break;
	default:
		sprintf(filename, "%s/TMP_%04d%02d%02d_%d", m_filepath.c_str(), p_time->tm_year + 1900, p_time->tm_mon + 1, p_time->tm_mday, sys_pid);
		break;
	}
	return filename;
}

void np_log::init_nplog(const char* plog_path)
{
   sys_pid = getpid();
   
   m_filepath = plog_path;
   //create file
   MakeDir((char*)m_filepath.c_str(), (short unsigned)m_filepath.length());
   //
   pthread_mutex_init(&m_lock, NULL);
}

void np_log::clean_nplog( )
{
	pthread_mutex_destroy(&m_lock);
}