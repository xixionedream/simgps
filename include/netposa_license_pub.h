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
	/// 基本信息
//public:
	union
	{
	KeyFileType keyType;			///< key类型
	INT64 keyTypeI64_;
	};

	LTMSEL validTime;				///< 当前key的有效日期（此日期之前有效，INVALID_UTC_MSEL表示永远有效）
	LTMSEL toolKeyRequestedTime;	///< 工具授权key的申请时间
	LTMSEL toolKeyGrantedTime;		///< 工具授权key的授予时间

	BYTE machineID[NETPOSA_MACHINE_ID_LENGTH];///< 硬件标识（额外硬件标识见'additionalMIDs'）

	/// 申请者信息
//public:
	char applicant[128];			///< 姓名
	char corp[128];					///< 公司
	char department[128];			///< 部门
	char tel[16];					///< 手机号
	char email[128];				///< 电子邮箱
	char qq[16];					///< QQ号

	BYTE reserved_1k_128[1024+128];	///< 保留字段

//public:
	/// 额外硬件标识（主硬件标识或第一个硬件标识见'machineID'）
	struct AdditionalMachineIDs
	{
		INT32 machines;				///< 额外硬件标识数量，mids的前machines个元素有效
		BYTE reserved_PVA_HID[NETPOSA_MACHINE_ID_LENGTH - sizeof(INT32)];
		struct MID
		{
			BYTE id[NETPOSA_MACHINE_ID_LENGTH];
		};
		struct MID mids[(1024*3-NETPOSA_MACHINE_ID_LENGTH) / NETPOSA_MACHINE_ID_LENGTH];
	};
	struct AdditionalMachineIDs additionalMIDs;

	/// 项目、机器信息等
//public:
	char projectName[128];			///< 项目名称
	char projectRemark[1024];		///< 项目备注说明
	char contractNumber[128];		///< 合同编号
	char appRemark[256];			///< 所授权机器备注说明

	char appMacAddr[256];			///< 运行机器的MAC地址（十六进制字符格式，不同地址间使用逗号','分隔）
	char appIPAddr[256];			///< 运行机器的IP地址（不同IP间使用逗号','分隔）
	char appPathname[512];			///< 执行程序全路径
	LTMSEL appBuildTime;			///< 执行程序创建时间
	char appVersion[16];			///< 执行程序版本信息
	char appCompiler[16];			///< 执行程序编译器信息
	char appBinaryType[16];			///< 执行程序二进制类型
	char appOperatingSystem[128];	///< 操作系统信息
	INT64 appTotalMemoryBytes;		///< 物理内存大小
	char appCPUType[128];			///< CPU类型信息
	INT64 appCPUFreq;				///< CPU频率
	INT64 appCPUCount;				///< CPU核数（线程）

	BYTE reserved_2k_768[1024*2 + 256*2];///< 保留字段

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
	/// PCC云计算平台
	struct PCCAuthorizations
	{
		INT32 calculationNodes;		///< 计算节点数量
		INT32 cpuCount;				///< CPU核数
		INT32 storageNodes;			///< 能接入的存储节点数量
		BYTE reserved_PCC_A[128 - sizeof(INT32)*3];
	};
	struct PCCAuthorizations pccAuthorizations;
	struct PCCLimiteds
	{
		INT32 calculationNodesLimited;	///< 最大可授权的计算节点数量（0表示不限）
		INT32 cpuCountLimited;			///< 最大可授权的CPU核数（0表示不限）
		INT32 storageNodesLimited;		///< 最大可授权的能接入的存储节点数量（0表示不限）
		BYTE reserved_PCC_L[128 - sizeof(INT32)*3];
	};
	struct PCCLimiteds pccLimiteds;

//public:
	/// PVA实战应用平台
	struct PVAAuthorizations
	{
		INT32 videoDirector;		///< BOOL, 视频指挥
		INT32 deploymentControl;	///< BOOL, 布防布控
		INT32 viewLibrary;			///< BOOL, 视图库
		INT32 imageJudgement;		///< BOOL, 图像研判
		INT32 maintenanceManagement;///< BOOL, 运维管理
		INT32 trafficManagement;	///< BOOL, 交通管理
		BYTE reserved_PVA_A[128 - sizeof(INT32)*6];
	};
	struct PVAAuthorizations pvaAuthorizations;
	struct PVALimiteds
	{
		INT32 videoDirectorLimited;		///< BOOL, 是否限制授权视频指挥（0表示不限，非0表示禁止授权此项）
		INT32 deploymentControlLimited;	///< BOOL, 是否限制授权布防布控（0表示不限，非0表示禁止授权此项）
		INT32 viewLibraryLimited;		///< BOOL, 是否限制授权视图库（0表示不限，非0表示禁止授权此项）
		INT32 imageJudgementLimited;	///< BOOL, 是否限制授权图像研判（0表示不限，非0表示禁止授权此项）
		INT32 maintenanceManagementLimited;///< BOOL, 是否限制授权运维管理（0表示不限，非0表示禁止授权此项）
		INT32 trafficManagementLimited;	///< BOOL, 是否限制授权交通管理（0表示不限，非0表示禁止授权此项）
		BYTE reserved_PVA_L[128 - sizeof(INT32)*6];
	};
	struct PVALimiteds pvaLimiteds;

	/// GIS授权
