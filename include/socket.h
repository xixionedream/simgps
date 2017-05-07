#ifndef _SOCK_H
#define _SOCK_H

#include <netinet/tcp.h>
#include "utils.h"
#include "np_log.h"

class CSock 
{
public:
	CSock();
	virtual ~CSock();
				
	int CreatSocket(int iType);
	int SetBlocking(bool blocking);	
	int SetReuseAddr();
	int SetTimeout(unsigned long ulSec, unsigned long ulUsec);
	int SetLinger(int iOnOff, int iSecond);
	int SetNodelay();
	int GetSocket() const;
	int ShutdownSocket(int iType);
	void SetSocket(int iNewSocket);	
	int CloseSocket();
	int SetBuff(unsigned int uiSize);
	int KeepAlive();
			
protected:
	int m_iSockfd;
private:
	
};
	
int SetNonblocking(int socketfd, bool Nonblocking = true);
int GetServerName(int fd, char** psAddr, unsigned short& usPort);
int GetClientName(int fd, char** psAddr, unsigned short& usPort);
int Send(int fd, unsigned char* psSendBuf, unsigned int uiSendSize);
int Recv(int fd, unsigned char* psRecvBuf, unsigned int uiRecvSize);
#endif //endif _PUB_SOCK

