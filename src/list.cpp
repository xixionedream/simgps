/************************
Author:	magang
Date:	2015-05-18
************************/
#include "list.h"

CList::CList()
{
	m_pstHead = NULL;
	m_uiListSize = 0;
}

CList::~CList()
{
	if (m_pstHead) {	
		LISTNODE_S* pstDelNode = m_pstHead;	
		LISTNODE_S* pstNode = m_pstHead->m_pstNext;	
		unsigned int uiNum = 0;
		for (uiNum = 0; uiNum < m_uiListSize; ++uiNum)	{
			if (pstDelNode) {
				free(pstDelNode);
				pstDelNode = NULL;
			}
			if (pstNode == NULL) {
				break;	
			}
			pstDelNode = pstDelNode->m_pstNext;	
			pstNode = pstNode->m_pstNext;
		}	
	}
	m_uiListSize = 0;
}

int CList::AddNode(void* pData)
{
	LISTNODE_S* pstNode = (LISTNODE_S*)malloc(sizeof(LISTNODE_S));
	if (pstNode == NULL) {
		return -1;
	}
	pstNode->m_pData = pData;
	pstNode->m_pstNext = NULL;
		
	if (m_pstHead == NULL) {
		m_pstHead = pstNode;
	} else {
		pstNode->m_pstNext = m_pstHead->m_pstNext;
		m_pstHead->m_pstNext = pstNode;
	}
	++m_uiListSize;
	
	return 0;
}

int CList::DeleteNode(void* pData)
{
	LISTNODE_S* pstPreNode = m_pstHead;	
	LISTNODE_S* pstNode = m_pstHead;	
	unsigned int uiNum = 0;
	for (uiNum = 0; uiNum < m_uiListSize; ++uiNum) {
		if (pstNode->m_pData == pData) {
			if (pstNode == m_pstHead) {
				m_pstHead = m_pstHead->m_pstNext;
			} else {
				pstPreNode->m_pstNext = pstNode->m_pstNext;
			}
			pstNode->m_pData = NULL;
			pstNode->m_pstNext = NULL;
			free(pstNode);					
			--m_uiListSize;
			
			return 0;
		}
		pstPreNode = pstNode;
		pstNode = pstNode->m_pstNext;
	}
		
	return -1;
}

LISTNODE_S* CList::GetListHead()
{
	return m_pstHead;	
}

int CList::FindNode(void* pData)
{
	unsigned int uiNum = 0;
	LISTNODE_S* pstNode = m_pstHead->m_pstNext;	
	for (uiNum = 0; uiNum < m_uiListSize; ++uiNum)	{
		if (pstNode->m_pData == pData) {
			return 0;
		}			
		pstNode = pstNode->m_pstNext;
	}	
	
	return -1;
}


