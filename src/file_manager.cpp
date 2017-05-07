#include "file_manager.h"

file_manager::file_manager()
{
	m_file_save_days = SAVE_FILE_DAYS;
	m_dir_subscript = -1;
}

file_manager::~file_manager()
{
	
}

void file_manager::push_delfile_dir(std::string file_dir)
{
	m_delete_dirs.push_back(file_dir);
}


void file_manager::remove_delfile_dir(std::string file_dir)
{
	std::vector<std::string>::iterator it_find = m_delete_dirs.begin();
	for (; it_find != m_delete_dirs.end(); )
	{
		if (*it_find == file_dir)
		{
			it_find = m_delete_dirs.erase(it_find);
		}
		else
		{
			++it_find;
		}
	}
}

bool file_manager::update_dir_subscript(std::string dir_name)
{
	////set dir subscript
	bool ret_val = false;
	std::vector<std::string>::iterator it_find = m_delete_dirs.begin();
	for(int i=0; it_find != m_delete_dirs.end(); ++it_find,++i)
	{
		if(dir_name == (*it_find))
		{
			m_dir_subscript = i;
			ret_val = true;
			break;
		}
	}
	return ret_val;
}

void file_manager::read_files_to_vec(std::string dir_name)
{
	DIR* pdp;
	struct dirent* pdirp;
	m_files.clear();
	
	////set dir subscript
	if(!update_dir_subscript(dir_name))
	{
		return;
	}
	
	//read files
	if((pdp=opendir(dir_name.c_str()))==NULL)
	{
		return;
	}
	
	while((pdirp=readdir(pdp))!=NULL)
	{
		if((strcmp(pdirp->d_name,".")==0)||(strcmp(pdirp->d_name,"..")==0))
				continue;
		string file_name = pdirp->d_name;
		if(Atoi(file_name.c_str(),0,1) != 2)
		{	
			continue;
		}
		m_files.push_back(pdirp->d_name);
	}
	
}

void file_manager::clean_outdate_files()
{
	int day_time = get_local_time_days();
	reduce_days(day_time, m_file_save_days);
	std::vector<std::string>::iterator it_find = m_files.begin();
	for(; it_find!=m_files.end(); ++it_find)
	{
		if(day_time > Atoi((*it_find).c_str(),0,8))
		{
			m_delete_files.push_back(*it_find);
		}
	}
	
	//delte files
	it_find = m_delete_files.begin();
	std::string file_path;
	for(; it_find!=m_delete_files.end(); ++it_find)
	{
		file_path = m_delete_dirs[m_dir_subscript] + *it_find;
		remove(file_path.c_str());
	}

	//clear mem buffer
	m_files.clear();
	m_delete_files.clear();
	m_dir_subscript = -1;
}

void file_manager::set_save_file_days(int days)
{
	m_file_save_days = days;
}


