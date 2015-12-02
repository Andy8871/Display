/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	�ļ����ƣ�	MultiSelectShow.cpp
	�ĵ���ʶ��	007�������ƽ̨��ϸ���˵����
	ժ    Ҫ��	��ʾ��ĳ�Ա������������ɶ���ѡ���ܡ�
	��    �ܣ�
				1��������ѡ���ı����ݰ��涨��ʽд�뻺������֪ͨ��ʾ����ȡ�����ݲ���ʾ��
				2�����Ͳ����ߵ�ѡ��������ťѡ�񰴼���
				3��ֻ�����û�ѡ����ɺ����ŷ���

	��ʷ��¼��
	----------------------------------------------------------------------------
	ʱ     ��   ����		�汾��		��    ��    ��    ��                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			�������ࡣ                                           
	2006.07.27  wcx      	0.2006		�޸ĺ���bool CMultiSelectShow::CSelectedItemData::IsSelected (short iSequence)
	                                    ���趨���ѡ����󣬷���ֵ�ڲ����жϴ������ѡ
										������ʱ��Զ����false����

**************************************************************************************/


#include "MultiSelectShow.h"
#include "Database.h"
#include "assert.h"
#include "Display.h"
CMultiSelectShow::CMultiSelectShow()
{
	m_pDataBase = NULL;
	m_iItemNumber = 0;
}

CMultiSelectShow::~CMultiSelectShow()
{
	
}


CMultiSelectShow::CSelectedItemData::CSelectedItemData()
{
	CSelectedItemData::m_i16ScreenFirstLineItemSequence = 0;
	CSelectedItemData::m_i16ItemNumber = 0;
	CSelectedItemData::m_uiMaxSelectedNumber = 0xFFFF;	// ���ɱ�ѡ��������

	unsigned char ucValue = 0x00;
	CSelectedItemData::m_binMaskCode += ucValue;
}


/***************************************************************************************
��    �ܣ���ʼ������ѡ��
����˵����char* pTitle ����ѡ�񴰿ڱ��⣬��Ϊ����ʾ����ʹ��ȱʡ����
�� �� ֵ����
˵    ������
**************************************************************************************/
void CMultiSelectShow::Init(const char* pTitle)
{
	// ����ѡ�����
/*
	//2006.6.28 wcx
	if(NULL == pTitle)
	{
		char chChar = 0x00;
		m_pSendAndReceive->Add(chChar);
		return;
	}
*/
	
	m_iItemNumber = 0;  //ʵ�ʶ���ѡ������

	m_strMsg = "";//hpy added 20140120

	char chFuction = DT_MUTI_SELECT;
	m_pSendAndReceive->Init(chFuction);  // ��书�ܴ���͵��Ա�־
	
	// ����ѡ������
	unsigned short uiSelectedNumber = 0;
	m_pSendAndReceive->Add(uiSelectedNumber);

	// ��Ļ��һ�����
	unsigned short uiScreenFirstLineSequence = 0;
	m_pSendAndReceive->Add(uiScreenFirstLineSequence);

	// ����ѡ�����
//	m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);	

	if(NULL == pTitle){
		char chChar = 0x00;
		m_pSendAndReceive->Add(chChar);
	}
	else 
		m_pSendAndReceive->Add(pTitle, strlen(pTitle)+1);	

}



bool CMultiSelectShow::SetFlag(unsigned char iFlag)
{
	
	m_pSendAndReceive->SetFlag(iFlag);
	return true;
}

/***************************************************************************************
��    �ܣ���ʼ������ѡ��
����˵����string strTitle����ѡ�񴰿ڱ���
�� �� ֵ����
˵    ������
**************************************************************************************/
void CMultiSelectShow::Init (string strTitle)
{
	// �������غ���
	Init(strTitle.c_str());
}

