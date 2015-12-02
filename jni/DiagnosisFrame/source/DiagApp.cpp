#include "adsStd.h"
#include "DiagApp.h"
#include <time.h>

CDiagApp *CDiagApp::m_pMainApplication;

void adsSleep(int iMilliseconds)
{
	clock_t goal;
	    goal = iMilliseconds+clock();
	    while (goal>clock())
	    {
	        ;
	    }
}
CDiagApp::CDiagApp(void) 
{
	m_pMainApplication = this;
}


CDiagApp::~CDiagApp(void) 
{
}

short CDiagApp::InitInstance(void) 
{

//������ʾ��
#if 0
#if USMENUTREE	//ʹ�ò˵���
	CMenuTree MenuTree;

	//���ò˵��ص��������粻�����ȱʡ����Task()����
	MenuTree.SetTaskCallBackFunction(�˵��ص�����ָ��); 

	MenuTree.ShowMenu(��ʾ�˵���ID��ȱʡΪ = 0);
#else
	CDiagnosisUnit *pSystemUnit = NULL;

	/*
	�˴�����ϳ���Ա���룬�ṹ���£�
	*/

	//1.�˵�ѡ��  //������ϵͳɨ��֮��֧�ֵ�ϵͳ���̶��Ĳ˵���������ʽȷ�������ġ�
	//

	//2.ȷ��������ʽ�� 
	//�䶨����CDiagnosisUnit�У������ǽ�����ϡ�ɨ��ϵͳ�����ٲ����Ƿ���ڹ����롢
	//����ɾ�����й�����ȵ�
	unsigned short ui16Way = CDiagnosisUnit::DUF_QUICK_READ_TROUBLE_CODE; 

	short iRet = 0;
	switch(�û�ѡ��) {
	case ENG:
		pSystemUnit = new CENG;
		break;
	case ABS:
		pSystemUnit = new CABS;
		break;
	case SRS:
		pSystemUnit = new CSRS;
		break;
	case ����ϵͳ�����
		��
		break;
	default:
		#ifdef ADS_DEBUG
			//֪ͨ��ϳ������֪ͨ��ʽΪд���׼����
		#endif
		break;
	}

	try 
	{
		if(pSystemUnit) 
		{
			iRet = pSystemUnit->Enter(ui16Way);
			delete pSystemUnit;
			pSystemUnit = NULL;
		}
	}
	catch (int adsError) 
	{
		//�����������ڴ��ͷŵ�
	}
#endif
#endif
	return 0;
}

CDiagApp *adsGetDiagApp () 
{
	return CDiagApp::m_pMainApplication;
}





