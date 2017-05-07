#include "thread.h"

CThread::CThread():m_ThreadId(0),m_ThreadFunc(NULL) {
	memset(&m_stThreadAttr, 0, sizeof(m_stThreadAttr));
}

CThread::~CThread(){
	m_ThreadId = 0;
	m_ThreadFunc = NULL;
	memset(&m_stThreadAttr, 0, sizeof(m_stThreadAttr));
}

int CThread::CThreadCreate(void* para)
{
	if (NULL == para) {
		if (pthread_create(&m_ThreadId, &m_stThreadAttr, m_ThreadFunc, this) < 0) {
			NP_LOG(np_log::NP_ERROR,"pthread_create err.\n");
			return -1;
		}
	} else {
		if (pthread_create(&m_ThreadId, &m_stThreadAttr, m_ThreadFunc, para) < 0) {
			NP_LOG(np_log::NP_ERROR,"pthread_create err.\n");
			return -1;
		}
	}
	return 0;
}

void CThread::CThreadSetStartRoutine(void* (*Func)(void*))
{
	m_ThreadFunc = Func;
}

void CThread::CThreadSetDetach()
{
	pthread_detach(m_ThreadId);	
}

int CThread::CThreadAttrInit()
{
	if(pthread_attr_init(&m_stThreadAttr) < 0) {
		NP_LOG(np_log::NP_ERROR,"pthread_attr_init err.\n");
		return -1;
	}
	return 0;
}	

int CThread::CThreadAttrDestroy()
{
	if(pthread_attr_destroy(&m_stThreadAttr) < 0) {
		NP_LOG(np_log::NP_ERROR,"pthread_attr_destroy err.\n");
		return -1;
	}
	return 0;
}

pthread_attr_t* CThread::CThreadGetAttr()
{
	return &m_stThreadAttr;
}

int CThread::CThreadJoin()
{
	return pthread_join(m_ThreadId, NULL);
}

int CThread::CThreadDestroy()
{
	return pthread_cancel(m_ThreadId);
}

