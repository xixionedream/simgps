#include "gpsid_writer.h"

void GpsidWriter::Init(std::string file_path)
{
	m_persistence_file = "gpsid";
	char filepath[FILE_PATH_LENGTH];
	memset(filepath,0,FILE_PATH_LENGTH);
	memcpy(filepath,file_path.c_str(),(size_t)file_path.length());
	strcat(filepath,m_persistence_file.c_str());
	m_file_path = filepath;

	m_file_fd = open(m_file_path.c_str(),O_CREAT | O_RDWR | O_APPEND, 0666);
	if(m_file_fd < 0)
	{
		NP_LOG(np_log::NP_ERROR, "open file %s faild,err:%s", m_file_path.c_str(), strerror(errno));
		m_is_init = false;
		return;
	}
	if(ReadGpsidFromFile())
	{
		m_is_init = true;
	}
	m_cur_seek_seat = 0;
}

bool GpsidWriter::ReadGpsidFromFile()
{
	if(m_file_fd <= 0)
	{
		return false;
	}	
	//read data
	char tmpchr[READ_FILE_SIZE+1];
	char* ptmp = tmpchr;
	char gpsid[32];
	memset(tmpchr,0,READ_FILE_SIZE+1);
	memset(gpsid,0,32);
	int index =0;
	int len=0;
	while((len = read(m_file_fd,ptmp,READ_FILE_SIZE)) > 0)
	{
		*(ptmp+len) = 0;
		while(*ptmp != 0 && index <= 32)
		{
			if(*ptmp != '|')
			{
				gpsid[index++] = *ptmp;
			}
			else
			{
				gpsid[index] = 0;
				if(m_gpsid_map.find(gpsid) == m_gpsid_map.end())
				{
					m_gpsid_map.insert(pair<std::string,char>(gpsid,0x0));
				}
				index = 0;
			}	
			ptmp++;
		}		
		memset(tmpchr,0,READ_FILE_SIZE+1);
		ptmp = tmpchr;
	}
	return true;
}

void GpsidWriter::CloseFile()
{
	if(m_file_fd >0)
	{
		close(m_file_fd);
	}
}

std::unordered_map<std::string,char>* GpsidWriter::get_gpsid_map()
{
	return &m_gpsid_map;
}

void GpsidWriter::set_gpsid_map(std::unordered_map<std::string,char>* pgps_map)
{
	m_gpsid_map = *pgps_map;
}

std::string GpsidWriter::get_persistence_file()
{
	return m_persistence_file;
}

void GpsidWriter::set_persistence_file(std::string per_file)
{
	m_persistence_file = per_file;
}

std::string GpsidWriter::get_file_path()
{
	return m_file_path;
}

void GpsidWriter::set_file_path(std::string file_path)
{
	m_file_path = file_path;
}

int GpsidWriter::get_file_fd()
{
	return m_file_fd;
}

void GpsidWriter::set_file_fd(int file_fd)
{
	m_file_fd = file_fd;
}

int GpsidWriter::get_cur_seek_seat()
{
	return m_cur_seek_seat;
}

void GpsidWriter::set_cur_seek_seat(int cur_seek_seat)
{
	m_cur_seek_seat = cur_seek_seat;
}

bool GpsidWriter::get_is_init()
{
	return m_is_init;
}

void GpsidWriter::WriteGpsId(std::string gpsid)
{
	if(m_file_fd > 0 && m_is_init)
	{
		char gpsid_c[32];
		sprintf(gpsid_c,"%s%s",gpsid.c_str(),GPSID_SPILT_CHR);
		write(m_file_fd,gpsid_c,strlen(gpsid_c));
	}
	else
	{
		NP_LOG(np_log::NP_ERROR, "save gpsid error.\n");
	}	
}

bool GpsidWriter::InsertGpsIDMsg(std::string gpsid)
{
	if(m_gpsid_map.find(gpsid) == m_gpsid_map.end())
	{
		m_gpsid_map.insert(pair<std::string,char>(gpsid,GPSID_MAP_VALUE));
		WriteGpsId(gpsid);
		return true;
	}
	return false;
}
