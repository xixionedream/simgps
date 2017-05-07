 /********************************************
  *Author: magang
  *Data: 2014.05.18
  ********************************************/

#ifndef _EPOLL_H
#define _EPOLL_H

#include "tcp.h"
#define EPOLL_NUM 10000
#define WAIT_NUM 100

typedef struct host
{
	char m_sIp[20];
	unsigned int m_uiPort;
}host_s;

class CEpoll {
public:
	CEpoll();	
	~CEpoll();	
	
	void CEpollClose();	
	int CEpollInit(const char*, const char*);	
	int CEpollTcpWait(int iTime = -1);
	void CEpollSetFunc(int (*pFunc)(int iSocket));
		
private:
	int m_iEpollfd;
	CTcp m_cTcp;
	int (*m_pFunction)(int iSocket);
};

#endif //_PUB_EPOLL

