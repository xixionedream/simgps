#ifndef _TCP_H
#define _TCP_H

#include "socket.h"
#define BLOCKLOG_NUM 128
#define SEL_TIME 30

#define MEM_TYPE 0x01
#define MEM_POOL_TYPE 0x02

typedef struct Packet
{
	int m_fd;
	unsigned int m_PacketSize;
	unsigned int m_PacketOffset;
	unsigned char* m_PacketBuff;
	char m_type;
	Packet()
	{
		m_fd = -1;
		m_PacketSize = 0;
		m_PacketOffset = 0;
		m_PacketBuff =  NULL;
		m_type = 0x01;
	}   
	~Packet()
	{
	}   
}Packet;

class CTcp : public CSock 
{
public:
	CTcp();
	virtual ~CTcp();	

	int TcpInit(const char* psAddr, unsigned short usPort);
	int TcpAccept();
	int TcpConnect(const char* psAddr, const char* psPort);
	int TcpConnect(const struct sockaddr* pstSockaddr);

	int TcpRecv(char* psRecvBuf, unsigned int uiRecvLen);
	int TcpSend(char* psSendBuf, unsigned int uiSendLen);
private:
	
};


#endif // endif _PUB_TCP 

