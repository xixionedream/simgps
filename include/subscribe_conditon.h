#ifndef _SUBSCRIBE_CONDITION_H
#define _SUBSCRIBE_CONDITION_H

#include <map>
#include <vector>

template<class T>
class subscribe_conditon
{
public:
	subscribe_conditon(){}
	~subscribe_conditon(){}
public:
	T m_pcon;
	int type;
};

#endif