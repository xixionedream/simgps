#ifndef _FILE_MANAGER_H
#define _FILE_MANAGER_H

#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<vector>
#include<string>

#include<utils.h>

class file_manager;

class file_manager
{
public:
	file_manager();
	virtual ~file_manager();
	
private:
	file_manager(const file_manager& manager_service);
	void operator=(const file_manager& manager_service);
	
public:
	//add  dirs which will deleted to vector 
	void push_delfile_dir(std::string file_dir);
	//remove  dirs which will deleted from vector 
	void remove_delfile_dir(std::string file_dir);
	
	//read files in dir to vector
	void read_files_to_vec(std::string dir_name);
	//delte out date gps files
	void clean_outdate_files();
	//set save file days
	void set_save_file_days(int days);
private:
	bool update_dir_subscript(std::string dir_name);
	
private:
	std::vector<std::string> m_delete_dirs;
	std::vector<std::string> m_delete_files;
	std::vector<std::string> m_files;
	
	int m_file_save_days;
	int m_dir_subscript;
};

#endif // _FILE_MANAGER_H