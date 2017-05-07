#ifndef _THREAD_H
#define _THREAD_H

#include "utils.h"

/**************************************************************************
Copyright (c) netposa
Author: mg
Update: rjx
Last Date:2016-6-29
Description:thread. it provide all ways to use thread.
**************************************************************************/
class CThread
{
public:
	CThread();
	virtual ~CThread();

	/******************************************************************
	*Summary: start thread. assignment a work func to thread.
	*Parameters:
	*     Func: work func
	*Return : void
	*******************************************************************/
	void CThreadSetStartRoutine(void* (*Func)(void*));

	/******************************************************************
	*Summary: create thread
	*Parameters:
	*     para: threads prameter.your thread func can use it.
	*Return : 0:create success. -1 create failed.
	*******************************************************************/
	int CThreadCreate(void* para);

	/******************************************************************
	*Summary: detach thread
	*Parameters:null
	*Return : void
	*******************************************************************/
	void CThreadSetDetach();
		
	/******************************************************************
	*Summary: waite thread.it will release all resources by used
		thred when thread execute over.
	*Parameters:null
	*Return : 0:wait ok. -1:wait exception.
	*******************************************************************/
	int CThreadJoin();

	/******************************************************************
	*Summary: Forced to destruction thread. the thread will be interupted.
	*Parameters:null
	*Return :  0:destroy ok. -1:destroy exception.
	*******************************************************************/
	int CThreadDestroy();

	/******************************************************************
	*Summary: init the thread property.
	*Parameters:null
	*Return :  0:init ok. -1:init exception.
	*******************************************************************/
	int CThreadAttrInit();

	/******************************************************************
	*Summary: destroy the thread property.
	*Parameters:null
	*Return :  0:init ok. -1:init exception.
	*******************************************************************/
	int CThreadAttrDestroy();

	/******************************************************************
	*Summary: get the thread attr.
	*Parameters:null
	*Return :  m_stThreadAttr. thread property.
	*******************************************************************/
	pthread_attr_t* CThreadGetAttr();
private:
	pthread_t m_ThreadId;
	//thread func
	void* (*m_ThreadFunc)(void*);
	pthread_attr_t m_stThreadAttr;
};

#endif //define _PUB_THREADPOOL

