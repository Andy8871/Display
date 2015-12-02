/**************************************************************************************

	Copyright (c) 2005, AUTOBOSS Inc. 
	All rights reserved.
		
	文件名称：	MultiSelectShow.cpp
	文档标识：	007汽车诊断平台详细设计说明书
	摘    要：	显示类的成员变量，用于完成多项选择功能。
	功    能：
				1、将多项选择文本数据按规定格式写入缓冲区，通知显示程序取走数据并显示。
				2、回送操作者的选择结果及按钮选择按键。
				3、只有在用户选择完成后函数才返回

	历史记录：
	----------------------------------------------------------------------------
	时     间   作者		版本号		操    作    内    容                               
	----------------------------------------------------------------------------
	2006.02.08  fufushang	1.0			创建此类。                                           
	2006.07.27  wcx      	0.2006		修改函数bool CMultiSelectShow::CSelectedItemData::IsSelected (short iSequence)
	                                    在设定最大选中项后，返回值在参数判断大于最大选
										中项数时永远返回false错误。

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
	CSelectedItemData::m_uiMaxSelectedNumber = 0xFFFF;	// 最多可被选中项数量

	unsigned char ucValue = 0x00;
	CSelectedItemData::m_binMaskCode += ucValue;
}


/***************************************************************************************
功    能：初始化多项选择
参数说明：char* pTitle 多项选择窗口标题，如为空显示程序使用缺省标题
返 回 值：无
说    明：无
**************************************************************************************/
void CMultiSelectShow::Init(const char* pTitle)
{
	// 多项选择标题
/*
	//2006.6.28 wcx
	if(NULL == pTitle)
	{
		char chChar = 0x00;
		m_pSendAndReceive->Add(chChar);
		return;
	}
*/
	
	m_iItemNumber = 0;  //实际多项选择数量

	m_strMsg = "";//hpy added 20140120

	char chFuction = DT_MUTI_SELECT;
	m_pSendAndReceive->Init(chFuction);  // 填充功能代码和调试标志
	
	// 多项选择数量
	unsigned short uiSelectedNumber = 0;
	m_pSendAndReceive->Add(uiSelectedNumber);

	// 屏幕第一行序号
	unsigned short uiScreenFirstLineSequence = 0;
	m_pSendAndReceive->Add(uiScreenFirstLineSequence);

	// 多项选择标题
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
功    能：初始化多项选择
参数说明：string strTitle多项选择窗口标题
返 回 值：无
说    明：无
**************************************************************************************/
void CMultiSelectShow::Init (string strTitle)
{
	// 调用重载函数
	Init(strTitle.c_str());
}

/***************************************************************************************
功    能：初始化多项选择
参数说明：string idTitle多项选择窗口标题
返 回 值：无
说    明：本函数标题idTitle参数使用文本库查找对应的文本串
**************************************************************************************/
void CMultiSelectShow::Init(CBinary idTitle)
{
	// 根据ID转换成字符串
	string strTitle = adsGetTextString(idTitle);  // 要调用数据库类转换
	if(strTitle.size()<=0)
		strTitle = CDisplay::IdToString(idTitle);

	// 调用重载函数
	Init(strTitle);
}

/***************************************************************************************
功    能：添加多选项内容
参数说明：CBinary idDataStream 数据流ID，取数据流库中数据流内容文本
返 回 值：错误代码，成功为0
说    明：无
**************************************************************************************/
short CMultiSelectShow::Add(CBinary idDataStream)
{
	if(ID_BINARY_LENTH != idDataStream.GetSize())
	{
		return EC_DATA_FORMAT;  // 数据格式错
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
                                             
		// 递增多项选择数量                  
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
功    能：添加多选项内容
参数说明：多选项内容文本
返 回 值：错误代码，成功为0
说    明：无
**************************************************************************************/
short CMultiSelectShow::Add(string strMutiSelectedItem)
{
	m_pSendAndReceive->Add(strMutiSelectedItem);

	// 递增多项选择数量
	m_pSendAndReceive->IncreaseNum(4);

	m_iItemNumber++;  // 实际多项选择数量

	return 0;
}

/***************************************************************************************
功    能：添加多选项的说明
参数说明：内容文本
返 回 值：错误代码，成功为0
说    明：无
**************************************************************************************/
short CMultiSelectShow::AddMsg(string strMsg)
{
	//m_pSendAndReceive->Add(strMsg);
	m_strMsg = strMsg;
	return 0;
}

/***************************************************************************************
功    能：显示多项选择，回送用户按键及多选内容
参数说明：CSelectedItemData& SelectedData 缺省选项及用户选择结果
返 回 值：用户是否按OK键，是-ture 否-false 
说    明：1、在用户选择完成后本函数才返回；
          2、SelectedData中的多选项数量在本函数中根据添加的选项数据自动填加；
**************************************************************************************/
bool CMultiSelectShow::Show(CSelectedItemData& SelectedData)
{
	if(NULL != m_pDataBase)
	{
		m_pDataBase->Close();
		delete(m_pDataBase);
		m_pDataBase = NULL;
	}

	// 以实际添加的多项数量为准
	if(m_iItemNumber != SelectedData.GetItemNumber())
	{
		SelectedData.SetItemNumber(m_iItemNumber);
		m_pSendAndReceive->Modify(m_iItemNumber, 4);
	}

	// 屏幕第一行序号 2006.6.27
	m_pSendAndReceive->Modify(SelectedData.m_i16ScreenFirstLineItemSequence, 4+2);

	// 被选项掩码
	m_pSendAndReceive->Add(SelectedData.m_binMaskCode);

	// 最多可被选项中数量
	if(0xFFFF == SelectedData.m_uiMaxSelectedNumber)
	{
		// 如果没有设置最大数量，以实际添加的为最大项数
		short iItemNumber = SelectedData.GetItemNumber();
		m_pSendAndReceive->Add(iItemNumber);
	}
	else
	{
		m_pSendAndReceive->Add(SelectedData.m_uiMaxSelectedNumber);
	}

	m_pSendAndReceive->SetFlag(0);

	m_pSendAndReceive->Add(m_strMsg);//hpy added 20140113

	// 发送消息
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分
	short iUserKey;
	// 检查数据长度是否正确
	int iLenth = 256*iHigh + iLow;
	if(iLenth < SelectedData.m_binMaskCode.GetSize()+5)
	{
		assert(iLenth < SelectedData.m_binMaskCode.GetSize()+5);
	}	

	// 第5个字节表示:菜单用户选项
	iUserKey = pchContain[4];

	// 保存返回的按用按键
	SelectedData.m_iUserClickButtonKeyValue = iUserKey;

	// 保存屏幕第一行序号 2006.6.27 add
	SelectedData.m_i16ScreenFirstLineItemSequence = (((unsigned char)((pchContain+5)[0]))*256+(unsigned char)((pchContain+5)[1]));

	// 被选项掩码的长度
	short iMaskLenth = SelectedData.m_binMaskCode.GetSize();

	// 复制返回的掩码
//	CBinary binMaskcode((unsigned char*)pchContain+5, iMaskLenth);
	CBinary binMaskcode((unsigned char*)pchContain+5+2, iMaskLenth); //2006.6.27

	// 保存返回的掩码
	SelectedData.m_binMaskCode = binMaskcode;
	if(adsIDOK == iUserKey)
	{
		return true;
	}
	
	return false;
}


/***************************************************************************************
功    能：显示多项选择，回送用户按键及多选内容
参数说明：CSelectedItemData& SelectedData 缺省选项及用户选择结果，iSelectedIndex：用户所选项，iFlag:标志位
返 回 值：用户是否按OK键，是-ture 否-false 
说    明：1、在用户选择完成后本函数才返回；
          2、SelectedData中的多选项数量在本函数中根据添加的选项数据自动填加；
		  3、iSelectedIndex以0为起始，
**************************************************************************************/
bool CMultiSelectShow::Show(CSelectedItemData& SelectedData,short &iSelectedIndex,unsigned char iFlag)
{
	if(NULL != m_pDataBase)
	{
		m_pDataBase->Close();
		delete(m_pDataBase);
		m_pDataBase = NULL;
	}

	// 以实际添加的多项数量为准
	if(m_iItemNumber != SelectedData.GetItemNumber())
	{
		SelectedData.SetItemNumber(m_iItemNumber);
		m_pSendAndReceive->Modify(m_iItemNumber, 4);
	}

	// 屏幕第一行序号 2006.6.27
	m_pSendAndReceive->Modify(SelectedData.m_i16ScreenFirstLineItemSequence, 4+2);

	// 被选项掩码
	m_pSendAndReceive->Add(SelectedData.m_binMaskCode);

	// 最多可被选项中数量
	if(0xFFFF == SelectedData.m_uiMaxSelectedNumber)
	{
		// 如果没有设置最大数量，以实际添加的为最大项数
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

	// 发送消息
	m_pSendAndReceive->SendDataToDisplayWaitResponsion();

	// 获取收到的数据
	char *pchContain = m_pSendAndReceive->GetBuffer();
	assert(NULL != pchContain);

	unsigned char iHigh = (unsigned char)pchContain[0];    // 数据长度的高字节部分
    unsigned char iLow  = (unsigned char)pchContain[1];    // 数据长度的低字节部分
	short iUserKey;
	if(iFlag == 0)
	{
		// 检查数据长度是否正确
		int iLenth = 256*iHigh + iLow;
		if(iLenth < SelectedData.m_binMaskCode.GetSize()+5)
		{
			assert(iLenth < SelectedData.m_binMaskCode.GetSize()+5);
		}	

		// 第5个字节表示:菜单用户选项
		iUserKey = pchContain[4];

		// 保存返回的按用按键
		SelectedData.m_iUserClickButtonKeyValue = iUserKey;

		// 保存屏幕第一行序号 2006.6.27 add
		SelectedData.m_i16ScreenFirstLineItemSequence = (((unsigned char)((pchContain+5)[0]))*256+(unsigned char)((pchContain+5)[1]));

		// 被选项掩码的长度
		short iMaskLenth = SelectedData.m_binMaskCode.GetSize();

		// 复制返回的掩码
	//	CBinary binMaskcode((unsigned char*)pchContain+5, iMaskLenth);
		CBinary binMaskcode((unsigned char*)pchContain+5+2, iMaskLenth); //2006.6.27

		// 保存返回的掩码
		SelectedData.m_binMaskCode = binMaskcode;
	}
	else
	{
		// 检查数据长度是否正确
		int iLenth = 256*iHigh + iLow;
		
		// 第5个字节表示:菜单用户选项
		iUserKey = pchContain[3];

		// 保存返回的按用按键
		SelectedData.m_iUserClickButtonKeyValue = iUserKey;
		// 第5, 6个字节表示:菜单用户选项
		unsigned char chHigh = pchContain[4];    // 菜单用户选项的高字节部分
		unsigned char chLow  = pchContain[5];    // 菜单用户选项的低字节部分
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
功    能：设置最多可被选项中数量
参数说明：unsigned short uiMaxSelectedNumber 最多可被选项中数量
返 回 值：无 
说    明：无
**************************************************************************************/
void CMultiSelectShow::CSelectedItemData::SetMaxSelectedNumber(unsigned short uiMaxSelectedNumber)
{
	CSelectedItemData::m_uiMaxSelectedNumber = uiMaxSelectedNumber;
}

/***************************************************************************************
功    能：测试某一项目是否被选中
参数说明：unsigned short iSequence 被测试项序号
返 回 值：被选中-true 未被选中-false 
说    明：如序号大于总项目数，返回false；
          如序号等于SID_ALL_ITEM，则返回是否存在选中项
**************************************************************************************/
bool CMultiSelectShow::CSelectedItemData::IsSelected (short iSequence)
{
	if(iSequence > CSelectedItemData::m_i16ItemNumber-1)
	{
		return false;
	}
	
	short iIndex = iSequence/8;  // 字节序号
	char chCode = CSelectedItemData::m_binMaskCode.GetAt(iIndex);

	iIndex = (iSequence)%8;	        // 位序号
	char chTest = chCode>>(7-iIndex);
	chTest &= 0x01;

	if(chTest == 0x01)
	{
		return true;
	}

	return false;
}

/***************************************************************************************
功    能：设定多选项目数量
参数说明：unsigned short nNumber 多选项数量
返 回 值：错误代码
说    明：该函数在CMultiSelectShow.Show()中被自动调用，修正成实际的项目数量
**************************************************************************************/
short CMultiSelectShow::CSelectedItemData::SetItemNumber (short nNumber)
{
	short iByteNum = (nNumber-1)/8 + 1;  // 实际需要的掩码长度
	short iMaskLenth = CSelectedItemData::m_binMaskCode.GetSize();

	CSelectedItemData::m_i16ItemNumber = nNumber;

	// 实际需要的掩码长度不够
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
		// 保存旧的掩码
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
功    能：取得多选项目数量
参数说明：无
返 回 值：多选项目数量
说    明：无
**************************************************************************************/
short CMultiSelectShow::CSelectedItemData::GetItemNumber ()
{
	return CSelectedItemData::m_i16ItemNumber;
}

/***************************************************************************************
功    能：设置被选中项
参数说明：unsigned short iSequence 被测试项序号，如为SID_ALL_ITEM则针对所有项有效
         bool IsSelected 是否被选中标志，被选中-true 不被选中-false 
返 回 值：错误代码 
说    明：无
**************************************************************************************/
short CMultiSelectShow::CSelectedItemData::SetSelectedItem (short iSequence, bool IsSelected)
{
	if(iSequence > CSelectedItemData::m_i16ItemNumber-1)
	{
		return EC_OVER_ITEM_NUM;
	}

	// 掩码所能表达的位数
	short iBitSum = 8*m_binMaskCode.GetSize();  

	// 如果长度不够
	if(iSequence > iBitSum-1 )
	{
		return EC_OVER_MASK_LEN;
	}

	// 如果iSequence参数为SID_ALL_ITEM，设置所有为被选中
	if(SID_ALL_ITEM == iSequence)
	{
		for(short iIndex = 0;  iIndex < m_binMaskCode.GetSize(); iIndex++)
		{
			m_binMaskCode.SetAt(iIndex, IsSelected?0xFF:0x00);
		}
	}
	else 
	{	
		short iIndexByte = iSequence/8;         // 字节序号
		char chCode = CSelectedItemData::m_binMaskCode.GetAt(iIndexByte);

		short iIndexBit = (iSequence)%8;	        // 位序号
		char chTemp = 0x01<<(7-iIndexBit);

		m_binMaskCode.SetAt(iIndexByte, IsSelected ? (chCode|chTemp) : (chCode&~chTemp));
	}


	return true;
}


/***************************************************************************************
功    能：取得退出界面时用户按键键值
参数说明：无
返 回 值：用户按键键值 
说    明：该函数返回值与CMultiSelectShow.Show()的返回值相同，建议诊断程序员直接使用CMultiSelectShow.Show()的返回值
**************************************************************************************/
short CMultiSelectShow::CSelectedItemData::UserClickButtonKeyValue ()
{
	return CSelectedItemData::m_iUserClickButtonKeyValue;
}

