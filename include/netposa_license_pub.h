#ifndef _netposa_license_pub_h_
#define _netposa_license_pub_h_

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include "nptypes.h"

#define KEY_FILE_SIZE	(1024*32)

#define NETPOSA_MACHINE_ID_LENGTH		16

enum KeyFileType_
{
	kft_invalid = -1,
	kft_licenserB_key0 = 0,
	kft_licenserB_key1,
	kft_pvg_key0,
	kft_pvg_key1,
	kft_pva_key0,
	kft_pva_key1,
	kft_pcc_key0,
	kft_pcc_key1,
	kft_gis_key0,
	kft_gis_key1,
	kft_pvd_key0,
	kft_pvd_key1,
};
typedef enum KeyFileType_ KeyFileType;

struct KeyFileContents_;
typedef struct KeyFileContents_ KeyFileContents;

void KeyFileContents_Init(KeyFileContents* contents);

struct KeyFileContents_
{
	/// ������Ϣ
//public:
	union
	{
	KeyFileType keyType;			///< key����
	INT64 keyTypeI64_;
	};

	LTMSEL validTime;				///< ��ǰkey����Ч���ڣ�������֮ǰ��Ч��INVALID_UTC_MSEL��ʾ��Զ��Ч��
	LTMSEL toolKeyRequestedTime;	///< ������Ȩkey������ʱ��
	LTMSEL toolKeyGrantedTime;		///< ������Ȩkey������ʱ��

	BYTE machineID[NETPOSA_MACHINE_ID_LENGTH];///< Ӳ����ʶ������Ӳ����ʶ��'additionalMIDs'��

	/// ��������Ϣ
//public:
	char applicant[128];			///< ����
	char corp[128];					///< ��˾
	char department[128];			///< ����
	char tel[16];					///< �ֻ���
	char email[128];				///< ��������
	char qq[16];					///< QQ��

	BYTE reserved_1k_128[1024+128];	///< �����ֶ�

//public:
	/// ����Ӳ����ʶ����Ӳ����ʶ���һ��Ӳ����ʶ��'machineID'��
	struct AdditionalMachineIDs
	{
		INT32 machines;				///< ����Ӳ����ʶ������mids��ǰmachines��Ԫ����Ч
		BYTE reserved_PVA_HID[NETPOSA_MACHINE_ID_LENGTH - sizeof(INT32)];
		struct MID
		{
			BYTE id[NETPOSA_MACHINE_ID_LENGTH];
		};
		struct MID mids[(1024*3-NETPOSA_MACHINE_ID_LENGTH) / NETPOSA_MACHINE_ID_LENGTH];
	};
	struct AdditionalMachineIDs additionalMIDs;

	/// ��Ŀ��������Ϣ��
//public:
	char projectName[128];			///< ��Ŀ����
	char projectRemark[1024];		///< ��Ŀ��ע˵��
	char contractNumber[128];		///< ��ͬ���
	char appRemark[256];			///< ����Ȩ������ע˵��

	char appMacAddr[256];			///< ���л�����MAC��ַ��ʮ�������ַ���ʽ����ͬ��ַ��ʹ�ö���','�ָ���
	char appIPAddr[256];			///< ���л�����IP��ַ����ͬIP��ʹ�ö���','�ָ���
	char appPathname[512];			///< ִ�г���ȫ·��
	LTMSEL appBuildTime;			///< ִ�г��򴴽�ʱ��
	char appVersion[16];			///< ִ�г���汾��Ϣ
	char appCompiler[16];			///< ִ�г����������Ϣ
	char appBinaryType[16];			///< ִ�г������������
	char appOperatingSystem[128];	///< ����ϵͳ��Ϣ
	INT64 appTotalMemoryBytes;		///< �����ڴ��С
	char appCPUType[128];			///< CPU������Ϣ
	INT64 appCPUFreq;				///< CPUƵ��
	INT64 appCPUCount;				///< CPU�������̣߳�

