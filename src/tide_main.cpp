#include <getopt.h>
#include <sys/resource.h>
#include "utils.h"
#include "tide_server.h"
#include "netposa_license.h"
#include "unistd.h"
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/param.h>	
#include<iostream>
#include "stdlib.h"


void print_help()
{
    np_log::NP_STDOUT("Usage: GPSMain [options]\n");
    np_log::NP_STDOUT("Valid options are:\n");
    np_log::NP_STDOUT("-c or --Config:      load config.\n");
    np_log::NP_STDOUT("-h or --help:        display help.\n");
    np_log::NP_STDOUT("-v or --version:     display version.\n");
	np_log::NP_STDOUT("-d or --daemon:      run in the background.\n");
    np_log::NP_STDOUT("Copyright (C) 2016 NetPosa Inc.\n");
    np_log::NP_STDOUT("Version %s Build on %s %s.\n",CGpsServer::m_pbasic_info->version, __DATE__, __TIME__);
}


void show_title()
{
	std::cout<<"**********************tidedb-server**************************"<<std::endl;
	std::cout<<"* welcom to use tidedb service.                             *"<<std::endl;
	std::cout<<"*                _._                                        *"<<std::endl;
	std::cout<<"*           _.-``___ ''-._                                  *"<<std::endl;
	std::cout<<"*      _.-``      |       ''-._            "<<CGpsServer::m_pbasic_info->version<<" [64bit]    *"<<std::endl;
	std::cout<<"*  .-`` .-    '. ``` .'   \\/  ''-._                         *"<<std::endl;
	std::cout<<"* (      '    , ///// ,  (-:)      )                        *"<<std::endl;
	std::cout<<"* |`-._  '    ' .-__-.'    =  =_.-'|                        *"<<std::endl;
	std::cout<<"* |    `-._   `._____.`    _.-'    |                        *"<<std::endl;
	std::cout<<"*  `-._    `-._        _.-'    _.-'                         *"<<std::endl;
	std::cout<<"* |`-._`-._    `-.__.-'    _.-'_.-'|                        *"<<std::endl;
	std::cout<<"* |    `-._`-._        _.-'_.-'    |                        *"<<std::endl;
	std::cout<<"*  `-._    `-._`-.__.-'_.-'    _.-'                         *"<<std::endl;
	std::cout<<"* |`-._`-._    `-.__.-'    _.-'_.-'|                        *"<<std::endl;
	std::cout<<"* |    `-._`-._        _.-'_.-'    |                        *"<<std::endl;
	std::cout<<"*  `-._    `-._`-.__.-'_.-'    _.-'                         *"<<std::endl;
	std::cout<<"*      `-._    `-.__.-'    _.-'                             *"<<std::endl;
	std::cout<<"*          `-._        _.-'                                 *"<<std::endl;
	std::cout<<"*              `-.__.-'                                     *"<<std::endl;
	std::cout<<"*************************************************************"<<std::endl;
}

void init_daemon()
{
	char sCurrentPath[1024];
	memset(sCurrentPath, 0, sizeof(sCurrentPath));
	strcpy(sCurrentPath, getenv("PWD"));	
	int iPid;
	iPid = fork();
	if (iPid < 0) 
	{
		printf("can't fork.\n");
		exit(1);  
	}
	else if (iPid != 0) 
	{
		exit(0);
	}
	setsid(); 
	//ensure can't open new TTYs	
	iPid = fork();
	if (iPid > 0) 
	{
		exit(0);
	} 
	else if (iPid < 0) 
	{
		printf("can't fork child process.\n");
		exit(1);
	}

	int ifd = 0;
	for (ifd = 0; ifd < NOFILE; ++ifd) 
	{
		close(ifd);
	}
	//change work path		
	chdir(sCurrentPath); 
	umask(0);
	return;
}

int main(int argc, char* argv[])
{
	int iIndex = 0;
	char sConfigFileName[512];
	memset(sConfigFileName, 0, sizeof(sConfigFileName));
	int isdaemon = 0;
	struct option stOpt[] = 
	{
        	{"config", 1, 0, 'c'},
        	{"help", 0, 0, 'h'},
        	{"version", 0, 0, 'v'},
		    {"daemon", 0, 0, 'd'},
        	{0, 0, 0, 0}
	};
	
	while(1)
	{
		int iReturn = getopt_long(argc, argv, "c:vhd", stOpt, &iIndex);
		if (iReturn < 0) break;
        switch (iReturn) 
		{
		case 'c':
			memcpy(sConfigFileName, optarg, strlen(optarg));
			break;
		case 'v':
			np_log::NP_STDOUT("Copyright (C) 2016 NetPosa Inc.\n");
			np_log::NP_STDOUT("Version %s Build on %s %s.\n", CGpsServer::m_pbasic_info->version, __DATE__, __TIME__);
			return 0;
			break;
		case 'd':
			isdaemon = 1;
			break;
		case 'h':
			print_help();
			return 0;
			break;
		default:    
            break;
		}
	}

	struct rlimit stLimit;
	stLimit.rlim_cur = stLimit.rlim_max = 65536;
	if (setrlimit(RLIMIT_NOFILE, &stLimit) != 0)
	{
		np_log::NP_STDOUT("setrlimit err.\n");
		return -1;			
	}	
	if (strlen(sConfigFileName) == 0) 
	{
		if(get_exe_path(sConfigFileName,sizeof(sConfigFileName)) == NULL)
		{
			np_log::NP_STDOUT("get path err.\n");
			return -1;
		}
		strcat(sConfigFileName,"/../conf/gps.conf");	
	}

	//set log save path
	if (CGpsServer::m_cConf.ConfOpen(sConfigFileName) < 0) 
	{
		np_log::init_nplog(argv[0]);
		np_log::NPLOG_ERROR("open config file error");
		exit(-1);
	}
	char log_path[256];
	CGpsServer::m_cConf.ConfGetSecVal("LOG", "PATH", log_path);
	np_log::init_nplog(log_path);
	
	
	
	char rest_port[256];
	CGpsServer::m_cConf.ConfGetSecVal("SERVER", "REST_PORT", rest_port);
	//
	show_title();
	np_log::NP_STDOUT("# Server started.tidedb-server version %s.",CGpsServer::m_pbasic_info->version);
	np_log::NP_STDOUT("# The server now ready to provide service on port %s.",rest_port);
	CGpsServer *pgps_server = new CGpsServer();
	//
	if(isdaemon == 1)
	{
		init_daemon();
	}
	
	if (pgps_server->GpsInit(sConfigFileName) < 0) 
	{
		printf("start gps server failed!\n");
		exit(-1);
	}
	return 0;
}   