/***************************************************************************************
��    �ܣ���ʼ������ѡ��
����˵����string idTitle����ѡ�񴰿ڱ���
�� �� ֵ����
˵    ��������������idTitle����ʹ���ı�����Ҷ�Ӧ���ı���
**************************************************************************************/
void CMultiSelectShow::Init(CBinary idTitle)
{
	// ����IDת�����ַ���
	string strTitle = adsGetTextString(idTitle);  // Ҫ�������ݿ���ת��
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// �������غ���
	Init(strTitle);
}

/***************************************************************************************
��    �ܣ���Ӷ�ѡ������
����˵����CBinary idDataStream ������ID��ȡ���������������������ı�
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������
**************************************************************************************/
short CMultiSelectShow::Add(CBinary idDataStream)
{
	if(ID_BINARY_LENTH != idDataStream.GetSize())
	{
		return EC_DATA_FORMAT;  // ���ݸ�ʽ��
	}

	if(NULL == m_pDataBase)
	{
		m_pDataBase = new(CDatabase);
		m_pDataBase->Open(CDatabase::DB_DATA_STREAM);
	}

	vector<CBinary> idTemp;
	idTemp = m_pDataBase->SearchId(idDataStream);

	if( idTemp.size() > 0)
	{
		m_pSendAndReceive->Add(idTemp[0]);
                                             
		// ��������ѡ������                  
		m_pSendAndReceive->IncreaseNum(4);  
		
		m_iItemNumber++; 	
	
		return 0;
	}
	else
	{
		string str = CDisplay::IdToString(idDataStream);
		m_pSendAndReceive->Add(str.c_str());
		m_pSendAndReceive->IncreaseNum(4);  
		m_iItemNumber++; 
//		return EC_DB_DATA_STREAM;
	}
	return 0;
}

/***************************************************************************************
��    �ܣ���Ӷ�ѡ������
����˵������ѡ�������ı�
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������
**************************************************************************************/
short CMultiSelectShow::Add(string strMutiSelectedItem)
{
	m_pSendAndReceive->Add(strMutiSelectedItem);

	// ��������ѡ������
	m_pSendAndReceive->IncreaseNum(4);

	m_iItemNumber++;  // ʵ�ʶ���ѡ������

	return 0;
}

/***************************************************************************************
��    �ܣ���Ӷ�ѡ���˵��
����˵���������ı�
�� �� ֵ��������룬�ɹ�Ϊ0
˵    ������
**************************************************************************************/
short CMultiSelectShow::AddMsg(string strMsg)
{
	//m_pSendAndReceive->Add(strMsg);
	m_strMsg = strMsg;
	return 0;
}