	BYTE reserved_2k_768[1024*2 + 256*2];///< �����ֶ�

//public:
	/// PVD
	struct PVDAuthorizations
	{
		BYTE reserved_PVD_A[128];
	};
	struct PVDAuthorizations pvdAuthorizations;
	struct PVDLimiteds
	{
		BYTE reserved_PVD_L[128];
	};
	struct PVDLimiteds pvdLimiteds;

//public:
	/// PCC�Ƽ���ƽ̨
	struct PCCAuthorizations
	{
		INT32 calculationNodes;		///< ����ڵ�����
		INT32 cpuCount;				///< CPU����
		INT32 storageNodes;			///< �ܽ���Ĵ洢�ڵ�����
		BYTE reserved_PCC_A[128 - sizeof(INT32)*3];
	};
	struct PCCAuthorizations pccAuthorizations;
	struct PCCLimiteds
	{
		INT32 calculationNodesLimited;	///< ������Ȩ�ļ���ڵ�������0��ʾ���ޣ�
		INT32 cpuCountLimited;			///< ������Ȩ��CPU������0��ʾ���ޣ�
		INT32 storageNodesLimited;		///< ������Ȩ���ܽ���Ĵ洢�ڵ�������0��ʾ���ޣ�
		BYTE reserved_PCC_L[128 - sizeof(INT32)*3];
	};
	struct PCCLimiteds pccLimiteds;

//public:
	/// PVAʵսӦ��ƽ̨
	struct PVAAuthorizations
	{
		INT32 videoDirector;		///< BOOL, ��Ƶָ��
		INT32 deploymentControl;	///< BOOL, ��������
		INT32 viewLibrary;			///< BOOL, ��ͼ��
		INT32 imageJudgement;		///< BOOL, ͼ������
		INT32 maintenanceManagement;///< BOOL, ��ά����
		INT32 trafficManagement;	///< BOOL, ��ͨ����
		BYTE reserved_PVA_A[128 - sizeof(INT32)*6];
	};
	struct PVAAuthorizations pvaAuthorizations;
	struct PVALimiteds
	{
		INT32 videoDirectorLimited;		///< BOOL, �Ƿ�������Ȩ��Ƶָ�ӣ�0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 deploymentControlLimited;	///< BOOL, �Ƿ�������Ȩ�������أ�0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 viewLibraryLimited;		///< BOOL, �Ƿ�������Ȩ��ͼ�⣨0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 imageJudgementLimited;	///< BOOL, �Ƿ�������Ȩͼ�����У�0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 maintenanceManagementLimited;///< BOOL, �Ƿ�������Ȩ��ά����0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 trafficManagementLimited;	///< BOOL, �Ƿ�������Ȩ��ͨ����0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		BYTE reserved_PVA_L[128 - sizeof(INT32)*6];
	};
	struct PVALimiteds pvaLimiteds;

	/// GIS��Ȩ
//public:
	struct GISAuthorizations
	{
		INT32 requestSDK;				///< SDK����	��0 �����ã� >0 �շ��������� -1 �����ƣ�
		INT32 pathPlanningService;		///< ·���滮����0 �����ã� >0 �շ��������� -1 �����ƣ�
		INT32 pointsOfInterestService;	///< ��Ȥ�����	��0 �����ã� >0 �շ��������� -1 �����ƣ�
		INT32 roadService;				///< ��·����	��0 �����ã� >0 �շ��������� -1 �����ƣ�
		INT32 intersectionService;		///< ·�ڷ���	��0 �����ã� >0 �շ��������� -1 �����ƣ�
		BYTE reserved_GIS_A[128 - sizeof(INT32)*5];
	};
	struct GISAuthorizations gisAuthorizations;
	struct GISLimiteds
	{
		INT32 requestSDKLimited;				///< ������Ȩ��'SDK�����շ�����'��0��ʾ���ޣ�
		INT32 pathPlanningServiceLimited;		///< ������Ȩ��'·���滮�����շ�����'��0��ʾ���ޣ�
		INT32 pointsOfInterestServiceLimited;	///< ������Ȩ��'��Ȥ������շ�����'��0��ʾ���ޣ�
		INT32 roadServiceLimited;				///< ������Ȩ��'��·�����շ�����'��0��ʾ���ޣ�
		INT32 intersectionServiceLimited;		///< ������Ȩ��'·�ڷ����շ�����'��0��ʾ���ޣ�
		BYTE reserved_GIS_L[128 - sizeof(INT32)*5];
	};
	struct GISLimiteds gisLimiteds;

