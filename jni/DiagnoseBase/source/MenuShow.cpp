#include "MenuShow.h"
#include "Database.h"
#include "assert.h"
#include "Display.h"
#include "RunLog.h"
#include "CommWithEcu.h"
#include "DataCommon.h"
#include "RunEnvironment.h"
//����ID���ı��ⷵ���ַ���
extern string adsGetTextString (CBinary binTextID);

CDatabase m_MenuTxtDb;		//FOR MENU DISPLAY


#if 1// _WIN32_WCE
	const wchar_t* MENU_FLAG = L"MENU_CHOOSED";
#else
	const char* MENU_FLAG= "MENU_CHOOSED";
#endif


CMenuShow::CMenuShow()
{

}

CMenuShow::~CMenuShow()
{
	m_MenuTxtDb.Close();//by added FOR MENU DISPLAY OPTIMIZE 2012-5-18
}


/***************************************************************************************
��    �ܣ���ʼ���˵�
����˵����char *pTitle �˵����⣬��Ϊ����ʾ����ʹ��ȱʡ����
�� �� ֵ����
˵    ������������ʾ������ʽ�Ի�������ʽ��
**************************************************************************************/
void CMenuShow::Init(const char *pTitle)
{
	assert(NULL != pTitle);

	char chFuction = DT_MENU;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־

	// �˵�ȱʡѡ��
	short iDefault = 0; 
	m_pSendAndReceive->Add(iDefault);

	// ��ǰ����һ�в˵����
	short i16MenuScreenFirstLineItem = 0; 
	m_pSendAndReceive->Add(i16MenuScreenFirstLineItem);

	// �˵�����
	short iMenuNum = 0;
	m_pSendAndReceive->Add(iMenuNum);

	//���Java�ַ��������ָ��ʶ
	char cDelimiters = 0;
	m_pSendAndReceive->Add(cDelimiters);
	// �˵�����
	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);


}

/***************************************************************************************
��    �ܣ���ʼ���˵�
����˵����string strTitle �����ı��ַ���
�� �� ֵ����
˵    ������
**************************************************************************************/
void CMenuShow::Init(string strTitle)
{
	// �������غ���
	Init(strTitle.c_str());
}

/***************************************************************************************
��    �ܣ���ʼ���˵�
����˵����CBrinary idTitle �����ı�ID
�� �� ֵ����
˵    ������
**************************************************************************************/
void CMenuShow::Init(CBinary idTitle)
{
	// ����IDת�����ַ���
	string strTitle = adsGetTextString(idTitle);

	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// �������غ���
	Init(strTitle);
}

/***************************************************************************************
��    �ܣ����ñ�־λ
����˵����byFlag һ���ֽ�
�� �� ֵ����
˵    ������
**************************************************************************************/

bool CMenuShow::SetFlag (unsigned char byFlag)
{
	m_pSendAndReceive->SetFlag(byFlag);
	return true;
}

/***************************************************************************************
��    �ܣ���Ӳ˵�������
����˵����string strMenuItem �˵������ı��ַ���
�� �� ֵ���ɹ�-false ʧ��-true 
˵    ������
**************************************************************************************/
bool CMenuShow::Add(string strMenuItem)
{
	// ��Ӳ˵��ı�
	m_pSendAndReceive->Add(strMenuItem);

	// �����˵�����
	m_pSendAndReceive->IncreaseNum(8);  // ����8��ʾ�˵������Ǵӵ�8���ֽڿ�ʼ
	
	return true;
}

/***************************************************************************************
��    �ܣ���Ӳ˵�������
����˵����CBrinary idMenuItem �˵������ı�ID
�� �� ֵ���ɹ�-false ʧ��-true
˵    ������
**************************************************************************************/
bool CMenuShow::Add(CBinary idMenuItem)
{
	// ����IDת�����ַ���
	string strMenuItem = adsGetTextString(idMenuItem);

	if(strMenuItem.size()<=0){
		strMenuItem = CDisplay::IdToString(idMenuItem);
	}
		
	// �������غ���
	Add(strMenuItem);

	return true;
}

/***************************************************************************************
��    �ܣ���ʾ�˵�
����˵����CMenuStruct &MenuParameter �˵�ѡ�������Ϣ
�� �� ֵ����ѡ�еĲ˵���ţ���ѡ��BACK�����򷵻صĲ˵����Ϊ-1
˵    ����1�����û�ѡ��BACK��ʱ��MenuParameter.i16MenuSelected��ֵ�ǲ˵�����������ţ�����������ֵΪ-1��
             ������ϳ���Աֻ��ʹ�ú�������ֵ���жϲ˵����ˡ�
          2������MenuParameter�����û���Ų˵���ȷ��ʾ��ص����ݣ���ϳ���Աֻ�������ñ���������Щ��������
**************************************************************************************/
short CMenuShow::Show(CMenuStruct &MenuParameter)
{	
	// ��ȡ�յ�������
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);
DO_MENU:

//end add
	// �޸�  ���˵�ȱʡѡ�+����ǰ����һ�в˵���š�
	m_pSendAndReceive->Modify(MenuParameter.m_i16MenuSelected, 4);
	m_pSendAndReceive->Modify(MenuParameter.m_i16MenuScreenFirstLineItem, 6);
	// ������Ϣ
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	int lenOfFlag = 0;

#if 1//_WIN32_WCE
	lenOfFlag = wcslen(MENU_FLAG);
	wstring nameFlag = L"";
	if (wmemcmp(MENU_FLAG,(wchar_t*)(pchContain+8),lenOfFlag) == 0)//by modified for Bug V30_PCMAX_OE-3863  2012-5-9	
	{
		nameFlag = (wchar_t*)(pchContain+8+lenOfFlag*2);//"MENU_CHOOSED"�Ŀ��ַ���ռ24�����ֽ�   by2012-5-31
		string strbuf = "";
		//�ַ�ת��
		//Wstr2Str(nameFlag,strbuf);
		CRunEnvironment::SetMenuChoosedDir(strbuf);
	}

#else	
	lenOfFlag = strlen(MENU_FLAG);
	string nameFlag = "";	
	if (memcmp(MENU_FLAG,pchContain+8,lenOfFlag) == 0)//by modified for Bug V30_PCMAX_OE-3863  2012-5-9
	{
		nameFlag = pchContain+8+lenOfFlag;
		CRunEnvironment::SetMenuChoosedDir(nameFlag);
	}	
#endif

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���

	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	/*if(iLenth < 6)
	{
		assert(iLenth < 6);
	}	*/

	// ��5, 6���ֽڱ�ʾ:�˵��û�ѡ��
	unsigned char chHigh = pchContain[4];    // �˵��û�ѡ��ĸ��ֽڲ���
    unsigned char chLow  = pchContain[5];    // �˵��û�ѡ��ĵ��ֽڲ���
	int i16MenuSelected;
	if(chHigh == 0xff && chLow == 0xff)
	{
		i16MenuSelected = -1;
	}
	else
	{
		i16MenuSelected = 256*chHigh + chLow;
	}

	// ��7, 8���ֽڱ�ʾ:��ǰ����һ�в˵����
	iHigh = pchContain[6];    // ���ֽڲ���
    iLow  = pchContain[7];    // ���ֽڲ���
	short i16MenuScreenFirstLineItem = 256*iHigh + iLow;

	MenuParameter.m_i16MenuSelected            = i16MenuSelected;
	MenuParameter.m_i16MenuScreenFirstLineItem = i16MenuScreenFirstLineItem;

	return i16MenuSelected;
}