/***************************************************************************************
��    �ܣ���ʾ����ѡ�񣬻����û���������ѡ����
����˵����CSelectedItemData& SelectedData ȱʡѡ��û�ѡ����
�� �� ֵ���û��Ƿ�OK������-ture ��-false 
˵    ����1�����û�ѡ����ɺ󱾺����ŷ��أ�
          2��SelectedData�еĶ�ѡ�������ڱ������и�����ӵ�ѡ�������Զ���ӣ�
**************************************************************************************/
bool CMultiSelectShow::Show(CSelectedItemData& SelectedData)
{
	if(NULL != m_pDataBase)
	{
		m_pDataBase->Close();
		delete(m_pDataBase);
		m_pDataBase = NULL;
	}

	// ��ʵ����ӵĶ�������Ϊ׼
	if(m_iItemNumber != SelectedData.GetItemNumber())
	{
		SelectedData.SetItemNumber(m_iItemNumber);
		m_pSendAndReceive->Modify(m_iItemNumber, 4);
	}

	// ��Ļ��һ����� 2006.6.27
	m_pSendAndReceive->Modify(SelectedData.m_i16ScreenFirstLineItemSequence, 4+2);

	// ��ѡ������
	m_pSendAndReceive->Add(SelectedData.m_binMaskCode);

	// ���ɱ�ѡ��������
	if(0xFFFF == SelectedData.m_uiMaxSelectedNumber)
	{
		// ���û�����������������ʵ����ӵ�Ϊ�������
		short iItemNumber = SelectedData.GetItemNumber();
		m_pSendAndReceive->Add(iItemNumber);
	}
	else
	{
		m_pSendAndReceive->Add(SelectedData.m_uiMaxSelectedNumber);
	}

	m_pSendAndReceive->SetFlag(0);

	m_pSendAndReceive->Add(m_strMsg);//hpy added 20140113

	// ������Ϣ
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���
	short iUserKey;
	// ������ݳ����Ƿ���ȷ
	int iLenth = 256*iHigh + iLow;
	if(iLenth < SelectedData.m_binMaskCode.GetSize()+5)
	{
		assert(iLenth < SelectedData.m_binMaskCode.GetSize()+5);
	}	

	// ��5���ֽڱ�ʾ:�˵��û�ѡ��
	iUserKey = pchContain[4];

	// ���淵�صİ��ð���
	SelectedData.m_iUserClickButtonKeyValue = iUserKey;

	// ������Ļ��һ����� 2006.6.27 add
	SelectedData.m_i16ScreenFirstLineItemSequence = (((unsigned char)((pchContain+5)[0]))*256+(unsigned char)((pchContain+5)[1]));

	// ��ѡ������ĳ���
	short iMaskLenth = SelectedData.m_binMaskCode.GetSize();

	// ���Ʒ��ص�����
//	CBinary binMaskcode((unsigned char*)pchContain+5, iMaskLenth);
	CBinary binMaskcode((unsigned char*)pchContain+5+2, iMaskLenth); //2006.6.27

	// ���淵�ص�����
	SelectedData.m_binMaskCode = binMaskcode;
	if(adsIDOK == iUserKey)
	{
		return true;
	}
	
	return false;
}


