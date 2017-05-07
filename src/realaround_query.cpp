
#include "realaround_query.h"
#include <iostream>

#include "gdal.h"
#include "ogrsf_frmts.h"
using namespace std;

//rest
CRestQryRealAround::CRestQryRealAround()
{

}

CRestQryRealAround::~CRestQryRealAround()
{

}

void CRestQryRealAround::init(unordered_map<string, vector<GPSVALUE_S> >** ppnum_tide, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data1, unordered_map<string, vector<GPSVALUE_S> >* pnum_tide_data2, PQUERY_REAL_AROUND_CON pquery_con,PQUERY_REAL_RECTAROUND_CON pqueryrect_con,PQUERY_REAL_RECTAROUND_TWOPOINT_CON pqueryrect_twopoint_con)
{
	this->m_ppmem_tide = ppnum_tide;
	this->m_pmem_tide_data1 = pnum_tide_data1;
	this->m_pmem_tide_data2 = pnum_tide_data2;
	this->m_pquery_con = pquery_con;
	this->m_pqueryrect_con = pqueryrect_con;
	this->m_pqueryrect_twopoint_con = pqueryrect_twopoint_con;
}

void CRestQryRealAround::query_real_around_wktpolygon(Json::Value& json_response,std::string& polygon_con)
{

	unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
	unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	unordered_map<string, int> unique_keymap;
	int query_res_size = 0;	
	GPSVALUE_S* pgps_value;
	
	if ((*m_ppmem_tide) == m_pmem_tide_data1)
	{
		slave_mem = m_pmem_tide_data2;
	}		
	else
	{
		slave_mem = m_pmem_tide_data1;
	}


	Json::Reader json_reader;
	Json::Value data_root;
	if (!json_reader.parse(polygon_con.c_str(), data_root))
	{
		//response
		json_response[REST_CODE] = RESPONSE_ARRAY[1].code;
		json_response[REST_MSG] = RESPONSE_ARRAY[1].description;
		return;
	}

	if (data_root["querycon"].empty()) 
	{
		json_response["return_code"] = -1;
		json_response["return_msg"] = "querycon is NULL";
		return;
	}
	std::string querycon = data_root["querycon"].asString();
	char* p =(char*) querycon.c_str();
	OGRPolygon Polygon;
	int ret = Polygon.importFromWkt(&p);
	if(ret !=0)
	{
		json_response[REST_CODE] = REST_PARAMREEOR;
		json_response[REST_MSG] = "the input polygon is wrong.";
		return;
	}
	Polygon.closeRings();
	
	for (itor_tide_data = (*m_ppmem_tide)->begin(); itor_tide_data != (*m_ppmem_tide)->end(); ++itor_tide_data)
	{
		if (unique_keymap.find(itor_tide_data->first) != unique_keymap.end())
		{
			continue;
		}
		unique_keymap.insert(make_pair(itor_tide_data->first, 0x1));
		pgps_value = &(itor_tide_data->second.back());
		OGRPoint Point(atof(pgps_value->m_sLat),atof(pgps_value->m_sLong));
		OGRGeometry* PGeometry=Point.clone();

        OGRGeometry* Geometry=Polygon.getCurveGeometry();
        if(Geometry->IsValid() ==0)
        {
            json_response[REST_CODE] = REST_PARAMREEOR;
			json_response[REST_MSG] = "the input polygon is wrong.";
            return;
        }
		
		if ( 1==Geometry->Contains(PGeometry))
		{
			if (query_res_size > MAX_QUERY_RESULTS)
			{
				json_response[REST_CODE] = REST_OK;
				json_response[REST_MSG] = REST_SUCCESS;
				return;
			}
			query_res_size++;
			add_json_response(pgps_value, itor_tide_data->first, json_response);
		}
	}
	for (itor_tide_data = (slave_mem)->begin(); itor_tide_data != (slave_mem)->end(); ++itor_tide_data)
	{
		if (unique_keymap.find(itor_tide_data->first) != unique_keymap.end())
		{
			continue;
		}
		unique_keymap.insert(make_pair(itor_tide_data->first, 0x1));
		pgps_value = &(itor_tide_data->second.back());
		OGRPoint Point(atof(pgps_value->m_sLat),atof(pgps_value->m_sLong));
		OGRGeometry* PGeometry=Point.clone();

        OGRGeometry* Geometry=Polygon.getCurveGeometry();
        if(Geometry->IsValid() ==0)
        {
            json_response[REST_CODE] = REST_PARAMREEOR;
			json_response[REST_MSG] = "the input polygon is wrong.";
            return;
        }
		
		if ( 1==Geometry->Contains(PGeometry))
		{
			if (query_res_size > MAX_QUERY_RESULTS)
			{
				json_response[REST_CODE] = REST_OK;
				json_response[REST_MSG] = REST_SUCCESS;
				return;
			}
			query_res_size++;
			add_json_response(pgps_value, itor_tide_data->first, json_response);
		}
	}

	json_response[REST_CODE] = REST_OK;
	json_response[REST_MSG] = REST_SUCCESS;

}

