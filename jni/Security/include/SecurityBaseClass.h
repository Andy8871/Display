#ifndef _SECURITYBASECLASS_H_
#define _SECURITYBASECLASS_H_

#ifndef MAKEWORD
#define MAKEWORD(a, b) ((unsigned short)(((BYTE)(a)) | ((unsigned short)((BYTE)(b))) << 8))
#endif

#define CHECK_SUCCESS				MAKEWORD(0x00,0x00)  //У��ɹ�
#define CHECK_LANGUAGE_ERROR		MAKEWORD(0x01,0x00)  //����У��ʧ��
#define CHECK_SERIALNUMBER_ERROR	MAKEWORD(0x02,0x00)  //���к�У��ʧ��
#define CHECK_VECHICLECODE_ERROR	MAKEWORD(0x04,0x00)  //��ϵ����У��ʧ��
#define CHECK_VERSION_ERROR			MAKEWORD(0x08,0x00)	 //�汾У��ʧ��
#define CHECK_USETIME_ERROR			MAKEWORD(0x10,0x00)	 //ʹ��ʱ��У��ʧ��
#define CHECK_USECOUNT_ERROR		MAKEWORD(0x20,0x00)	 //ʹ�ô�������
#define CHECK_INITPWD_ERROR			MAKEWORD(0x40,0x00)	 //����У��ʧ��
#define CHECK_MCUID_ERROR			MAKEWORD(0x80,0x00)	 //MCUIDУ��ʧ��
#define CHECK_OTHER_ERROR			MAKEWORD(0x00,0x01)	 //�����쳣(�ļ������)

#define MAX_FILEPATH 512
#define BLOCK_SIZE 32
#define MAX_TASKID_NUMBER 400

#define   MAKEDW(ch0,ch1,ch2,ch3) ((unsigned long)(BYTE)(ch0) | ((unsigned long)(BYTE)(ch1)   <<   8) |  ((unsigned long)(BYTE)(ch2)<<16) | ((unsigned long)(BYTE)(ch3)<<24))

//void TrimRightBlank(char *pP);

typedef struct 
{
	unsigned char	ucFlag[4];					//3	    // �ɼ���־"LF"����ΪLicense Information Data File ͷ
	unsigned char   aucVersion[8];				//5	    //��ʾlicense �ļ��İ汾
	unsigned char	ucCreateTime[8];				//9     // ʱ���
	unsigned char	ucCreateData[8];				//9     // �ļ��������� �磺20091104 ��ʾ2009��11��4������
	unsigned char	ucExpiredData[8];				//9	    // �ļ�ʧЧ���� �磺20101104 ��ʾ2010��11��4��ʧЧ
	unsigned char	ucUseCount[8];				//9     //ʹ�ô���
	unsigned char   aucSerialNumber[16];			//13	//���к�
	unsigned char   aucLanguage[64];				//40	//������Ϣ
	unsigned char   auDiagVersion[8];				//9		//�汾��Ϣ
	unsigned char   auVechicleCode[64];			//65	//��ϵ��Ϣ
	unsigned char   auInitPassWord[8];			//9		//��ʼ����
	unsigned char   auMCUiD[32];					//32	//MCU_ID
	unsigned char	auStatus[4];					//2		//״̬��ʾ 0:��ʼ״̬ 1:����״̬(�Ѽ���)  2:����
	unsigned long	dwTaskIdCount;					//2		TaskId����
	unsigned long	dwTaskID[MAX_TASKID_NUMBER];	//400*4	//TASKID ����	
	unsigned char	auReserved[16];				//11	//Ԥ���ֽ�
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
	/*����˵����
			У�����
	   ����:
		IN:		pSerialNumber:	���к�
		IN:		pchFileName:	����ļ�������(��������·��)
		IN:		pVecleName:		���ʹ���
		IN:		pLanguage:		���Դ���
		IN:		fVersion:		�汾��
		IN:		nUsedCount:		ʹ�ô���
		IN:		szInitPwd:		��ʼ����
		IN:		szMCUID:		MCUID

		OUT:	nCount  ʣ��ʹ�ô��� -1:ע��δ����  -2:�Ѽ���
		����:
		WORD�ͣ������ֽ�,16��bitλ��ÿ��bit����һ��������Ϣ
			����궨��
	*/
	unsigned short CheckLisence(const char *pSerialNumber,/*const char *pchFileName,*/const char* pVecleName,const char* pLanguage,  \
				float fVersion,double nUsedCount,const char* szInitPwd,const char* szMCUID,int& nCount);


	//У������ļ�
	int CheckLisence(const char *pSerialNumber,const char *pchFileName,const char* pVecleName,const char* pLanguage,float fVersion,double nUsedCount);

	
	//����	
	void DecryptBuffer(const  char* pInBuffer,int nInputLen,char* pOutBuffer);

	//����
	void EncryptData(const  char* pInBuffer,int nInputLen,char* pOutBuffer);

	//����	
	void DecryptData(const  char* pInBuffer,int nInputLen,char* pOutBuffer);

	//У���ļ��ĺϷ���
	bool CheckObjectFile(const char *pchFileName);

/*
	��ȡ��ǰ�û���״̬
	0: δע��
	1: ����״̬
	2: ����״̬
*/

	int GetProductStatus();

/*
У�鹦��ID�Ƿ�Ϸ�
dwVehicleID: ��ϵID
dwFunctionId:����ID
���أ� TRUE �Ϸ�
	   FALSE �Ƿ�
*/
	bool CheckFunctionIdLegal(unsigned short dwVehicleID,unsigned short dwFunctionId);


	/*
У�鹦��ID�Ƿ�Ϸ�
dwVehicleID: ��ϵID
dwFunctionId:����ID
���أ� TRUE �Ϸ�
	   FALSE �Ƿ�
*/
	bool CheckFunctionIdLegal(unsigned long dwFunctionId);

/*
��ȡ���ô���
���أ� ���ô���
*/
	int GetTryUseCount();
	
/*
��ȡMCUID
���أ� McuID
*/
	void GetMcuId(char* szMcuID);

	
	//�õ�MCU�汾��
	unsigned long GetNewMCUMainVersion(char * filedata);

	
/*
	��ȡMacAddress ���ظ�ʽ �磺"AB0C87AC1FEE"
*/
void GetMacAddress(unsigned char *szMacAddress);
//#endif

};
#endif