/***************************************************************************************
��    �ܣ���ʾ����ѡ�񣬻����û���������ѡ����
����˵����CSelectedItemData& SelectedData ȱʡѡ��û�ѡ������iSelectedIndex���û���ѡ�iFlag:��־λ
�� �� ֵ���û��Ƿ�OK������-ture ��-false 
˵    ����1�����û�ѡ����ɺ󱾺����ŷ��أ�
          2��SelectedData�еĶ�ѡ�������ڱ������и�����ӵ�ѡ�������Զ���ӣ�
		  3��iSelectedIndex��0Ϊ��ʼ��
**************************************************************************************/
bool CMultiSelectShow::Show(CSelectedItemData& SelectedData,short &iSelectedIndex,unsigned char iFlag)
{
	if(NULL != m_pDataBase)
	{
		m_pDataBase->Close();
		delete(m_pDataBase);
		m_pDataBase = NULL;
	}

	// ��ʵ����ӵĶ�������Ϊ׼
	if(m_iItemNumber != SelectedData.GetItemNumber())
	{
		SelectedData.SetItemNumber(m_iItemNumber);
		m_pSendAndReceive->Modify(m_iItemNumber, 4);
	}

	// ��Ļ��һ����� 2006.6.27
	m_pSendAndReceive->Modify(SelectedData.m_i16ScreenFirstLineItemSequence, 4+2);

	// ��ѡ������
	m_pSendAndReceive->Add(SelectedData.m_binMaskCode);

	// ���ɱ�ѡ��������
	if(0xFFFF == SelectedData.m_uiMaxSelectedNumber)
	{
		// ���û�����������������ʵ����ӵ�Ϊ�������
		short iItemNumber = SelectedData.GetItemNumber();
		m_pSendAndReceive->Add(iItemNumber);
	}
	else
	{
		m_pSendAndReceive->Add(SelectedData.m_uiMaxSelectedNumber);
	}

	//add by scf 2009.12.02 
	unsigned char byTmp = m_pSendAndReceive->GetFlag();
	byTmp = byTmp|iFlag;
//	byTmp = byTmp&0xF2;
	m_pSendAndReceive->SetFlag(byTmp);

	//m_pSendAndReceive->SetFlag(iFlag);

	m_pSendAndReceive->Add(m_strMsg);

	// ������Ϣ
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// ��ȡ�յ�������
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // ���ݳ��ȵĸ��ֽڲ���
    unsigned char iLow  = (unsigned char)pchContain[1];    // ���ݳ��ȵĵ��ֽڲ���
	short iUserKey;
	if(iFlag == 0)
	{
		// ������ݳ����Ƿ���ȷ
		int iLenth = 256*iHigh + iLow;
		if(iLenth < SelectedData.m_binMaskCode.GetSize()+5)
		{
			assert(iLenth < SelectedData.m_binMaskCode.GetSize()+5);
		}	

		// ��5���ֽڱ�ʾ:�˵��û�ѡ��
		iUserKey = pchContain[4];

		// ���淵�صİ��ð���
		SelectedData.m_iUserClickButtonKeyValue = iUserKey;

		// ������Ļ��һ����� 2006.6.27 add
		SelectedData.m_i16ScreenFirstLineItemSequence = (((unsigned char)((pchContain+5)[0]))*256+(unsigned char)((pchContain+5)[1]));

		// ��ѡ������ĳ���
		short iMaskLenth = SelectedData.m_binMaskCode.GetSize();

		// ���Ʒ��ص�����
	//	CBinary binMaskcode((unsigned char*)pchContain+5, iMaskLenth);
		CBinary binMaskcode((unsigned char*)pchContain+5+2, iMaskLenth); //2006.6.27

		// ���淵�ص�����
		SelectedData.m_binMaskCode = binMaskcode;
	}
	else
	{
		// ������ݳ����Ƿ���ȷ
		int iLenth = 256*iHigh + iLow;
		
		// ��5���ֽڱ�ʾ:�˵��û�ѡ��
		iUserKey = pchContain[3];

		// ���淵�صİ��ð���
		SelectedData.m_iUserClickButtonKeyValue = iUserKey;
		// ��5, 6���ֽڱ�ʾ:�˵��û�ѡ��
		unsigned char chHigh = pchContain[4];    // �˵��û�ѡ��ĸ��ֽڲ���
		unsigned char chLow  = pchContain[5];    // �˵��û�ѡ��ĵ��ֽڲ���
		if(chHigh == 0xff && chLow == 0xff)
		{
			iSelectedIndex = -1;
		}
		else
		{
			iSelectedIndex = 256*chHigh + chLow;
		}
	}
	if(adsIDOK == iUserKey)
	{
		return true;
	}
	
	return false;
}

/***************************************************************************************
��    �ܣ��������ɱ�ѡ��������
����˵����unsigned short uiMaxSelectedNumber ���ɱ�ѡ��������
�� �� ֵ���� 
˵    ������
**************************************************************************************/
void CMultiSelectShow::CSelectedItemData::SetMaxSelectedNumber(unsigned short uiMaxSelectedNumber)
{
	CSelectedItemData::m_uiMaxSelectedNumber = uiMaxSelectedNumber;
}

/***************************************************************************************
��    �ܣ�����ĳһ��Ŀ�Ƿ�ѡ��
����˵����unsigned short iSequence �����������
�� �� ֵ����ѡ��-true δ��ѡ��-false 
˵    ��������Ŵ�������Ŀ��������false��
          ����ŵ���SID_ALL_ITEM���򷵻��Ƿ����ѡ����
**************************************************************************************/
bool CMultiSelectShow::CSelectedItemData::IsSelected (short iSequence)
{
	if(iSequence > CSelectedItemData::m_i16ItemNumber-1)
	{
		return false;
	}
	
	short iIndex = iSequence/8;  // �ֽ����
	char chCode = CSelectedItemData::m_binMaskCode.GetAt(iIndex);

	iIndex = (iSequence)%8;	        // λ���
	char chTest = chCode>>(7-iIndex);
	chTest &= 0x01;

	if(chTest == 0x01)
	{
		return true;
	}

	return false;
}