void CRestQryRealAround::query_real_around_polygon(Json::Value& json_response,PQUERY_REAL_POLYGONAROUND_CON polygon_con)
{	
	try
	{
		unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
		unordered_map<string, int> unique_keymap;	
		int query_res_size = 0;
		if ((*m_ppmem_tide) == m_pmem_tide_data1)
		{
			slave_mem = m_pmem_tide_data2;
		}		
		else
		{
			slave_mem = m_pmem_tide_data1;
		}
		if(query_mem(QRY_AROUND_POLYGON,polygon_con,*m_ppmem_tide,unique_keymap,query_res_size,json_response) ==0)
			return;
		if(query_mem(QRY_AROUND_POLYGON,polygon_con,slave_mem,unique_keymap,query_res_size,json_response) == 0)
			return;

		json_response[REST_CODE] = REST_OK;
		json_response[REST_MSG] = REST_SUCCESS;
	}
	catch (std::exception& ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsid error:%s\n", ex.what());
	}
}
bool CRestQryRealAround::judge_inrange(double longitude,double latitude,judge_con* con,int type)
{
	bool ret = false;
	switch(type)
	{
		case QRY_AROUND_CIRCLE://circle
			{
				ret = IsInCircle(longitude,latitude,(con->query_con_circle));
			}
			break;
		case QRY_AROUND_RECTANGLE://rect of len
		case QRY_AROUND_RECTANGLE_TWOPOINT://rect of two points
			{
				ret = IsInRect(longitude,latitude,(con->query_con_rect));
			}
			break;
		case QRY_AROUND_POLYGON://polygon
			{
				ret = IsInPolygon(longitude,latitude,*(con->query_con_polygon));
			}
			break;
		default:
			break;
	}

	return ret;
}

