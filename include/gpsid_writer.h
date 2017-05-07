/**
 *Author:rjx
 *nstuction:provide interface to keep gpsid to memory and  file, and read gpsid from\
	 file when servic statring. the gpsid only to keep one.
 *Copyright 2016 netposa Inc
**/

#ifndef TIDE_GPSID_WRITE_H_
#define TIDE_GPSID_WRITE_H_

#include <unordered_map>
#include <iostream>
#include <string>
#include "utils.h"

class GpsidWriter
{
public:
        GpsidWriter(){};
        inline ~GpsidWriter(){};
        void Init(std::string);
private:
	//copy stuction and  assign  rewrite
	DISALLOW_COPY_AND_ASSIGN(GpsidWriter);
	//read gpis id from file to memroy
	bool ReadGpsidFromFile();
public:
	//get set func
        std::unordered_map<std::string,char>* get_gpsid_map();
        void set_gpsid_map(std::unordered_map<std::string,char>*);
        std::string get_persistence_file();
        void set_persistence_file(std::string);
        std::string get_file_path();
        void set_file_path(std::string);
        int get_file_fd();
        void set_file_fd(int);
        int get_cur_seek_seat();
        void set_cur_seek_seat(int);
        bool get_is_init();
public:
	//write gps id to file
	void WriteGpsId(std::string);
	//write gps id to memroy and file
	bool InsertGpsIDMsg(std::string);
	void CloseFile();
private:
        std::unordered_map<std::string,char> m_gpsid_map;
        std::string m_persistence_file;
        std::string m_file_path;
        int m_file_fd;
        int m_cur_seek_seat;
     	bool m_is_init;
};

#endif  //TIDE_GPSID_WRITE_H_
                                                   
