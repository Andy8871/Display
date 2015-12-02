#ifndef	__RUN_ENVIRONMNET_H__
#define	__RUN_ENVIRONMNET_H__
#include "adsStd.h"
#include <string>
using namespace std;

//hpy add
typedef struct TAG_LANGUAGE_ITEM
{
	char csCountryCode[4];
	char csLanguage[30];
	unsigned int	iCodePage;
}LANGUAGE_ITEM;

class CRunEnvironment
{
	protected:
		static string m_strDisplayDirectory;    // ��ʾ������Ŀ¼
		static string m_strDiagnosisDirectory;  // ��ϳ�����Ŀ¼
		static string m_strLanguage;            // ���Դ���
		//add by scf 2007.2.13
		static string m_strSerialNumber;         //�������к�
		static bool      m_bDemoMode;
		static  unsigned char    m_cScreenType;		//��Ļ����
		static string m_strDisplayVersion;
		static string m_ECUIDinfo;
		static bool m_bDsPrintMode;

		static bool m_cHeckMode;    //true��ѡ�м�¼ģʽ��false:�Ǽ�¼ģʽ
		static string m_menuChoosedDir;
		static unsigned int m_CodePage;
		static bool m_bNewMcuFlag;
		static unsigned char m_ProuctType;

		static unsigned char m_UnitType;//hpy add 20131126 //��λ����

	public:
		// ������ʾ������Ŀ¼
		static void SetDisplayDirectory(char* pDirectory);

		// ȡ����ʾ������Ŀ¼
		static string GetDisplayDirectory();

		// ������ϳ�����Ŀ¼
		static void SetDiagnosisDirectory(char* pDirectory);

		// ȡ����ϳ�����Ŀ¼
		static string GetDiagnosisDirectory();

		// �������Դ���
		static void SetLanguage(char* pLanguage);

		// ȡ�����Դ���
		static string GetLanguage();

		//add by scf 2007.2.13
		// �������к�
		static void SetSerialNumber(char* pSerailNumber);

		//add by scf 2007.2.13
		// �������к�
		static void SetDemoMode(bool bDemoMode=false);


		// ȡ�����к�
		static string GetSerialNumber();
		
		// ȡ����ʾģʽ
		static bool GetDemoMode();
		//set the ECUID info
		static void SetECUInfo(string& info);
#ifdef DATA_DRV_EXT
		//���ü�¼ģʽ
		static void SetCheckMode(bool checkMode = false);
		// ȡ�ü�¼ģʽ
		static bool GetCheckMode(void);
		//����menuѡ��·��
		static void SetMenuChoosedDir(string& dir);
		//��ȡmenuѡ��·��
		static string GetMenuChoosedDir(void);

		//��������ҳ����
		static void SetCodePage(unsigned int nCodePage);
		// ȡ�ü�¼ģʽ
		static unsigned int GetCodePage(void);

#endif
		static string GetItemLanguage(string CountryCode);//hpy add
		static unsigned int GetItemCodePage(string CountryCode);//hpy add
		
		//0:�ڰ��� 1������ 2: PC(1024*768)
		static unsigned char GetScreenType();

		static void SetScreenType(unsigned char uiType);

		// ȡ����ʾ������Ŀ¼
		static string GetDisplayVersion();

		// ������ϳ�����Ŀ¼
		static void SetDisplayVersion(char* pDisplayVersion);
		
		//����mcu��־��Ϣ
		static void SetNewMcuFlag(bool bFlag);
		static bool GetNewMcuFlag();

		//����mcu��־��Ϣ
		static void SetDsPrintMode(bool bFlag);
		static bool GetDsPrintMode();

		//BEN ADD 20131008
		static void SetDBFormat(int iDBFormat);
		static int GetDBFormat();
		static int m_iDBFormat;


		//��ȡ��Ʒ����
		static unsigned char GetProductType();
		static void SetProductType(unsigned char uiProductType);

		//��ȡ��λ����	//hpy add 20131126
		static unsigned char GetUnitType();
		static void SetUnitType(unsigned char uiUnitType);

};
	
#endif	//_RUNENTIRONMNET_H_