/***************************************************************************************
��    �ܣ��趨��ѡ��Ŀ����
����˵����unsigned short nNumber ��ѡ������
�� �� ֵ���������
˵    �����ú�����CMultiSelectShow.Show()�б��Զ����ã�������ʵ�ʵ���Ŀ����
**************************************************************************************/
short CMultiSelectShow::CSelectedItemData::SetItemNumber (short nNumber)
{
	short iByteNum = (nNumber-1)/8 + 1;  // ʵ����Ҫ�����볤��
	short iMaskLenth = CSelectedItemData::m_binMaskCode.GetSize();

	CSelectedItemData::m_i16ItemNumber = nNumber;

	// ʵ����Ҫ�����볤�Ȳ���
	if(iMaskLenth < iByteNum)
	{
		iMaskLenth = (iMaskLenth<0)?0:iMaskLenth;

		unsigned char ucValue = 0x00;
		for(short iIndex = iMaskLenth; iIndex < iByteNum; iIndex++)
		{					
			CSelectedItemData::m_binMaskCode += ucValue;
		}
	}
	else if(iMaskLenth > iByteNum)
	{
		// ����ɵ�����
		CBinary binTemp;
		for(short iIndex = 0; iIndex < iByteNum; iIndex++)
		{
			binTemp += CSelectedItemData::m_binMaskCode[iIndex];
		}

		CSelectedItemData::m_binMaskCode = binTemp;
	}

	return true;
}

/***************************************************************************************
��    �ܣ�ȡ�ö�ѡ��Ŀ����
����˵������
�� �� ֵ����ѡ��Ŀ����
˵    ������
**************************************************************************************/
short CMultiSelectShow::CSelectedItemData::GetItemNumber ()
{
	return CSelectedItemData::m_i16ItemNumber;
}

/***************************************************************************************
��    �ܣ����ñ�ѡ����
����˵����unsigned short iSequence ����������ţ���ΪSID_ALL_ITEM�������������Ч
         bool IsSelected �Ƿ�ѡ�б�־����ѡ��-true ����ѡ��-false 
�� �� ֵ��������� 
˵    ������
**************************************************************************************/
short CMultiSelectShow::CSelectedItemData::SetSelectedItem (short iSequence, bool IsSelected)
{
	if(iSequence > CSelectedItemData::m_i16ItemNumber-1)
	{
		return EC_OVER_ITEM_NUM;
	}

	// �������ܱ���λ��
	short iBitSum = 8*m_binMaskCode.GetSize();  

	// ������Ȳ���
	if(iSequence > iBitSum-1 )
	{
		return EC_OVER_MASK_LEN;
	}

	// ���iSequence����ΪSID_ALL_ITEM����������Ϊ��ѡ��
	if(SID_ALL_ITEM == iSequence)
	{
		for(short iIndex = 0;  iIndex < m_binMaskCode.GetSize(); iIndex++)
		{
			m_binMaskCode.SetAt(iIndex, IsSelected?0xFF:0x00);
		}
	}
	else 
	{	
		short iIndexByte = iSequence/8;         // �ֽ����
		char chCode = CSelectedItemData::m_binMaskCode.GetAt(iIndexByte);

		short iIndexBit = (iSequence)%8;	        // λ���
		char chTemp = 0x01<<(7-iIndexBit);

		m_binMaskCode.SetAt(iIndexByte, IsSelected ? (chCode|chTemp) : (chCode&~chTemp));
	}


	return true;
}


/***************************************************************************************
��    �ܣ�ȡ���˳�����ʱ�û�������ֵ
����˵������
�� �� ֵ���û�������ֵ 
˵    �����ú�������ֵ��CMultiSelectShow.Show()�ķ���ֵ��ͬ��������ϳ���Աֱ��ʹ��CMultiSelectShow.Show()�ķ���ֵ
**************************************************************************************/
short CMultiSelectShow::CSelectedItemData::UserClickButtonKeyValue ()
{
	return CSelectedItemData::m_iUserClickButtonKeyValue;
}

