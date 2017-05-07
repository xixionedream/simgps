
#include "thread_pool.h"

CThreadPool::CThreadPool(unsigned short usThreadsTotal):m_usThreadsTotal(usThreadsTotal)
{
	pthread_mutex_init(&m_stLock, NULL);	
	pthread_cond_init(&m_stCond, NULL);	
	m_pcThreads = new CThread[usThreadsTotal];
	m_PoolWorkFunc = NULL;
}

CThreadPool::~CThreadPool() 
{
	m_PoolWorkFunc = NULL;
	if (m_pcThreads) {
		delete []m_pcThreads;
		m_pcThreads = NULL;	
	}
	pthread_cond_destroy(&m_stCond);	
	pthread_mutex_destroy(&m_stLock);	
}

void CThreadPool::SetThreadsTotal(unsigned short usThreadsTotal)
{
	m_usThreadsTotal = usThreadsTotal;
	if (m_pcThreads) {
		delete []m_pcThreads;
		m_pcThreads = new CThread[usThreadsTotal];
	}
}

int CThreadPool::CreateThreadPool()
{
	unsigned short usThreadNum  = 0;
	for (; usThreadNum < m_usThreadsTotal; ++usThreadNum) {
		m_pcThreads[usThreadNum].CThreadSetStartRoutine(StartRoutine);
		if (m_pcThreads[usThreadNum].CThreadCreate(this) < 0) {
			Destroy();			
			return -1;
		}	
	}	
	return 0;
}

void CThreadPool::SetStartRoutineFunc(void (*pFunc)(void*, void*), void* ptr)
{
	m_PoolWorkFunc = pFunc;
	m_ptr = ptr;
}

void CThreadPool::Join()
{
	for (unsigned short usThreadNum = 0; usThreadNum < m_usThreadsTotal; ++usThreadNum) {
		m_pcThreads[usThreadNum].CThreadJoin();
	}
}

void CThreadPool::Destroy()
{
	for (unsigned short usThreadNum= 0; usThreadNum < m_usThreadsTotal; ++usThreadNum) {
		 m_pcThreads[usThreadNum].CThreadDestroy();
	}
}


void CThreadPool::Distribute(void* pPara)
{
	pthread_mutex_lock(&m_stLock);	
	m_qData.push(pPara);	
	pthread_mutex_unlock(&m_stLock);	
	pthread_cond_signal(&m_stCond);
}

void* CThreadPool::StartRoutine(void* pPara)
{
	CThreadPool* pcThreadPool = (CThreadPool*)pPara;	
	while (1) {
		pthread_mutex_lock(pcThreadPool->GetLock());	
		while (pcThreadPool->GetQueue()->empty()) {
			pthread_cond_wait(pcThreadPool->GetCond(), pcThreadPool->GetLock());	
		}
		void* pData = pcThreadPool->GetQueue()->front();
		pcThreadPool->GetQueue()->pop();
		pthread_mutex_unlock(pcThreadPool->GetLock());		
		(*(pcThreadPool->m_PoolWorkFunc))(pData, pcThreadPool->m_ptr);
	}
		
	return NULL;
}

