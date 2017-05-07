#ifndef _CONFIG_H
#define _CONFIG_H

#include <set>
#include "utils.h"
using namespace std;
class CConfig 
{
public:
	CConfig();
	~CConfig();

	int ConfOpen(const char* pszFileName);
	void ConfClose();
	
	int ConfGetSecVal(const char* pszSec, const char* pszKey, char* pszVal);
	void ConfGetSecLine(const char* pszSec, vector<string>* pvSecLine);
	bool ConfIsSec(const char* pszSec);
	int get_allSec_by_secPrefix(const char* prefix, set<string>* secs);
				
private:
	FILE* m_pFileId;
};

#endif  //_CONFIG_H
