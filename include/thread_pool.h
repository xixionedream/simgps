#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "thread.h"

/**************************************************************************
Copyright (c) netposa
Author: mg
Update: rjx
Last Date:2015-6-29
Description:thread pool. it's used to manage all threads.
**************************************************************************/
class CThreadPool
{
public:
	CThreadPool(unsigned short usThreadsTotal = 1);
	virtual ~CThreadPool();
	
	/******************************************************************
	*Summary: set threads num
	*Parameters:
	*     usTheadsTotal: threads num
	*Return : void
	*******************************************************************/
	void SetThreadsTotal(unsigned short usTheadsTotal);

	/******************************************************************
	*Summary: it's used to create thread pool.you must first to create 
		thread pool.then you can use it.
	*	Parameters:null
	*Return : void
	*******************************************************************/
	int CreateThreadPool();

	/******************************************************************
	*Summary: Thread function assignment. it includes thread processing 
		function and prameter.
	*Parameters:
	*	pFunc: thread processing function
	*	ptr:thread processing function prameter.
	*Return : void
	*******************************************************************/
	void SetStartRoutineFunc(void (*pFunc)(void*, void*), void* ptr);

	/******************************************************************
	*Summary: threads distribute func
	*Parameters:
	*	pPara: put it int your queue.
	*Return : void
	*******************************************************************/
	void Distribute(void* pPara);

	/******************************************************************
	*Summary: stop all threads and release all resources.
	*Parameters:null
	*Return : void
	*******************************************************************/
	void Destroy();

	/******************************************************************
	*Summary: wait threads to execute over.
	*Parameters:null
	*Return : void
	*******************************************************************/
	void Join();	
	
	pthread_mutex_t* GetLock() 
	{
		return &m_stLock;
	}

	pthread_cond_t* GetCond() 
	{
		return &m_stCond;
	}

	queue<void*>* GetQueue() 
	{
		return &m_qData;
	}
public:		
	//thread func
	void (*m_PoolWorkFunc)(void*, void*);
	void* m_ptr;
private:
	//start threads
	static void* StartRoutine(void* pPara);
	//threads num
	unsigned short m_usThreadsTotal;	
	//lock
	pthread_mutex_t m_stLock;
	pthread_cond_t m_stCond;
	//thread queue
	queue<void*> m_qData;
	CThread* m_pcThreads;
};


#endif //define _PUB_THREADPOOL