int CRestQryRealAround::query_mem(int type,void* pquery_con,unordered_map<string, vector<GPSVALUE_S> >* ppmem_tide,unordered_map<string, int>& unique_keymap,int& query_res_size,Json::Value& json_response)
{
	unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	GPSVALUE_S* pgps_value;
	judge_con con;
	switch(type)
	{
		case QRY_AROUND_CIRCLE://circle
			{
				PQUERY_REAL_AROUND_CON p = (PQUERY_REAL_AROUND_CON)pquery_con;				
				con.query_con_circle[0]=p->longitude;
				con.query_con_circle[1]=p->latitude;
				con.query_con_circle[2]=p->redius;
			}
			break;
		case QRY_AROUND_RECTANGLE://rect of len
			{
				PQUERY_REAL_RECTAROUND_CON p = (PQUERY_REAL_RECTAROUND_CON)pquery_con;				
				GetRectangleEdge(p->longitude,p->latitude,p->ew_len,p->sn_len,con.query_con_rect[0],con.query_con_rect[1],con.query_con_rect[2],con.query_con_rect[3]);		
			}
			break;
		case QRY_AROUND_RECTANGLE_TWOPOINT://rect of two points
			{
				PQUERY_REAL_RECTAROUND_TWOPOINT_CON p = (PQUERY_REAL_RECTAROUND_TWOPOINT_CON)pquery_con;	
				con.query_con_rect[0] = p->firstlongitude > p->secondlongitude ? p->firstlongitude:p->secondlongitude;
				con.query_con_rect[1] = p->firstlongitude + p->secondlongitude - con.query_con_rect[0];
				con.query_con_rect[3] = p->firstlatitude > p->secondlatitude ? p->firstlatitude:p->secondlatitude;
				con.query_con_rect[2] = p->firstlatitude + p->secondlatitude - con.query_con_rect[3];
			}
			break;
		case QRY_AROUND_POLYGON://polygon
			{
				PQUERY_REAL_POLYGONAROUND_CON p = (PQUERY_REAL_POLYGONAROUND_CON)pquery_con;
				con.query_con_polygon = &(p->point_array);
			}
			break;
		default:
			return 1;
	}
	
	for (itor_tide_data = (ppmem_tide)->begin(); itor_tide_data != (ppmem_tide)->end(); ++itor_tide_data)
	{
		if (unique_keymap.find(itor_tide_data->first) != unique_keymap.end())
		{
			continue;
		}
		unique_keymap.insert(make_pair(itor_tide_data->first, 0x1));
		pgps_value = &(itor_tide_data->second.back());
		if (judge_inrange( atof(pgps_value->m_sLong),atof(pgps_value->m_sLat),&con,type))
			{
				if (query_res_size > MAX_QUERY_RESULTS)
				{
					json_response[REST_CODE] = REST_OK;
					json_response[REST_MSG] = REST_SUCCESS;
					return 0;
				}
				query_res_size++;
				add_json_response(pgps_value, itor_tide_data->first, json_response);
			}
	}		
	return 1;
}

void CRestQryRealAround::query_real_around(Json::Value& json_response,int range_shape)
{
	
	try
	{	
		unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
		unordered_map<string, int> unique_keymap;
		int query_res_size = 0;
		void* pquery_con;
		if(range_shape == QRY_AROUND_CIRCLE)
		{
			pquery_con =(void*)m_pquery_con;
		}
		else if(range_shape == QRY_AROUND_RECTANGLE)
		{
			pquery_con = (void*)m_pqueryrect_con;
		}
		else if(range_shape == QRY_AROUND_RECTANGLE_TWOPOINT)
		{
			pquery_con =(void*)m_pqueryrect_twopoint_con;
		}
		else
			return;

		if ((*m_ppmem_tide) == m_pmem_tide_data1)
		{
			slave_mem = m_pmem_tide_data2;
		}		
		else
		{
			slave_mem = m_pmem_tide_data1;
		}
		
		if(query_mem(range_shape,pquery_con,*m_ppmem_tide,unique_keymap,query_res_size,json_response) ==0)
			return;
		if(query_mem(range_shape,pquery_con,slave_mem,unique_keymap,query_res_size,json_response) == 0)
			return;
		
		json_response[REST_CODE] = REST_OK;
		json_response[REST_MSG] = REST_SUCCESS;
	}
	catch (std::exception& ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsid error:%s\n", ex.what());
	}
}
void CRestQryRealAround::query_real_around_bykey_inrectangle(Json::Value& json_response)
{
	unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
	unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	unordered_map<string, int> unique_keymap;
		
	try
	{
		GPSVALUE_S* pgps_value;
		int query_res_size = 0;
		if ((*m_ppmem_tide) == m_pmem_tide_data1)
		{
			slave_mem = m_pmem_tide_data2;
		}		
		else
		{
			slave_mem = m_pmem_tide_data1;
		}
		
		if ((itor_tide_data =(*m_ppmem_tide)->find(this->m_pqueryrect_con->key)) != (*m_ppmem_tide)->end())
		{
			pgps_value = &(itor_tide_data->second.back());
			m_pqueryrect_con->latitude = atof(pgps_value->m_sLat);
			m_pqueryrect_con->longitude = atof(pgps_value->m_sLong);
		}
		else if((itor_tide_data = slave_mem->find(this->m_pqueryrect_con->key)) != slave_mem->end())
		{
			pgps_value = &(itor_tide_data->second.back());
			m_pqueryrect_con->latitude  = atof(pgps_value->m_sLat);
			m_pqueryrect_con->longitude = atof(pgps_value->m_sLong);
		}
		else
		{
			json_response[REST_CODE] = REST_OK;
			json_response[REST_MSG] = REST_SUCCESS;
			return;
		}

		if(query_mem(QRY_AROUND_RECTANGLE,m_pqueryrect_con,*m_ppmem_tide,unique_keymap,query_res_size,json_response) ==0)
			return;
		if(query_mem(QRY_AROUND_RECTANGLE,m_pqueryrect_con,slave_mem,unique_keymap,query_res_size,json_response) == 0)
			return;

		json_response[REST_CODE] = REST_OK;
		json_response[REST_MSG] = REST_SUCCESS;
	}
	catch (std::exception& ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsid error:%s\n", ex.what());
	}
}

