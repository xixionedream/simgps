#include "socket.h"

CSock::CSock()
{
	m_iSockfd = -1;
}
CSock::~CSock()
{
}

int CSock::CreatSocket(int iType)
{
	m_iSockfd = socket(AF_INET, iType, 0);
	if (m_iSockfd == -1) 
	{
		NP_LOG(np_log::NP_ERROR, "socket err[%s].", strerror(errno));
		return -1;
	}
	return 0;
}

int CSock::GetSocket() const
{
	return m_iSockfd;
}

void CSock::SetSocket(int iNewSocket)
{
	m_iSockfd = iNewSocket;
}

int CSock::ShutdownSocket(int iType)
{
	if (m_iSockfd) 
	{
		shutdown(m_iSockfd, iType);
		m_iSockfd = -1;
	}
	
	return 0;
}

int CSock::CloseSocket()
{
	NP_LOG(np_log::NP_INFO, "close socket:%d", m_iSockfd);
	if (m_iSockfd) 
	{
		close(m_iSockfd);
		m_iSockfd = -1;
	}
	return 0;
}

int CSock::SetBlocking(bool blocking)
{
	int iFlag = fcntl(m_iSockfd, F_GETFL);
	if (iFlag < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "F_GETFL err[%s].", strerror(errno));
		return -1;
	}
	if (blocking)
		iFlag &= ~O_NONBLOCK;
	else
		iFlag |= O_NONBLOCK;
	
	return fcntl(m_iSockfd, F_SETFL, iFlag);	
}

int CSock::SetReuseAddr()
{
	int yes = 1; 
	int retopt = setsockopt(m_iSockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if (retopt < 0) 
	{
		NP_LOG(np_log::NP_ERROR, "setsockopt err[%s].", strerror(errno));
		return -1;
	}	
	return 0;
}

int CSock::SetNodelay()
{
	int enable = 1;
	return setsockopt(m_iSockfd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}
	
int CSock::SetLinger(int iOnoff, int iSeconds)
{
	struct linger stLinger = {iOnoff, iSeconds};
	return setsockopt(m_iSockfd, SOL_SOCKET, SO_LINGER, &stLinger, sizeof(struct linger));
}

int CSock::SetTimeout(unsigned long ulSec, unsigned long ulUsec)
{
	struct timeval stTime;
	stTime.tv_sec = ulSec;
	stTime.tv_usec = ulUsec;
	
	if (setsockopt(m_iSockfd, SOL_SOCKET, SO_SNDTIMEO, &stTime, sizeof(struct timeval)) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"set send timeval err[%s].", strerror(errno));	
		return -1;
	}

	if (setsockopt(m_iSockfd, SOL_SOCKET, SO_RCVTIMEO, &stTime, sizeof(struct timeval)) < 0) 
	{
		NP_LOG(np_log::NP_ERROR,"set recv timeval err[%s].", strerror(errno));
		return -1;
	}
	
	return 0;
}

int CSock::SetBuff(unsigned int uiBuffSize)
{
	if (setsockopt(m_iSockfd, SOL_SOCKET, SO_SNDBUF, &uiBuffSize, sizeof(unsigned int)) < 0) 
	{
		return -1;
	} 
	
	if (setsockopt(m_iSockfd, SOL_SOCKET, SO_RCVBUF, &uiBuffSize, sizeof(unsigned int)) < 0) 
	{
		return -1;
	} 
	
	return 0;
}

int CSock::KeepAlive()
{
	int yes = 1;
	if (setsockopt(m_iSockfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes)) == -1) 
	{
		NP_LOG(np_log::NP_ERROR,"setsockopt SO_KEEPALIVE: %s", strerror(errno));
		return -1;
	}
	return 0;
}

int SetNonblocking(int socketfd, bool Nonblocking)
{
    int iFlag = fcntl(socketfd, F_GETFL);
    if (iFlag < 0) 
	{
        NP_LOG(np_log::NP_ERROR,"F_GETFL err[%s].", strerror(errno));
        return -1;
    }
    if (Nonblocking)
	{
		iFlag |= O_NONBLOCK;
	}     
    else
	{
		iFlag &= ~O_NONBLOCK;
	}
    return fcntl(socketfd, F_SETFL, iFlag);
}

int GetServerName(int fd, char** psAddr, unsigned short& usPort)
{
    struct sockaddr_in stAddr;
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in); 
    if (getsockname(fd, (struct sockaddr*)&stAddr, &len) < 0) 
	{
        NP_LOG(np_log::NP_ERROR,"getsockname fd:%d err[%s].", fd, strerror(errno));
        return -1;
    }                   
    *psAddr = inet_ntoa(stAddr.sin_addr);
    usPort = ntohs(stAddr.sin_port);   
    return 0;
}

int GetClientName(int fd, char** psAddr, unsigned short& usPort)
{   
    struct sockaddr_in stAddr;
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
    if (getpeername(fd, (struct sockaddr*)&stAddr, &len) < 0) 
	{
        NP_LOG(np_log::NP_ERROR,"getsockname fd:%d err[%s].", fd, strerror(errno));
        return -1;
    }
    *psAddr = inet_ntoa(stAddr.sin_addr);
    usPort = ntohs(stAddr.sin_port);

    return 0;
}

int Recv(int fd, unsigned char* psRecvBuf, unsigned int uiRecvSize)
{
	unsigned int iTotal = 0;
	while (iTotal < uiRecvSize) 
	{
		int iSize = read(fd, psRecvBuf + iTotal, uiRecvSize - iTotal);
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
					usleep(100);
					continue;
				}
			}
			NP_LOG(np_log::NP_ERROR,"recv fd[%d] err[%s].", fd, strerror(errno));
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

int Send(int fd, unsigned char* psSendBuf, unsigned int uiSendSize)
{
	unsigned int iTotal = 0;
	while (iTotal < uiSendSize) 
	{
		int iSize = write(fd, psSendBuf + iTotal, uiSendSize - iTotal);
		if (iSize < 0) 
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) 
			{
				usleep(100);
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
