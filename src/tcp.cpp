#include "tcp.h"

CTcp::CTcp()
{
}

CTcp::~CTcp()
{
}

int CTcp::TcpInit(const char* psAddr, unsigned short usPort)
{
	struct sockaddr_in stServer;
	bzero(&stServer, sizeof(struct sockaddr_in));
	if (psAddr == NULL || strlen(psAddr) <= 0) 
	{
		stServer.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		inet_aton(psAddr, &stServer.sin_addr);
		//stServer.sin_addr.s_addr = inet_addr(psAddr);
	}
	stServer.sin_port = htons(usPort);
	stServer.sin_family = AF_INET;

	if (CreatSocket(SOCK_STREAM) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"creat sock err[%s].", strerror(errno));
		return -1;
	}
	
 	if (SetReuseAddr() < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"set reuse err[%s].", strerror(errno));
		CloseSocket();
		return -1;
	}
	
	if (bind(m_iSockfd, (struct sockaddr*)&stServer, sizeof(struct sockaddr)) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"bind err[%s]", strerror(errno));
		CloseSocket();
		return -1;
   	}
	
	if (listen(m_iSockfd, BLOCKLOG_NUM) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"lisen err[%s].", strerror(errno));
		CloseSocket();
		return -1;
   	}	
	//SetBuff(0);	
	SetNodelay();
		
	return 0;
}

int CTcp::TcpAccept()
{
	struct sockaddr_in stClientAddr;
	socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
	int connSocketfd = 0;
	connSocketfd = accept(m_iSockfd, (struct sockaddr*)&stClientAddr, &len);
			
	return connSocketfd;
}

int CTcp::TcpConnect(const char* psAddr, const char* psPort)
{
	struct sockaddr_in stServer;
	bzero(&stServer, sizeof(struct sockaddr_in));
	stServer.sin_addr.s_addr = inet_addr(psAddr);
	stServer.sin_port = htons(atoi(psPort));
	stServer.sin_family = AF_INET;
	
	if (CreatSocket(SOCK_STREAM) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"creat sock err[%s].", strerror(errno));
		return -1;
	}
	
	if (SetTimeout(3, 0) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"set timeout err[%s].", strerror(errno));
		CloseSocket();
		return -1;
	}
	
	if (connect(m_iSockfd, (struct sockaddr*)&stServer, sizeof(struct sockaddr_in)) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"connect[%s:%s] err[%s].", psAddr, psPort, strerror(errno));
		CloseSocket();
		return -1;
	}

	return 0;
}

int CTcp::TcpRecv(char* psRecvBuf, unsigned int uiRecvSize)
{
	unsigned int iTotal = 0;
	while (iTotal < uiRecvSize) 
	{
		int iSize = read(m_iSockfd, psRecvBuf + iTotal, uiRecvSize - iTotal);	
		if (iSize < 0) 
		{
			if ((errno == EWOULDBLOCK || errno == EAGAIN)) 
			{
				if (iTotal > 0) 
				{
					break;	
				} 
				else 
				{
					usleep(1000);
					continue;
				} 
			}
			NP_LOG(np_log::NP_ERROR,"recv fd[%d] err[%s].", m_iSockfd, strerror(errno));
			return -1;
		} 
		else if (iSize == 0) 
		{
			return 0;
		}
		iTotal += iSize;
	}
	
	return iTotal;
}

int CTcp::TcpSend(char* psSendBuf, unsigned int uiSendSize)
{
	unsigned int iTotal = 0;
	while (iTotal < uiSendSize) 
	{
		int iSize = write(m_iSockfd, psSendBuf + iTotal, uiSendSize - iTotal);	
		if (iSize < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) 
			{
				usleep(1000);
				continue;		
			}
			NP_LOG(np_log::NP_ERROR,"send err[%s].", strerror(errno));
			return -1;
		} 
		else if (iSize == 0) 
		{
			return 0;
		}
		
		iTotal += iSize;
	}
	
	return iTotal;
}

/*
int CTcp::TcpSelect(fd_set* pFdSet, int* ifd, int iMaxfd)
{
	int iCount = 0;
	struct timeval tv = {SEL_TIME, 0};
	FD_ZERO(pFdSet);
	FD_SET(m_iSockfd, pFdSet);
	while(iCount < iMaxfd) {
		if (ifd[iCount] != 0) {
			FD_SET(ifd[iCount], pFdSet);
		}
		++iCount;
	}
	
	int iRet = select(iMaxfd + 1, pFdSet, NULL, NULL, &tv);
	if(FD_ISSET(m_iSockfd, pFdSet)) {
		int iNewSocket = TcpAccept();
		if (iNewSocket) {
			iCount = 0;
			while(iCount < iMaxfd) {
				if (0 == ifd[iCount]) {
					ifd[iCount] = iNewSocket;
					break;
				}
				++iCount;
			}
		}
	}
	return iRet;
}
*/


