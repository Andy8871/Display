#ifndef _SECURITYBASECLASS_H_
#define _SECURITYBASECLASS_H_

#ifndef MAKEWORD
#define MAKEWORD(a, b) ((unsigned short)(((BYTE)(a)) | ((unsigned short)((BYTE)(b))) << 8))
#endif

#define CHECK_SUCCESS				MAKEWORD(0x00,0x00)  //校验成功
#define CHECK_LANGUAGE_ERROR		MAKEWORD(0x01,0x00)  //语言校验失败
#define CHECK_SERIALNUMBER_ERROR	MAKEWORD(0x02,0x00)  //序列号校验失败
#define CHECK_VECHICLECODE_ERROR	MAKEWORD(0x04,0x00)  //车系代码校验失败
#define CHECK_VERSION_ERROR			MAKEWORD(0x08,0x00)	 //版本校验失败
#define CHECK_USETIME_ERROR			MAKEWORD(0x10,0x00)	 //使用时间校验失败
#define CHECK_USECOUNT_ERROR		MAKEWORD(0x20,0x00)	 //使用次数超限
#define CHECK_INITPWD_ERROR			MAKEWORD(0x40,0x00)	 //密码校验失败
#define CHECK_MCUID_ERROR			MAKEWORD(0x80,0x00)	 //MCUID校验失败
#define CHECK_OTHER_ERROR			MAKEWORD(0x00,0x01)	 //其他异常(文件有误等)

#define MAX_FILEPATH 512
#define BLOCK_SIZE 32
#define MAX_TASKID_NUMBER 400

#define   MAKEDW(ch0,ch1,ch2,ch3) ((unsigned long)(BYTE)(ch0) | ((unsigned long)(BYTE)(ch1)   <<   8) |  ((unsigned long)(BYTE)(ch2)<<16) | ((unsigned long)(BYTE)(ch3)<<24))

//void TrimRightBlank(char *pP);

typedef struct 
{
	unsigned char	ucFlag[4];					//3	    // 可件标志"LF"含义为License Information Data File 头
	unsigned char   aucVersion[8];				//5	    //表示license 文件的版本
	unsigned char	ucCreateTime[8];				//9     // 时间戳
	unsigned char	ucCreateData[8];				//9     // 文件生成日期 如：20091104 表示2009年11月4号生成
	unsigned char	ucExpiredData[8];				//9	    // 文件失效日期 如：20101104 表示2010年11月4号失效
	unsigned char	ucUseCount[8];				//9     //使用次数
	unsigned char   aucSerialNumber[16];			//13	//序列号
	unsigned char   aucLanguage[64];				//40	//语言信息
	unsigned char   auDiagVersion[8];				//9		//版本信息
	unsigned char   auVechicleCode[64];			//65	//车系信息
	unsigned char   auInitPassWord[8];			//9		//初始密码
	unsigned char   auMCUiD[32];					//32	//MCU_ID
	unsigned char	auStatus[4];					//2		//状态表示 0:初始状态 1:正常状态(已激活)  2:冻结
	unsigned long	dwTaskIdCount;					//2		TaskId数量
	unsigned long	dwTaskID[MAX_TASKID_NUMBER];	//400*4	//TASKID 数组	
	unsigned char	auReserved[16];				//11	//预留字节
}LICENSE_STRUCT; 

class CSecurityBaseClass  
{
private:
	void TrimRightBlank(char *pP);
	char m_strLicenseFileName[512];
	LICENSE_STRUCT m_LicenseData;
	void DecryptString(char* str);
public:
	
	CSecurityBaseClass();
	bool Initialize(const char* pFilePath);
	/*功能说明：
			校验许可
	   参数:
		IN:		pSerialNumber:	序列号
		IN:		pchFileName:	许可文件的名称(包含绝对路径)
		IN:		pVecleName:		车型代码
		IN:		pLanguage:		语言代码
		IN:		fVersion:		版本号
		IN:		nUsedCount:		使用次数
		IN:		szInitPwd:		初始密码
		IN:		szMCUID:		MCUID

		OUT:	nCount  剩余使用次数 -1:注册未激活  -2:已激活
		返回:
		WORD型，两个字节,16个bit位，每个bit代表一个错误信息
			详见宏定义
	*/
	unsigned short CheckLisence(const char *pSerialNumber,/*const char *pchFileName,*/const char* pVecleName,const char* pLanguage,  \
				float fVersion,double nUsedCount,const char* szInitPwd,const char* szMCUID,int& nCount);


	//校验许可文件
	int CheckLisence(const char *pSerialNumber,const char *pchFileName,const char* pVecleName,const char* pLanguage,float fVersion,double nUsedCount);

	
	//解密	
	void DecryptBuffer(const  char* pInBuffer,int nInputLen,char* pOutBuffer);

	//加密
	void EncryptData(const  char* pInBuffer,int nInputLen,char* pOutBuffer);

	//解密	
	void DecryptData(const  char* pInBuffer,int nInputLen,char* pOutBuffer);

	//校验文件的合法性
	bool CheckObjectFile(const char *pchFileName);

/*
	获取当前用户的状态
	0: 未注册
	1: 正常状态
	2: 冻结状态
*/

	int GetProductStatus();

/*
校验功能ID是否合法
dwVehicleID: 车系ID
dwFunctionId:功能ID
返回： TRUE 合法
	   FALSE 非法
*/
	bool CheckFunctionIdLegal(unsigned short dwVehicleID,unsigned short dwFunctionId);


	/*
校验功能ID是否合法
dwVehicleID: 车系ID
dwFunctionId:功能ID
返回： TRUE 合法
	   FALSE 非法
*/
	bool CheckFunctionIdLegal(unsigned long dwFunctionId);

/*
获取试用次数
返回： 试用次数
*/
	int GetTryUseCount();
	
/*
获取MCUID
返回： McuID
*/
	void GetMcuId(char* szMcuID);

	
	//得到MCU版本号
	unsigned long GetNewMCUMainVersion(char * filedata);

	
/*
	获取MacAddress 返回格式 如："AB0C87AC1FEE"
*/
void GetMacAddress(unsigned char *szMacAddress);
//#endif

};
#endif