void CRestQryRealAround::query_real_around_bykey_incircular(Json::Value& json_response)
{
	unordered_map<string, vector<GPSVALUE_S> >* slave_mem;
	unordered_map<string, vector<GPSVALUE_S> >::iterator itor_tide_data;
	unordered_map<string, int> unique_keymap;
	
	try
	{
		GPSVALUE_S* pgps_value;
		int query_res_size = 0;
		if ((*m_ppmem_tide) == m_pmem_tide_data1)
		{
			slave_mem = m_pmem_tide_data2;
		}		
		else
		{
			slave_mem = m_pmem_tide_data1;
		}
		
		if ((itor_tide_data =(*m_ppmem_tide)->find(this->m_pquery_con->key)) != (*m_ppmem_tide)->end())
		{
			pgps_value = &(itor_tide_data->second.back());
			m_pquery_con->latitude = atof(pgps_value->m_sLat);
			m_pquery_con->longitude = atof(pgps_value->m_sLong);
		}
		else if((itor_tide_data = slave_mem->find(this->m_pquery_con->key)) != slave_mem->end())
		{
			pgps_value = &(itor_tide_data->second.back());
			m_pquery_con->latitude = atof(pgps_value->m_sLat);
			m_pquery_con->longitude = atof(pgps_value->m_sLong);
		}
		else
		{
			json_response[REST_CODE] = REST_OK;
			json_response[REST_MSG] = REST_SUCCESS;
			return;
		}

		if(query_mem(QRY_AROUND_CIRCLE,m_pquery_con,*m_ppmem_tide,unique_keymap,query_res_size,json_response) ==0)
			return;
		if(query_mem(QRY_AROUND_CIRCLE,m_pquery_con,slave_mem,unique_keymap,query_res_size,json_response) == 0)
			return;

		json_response[REST_CODE] = REST_OK;
		json_response[REST_MSG] = REST_SUCCESS;
	}
	catch (std::exception& ex)
	{
		json_response[REST_CODE] = REST_UNEXPECTED;
		json_response[REST_MSG] = REST_EXCEPTION;
		NP_LOG(np_log::NP_ERROR, "query gpsid error:%s\n", ex.what());
	}
}

void CRestQryRealAround::query_real_around_bykey(Json::Value& json_response,int type)
{

	switch(type)
    {
        case QRY_AROUND_CIRCLE:
			query_real_around_bykey_incircular(json_response);
            break;
		case QRY_AROUND_RECTANGLE:
			query_real_around_bykey_inrectangle(json_response);
            break;
        default:
			{
				json_response[REST_CODE] = REST_OK;
				json_response[REST_MSG] = REST_SUCCESS;
           		break;
        	}
    }
}

