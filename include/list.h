#ifndef _LIST_H
#define _LIST_H

#include "utils.h"

typedef struct ListNode
{
	void* m_pData;
	ListNode* m_pstNext;
}LISTNODE_S;

class CList 
{
public:
	CList();
	virtual ~CList();

	int AddNode(void* pData);
	int DeleteNode(void* pData);
	
	int FindNode(void*);
	LISTNODE_S* GetListHead();
	unsigned int size()
	{
		return m_uiListSize;
	}
	
private:
	unsigned int m_uiListSize;
	LISTNODE_S* m_pstHead;	
};

#endif
