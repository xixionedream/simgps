/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: wangzhen
Last Modify Date:2016-08-24
Description:Persistence the memory of the GPS data to the file
**************************************************************************/

#ifndef _GPSPERSISITENCE_H
#define _GPSPERSISITENCE_H

#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include "utils.h"
#include "config.h"
#include "thread.h"
#include "geohash.h"

#define PERSISIT_VAL_SIZE (1024 * 1024 * 100)
#define MAX_FILEFD 20460

class CPersisitence{
public:
	/*************************************************
		Function: CPersisitence
		Description: Constructor
		Input: 		ppfd: Pointer to the current temporary file for saving data
			    pstFile1: Pointer to the first temporary file for saving data
			    pstFile2: Pointer to the second temporary file for saving data
			   	pcConfig: Pointer to the class of configuration file
			 ppumGpsInfo: Pointer to the pointer of real-time memory data
			 pumGpsInfo1: Pointer to the first memory area
			 pumGpsInfo2: Pointer to the second memory area
			   plastData: Pointer to the last data for the device on the last persistence
		  pstatisticData: Pointer to the statistical data on the last persistence
		Return:NULL
	*************************************************/	
	CPersisitence(int* ppfd, FILEINFO_S* pstFile1, FILEINFO_S* pstFile2, CConfig* pcConfig, unordered_map<string, vector<GPSVALUE_S> >** ppumGpsInfo, \
								unordered_map<string, vector<GPSVALUE_S> >* pumGpsInfo1, unordered_map<string, vector<GPSVALUE_S> >* pumGpsInfo2,\
								unordered_map<string,GPSVALUE_S>* plastData,unordered_map<string,unsigned long long>* pstatisticData);
	~CPersisitence();
	
	int Core();

private:
	/*************************************************
		Function: MemToLocusFile
		Description: Persistence memory data into the locus file
		Input: 	void*: Pointer to the real-time memory data
		Return: void*
	*************************************************/	
	static void* MemToLocusFile(void*);

	/*************************************************
		Function: MemToAroundFile
		Description: Persistence memory data into the around file,through geohash
		Input: 	void*: Pointer to the real-time memory data
		Return: void*
	*************************************************/	
	static void* MemToAroundFile(void*);

	/*************************************************
		Function: MemToLastFile
		Description: Persistence the last data for the device into the file
		Input: 	void*: Pointer to the class of CPersisitence
		Return: void*
	*************************************************/	
	static void* MemToLastFile(void*);
	
	int* m_ppLogfd;	/*Pointer to the current temporary file for saving data*/
	FILEINFO_S* m_pstFile1;	/*Pointer to the first temporary file for saving data*/
	FILEINFO_S* m_pstFile2;	/*Pointer to the second temporary file for saving data*/
	CConfig* m_pcConfig;	/*Pointer to the class of configuration file*/
	unordered_map<string, vector<GPSVALUE_S> >** m_ppumGpsInfo;		/*Pointer to the pointer of real-time memory data*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pumGpsInfo1;		/*Pointer to the first memory area*/
	unordered_map<string, vector<GPSVALUE_S> >* m_pumGpsInfo2;		/*Pointer to the second memory area*/

	unordered_map<string,GPSVALUE_S>* m_plastdata;	/*Pointer to the last data for the device on the last persistence*/
	unordered_map<string,unsigned long long>* m_pstatisticdata;	/* Pointer to the statistical data on the last persistence*/
	int m_iDelayLoop;	/* Cycle time of persistent*/
};

#endif

