#include "config.h"
#define MAX_LINE_SIZE 4096

CConfig::CConfig()
{
	m_pFileId = NULL;
}

CConfig::~CConfig()
{
}

int CConfig::ConfOpen(const char* pszFileName)
{
	m_pFileId = fopen(pszFileName, "r");
	if (NULL == m_pFileId) 
	{
		NP_LOG(np_log::NP_ERROR, "open file[%s] err:[%s].", pszFileName, strerror(errno));
		return -1;
	}

	return 0;	
}

void CConfig::ConfClose()
{
	if (m_pFileId) 
	{
		fclose(m_pFileId);
		m_pFileId = NULL;
	}
}

int CConfig::get_allSec_by_secPrefix(const char* prefix, std::set<string>* secs)
{
	char sLine[MAX_LINE_SIZE];
	const unsigned int uiPrefixLen = strlen(prefix);
	if (m_pFileId) 
	{	
		fseek(m_pFileId, 0, SEEK_SET);	
		memset(sLine, 0, sizeof(sLine));
		unsigned int uiLineLength = 0;
		char* p = NULL;
		while (fgets(sLine, sizeof(sLine), m_pFileId)) 
		{
			Trim(sLine);
			if ('#' == *sLine) continue;
			if (*sLine != '[') continue;
			uiLineLength = strlen(sLine);
			if (*(sLine + uiLineLength - 1) == ']') 
			{
				p = sLine + 1;	
				Trim(p);
				if (strncasecmp(p, prefix, uiPrefixLen) == 0) 
				{
					*(sLine + strlen(sLine) - 1) = 0;
					Trim(p);
					if (secs->find(p) != secs->end()) 
					{
						NP_LOG(np_log::NP_ERROR,"[%s] already exsit", p);
						return -1;	
					}
					secs->insert(p);
				}	
			}
			memset(sLine, 0, sizeof(sLine));
		}	
	}
	
	return 0;
}

bool CConfig::ConfIsSec(const char* pszSec)
{
	bool bFlag = false;
	char sLine[MAX_LINE_SIZE];
	if (m_pFileId) {	
		fseek(m_pFileId, 0, SEEK_SET);	
		memset(sLine, 0, sizeof(sLine));
		while (fgets(sLine, sizeof(sLine), m_pFileId)) 
		{
			Trim(sLine);
			if (*sLine != '[') continue;
			char* p = sLine + 1;
			Trim(p);
			if (*(p + strlen(p) - 1) == ']') 
			{
				if (strncmp(p, pszSec, strlen(pszSec)) == 0) 
				{
					bFlag = true;
					break;
				}
			}
			memset(sLine, 0, sizeof(sLine));
		}	
	}
	return bFlag;
}

void CConfig::ConfGetSecLine(const char* pszSec, vector<string>* pvSecLine)
{
	if (ConfIsSec(pszSec)) {
		char sLine[MAX_LINE_SIZE];
		memset(sLine, 0, sizeof(sLine));
		while (fgets(sLine, sizeof(sLine), m_pFileId)) 
		{
			Trim(sLine);
			if ('#' == *sLine) continue;
			if ('[' == *sLine) 
			{
				break;
			}
			pvSecLine->push_back(sLine);		
			memset(sLine, 0, sizeof(sLine));
		}
	}
	rewind(m_pFileId);		
}

int CConfig::ConfGetSecVal(const char* pszSec, const char* pszKey, char* pszVal)
{
	char sLine[MAX_LINE_SIZE];
	if (NULL == pszVal) 
	{
		np_log::NP_STDOUT("u input val is NULL.");
		return -1;
	}
	if (ConfIsSec(pszSec)) 
	{
		memset(sLine, 0, sizeof(sLine));
		while (fgets(sLine, sizeof(sLine), m_pFileId)) 
		{
			Trim(sLine);
			if ('#' == *sLine) continue;
			if ('[' == *sLine) 
			{
				return -1;
			}
			if (strncmp(sLine, pszKey, strlen(pszKey)) == 0) 
			{
				char* pszPos = sLine + strlen(pszKey);	
				while (*pszPos != '=' && *pszPos != 0) pszPos++;
				if (*pszPos == 0) 
				{	
					NP_LOG(np_log::NP_ERROR,"[%s] config err", pszSec);
					break;
				}
				++pszPos;
				Trim(pszPos);
				memcpy(pszVal, pszPos, strlen(pszPos));
				pszVal[strlen(pszPos)] = 0;
				rewind(m_pFileId);		
				return 0;
			}
			memset(sLine, 0, sizeof(sLine));
		}
	}
	
	rewind(m_pFileId);		
	return -1;
}

