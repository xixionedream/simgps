#include <algorithm>
#include <math.h>
#include "utils.h"
#include <time.h>
#include "cpprest_condition_define.h"

extern bool g_isCompareRemain;

int  GetLocalTimeHour()
{
	time_t time_now;
	time(&time_now);
	struct tm* tm_now;    
	tm_now = localtime(&time_now);
	char i_time[11];
	memset(i_time,0,11);
	sprintf(i_time,"%04d%02d%02d%02d",tm_now->tm_year+1900,tm_now->tm_mon+1,\
	tm_now->tm_mday,tm_now->tm_hour);
	return  atoi(i_time);
}

char* Trim(char* psStr)
{
	char* psRst = psStr;
	char* psStart = psStr;
	char* psEnd = psStr + strlen(psStr) - 1;
	while (strchr(" \t\n\r", *psStart) && psEnd - psStart > 0) psStart++;
	while (strchr(" \t\n\r", *psEnd) && psEnd - psStart > 0) psEnd--;

	int iSize = psEnd - psStart;
	while (iSize-- >= 0) {
		*psStr++ = *psStart++;
	}
	*psStr = '\0';
	return psRst;
}

int spliterByStr(char* psStr, int iSepLen, const char* psSep, vector<char*>* vRet)
{
	char* psPos = psStr;
	char* psBegin = psStr;
	while (1) 
	{
		psPos = strstr(psBegin, psSep);
		if (NULL == psPos) 
		{
			psPos = psStr + strlen(psStr);
		}		
		char* psData = (char*)malloc(psPos - psBegin + 1);
		if(psData != NULL)
		{
			memset(psData, 0, psPos - psBegin + 1);
			memcpy(psData, psBegin, psPos - psBegin);
			vRet->push_back(psData);
			if (*psPos == 0) break;
			psBegin = psPos + iSepLen;
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

void freeSpliter(vector<char*>* vRet)
{
	vector<char*>::iterator iter;
	for (iter = vRet->begin(); iter != vRet->end(); ++iter) 
	{
		if (NULL != *iter) {
			free(*iter);
			*iter = NULL;
		}
	}

	vRet->clear();
}

int codeConvert(const char* psFromCharset, const char* psToCharset, char* psInStr, char* psRst)
{
	iconv_t ict = iconv_open(psToCharset, psFromCharset);

	if (ict == (iconv_t)-1) 
	{
		iconv_close(ict);
		return -1;
	}
	size_t szInLen = strlen(psInStr);
	size_t szOutLen = szInLen << 3;
	
	if (iconv(ict, &psInStr, &szInLen, &psRst, &szOutLen) < 0) 
	{
		iconv_close(ict);
		return -1;
	}
	iconv_close(ict);
	return 0;
}

uint64_t Fnv64a(const void *buf, size_t len)  
{  
	uint64_t hval = (uint64_t)0xcbf29ce484222325ULL;
	unsigned char *bp = (unsigned char *) buf;  
	unsigned char *be = bp + len;  
	while (bp < be) 
	{
		hval ^= (uint64_t) *bp++;  
		hval += (hval << 1) + (hval << 4) + (hval << 5) +  
			(hval << 7) + (hval << 8) + (hval << 40);  
	}
	return hval;  
}

void GeoHashCode(double dLatitude, double dLongitude, char* psGeoKey, int iPrecision, int type)
{
	memset(psGeoKey, 0, iPrecision + 1);
	char BASE32[33];//[17]= "0123456789bcdefg";
	memset(BASE32,0,sizeof(BASE32));
	int iBits[5]={0}, iNext =0;
	if(type == 4)
	{
		int bits[4] = {8,4,2,1};
		memcpy(BASE32,"0123456789bcdefg",17);
		memcpy(iBits,bits,sizeof(bits));
	}
	else
	{
		int bits[5] = {16,8,4,2,1};
		memcpy(BASE32,"0123456789bcdefghjkmnpqrstuvwxyz",33);
		memcpy(iBits,bits,sizeof(bits));
	}
	
	
	bool bOddEven = true;       //longitude is Odd , lattitude is Even.
	double dLat[2] = {-90.00, 90.00}, dLon[2] = {-180.00, 180.00}, dMid = 0;
	int iPosVal = 0;
	int iNum = 0;
	
	while (iNum < iPrecision) 
	{
		if (bOddEven) 
		{
			dMid = (dLon[0] + dLon[1]) / 2;
			if (dLongitude > dMid) 
			{
				iPosVal |= iBits[iNext];
				dLon[0] = dMid;
			}
			else 
			{
				dLon[1] = dMid;
			}
		} 
		else 
		{
			dMid = (dLat[0] + dLat[1]) / 2;
			if (dLatitude > dMid) 
			{
				iPosVal |= iBits[iNext];
				dLat[0] = dMid;
			} 
			else 
			{
				dLat[1] = dMid;
			}
		}
		if (iNext < (type -1)) 
		{
			iNext++;
		} 
		else 
		{
			psGeoKey[iNum++] = BASE32[iPosVal];
			iNext = 0;
			iPosVal = 0;
		}
		bOddEven = !bOddEven;
	}
	psGeoKey[iNum] = 0;
}

void GeohashEncode4(double dLatitude, double dLongitude, char* psGeoKey, int iPrecision)
{
	//memset(psGeoKey, 0, iPrecision + 1);
	GeoHashCode(dLatitude,dLongitude,psGeoKey,iPrecision,4);
}

void GeohashEncode5(double dLatitude, double dLongitude, char* psGeoKey, int iPrecision)
{
	GeoHashCode(dLatitude,dLongitude,psGeoKey,iPrecision,5);
}

int Atoi(const char* psStr, unsigned short usBegin, unsigned short usLen)
{
	int iNum = 0;
	unsigned short usCPos = 0;
	const char* psBegin = psStr + usBegin;
	if (psBegin[usCPos] == '-') 
	{
		usCPos++;
	}
	while (usCPos < usLen) 
	{
		if (psBegin[usCPos] == 0) break;
		if (psBegin[usCPos] >= '0' && psBegin[usCPos] <= '9') 
		{
			iNum = iNum * 10 + (psBegin[usCPos] - '0');
		} 
		else 
		{
			break;
		}
		usCPos++;
	}
	if (psBegin[0] == '-') 
	{
		return 0 - iNum;
	}
	return iNum;
}

long Atol(const char* psStr, unsigned short usBegin, unsigned short usLen)
{
	long lNum = 0;
	unsigned short usCPos = 0;
	const char* psBegin = psStr + usBegin;
	if (psBegin[usCPos] == '-') 
	{
		usCPos++;
	}

	while (usCPos < usLen) {
		if (psBegin[usCPos] == 0) break;
		if (psBegin[usCPos] >= '0' && psBegin[usCPos] <= '9') 
		{
			lNum = lNum * 10 + (psBegin[usCPos] - '0');
		} 
		else 
		{
			break;
		}
		usCPos++;
	}

	if (psBegin[0] == '-') 
	{
		return 0 - lNum;
	}
	
	return lNum;
}

static double Rad(double dDegree)
{
	return dDegree * PI / 180.0;
}

static double Deg(double dRadian)
{
        return dRadian * 180.0 / PI;
}
void GetRectangleEdge(double dLong, double dLat,double ewDistance,double snDistance, double& eastLong, double& westLong, double& southLat, double& northLat)
{
     double tlong=Deg(2*asin(sin(ewDistance/1000.0/(2*EARTH_RADIUS))/cos(Rad(dLat))));
     double tlat=Deg(snDistance/1000.0/EARTH_RADIUS);
     eastLong= dLong + tlong;
     westLong= dLong - tlong;
     southLat= dLat - tlat;
     northLat= dLat + tlat;
}
double GetDistance(double dLat1, double dLong1, double dLat2, double dLong2)
{
	double dRadLat1 = Rad(dLat1);
	double dRadLat2 = Rad(dLat2);
	double a = dRadLat1 - dRadLat2;
	double b = Rad(dLong1) - Rad(dLong2);

	double dRad = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(dRadLat1) * cos(dRadLat2) * pow(sin(b / 2), 2)));
	double dDistance = dRad * EARTH_RADIUS;
	dDistance = round(dDistance * 10000) / 10000;
	return dDistance * 1000;
}

void GetNTUByDistance(unsigned int uiMile, double dLat, double dLong, unsigned int& uiLat, unsigned int& uiLong)
{
	unsigned long uiEarthRadius = (unsigned long)(EARTH_RADIUS * 1000); //mile
	uiLat = uiMile / (PI * uiEarthRadius) * (180 * 100000);	//纬度差值		
//	uiLat = uiMile * (180 * 1000000) / (PI * uiEarthRadius);	//纬度差值		
	double uiLittleRadius = uiEarthRadius * cos(dLat / 180 * PI);
	//printf("radius = %0.2lf\n", uiLittleRadius);
	//printf("uiMile=%u,pi*R = %0.2lf\n", uiMile, (PI * uiLittleRadius));
	//uiLong = uiMile / (PI * uiLittleRadius) * (180 * 100000);	//经度差值		
	uiLong = uiMile *(180 * 100000) / (PI * uiLittleRadius);	//经度差值		
}

int ltos(long lNumber, char* psRst)
{
	const unsigned int radix = 10;
	char* pVal = psRst;
	char* psBegin = psRst;
	int len = 0;
	if (lNumber < 0)
	{
		*pVal++ = '-';
		lNumber = 0 - lNumber;
		psRst++;
	} 
	else if (lNumber == 0) 
	{
		*psRst++ = '0';
		*psRst = 0;
		return 1;			
	}

	unsigned long ulVal = (unsigned long)lNumber;
	while (ulVal > 0) 
	{
		*pVal++ = ulVal % radix + '0';
		ulVal /= radix;
	}
	
	len = pVal - psBegin;
	*pVal-- = 0;
	//swap
	int iNum = 0;
	int iMid = len /2;
	while (iNum < iMid) 
	{
		char temp = *(pVal - iNum);
		*(pVal - iNum) = *(psRst + iNum);
		*(psRst + iNum) = temp;
		++iNum;
	}
	return len;
}

static int GetDaysOfMonth(int iYear,int iMonth)
{
	int iDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	if((iYear % 4 == 0 && iYear % 100 != 0) || iYear % 400 == 0)	
	{
		iDays[1] = 29;
	}
	return iDays[iMonth - 1];
}
	
int GetDateByHour(int iYearMonthDayHour, int iNum)
{
	int iHour = iYearMonthDayHour % 100;
	iYearMonthDayHour /= 100;
	int iDay = iYearMonthDayHour % 100;
	iYearMonthDayHour /= 100;
	int iMonth = iYearMonthDayHour % 100;
	int iYear = iYearMonthDayHour/ 100;			
	
	int iRealHours = iHour + iNum;
	if (iRealHours >= 24) 
	{
		iRealHours %= 24;
		++iDay;
		int iMaxDays = GetDaysOfMonth(iYear, iMonth);
		if (iDay > iMaxDays) 
		{
			iDay %= iMaxDays;
			if (++iMonth > 12) 
			{
				++iYear;
				iMonth = 1;
			}
			return iYear * 1000000 + iMonth * 10000 + iDay * 100 + iRealHours;
		} 
	}	
	return iYear * 1000000 + iMonth * 10000 + iDay * 100 + iRealHours;
}

int MakeDir(char* psDirPath, unsigned short usDirPathLength)
{
    char sSubDirPath[usDirPathLength + 1];
    char* psPos = strchr(psDirPath, '/');
    if (psPos == NULL) 
	{
        return -1;
    }
    ++psPos;
    while (*psPos) 
	{
        while (*psPos != '/' && *psPos !=0 ) ++psPos;

        unsigned short usSubLength = psPos - psDirPath;
        memcpy(sSubDirPath, psDirPath, usSubLength);
        sSubDirPath[usSubLength] = 0;
        if (access(sSubDirPath, F_OK) != 0) 
		{
            mkdir(sSubDirPath, 0766);
        }
        if (*psPos == 0) 
		{
            break;
        } 
		else 
		{
            ++psPos;
        }
    }

    return 0;
}

void ReduceHours(int &time, int hours)
{
	if (time >= 10000000000 || time < 1000000000 || hours > 720)
	{
		return;
	}
	int tmpTime = time;
	int tmpHours = tmpTime % 100;
	tmpTime /= 100;
	int tmpDays = tmpTime % 100;
	tmpTime /= 100;
	int tmpMonth = tmpTime % 100;
	tmpTime /= 100;
	int tmpYears = tmpTime;
	int hHours = tmpHours - hours;
	if (hHours < 0)
	{
		int dhHours = tmpDays * 24 + tmpHours - hours;
		if (dhHours < 0)
		{
			int mdhHours = tmpMonth * 720 + tmpDays * 24 + tmpHours - hours;
			if (mdhHours < 0)
			{
				mdhHours = 365 * 24 + tmpDays * 24 + tmpHours - hours;
				tmpYears -= 1;
				tmpMonth = mdhHours / 720;
				tmpDays = (mdhHours % 720) / 24;
				tmpHours = (mdhHours % 720) % 24;
			}
			tmpMonth = mdhHours / 720;
			tmpDays = (mdhHours % 720) / 24;
			tmpHours = (mdhHours % 720) % 24;
		}
		else
		{
			tmpDays = dhHours / 24;
			tmpHours = dhHours % 24;
		}
	}
	else
	{
		tmpHours = hHours;
	}
	time = tmpYears * 1000000 + tmpMonth * 10000 + tmpDays * 100 + tmpHours;
}

void add_response(GPSVALUE_S* ptide_value, std::string& response)
{
	response.append(";");
	response.append(ptide_value->m_sTime);
	response.append(";");
	response.append(ptide_value->m_sLat);
	response.append(";");
	response.append(ptide_value->m_sLong);
	response.append(";");
	response.append(ptide_value->m_sRemain);
	response.append("|");
}

void add_json_response(GPSVALUE_S* ptide_value, std::string key, Json::Value& json_response)
{
	Json::Value gps_value;
	gps_value[REST_KEY] = key;
	gps_value[REST_TIME] = ptide_value->m_sTime;
	gps_value[REST_LATITUDE] = ptide_value->m_sLat;
	gps_value[REST_LONGTITUDE] = ptide_value->m_sLong;
	gps_value[REST_EXPAND] = ptide_value->m_sRemain;

	json_response[REST_RESULT].append(gps_value);
}

void add_json_response2(GPSDATA_S* ptide_value, Json::Value& json_response)
{
	Json::Value gps_value;
	gps_value[REST_KEY] = ptide_value->m_sKey;
	gps_value[REST_TIME] = ptide_value->m_sTime;
	gps_value[REST_LATITUDE] = ptide_value->m_sLat;
	gps_value[REST_LONGTITUDE] = ptide_value->m_sLong;
	gps_value[REST_EXPAND] = ptide_value->m_sRemain;

	json_response[REST_RESULT].append(gps_value);
}

void reduce_days(int& day_time, int days)
{
	if (day_time >= 100000000 || day_time < 10000000 || days > 1000)
	{
		return;
	}
	int tmpTime = day_time;
	int tmpDays = tmpTime % 100;
	tmpTime /= 100;
	int tmpMonth = tmpTime % 100;
	tmpTime /= 100;
	int tmpYears = tmpTime;
	
	int hDays = tmpDays - days;
	if (hDays < 0)
	{
		int dDays = tmpMonth * 30 + tmpDays - days;
		if (dDays < 0)
		{
			int dmDays = 3*365 + tmpMonth * 30 + tmpDays - days;
			tmpYears = (tmpYears -3) + dmDays/365;
			tmpMonth = (dmDays % 365) / 30;
			tmpDays = (dmDays % 365) % 30;
		}
		else
		{
			tmpMonth = dDays / 30;
			tmpDays = dDays % 30;
		}
	}
	else
	{
		tmpDays = hDays;
	}
	day_time = tmpYears * 10000 + tmpMonth * 100 + tmpDays ;
}

void reduce_days2(int& day_time,int days)
{
	if (day_time >= 100000000 || day_time < 10000000)
	{
		return;
	}
	int tmpTime = day_time;
	int tmpDays = tmpTime % 100;
	tmpTime /= 100;
	int tmpMonth = tmpTime % 100;
	tmpTime /= 100;
	int tmpYears = tmpTime;

	while(days)
	{
		tmpDays--;
		if(tmpDays==0)
		{
			tmpMonth--;
			if(tmpMonth==0)
			{
				tmpDays=31;
				tmpMonth=12;
				tmpYears--;
			}
			else
			{
				switch(tmpMonth)
				{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
					tmpDays=31; break;
				case 4:
				case 6:
				case 9:
				case 11:
					tmpDays=30; break;
				case 2:
					tmpDays=((tmpYears%400==0 || (tmpYears%4==0 && tmpYears%100!=0)) ? 29 : 28 );
					break;
				}
			}
		}
		days--;
	}
	day_time = tmpYears * 10000 + tmpMonth * 100 + tmpDays ;
}

int  get_local_time_days()
{
	time_t time_now;
	time(&time_now);
	struct tm* tm_now;    
	tm_now = localtime(&time_now);
	char i_time[11];
	memset(i_time,0,11);
	sprintf(i_time,"%04d%02d%02d",tm_now->tm_year+1900,tm_now->tm_mon+1,\
	tm_now->tm_mday);
	return  atoi(i_time);
}

void get_local_h_m(int& hour, int &min)
{
	time_t time_now;
	time(&time_now);
	struct tm* tm_now;
	tm_now = localtime(&time_now);

	hour = tm_now->tm_hour;
	min = tm_now->tm_min;
}
int GetTimeZone()
{
	 time_t time_utc;  
    struct tm tm_local;  
  
    // Get the UTC time  
    time(&time_utc);  
  
    // Get the local time  
    // Use localtime_r for threads safe  
    localtime_r(&time_utc, &tm_local);  
  
    struct tm tm_gmt;    
    // Change it to GMT tm  
    gmtime_r(&time_utc, &tm_gmt);  
  
    int time_zone = tm_local.tm_hour - tm_gmt.tm_hour;  
    if (time_zone < -12) {  
        time_zone += 24;   
    } else if (time_zone > 12) {  
        time_zone -= 24;  
    }
	return time_zone;
}
time_t GetTick(int iY,int iM,int iD,int iH,int iMin,int iS)
{
    struct tm stm;
    memset(&stm,0,sizeof(stm));

    stm.tm_year=iY-1900;
    stm.tm_mon=iM-1;
    stm.tm_mday=iD;
    stm.tm_hour=iH;
    stm.tm_min=iMin;
    stm.tm_sec=iS;

    return mktime(&stm);
}

bool IsInCircle(double longitude,double latitude,double* query_con)
{
	double around_distance = GetDistance(query_con[1], query_con[0],latitude, longitude);
	if (around_distance <= query_con[2])
	{
		return true;
	}
	return false;
}

bool IsInRect(double longitude,double latitude,double* query_con)
{
	if (((query_con[2]<latitude || IS_DOUBLE_ZERO(query_con[2]-latitude)) \
		&& (latitude < query_con[3] || IS_DOUBLE_ZERO(query_con[3]-latitude)))&&\
		((query_con[1]<longitude|| IS_DOUBLE_ZERO(query_con[1]-longitude)) \
		&& (longitude < query_con[0]|| IS_DOUBLE_ZERO(query_con[0]-longitude)) ))
	{
		return true;
	}
	return false;
}

bool IsInPolygon(double longitude,double latitude, std::vector<QUERY_CON_POLYGONAROUND>& query_con)
{
	double px = longitude;
	double	py = latitude;
	bool flag = false;
	int nCount = query_con.size();
	for(int i = 0, l = nCount, j = l - 1; i < l; j = i, i++) {
		double sx = query_con[i].longitude;
		double sy = query_con[i].latitude;
		double tx = query_con[j].longitude;
		double ty = query_con[j].latitude;

		// 点与多边形顶点重合
		if((sx == px && sy == py) || (tx == px && ty == py)) {
			return true;
		}

		// 判断线段两端点是否在射线两侧
		if((sy < py && ty >= py) || (sy >= py && ty < py)) {
			// 线段上与射线 Y 坐标相同的点的 X 坐标
			double x = sx + (py - sy) * (tx - sx) / (ty - sy);

			// 点在多边形的边上
			if(x == px) {
				return true;
			}

			// 射线穿过多边形的边界
			if(x > px) {
				flag = !flag;
			}
		}
	}

	// 射线穿过多边形边界的次数为奇数时点在多边形内
	return flag ? true : false;
}
char* get_exe_path( char * buf, int count)
{
	if( buf == NULL)
	{
		return NULL;
	}
    int i;
    int rslt = readlink("/proc/self/exe", buf, count - 1);
    if (rslt < 0 || (rslt >= count - 1))
    {
        return NULL;
    }
    buf[rslt] = '\0';
    for (i = rslt; i >= 0; i--)
    {
        if (buf[i] == '/')
        {
            buf[i + 1] = '\0';
            break;
        }
    }
    return buf;
}

