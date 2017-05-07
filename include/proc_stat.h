
#ifndef _PROC_STAT_H
#define _PROC_STAT_H

#include "utils.h"
#include <sys/statfs.h> 

struct sys_cpu_time
{
    unsigned long long user,old_user;
    unsigned long long nice,old_nice;
    unsigned long long sys,old_sys;
    unsigned long long idle,old_idle;
    unsigned long long wait,old_wait;
    unsigned long long hirq,old_hirq;
    unsigned long long sirq,old_sirq;
};
 
struct sys_uptime
{
    double uptime;
    double idle;
};
 
struct sys_mem_info
{
    unsigned long main_total;
    unsigned long main_free;
    unsigned long main_used; 
    unsigned long main_buffers;
    unsigned long main_cached;
    unsigned long swap_total;
    unsigned long swap_free;
    unsigned long swap_used;
    unsigned long swap_cached;
};
 
struct system_stat
{
    sys_cpu_time ct;
    sys_mem_info mi;
    sys_uptime   ut;
};
 
//include some special process's cpu and memory usage information
struct process_stat
{
    char name[16];
    char state;
    int ppid;
    int pgrp;
    int session;
    int tty_nr;
    int tpgid;
    unsigned int flags;
    unsigned long minflt;
    unsigned long cminflt;
    unsigned long majflt;
    unsigned long cmajflt;
    unsigned long utime;
    unsigned long stime;
    long cutime;
    long cstime;
    long priority;
    long nice;
    long threads;
    long iterealvalue;
    unsigned long long starttime;
    unsigned long vsize;
    long rss;
};
 
struct sys_mem_entry
{
    const char *name;
    unsigned long *val;
} ;
 
static const int PROC_STAT   = 0x0001;
static const int PROC_MEM    = 0x0002;
static const int PROC_UPTIME = 0x0004;
static const int PROC_ALL    = PROC_STAT|PROC_MEM|PROC_UPTIME;
 
/*************************************************
Function: get_system_stat
Description: get the system message
*************************************************/
bool get_system_stat(system_stat& ss,int flag);
 
/*************************************************
Function: get_system_usage
Description: get the cpu and mem usage of system
*************************************************/
bool get_system_usage(float& cpu,float& mem);
 
/*************************************************
Function: get_process_stat
Description: get the process running stat
*************************************************/
bool get_process_stat(pid_t id,process_stat& ps);

/*************************************************
Function: get_process_usage
Description: get the process usage situation.it include(cpu. mem. uptime of the process)
*************************************************/
bool get_process_usage(pid_t id,float& cpu,float& mem,unsigned long long& uptime); 


/*************************************************
Function: get_cpu_mem
Description: get the memroy and cpu usage rate by pid and tid .
Input:hour:cpu:save cpu usage;mem:save mem usage;pid:process number;tide:process
Return:cpu and mem usage.
*************************************************/
bool get_cpu_mem(float &cpu,size_t &mem, int pid, int tid = -1);

/*************************************************
Function: get_serverinfo
Description: get the memory and process info of the server.
Input:memeroy can used size; memeroy used range; process numm.
Return:memeroy can used size; memeroy used range; process numm.
*************************************************/
bool get_mem_process_info(std::string &mem_remain_size, std::string &mem_use_range, std::string &procee_num);

/**************************************
Function:get disk  usage //M
**************************************/
bool get_disk_usage(size_t &total_size, size_t &available_size,size_t &free_size);
;
#endif