	/// PVG��չ��Ȩ
//public:
	struct PVGAuthorizationsEx
	{
		INT32 camerasConfig;		///< ��������������������0��ʾ���ޣ�
		INT32 realStreamMBPS;		///< �����ʵʱ���������޶0��ʾ���ޣ�
		INT32 vodMBPS;				///< ����ĵ㲥�������޶0��ʾ���ޣ�
		INT32 storageMBPS;			///< ����Ĵ洢�������޶0��ʾ���ޣ�
		INT32 clusterEnabled;		///< BOOL���Ƿ�����Ⱥ
		INT32 hotStandbyEnabled;	///< BOOL���Ƿ������ȱ�
		INT32 chatEnabled;			///< BOOL���Ƿ�����Խ�
		INT32 newPlatformEnabled;	///< BOOL���Ƿ���������ƽ̨����
		INT32 newDeviceEnabled;		///< BOOL���Ƿ����������豸����
		INT32 gisEnabled;			///< BOOL���Ƿ�������ӵ�ͼ
		BYTE reserved_PVG_A_EX[256 - sizeof(INT32)*10];
	};
	struct PVGAuthorizationsEx pvgAuthorizationsEx;
	struct PVGLimitedsEx
	{
		INT32 camerasConfigLimited;		///< ������Ȩ������������������0��ʾ���ޣ�
		INT32 realStreamMBPSLimited;	///< ������Ȩ�����ʵʱ���������޶0��ʾ���ޣ�
		INT32 vodMBPSLimited;			///< ������Ȩ�����㲥�������޶0��ʾ���ޣ�
		INT32 storageMBPSLimited;		///< ������Ȩ�����洢�������޶0��ʾ���ޣ�
		INT32 clusterEnabledLimited;	///< BOOL���Ƿ�����Ⱥ��0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 hotStandbyEnabledLimited;	///< BOOL���Ƿ������ȱ���0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 chatEnabledLimited;		///< BOOL���Ƿ�����Խ���0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 newPlatformEnabledLimited;///< BOOL���Ƿ���������ƽ̨���루0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 newDeviceEnabledLimited;	///< BOOL���Ƿ����������豸���루0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 gisEnabledLimited;		///< BOOL���Ƿ�������ӵ�ͼ��0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		BYTE reserved_PVG_L_EX[256 - sizeof(INT32)*10];
	};
	struct PVGLimitedsEx pvgLimitedsEx;

	LTMSEL keyRequestedTime;		///< ��Ȩkey������ʱ��
	LTMSEL keyGrantedTime;			///< ��Ȩkey������ʱ��

	struct PVGAuthorizations
	{
		INT32 realStreams;			///< ֱ��·��
		INT32 vodChannels;			///< �㲥·��
		INT32 storageChannels;		///< �洢·��

		INT32 matrixKeyboard;		///< BOOL�����̾���
		INT32 frameTag;				///< BOOL��֡���
		INT32 alarmHost;			///< BOOL����������
	};
	struct PVGAuthorizations pvgAuthorizations;
	INT32 limitedDaysLimited;		///< ������Ȩ��������0��ʾ���ޣ�

	struct PVGLimiteds
	{
		INT32 realStreamsLimited;	///< ���ֱ��·����0��ʾ���ޣ�
		INT32 vodChannelsLimited;	///< ���㲥·����0��ʾ���ޣ�
		INT32 storageChannelsLimited;///< ���洢·����0��ʾ���ޣ�

		INT32 matrixKeyboardLimited;///< BOOL���Ƿ�����ʹ�ü��̾���0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 frameTagLimited;		///< BOOL���Ƿ�����ʹ��֡��ǣ�0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
		INT32 alarmHostLimited;		///< BOOL���Ƿ�����ʹ�ñ���������0��ʾ���ޣ���0��ʾ��ֹ��Ȩ���
	};
	struct PVGLimiteds pvgLimiteds;

	INT32 grantCountLimited;		///< ������Ȩ�Ļ�������0��ʾ���ޣ�
	LTMSEL grantTimeLimited;		///< ��Ȩ���ߵ�ʹ��ʱ�����ƣ�INVALID_UTC_MSEL��ʾ���ޣ�

//public:
#ifdef __cplusplus
	void Init()
		{	KeyFileContents_Init(this);	}
#endif
};

inline void KeyFileContents_Init(KeyFileContents* contents)
{
	int i;
	for(i=0; i<(int)sizeof(*contents); ++i)
		((BYTE*)contents)[i] = 0;

	contents->keyType = kft_invalid;
	contents->validTime = INVALID_UTC_MSEL;
	contents->toolKeyRequestedTime = INVALID_UTC_MSEL;
	contents->toolKeyGrantedTime = INVALID_UTC_MSEL;
	contents->appBuildTime = INVALID_UTC_MSEL;
	contents->keyRequestedTime = INVALID_UTC_MSEL;
	contents->keyGrantedTime = INVALID_UTC_MSEL;

	contents->grantTimeLimited = INVALID_UTC_MSEL;
}

#ifdef __cplusplus
extern "C"
#endif
const char* GetLicenserInsidePassword(BOOL licenserOrAPP);

#endif	// #ifndef _netposa_license_pub_h_
