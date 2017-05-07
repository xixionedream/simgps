#ifndef _BASIC_INFO_H
#define _BASIC_INFO_H


#if 1 // server basic message
typedef struct BASIC_MODEL_INFO_
{
	char version[20];
	char company[64];
	char server_name[20];
	char server_addr[64];
	
	BASIC_MODEL_INFO_()
	{
		sprintf(version,"%s","2.3.4");
		sprintf(server_name,"%s","tidedb-server");
		sprintf(server_addr,"%s","http://www.netposa.com");
		sprintf(company,"%s","NetPosa");
	}
}BASIC_MODEL_INFO,*PBASIC_MODEL_INFO;

#endif

#endif