//public:
	struct GISAuthorizations
	{
		INT32 requestSDK;				///< SDK请求	（0 不可用， >0 日访问总量， -1 无限制）
		INT32 pathPlanningService;		///< 路径规划服务（0 不可用， >0 日访问总量， -1 无限制）
		INT32 pointsOfInterestService;	///< 兴趣点服务	（0 不可用， >0 日访问总量， -1 无限制）
		INT32 roadService;				///< 道路服务	（0 不可用， >0 日访问总量， -1 无限制）
		INT32 intersectionService;		///< 路口服务	（0 不可用， >0 日访问总量， -1 无限制）
		BYTE reserved_GIS_A[128 - sizeof(INT32)*5];
	};
	struct GISAuthorizations gisAuthorizations;
	struct GISLimiteds
	{
		INT32 requestSDKLimited;				///< 最大可授权的'SDK请求日访问量'（0表示不限）
		INT32 pathPlanningServiceLimited;		///< 最大可授权的'路径规划服务日访问量'（0表示不限）
		INT32 pointsOfInterestServiceLimited;	///< 最大可授权的'兴趣点服务日访问量'（0表示不限）
		INT32 roadServiceLimited;				///< 最大可授权的'道路服务日访问量'（0表示不限）
		INT32 intersectionServiceLimited;		///< 最大可授权的'路口服务日访问量'（0表示不限）
		BYTE reserved_GIS_L[128 - sizeof(INT32)*5];
	};
	struct GISLimiteds gisLimiteds;

	/// PVG扩展授权
//public:
	struct PVGAuthorizationsEx
	{
		INT32 camerasConfig;		///< 允许配置最多摄像机数（0表示不限）
		INT32 realStreamMBPS;		///< 允许的实时流总码流限额（0表示不限）
		INT32 vodMBPS;				///< 允许的点播总码流限额（0表示不限）
		INT32 storageMBPS;			///< 允许的存储总码流限额（0表示不限）
		INT32 clusterEnabled;		///< BOOL，是否允许集群
		INT32 hotStandbyEnabled;	///< BOOL，是否允许热备
		INT32 chatEnabled;			///< BOOL，是否允许对讲
		INT32 newPlatformEnabled;	///< BOOL，是否允许新增平台接入
		INT32 newDeviceEnabled;		///< BOOL，是否允许新增设备接入
		INT32 gisEnabled;			///< BOOL，是否允许电子地图
		BYTE reserved_PVG_A_EX[256 - sizeof(INT32)*10];
	};
	struct PVGAuthorizationsEx pvgAuthorizationsEx;
	struct PVGLimitedsEx
	{
		INT32 camerasConfigLimited;		///< 允许授权的最多配置摄像机数（0表示不限）
		INT32 realStreamMBPSLimited;	///< 允许授权的最大实时流总码流限额（0表示不限）
		INT32 vodMBPSLimited;			///< 允许授权的最大点播总码流限额（0表示不限）
		INT32 storageMBPSLimited;		///< 允许授权的最大存储总码流限额（0表示不限）
		INT32 clusterEnabledLimited;	///< BOOL，是否允许集群（0表示不限，非0表示禁止授权此项）
		INT32 hotStandbyEnabledLimited;	///< BOOL，是否允许热备（0表示不限，非0表示禁止授权此项）
		INT32 chatEnabledLimited;		///< BOOL，是否允许对讲（0表示不限，非0表示禁止授权此项）
		INT32 newPlatformEnabledLimited;///< BOOL，是否允许新增平台接入（0表示不限，非0表示禁止授权此项）
		INT32 newDeviceEnabledLimited;	///< BOOL，是否允许新增设备接入（0表示不限，非0表示禁止授权此项）
		INT32 gisEnabledLimited;		///< BOOL，是否允许电子地图（0表示不限，非0表示禁止授权此项）
		BYTE reserved_PVG_L_EX[256 - sizeof(INT32)*10];
	};
	struct PVGLimitedsEx pvgLimitedsEx;

	LTMSEL keyRequestedTime;		///< 授权key的申请时间
	LTMSEL keyGrantedTime;			///< 授权key的授予时间

	struct PVGAuthorizations
	{
		INT32 realStreams;			///< 直播路数
		INT32 vodChannels;			///< 点播路数
		INT32 storageChannels;		///< 存储路数

		INT32 matrixKeyboard;		///< BOOL，键盘矩阵
		INT32 frameTag;				///< BOOL，帧标记
		INT32 alarmHost;			///< BOOL，报警主机
	};
	struct PVGAuthorizations pvgAuthorizations;
	INT32 limitedDaysLimited;		///< 最多可授权的天数（0表示不限）

	struct PVGLimiteds
	{
		INT32 realStreamsLimited;	///< 最大直播路数（0表示不限）
		INT32 vodChannelsLimited;	///< 最大点播路数（0表示不限）
		INT32 storageChannelsLimited;///< 最大存储路数（0表示不限）

		INT32 matrixKeyboardLimited;///< BOOL，是否限制使用键盘矩阵（0表示不限，非0表示禁止授权此项）
		INT32 frameTagLimited;		///< BOOL，是否限制使用帧标记（0表示不限，非0表示禁止授权此项）
		INT32 alarmHostLimited;		///< BOOL，是否限制使用报警主机（0表示不限，非0表示禁止授权此项）
	};
	struct PVGLimiteds pvgLimiteds;

	INT32 grantCountLimited;		///< 最多可授权的机器数（0表示不限）
	LTMSEL grantTimeLimited;		///< 授权工具的使用时间限制（INVALID_UTC_MSEL表示不限）

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
