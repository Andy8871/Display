#include <math.h>
#include "arithmetic.h"


// 调试状态时允许显示的最大从ECU接收的数据内容
#define DBGFRMMSGLENGTH		(512*3+1)
		
//added by johnnyling start
#define MAX_EXPRESSION_COUNT 4
//added by johnnyling end		
                        
CArithmetic::CArithmetic(void)
{
}

CArithmetic::~CArithmetic(void)
{
}


/******************************************************************************
功    能：	是否允许以调试信息的形式显示
参数说明：	UINT16 uiDebugItem 调试项目，
			bool bEnable 允许状态使能，允许– true 
									   禁止 – false
返 回 值：	无
说    明：	CDataStreamShow和CActiveTestShow类的Show()函数
			设定数据流调试状态
******************************************************************************/
unsigned char CArithmetic::m_ui8Debug;
void  CArithmetic::SetDebugState (unsigned char uiDebugItem, bool bEnable=false)
{
	if(bEnable)
	{
		m_ui8Debug = uiDebugItem;
	}
}


/***********************************************************
功    能：	清空记录ID与算法之间的对应关系的成员变量，释放内存
参数说明：	无
返 回 值：	无
说    明：	无
***********************************************************/
void CArithmetic::Clear (void)
{
	m_vDataStream.clear();
}


/*********************************************************************************
功    能：	添加一项数据流算法相关项内容
参数说明：	CBinary idDataStream 数据流ID 
			vecter<CBinary> &aDsItem ID对应的数据流库内容
返 回 值：	错误代码
说    明：	本函数要检查数据数据流算法是否存在、合法，如不
			能计算则返回小于0的值，如能计算，则将表达式转换
			成计算最容易使用的最快速的中间表达式，并保存之。
**********************************************************************************/
// 数据流内容定义
#define DATASTREAMITEMNUM	 5	// 数据流内容的项数
#define	DSEXPRESSPOS		 3	// 数据流公式表达式的位置
#define DSFORMATPOS			 2	// 数据流显示格式的位置

int CArithmetic::AddDataStreamItem (CBinary idDataStream, vector<CBinary> &aDsItem)
{
	// 空的数据流内容或数据流内容不对
	if (aDsItem.size() < DATASTREAMITEMNUM)
	{
		return DATASTEAM_ERRORDSITEM;
	}

	// 表达式为空
	if (aDsItem[DSEXPRESSPOS].GetSize() == 0)
	{
		return DATASTEAM_NOEXPRESS;
	}

	// 格式控制串为空
	if (aDsItem[DSFORMATPOS].GetSize() == 0)
	{
		return DATASTEAM_NOFORMAT;
	}

	DATASTREAM dsItem;
	dsItem.idDataStream  = idDataStream;
	dsItem.binArithMetic = aDsItem[DSEXPRESSPOS];
	dsItem.binFormat     = aDsItem[DSFORMATPOS];
	m_vDataStream.push_back(dsItem);

	return 0;
}

/***********************************************************************************
功    能：	计算数据流值
参数说明：	const CBinary idDataStream          数据流ID
			const CReceiveFrame & rcvfrmFromEcu 接收帧 
返 回 值：	数据流值
说    明：	1、在RELEASE版，对于存在数据流ID又出现计算错误
			  时，返回值为“N/A”；在DEBUG版，返回值“DS ERROR”。
			2、如果在数据流调试状态下(m_ui8Debug& AD_DataStream
			  为真),回送内容为“程序员编写的算法表达式” +“2个半
			  角空格”+“十六进制表示的接收帧内容”。
***********************************************************************************/
string CArithmetic::DataStream (const CBinary idDataStream, CReceiveFrame& rcvfrmFromEcu)
{
	string strRet;	// 返回字符串

	stack <STACKSTRUCT> sComputeStack;	// 运算栈

	CBinary binDSExp;	// 数据流算法	
	vector<DATASTREAM>::iterator vIter;
	for (vIter = m_vDataStream.begin(); vIter != m_vDataStream.end(); vIter++)
	{
		// 根据数据流ID找到对应的表达式
		if(vIter->idDataStream == idDataStream)
		{
			binDSExp = vIter->binArithMetic;

			if(0 == binDSExp.GetSize())
			{
				return strRet;  // 返回空串
			}
			break;
		}
	}

	if(vIter == m_vDataStream.end())
	{
		// 返回空串
		return strRet;
	}

#ifdef ADS_DEBUG	
	strRet = "DS ERROR";  // DEBUG版本输出
#else	
	strRet = "N/A";  // RELEASE版本输出
#endif

	// 如果在数据流调试状态下(m_ui8Debug&AD_DataStream为真)
	if(m_ui8Debug&AD_DataStream)
	{
		int iLength = binDSExp.GetSize();

		char* pchBuf = (char*)new unsigned char[iLength+1];
		memset(pchBuf, 0, iLength+1);
		memcpy(pchBuf, binDSExp.GetBuffer(), iLength);
		pchBuf[iLength] = '\0';
		strRet = pchBuf;
		strRet += "  ";		// 两个空格
		delete []pchBuf;

		for(int iGroupOrder = 0; iGroupOrder < rcvfrmFromEcu.size(); iGroupOrder++)
		{
			CBinaryGroup binGroup = rcvfrmFromEcu[iGroupOrder];
			
			vector<CBinary>::iterator vBinIter;
			for(vBinIter = binGroup.begin(); vBinIter != binGroup.end(); vBinIter++)
			{
				// 调试状态时允许显示的最大从ECU接收的数据内容
				char temp[DBGFRMMSGLENGTH];  
				memset(temp, 0, DBGFRMMSGLENGTH);

				int iDataLen = vBinIter->GetSize();
				for(int i = 0; i < iDataLen; i++)
				{
					sprintf(temp+3*i, "%.2X ", (unsigned char)((*vBinIter)[i]));
				}
				strRet += temp;
			}
		}
	
		return strRet;
	}

	int iDataLength = binDSExp.GetSize();

	// 取得RPN表达式
	int expLength = iDataLength + 2;
	
	char *pchExpChar = (char*)new unsigned char[expLength];
	memset(pchExpChar, 0, expLength);

	// 在前面加上空格
	pchExpChar[0] = ' ';
	memcpy(pchExpChar+1, binDSExp.GetBuffer(), iDataLength);

	int begin, end;
	begin = 0;
	for (int i = 1; i<expLength; i++)  // 第0个字节恒为" "
	{
		if(pchExpChar[i] != ' ')
			continue;
		
		end = i;  // 记录第2个空格所在的位置

		// 如果在两个分隔符间无内容
		if ((end-begin)<2)
		{
			delete [] pchExpChar;
			return strRet;  // 返回空串
		}

		// 记录单个算法的缓冲
		char *pchMeta = (char*)new unsigned char[end-begin]; 
		memset(pchMeta, 0, end-begin);
		memcpy(pchMeta, pchExpChar + begin + 1, end - begin - 1);

		// 判断是否是字母或数字(A~Z, a~z, 0~9)
		if(isalnum(pchMeta[0]))
		{
			STACKSTRUCT value;
			memset(&value, 0, sizeof(STACKSTRUCT));

			// 如果是数字
			if(isdigit(pchMeta[0]))
			{
				// 没有找到小数点，说明是整数
				char *pdest = strchr(pchMeta, '.');
				if(NULL == pdest)
				{
					value.vt = ADSVT_I4;  // 整型
					value.ival = atoi(pchMeta);  
				}
				else 
				{
					value.vt = ADSVT_R8;  // 双精度
					value.dval = atol(pchMeta);
				}
			}
			else  // 如果是字母，即是关键字G、F、C、B或b
			{	
				switch(pchMeta[0])
				{
					case 'G':
						value.vt = ADSVT_KG;     // 单关键字G类型, 组标识，仅用于表示返回数据中的对应某发送单帧数据的一组FRAME的集合
						break; 
			
					case 'C':
						value.vt = ADSVT_KCHAR;	 // 单关键字C类型, CHAR有符号单字节数据标识
						break;
					
					case 'F':
						value.vt = ADSVT_KF;	 //单关键字F类型, FRAME：帧标识，二进制数据集合，通常代表一条收发命令或数据
						break;
				
					case 'B':
						value.vt = ADSVT_KBYTE;	 //单关键字B类型, unsigned char：无符号单字节数据标识
						break;

					case 'b':
						value.vt = ADSVT_KBIT;	 //单关键字b类型,  BIT：字节中的数据位标识
						break;

					default:
						delete [] pchMeta;
						delete [] pchExpChar;
						return strRet;  // 返回空串
				}
			}
			
			// 压入运算表达式栈
			sComputeStack.push(value);	
		}
		else  // 如果是运算符
		{
			bool bSuccess = false;
			bSuccess = ComputeExpress(sComputeStack, pchMeta, rcvfrmFromEcu);
			if (false == bSuccess)
			{
				delete [] pchExpChar;
				return strRet;
			}
		}
		
		delete [] pchMeta;
		begin = end;

	} // end for

	if(sComputeStack.size() != 1)
	{
		delete [] pchExpChar;
		return strRet;  // 返回空串
	}
	
	// 数据流格式为空
	if (0 == vIter->binFormat.GetSize())
	{
		delete [] pchExpChar;
		return strRet;
	}

	int strFmtLength = vIter->binFormat.GetSize();
	char *pchFormat = (char*)new unsigned char[strFmtLength+1];
	char *pchValue	= (char*)new unsigned char[strFmtLength+16];

	// 复制数据流格式到缓冲
	memset(pchFormat, 0, strFmtLength+1);
	memcpy(pchFormat, vIter->binFormat.GetBuffer(), strFmtLength);

	// 从栈顶取得运算结果
	STACKSTRUCT Val = sComputeStack.top();
	if(Val.vt<7) // 整数型
	{
		int value;
		switch(Val.vt)
		{
			case ADSVT_I1:               // char
			case ADSVT_KCHAR:            // 单关键字C类型
				value = (int)Val.cval;
				break;

			case ADSVT_I4:                // int
				value = (int)Val.ival;
				break;

			case ADSVT_UI1:               // unsigned char
			case ADSVT_KBYTE:             // 单关键字B类型
				value = (unsigned char)Val.ucval;
				break;

			case ADSVT_UI4:               // unsigned int
				value = (int)Val.uival;
				break;

			default:
				delete [] pchExpChar;
				return strRet;
		}
		sprintf(pchValue, pchFormat, value);
	}
	else if(Val.vt >10)//浮点运算
	{
		double value;
		if(Val.vt == ADSVT_R4)            // float
		{
			value = (double)Val.fval;
		}
		else if(Val.vt == ADSVT_R8)       // double
		{
			value = Val.dval;
		}
		else
		{
			delete [] pchExpChar;
			return strRet;  // 返回空串
		}

		sprintf(pchValue, pchFormat, value);
	}
	else // 如果类型为7、8、9或10，则出错
	{
		delete [] pchExpChar;
		return strRet;
	}

	strRet.append(pchValue);

	delete [] pchValue;
	delete [] pchFormat;
	delete [] pchExpChar;

	return strRet; // 正确返回
}

//added by johnnyling 
//扩展的读数据流成员函数

string CArithmetic::DataStreamEx(const CBinary idDataStream, CReceiveFrame& rcvfrmFromEcu)
{
	string strRet;	// 返回字符串

	stack <STACKSTRUCT> sComputeStack;	// 运算栈

	CBinary binDSExp;
	CBinary binFormat;
	vector<CBinary> listBinExp;

	vector<DATASTREAM>::iterator vIter;

	for (vIter = m_vDataStream.begin(); vIter != m_vDataStream.end(); vIter++)
	{
		// 根据数据流ID找到对应的表达式
		if(vIter->idDataStream == idDataStream)
		{
			binDSExp  = vIter->binArithMetic;
			binFormat = vIter->binFormat;

			int nCount=GetExpList(binDSExp,listBinExp);

			if(nCount>0)
			{
				//if(binFormat.GetSize()<2)
				//	strRet = "USERHANDLE"; //格式化输出字符串为空，需要用户自己处理数据流算法，凌义川
				//else
					strRet = ComputeExpressEx(listBinExp,binFormat,rcvfrmFromEcu);
			}
			else
			{
				strRet = "@@??@@"; //表达式为空，需要用户自己处理数据流算法，凌义川

				for(int i=0;i<binFormat.GetSize();i++)
					strRet+=(char)binFormat[i];
			}

			break;
		}
	}

	return strRet; // 正确返回
}

/***********************************************************************************
功    能：	根据所给的运算式、栈及数据帧进行数据计算数据流值
参数说明：	stack<string>& strExpress 运算分量栈
			char* meta	运算符
			const CReceiveFrame & rcvfrmFromEcu 数据帧 
返 回 值：	true	-进行了正确的运算
			false	-运算过程出错
说    明：	无
***********************************************************************************/
bool CArithmetic::ComputeExpress (stack<STACKSTRUCT>& sComputeStack, char* meta, CReceiveFrame& rcvfrmFromEcu)
{
	// 所支持的运算符
	char* sOperator[]=
	{
		"?", ":",  "||", "&&", "|",
		"^", "&",  "!=", "==", ">=",
		">", "<=", "<",  ">>", "<<",
		"-", "+",  "%",  "/",  "*",
		"-", "!",  "~",  "#", ".",
	};

	static bool bConditionExp = false;

	int op = 0;
	int iIndex;
	for(iIndex = 0; iIndex < 25; iIndex++)
	{
		// 找到运算符的序号
		if(strcmp(sOperator[iIndex], meta) == 0)
		{
			op = iIndex;
			break;
		}
	}
	
	if(iIndex == 25)
	{
		return false;  // 没有找到运算符
	}
	
	bool bSingle = false;	
	
	// 单目运算符 '!'， '~'
	if(iIndex == 21 || iIndex == 22) 
	{
		bSingle = true;  
	}

	// 运算分量
	STACKSTRUCT v1, v2, vResult,vTop;

	//added by johnnyling start
	memset((unsigned char *)&vResult,0,sizeof(STACKSTRUCT));
	//added by johnnyling end

	// 如果是双目运算符
	if(!bSingle)
	{
		// 取出一个运算量
		v2 = sComputeStack.top();
		sComputeStack.pop();
	}

	// 第二个运算量
	v1 = sComputeStack.top();

	sComputeStack.pop();



	// 双目运算符
	if(!bSingle)
	{
		// 整形运算
		if ((v1.vt<11) && (v2.vt<11))
		{
			int i1, i2;
			switch(v1.vt)
			{
				case ADSVT_I1:      // char
				{
					i1 = (int)v1.cval;
					break;
				}

				case ADSVT_I4:		// int
				case ADSVT_KG:		// 单关键字G类型
				case ADSVT_KGF:		// 复合关键字G.F类型
				case ADSVT_KF:		// 单关键字F类型
				case ADSVT_KCHAR:	// 单关键字C类型
				case ADSVT_KBYTE:   // 单关键字B类型
				case ADSVT_KBIT:    // 单关键字b类型
				{
					i1 = (int)v1.ival;
					break;
				}

				case ADSVT_UI1:     // unsigned char
				{
					i1 = (int)v1.ucval;
					break;
				}

				case ADSVT_UI4:     // unsigned int
				{
					i1 = (int)v1.uival;
					break;
				}

				default:
					return false;
			}

			switch(v2.vt)
			{
				case ADSVT_I1:
					i2 = (int)v2.cval;
					break;

				case ADSVT_I4:
				case ADSVT_KG:
				case ADSVT_KGF:
				case ADSVT_KF:
				case ADSVT_KCHAR:
				case ADSVT_KBYTE:
				case ADSVT_KBIT:
					i2 = (int)v2.ival;
					break;

				case ADSVT_UI1:
					i2 = (int)v2.ucval;
					break;

				case ADSVT_UI4:
					i2 = (int)v2.uival;
					break;

				default:
					return false;
			}

			switch(op)
			{
				case CONDITION_QS: //     0  //条件表达式: 问号
				{
					bConditionExp = false;	//先置为false
					if(i1 != 0)			//表达式为真
					{
						bConditionExp = true;
					}
					sComputeStack.push(v2);
					return true;
				}

				case CONDITION_CO: //	 1	//条件表达式：分号 －不用做任何处理
				{
					if(bConditionExp)		//如果表达式为真则取前值
					{
						sComputeStack.push(v1);
					}
					else					//表达式为假取后值
					{
						sComputeStack.push(v2);					
					}
				
					// 恢复条件表达式的值
					bConditionExp = false;
					return true;
				}

			case LOGICAL_OR: //      2  //逻辑或	
				vResult.ucval = i1 || i2;
				vResult.vt = ADSVT_UI1;
				break;

			case LOGICAL_AND: //     3  //逻辑与
				vResult.ucval = i1 && i2;
				vResult.vt = ADSVT_UI1;
				break;

			case BIT_OR: //          4  //按位或																																							
				vResult.ival = i1|i2;
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case BIT_XOR: //         5  //按位异或																																							
				vResult.ival = i1 ^ i2;
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case BIT_AND: //         6  //按位与																																							
				vResult.ival = i1 & i2;
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case NOT_EQUAL: //       7  //不等于																																							
				vResult.ucval = (i1 != i2);
				vResult.vt = ADSVT_UI1;
				break;

			case EQUAL: //           8  //等于																																								
				vResult.ucval = (i1 == i2);
				vResult.vt = ADSVT_UI1;
				break;

			case GREATER_EQ	: // 	 9 //大于等于																																							
				vResult.ucval = (i1 >= i2);
				vResult.vt = ADSVT_UI1;
				break;

			case GREATER_THAN: //	10  //大于																																								
				vResult.ucval = (i1 > i2);
				vResult.vt = ADSVT_UI1;
				break;

			case LESS_EQ: //		11  //小于等于																																							
				vResult.ucval = (i1 <= i2);
				vResult.vt = ADSVT_UI1;
				break;

			case LESS_THAN: //		12  //小于																																								
				vResult.ucval = (i1 < i2);
				vResult.vt = ADSVT_UI1;
				break;

			case SHFT_RIGHT: //		13  //按位右移																																							
				vResult.ival =  (i1 >> (unsigned int)i2);
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case SHFT_LEFT: //		14  //按位左移																																							
				vResult.ival =  (i1 << (unsigned int)i2);
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case SUB: //			15  //减																																								
				vResult.ival =  i1 - i2;
				vResult.vt = ADSVT_I4;
				break;

			case ADD: //			16  //加																																								
				vResult.ival = i1 + i2;
				vResult.vt = ADSVT_I4;
				break;

			case MODULUS: //		17  //取模求余																																							
				if(!i2)
				{
					return false;
				}

				vResult.ival = i1 % i2;
				vResult.vt = ADSVT_I4;
				break;

			case DIVIDE: //			18  //除																																								
				if(!i2)
				{
					return false;
				}

				vResult.ival =  i1 / i2;
				vResult.vt = ADSVT_I4;
				break;

			case MULTIPLY: //		19  //乘                																																				
				vResult.ival =  i1 * i2;
				vResult.vt = ADSVT_I4;
				break;

			case SUFFIX: //			23  //下标运算	

				//如果栈为空或者栈顶不是FRAME[],直接取默认帧对应的字节，否则取其索引值
				if(sComputeStack.empty())
				{
					//added by johnnyling start
					if(v1.vt==ADSVT_KBYTE)
					{
						if(i2<rcvfrmFromEcu[0][0].GetSize())
						{
							vResult.ucval=rcvfrmFromEcu[0][0][i2];	
							vResult.vt=ADSVT_UI1;
						}
						else
							return false;
					}
					else if(v1.vt==ADSVT_KCHAR)
					{
						if(i2<rcvfrmFromEcu[0][0].GetSize())
						{
							vResult.cval=(char)rcvfrmFromEcu[0][0][i2];	
							vResult.vt=ADSVT_I1;
						}
						else
							return false;
					}
					else
					{
						vResult.vt=v1.vt;
						vResult.ival = i2;
					}
				}
				else
				{				
					vTop=sComputeStack.top();

					//added by johnnyling start
					if(v1.vt==ADSVT_KBYTE && vTop.vt!= ADSVT_KF && vTop.vt!=ADSVT_KGF)
					{
						if(i2<rcvfrmFromEcu[0][0].GetSize())
						{
							vResult.ucval=rcvfrmFromEcu[0][0][i2];	
							vResult.vt=ADSVT_UI1;
						}
						else
							return false;
					}
					else if(v1.vt==ADSVT_KCHAR && vTop.vt!= ADSVT_KF && vTop.vt!=ADSVT_KGF)
					{
						if(i2<rcvfrmFromEcu[0][0].GetSize())
						{
							vResult.cval=(char)rcvfrmFromEcu[0][0][i2];	
							vResult.vt=ADSVT_I1;
						}
						else
							return false;
					}
					else
					{
						vResult.vt=v1.vt;
						vResult.ival = i2;
					}
				}
				//added by johnnyling end
				
				break;

			case MEMBER_SEL: //		24  //成员选择(存在的情况：G.F、KGF.B、KGF.C、F.B、F.C、B.b 或 数据.b)
				if ((v1.vt == ADSVT_KG) && (v2.vt == ADSVT_KF))	//G.F
				{
					vResult.vt = ADSVT_KGF;		//中间运算类型
					vResult.ival = (v1.ival<<16) + v2.ival;//高16位存组号，低16位存帧号
				}
				else if ((v1.vt == ADSVT_KGF) && (v2.vt == ADSVT_KCHAR))	//KGF.C
				{
					int group = (v1.ival>>16)&0XFFFF;//高16位存组号
					int frame = (v1.ival)&0xFFFF;	//低16位存帧号

					if(group>=rcvfrmFromEcu.size())
						return false;

					if(frame>=rcvfrmFromEcu[group].size())
						return false;

					if(i2>=rcvfrmFromEcu[group][frame].GetSize())
						return false;

					vResult.cval = rcvfrmFromEcu[group][frame][i2];
					vResult.vt = ADSVT_I1;
				}
				else if ((v1.vt == ADSVT_KGF) && (v2.vt == ADSVT_KBYTE))	//KGF.B
				{
					int group = (v1.ival>>16)&0XFFFF;//高16位存组号
					int frame = (v1.ival)&0xFFFF;	//低16位存帧号

					if(group>=rcvfrmFromEcu.size())
						return false;

					if(frame>=rcvfrmFromEcu[group].size())
						return false;

					if(i2>=rcvfrmFromEcu[group][frame].GetSize())
						return false;

					vResult.ucval = rcvfrmFromEcu[group][frame][i2];
					vResult.vt = ADSVT_UI1;
				}
				else if ((v1.vt == ADSVT_KF) && (v2.vt == ADSVT_KCHAR))	//F.C
				{
					if(i1>=rcvfrmFromEcu[0].size())
						return false;

					if(i2>=rcvfrmFromEcu[0][i1].GetSize())
						return false;

					vResult.cval = rcvfrmFromEcu[0][i1][i2];
					vResult.vt = ADSVT_I1;
				}
				else if ((v1.vt == ADSVT_KF) && (v2.vt == ADSVT_KBYTE))	//F.B
				{
					CBinary binData;

					if(i1<rcvfrmFromEcu[0].size())
						binData = rcvfrmFromEcu[0][i1];
					else
						return false;

					if(i2<binData.GetSize())
					{
						vResult.ucval = binData[i2];
						vResult.vt = ADSVT_UI1;
					}
					else
						return false;
				}
				else if ((v1.vt == ADSVT_KBYTE) && (v2.vt == ADSVT_KBIT))	//B.b
				{
					if(i1<rcvfrmFromEcu[0][0].GetSize())
					{
						vResult.ucval =rcvfrmFromEcu[0][0][i1] & (unsigned char)(1<<i2);
						vResult.vt = ADSVT_UI1;
					}
					else
						return false;
				}
				else if ((v1.vt == ADSVT_UI1) && (v2.vt == ADSVT_KBIT))		//数据.b
				{
					//modified by johnnyling start
					//vResult.ucval = i1 & (unsigned char)(1<<i2);
					vResult.ucval = (i1 & (unsigned char)(1<<i2))?1:0;
					//modified by johnnyling end
					vResult.vt = ADSVT_UI1;
				}
				else
				{
					return false;
				}

				break;

			default:
				return false;
			}
		}
		else
		{
			// 浮点双目运算
			double d1, d2;
			switch(v1.vt)
			{
				case ADSVT_I1:					// char
					d1 = (double)v1.cval;
					break;
				case ADSVT_I4:                  // int
					d1 = (double)v1.ival;
					break;

				case ADSVT_UI1:                 // unsigned char
					d1 = (double)v1.ucval;
					break;

				case ADSVT_UI4:                 // unsigned int
					d1 = (double)v1.uival;
					break;

				case ADSVT_R4:	                // float
					d1 = (double)v1.fval;
					break;

				case ADSVT_R8:	                // double
					d1 = v1.dval;
					break;

				default:
					return false;
			}

			switch(v2.vt)
			{
				case ADSVT_I1:
					d2 = (double)v2.cval;
					break;

				case ADSVT_I4:
					d2 = (double)v2.ival;
					break;


				case ADSVT_UI1:
					d1 = (double)v2.ucval;
					break;

				case ADSVT_UI4:
					d2 = (double)v2.uival;
					break;

				case ADSVT_R4:	
					d2 = (double)v2.fval;
					break;

				case ADSVT_R8:	
					d2 = v2.dval;
					break;

				default:
					return false;
			}

			switch(op)
			{
				case NOT_EQUAL: //       7  //不等于																																							
					//vResult.ucval = (abs(d1-d2)>(double)0.000000001);
					vResult.ucval = (fabs(d1-d2)>(double)0.000000001);
					vResult.vt = ADSVT_UI1;
					break;

				case EQUAL: //           8  //等于																																								
					//vResult.ucval = (abs(d1-d2)<=(double)0.000000001);
					vResult.ucval = (fabs(d1-d2)<=(double)0.000000001);
					vResult.vt = ADSVT_UI1;
					break;

			    case GREATER_EQ	: // 	 9 //大于等于																																							
					vResult.ucval = (d1 >= d2);
					vResult.vt = ADSVT_UI1;
					break;

				case GREATER_THAN: //	10  //大于																																								
					vResult.ucval = (d1 > d2);
					vResult.vt = ADSVT_UI1;
					break;

				case LESS_EQ: //		11  //小于等于																																							
					//vResult.ucval = (d1 < d2) || (abs(d1-d2)<=(double)0.000000001);
					vResult.ucval = (d1 < d2) || (fabs(d1-d2)<=(double)0.000000001);
					vResult.vt = ADSVT_UI1;
					break;

				case LESS_THAN: //		12  //小于																																								
					//vResult.ucval = (d1 < d2) && (abs(d1-d2)>(double)0.000000001);
					vResult.ucval = (d1 < d2) && (fabs(d1-d2)>(double)0.000000001);
					vResult.vt = ADSVT_UI1;
					break;

				case SUB: //			15  //减																																								
					vResult.dval =  d1 - d2;
					vResult.vt = ADSVT_R8;
					break;

				case ADD: //			16  //加																																								
					vResult.dval =  d1 + d2;
					vResult.vt = ADSVT_R8;
					break;

				case DIVIDE: //			18  //除																																								
					//if( abs(d2) <= 0.000000001)//d2为0值
					if(fabs(d2) <= 0.000000001)//d2为0值
					{
						return false;
					}

					vResult.dval =  d1/d2;
					vResult.vt = ADSVT_R8;
					break;

				case MULTIPLY: //		19  //乘                																																				
					vResult.dval = d1 * d2;
					vResult.vt = ADSVT_R8;
					break;

				default:
					return false;
			}
		}//		else{//浮点双目运算
	}
	else  //单目运算符
	{
		int i1;
		switch(v1.vt)
		{
			case ADSVT_I1:
				i1 = (int)v1.cval;
				break;

			case ADSVT_I4:
				case ADSVT_KG:
				case ADSVT_KGF:
				case ADSVT_KF:
				case ADSVT_KCHAR:
				case ADSVT_KBYTE:
				case ADSVT_KBIT:
				i1 = (int)v1.ival;
				break;

			case ADSVT_UI1:
				i1 = (int)v1.ucval;

				//added by johnnyling start
				break;
				//added by johnnyling end

			case ADSVT_UI4:
				i1 = (int)v1.uival;
				break;

			case ADSVT_R4:	//若是浮点，则强制为整形
				i1 = (int)v1.fval;
				break;

			case ADSVT_R8:
				i1 = (int)v1.dval;
				break;
				
			default:
				return false;
		}

		switch(op)
		{
			case LOGICAL_NOT: //	21  //逻辑非	单目运算符
				vResult.ucval =  !i1;
				vResult.vt = ADSVT_UI1;  // unsigned char
				break;

			case BIT_NOT: //		22  //按位取反	单目运算符
				//modified by johnnyling start
				//v1.ival =  ~i1;
				vResult.ucval =  ~(unsigned char)i1;
				vResult.vt = ADSVT_UI1;
				//modified by johnnyling end
				break;

			default:
				return false;
		}

	}//	else{//单目运算符

	sComputeStack.push(vResult);
	return true;
} 

#if 0
string CArithmetic::ComputeTest(CBinary binDSExp, CBinary binFormat, CReceiveFrame rcvfrmFromEcu)
{
	//vector<DATASTREAM>::iterator vIter;
	string strRet;	// 返回字符串
	stack <STACKSTRUCT> sComputeStack;	// 运算栈
	int iDataLength = binDSExp.GetSize();

	// 取得RPN表达式
	int expLength = iDataLength + 2;
	
	char *pchExpChar = (char*)new unsigned char[expLength];
	memset(pchExpChar, 0, expLength);

	// 在前面加上空格
	pchExpChar[0] = ' ';
	memcpy(pchExpChar+1, binDSExp.GetBuffer(), iDataLength);

	int begin, end;
	begin = 0;
	for (int i = 1; i<expLength; i++)  // 第0个字节恒为" "
	{
		if(pchExpChar[i] != ' ')
			continue;
		
		end = i;  // 记录第2个空格所在的位置

		// 如果在两个分隔符间无内容
		if ((end-begin)<2)
		{
			delete [] pchExpChar;
			return strRet;  // 返回空串
		}

		// 记录单个算法的缓冲
		char *pchMeta = (char*)new unsigned char[end-begin]; 
		memset(pchMeta, 0, end-begin);
		memcpy(pchMeta, pchExpChar + begin + 1, end - begin - 1);

		// 判断是否是字母或数字(A~Z, a~z, 0~9)
		if(isalnum(pchMeta[0]))
		{
			STACKSTRUCT value;
			memset(&value, 0, sizeof(STACKSTRUCT));

			// 如果是数字
			if(isdigit(pchMeta[0]))
			{
				// 没有找到小数点，说明是整数
				char *pdest = strchr(pchMeta, '.');
				if(NULL == pdest)
				{
					value.vt = ADSVT_I4;  // 整型
					value.ival = atoi(pchMeta);  
				}
				else 
				{
					value.vt = ADSVT_R8;  // 双精度

					//modified by johnnyling start
					//value.dval = atol(pchMeta);
					value.dval = atof(pchMeta);
					//modified by johnnyling end
				}
			}
			else  // 如果是字母，即是关键字G、F、C、B或b
			{	
				//added by johnnying start
				memset((unsigned char *)&value,0,sizeof(STACKSTRUCT));
				//added by johnnyling end

				switch(pchMeta[0])
				{
					case 'G':
						value.vt = ADSVT_KG;     // 单关键字G类型, 组标识，仅用于表示返回数据中的对应某发送单帧数据的一组FRAME的集合
						break; 
			
					case 'C':
						value.vt = ADSVT_KCHAR;	 // 单关键字C类型, CHAR有符号单字节数据标识
						break;
					
					case 'F':
						value.vt = ADSVT_KF;	 //单关键字F类型, FRAME：帧标识，二进制数据集合，通常代表一条收发命令或数据
						break;
				
					case 'B':
						value.vt = ADSVT_KBYTE;	 //单关键字B类型, unsigned char：无符号单字节数据标识
						break;

					case 'b':
						value.vt = ADSVT_KBIT;	 //单关键字b类型,  BIT：字节中的数据位标识
						break;

					default:
						delete [] pchMeta;
						delete [] pchExpChar;
						return strRet;  // 返回空串
				}
			}
			
			// 压入运算表达式栈
			sComputeStack.push(value);	
		}
		else  // 如果是运算符
		{
			bool bSuccess = false;
			bSuccess = ComputeExpress(sComputeStack, pchMeta, rcvfrmFromEcu); //
			if (false == bSuccess)
			{
				delete [] pchExpChar;
				return strRet;
			}
		}
		
		delete [] pchMeta;
		begin = end;

	} // end for

	if(sComputeStack.size() != 1)
	{
		delete [] pchExpChar;
		return strRet;  // 返回空串
	}
	
	// 数据流格式为空
	if (0 == binFormat.GetSize())
	{
		delete [] pchExpChar;
		return strRet;
	}

	int strFmtLength = binFormat.GetSize();
	char *pchFormat = (char*)new unsigned char[strFmtLength+1];
	char *pchValue	= (char*)new unsigned char[strFmtLength+16];

	// 复制数据流格式到缓冲
	memset(pchFormat, 0, strFmtLength+1);
	memcpy(pchFormat, binFormat.GetBuffer(), strFmtLength);

	// 从栈顶取得运算结果
	STACKSTRUCT Val = sComputeStack.top();
	if(Val.vt<7) // 整数型
	{
		int value;
		switch(Val.vt)
		{
			case ADSVT_I1:               // char
			case ADSVT_KCHAR:            // 单关键字C类型
				value = (int)Val.cval;
				break;

			case ADSVT_I4:                // int
				value = (int)Val.ival;
				break;

			case ADSVT_UI1:               // unsigned char
			case ADSVT_KBYTE:             // 单关键字B类型
				value = (unsigned char)Val.ucval;
				break;

			case ADSVT_UI4:               // unsigned int
				value = (int)Val.uival;
				break;

			default:
				delete [] pchExpChar;
				return strRet;
		}
		sprintf(pchValue, pchFormat, value);
	}
	else if(Val.vt >10)//浮点运算
	{
		double value;
		if(Val.vt == ADSVT_R4)            // float
		{
			value = (double)Val.fval;
		}
		else if(Val.vt == ADSVT_R8)       // double
		{
			value = Val.dval;
		}
		else
		{
			delete [] pchExpChar;
			return strRet;  // 返回空串
		}

		sprintf(pchValue, pchFormat, value);
	}
	else // 如果类型为7、8、9或10，则出错
	{
		delete [] pchExpChar;
		return strRet;
	}

	strRet.append(pchValue);

	delete [] pchValue;
	delete [] pchFormat;
	delete [] pchExpChar;

	return strRet; // 正确返回
}
#endif

int	CArithmetic::ComputeExpItem(CBinary binDSExp,CReceiveFrame rcvfrmFromEcu,STACKSTRUCT &retVal)
{
	stack <STACKSTRUCT> sComputeStack;	// 运算栈
	int iDataLength = binDSExp.GetSize();

	// 取得RPN表达式
	int expLength = iDataLength + 2;
	
	char *pchExpChar = (char*)new unsigned char[expLength];
	memset(pchExpChar, 0, expLength);

	// 在前面加上空格
	pchExpChar[0] = ' ';
	memcpy(pchExpChar+1, binDSExp.GetBuffer(), iDataLength);

	int begin, end;
	begin = 0;
	for (int i = 1; i<expLength; i++)  // 第0个字节恒为" "
	{
		if(pchExpChar[i] != ' ')
			continue;
		
		end = i;  // 记录第2个空格所在的位置

		// 如果在两个分隔符间无内容
		if ((end-begin)<2)
		{
			delete [] pchExpChar;
			return 0;  // 返回空串
		}

		// 记录单个算法的缓冲
		char *pchMeta = (char*)new unsigned char[end-begin]; 
		memset(pchMeta, 0, end-begin);
		memcpy(pchMeta, pchExpChar + begin + 1, end - begin - 1);

		// 判断是否是字母或数字(A~Z, a~z, 0~9)
		if(isalnum(pchMeta[0]))
		{
			STACKSTRUCT value;
			memset(&value, 0, sizeof(STACKSTRUCT));

			// 如果是数字
			if(isdigit(pchMeta[0]))
			{
				// 没有找到小数点，说明是整数
				char *pdest = strchr(pchMeta, '.');
				if(NULL == pdest)
				{
					value.vt = ADSVT_I4;  // 整型
					value.ival = atoi(pchMeta);  
				}
				else 
				{
					value.vt = ADSVT_R8;  // 双精度

					//modified by johnnyling start
					//value.dval = atol(pchMeta);
					value.dval = atof(pchMeta);
					//modified by johnnyling end
				}
			}
			else  // 如果是字母，即是关键字G、F、C、B或b
			{	
				//added by johnnying start
				memset((unsigned char *)&value,0,sizeof(STACKSTRUCT));
				//added by johnnyling end

				switch(pchMeta[0])
				{
					case 'G':
						value.vt = ADSVT_KG;     // 单关键字G类型, 组标识，仅用于表示返回数据中的对应某发送单帧数据的一组FRAME的集合
						break; 
			
					case 'C':
						value.vt = ADSVT_KCHAR;	 // 单关键字C类型, CHAR有符号单字节数据标识
						break;
					
					case 'F':
						value.vt = ADSVT_KF;	 //单关键字F类型, FRAME：帧标识，二进制数据集合，通常代表一条收发命令或数据
						break;
				
					case 'B':
						value.vt = ADSVT_KBYTE;	 //单关键字B类型, unsigned char：无符号单字节数据标识
						break;

					case 'b':
						value.vt = ADSVT_KBIT;	 //单关键字b类型,  BIT：字节中的数据位标识
						break;

					default:
						delete [] pchMeta;
						delete [] pchExpChar;
						return 0;  // 返回空串
				}
			}
			
			// 压入运算表达式栈
			sComputeStack.push(value);	
		}
		else  // 如果是运算符
		{
			bool bSuccess = false;
			bSuccess = ComputeExpress(sComputeStack, pchMeta, rcvfrmFromEcu);
			if (false == bSuccess)
			{
				delete [] pchMeta;
				delete [] pchExpChar;
				return 0;
			}
		}
		
		delete [] pchMeta;
		begin = end;

	} // end for

	if(sComputeStack.size() != 1)
	{
		delete [] pchExpChar;
		return 0;  // 返回空串
	}
	
	// 从栈顶取得运算结果
	STACKSTRUCT Val = sComputeStack.top();
	if(Val.vt<7) // 整数型
	{
		int value;
		switch(Val.vt)
		{
			case ADSVT_I1:               // char
			case ADSVT_KCHAR:            // 单关键字C类型
				value = (int)Val.cval;
				break;

			case ADSVT_I4:                // int
				value = (int)Val.ival;
				break;

			case ADSVT_UI1:               // unsigned char
			case ADSVT_KBYTE:             // 单关键字B类型
				value = (unsigned char)Val.ucval;
				break;

			case ADSVT_UI4:               // unsigned int
				value = (int)Val.uival;
				break;

			default:
				delete [] pchExpChar;
				return 0;
		}
	}
	else if(Val.vt >10)//浮点运算
	{
		double value;
		if(Val.vt == ADSVT_R4)            // float
		{
			value = (double)Val.fval;
		}
		else if(Val.vt == ADSVT_R8)       // double
		{
			value = Val.dval;
		}
		else
		{
			delete [] pchExpChar;
			return 0;  // 返回空串
		}
	}
	else // 如果类型为7、8、9或10，则出错
	{
		delete [] pchExpChar;
		return 0;
	}

	delete [] pchExpChar;

	memcpy(&retVal,&Val,sizeof(STACKSTRUCT));

	return 1; // 正确返回

}

//如果是列表选择，则格式如下：
//@@TABLE:XXX@XXX@XXX
//@@TABLE:打头
//每一项目之间用@分隔
int CArithmetic::GetTableList(CBinary binFormat, vector<string>& listTable)
{
	int nSize=binFormat.GetSize();
	int nStart;
	int nIndex;
	int i;

	string strTable;

	if(nSize<8)
		return 0;
	else
	{
		//判断前面是否是标志：@@TABLE:
		CBinary binFlag;

		for(i=0;i<8;i++)
			binFlag+=binFormat[i];

		if(!(binFlag==CBinary("@@TABLE:",8)))
			return 0;

		//搜索有几个选择项目

		nStart=8;
		nIndex=8;
		strTable="";

		while(nIndex<nSize)
		{	
			//@分隔多个表达式
			if(binFormat[nIndex]=='@')
			{
				for(i=0;i<nIndex-nStart;i++)
				{
					strTable+=(char)binFormat[nStart + i];
				}

				listTable.push_back(strTable);

				strTable="";
				nStart=nIndex+1;
			}

			nIndex++;
		}

		if(listTable.size()==0) //至少需要包含两项
		{
			return 0;
		}
		else
		{
			if(nStart<nSize) //把剩余部分作为一个表达式添加
			{
				strTable="";
				for(i=0;i<nSize-nStart;i++)
				{
					//字符串结束了
					if(binFormat[nStart + i]==0)
						break;

					strTable+=(char)binFormat[nStart + i];
				}

				listTable.push_back(strTable);
			}
		}

		return listTable.size();
	}
}

string CArithmetic::ComputeExpressEx(vector<CBinary> binExpList, CBinary binFormat, CReceiveFrame RcvFrame)
{
	vector<string> listTable;
	int			   nIndex;
	int			   nSize;

	STACKSTRUCT			retVal;
	vector<STACKSTRUCT> listRetVal;

	string				strRet;

	int		listIntVal[MAX_EXPRESSION_COUNT];
	double	listFloatVal[MAX_EXPRESSION_COUNT];

	int i;

	// 数据流格式为空
	if (0 == binFormat.GetSize())
		return "";

	if(GetTableList(binFormat,listTable)) //格式是真值表
	{
		if(binExpList.size()!=1)
			return "";
		else
		{
			if(ComputeExpItem(binExpList[0],RcvFrame,retVal)) //计算成功
			{
				//只有数值的类型才能从表中取值
				switch(retVal.vt)
				{
					case ADSVT_I1:               // char
					case ADSVT_KCHAR:            // 单关键字C类型
						nIndex = (int)retVal.cval;
						break;

					case ADSVT_I4:                // int
						nIndex = (int)retVal.ival;
						break;

					case ADSVT_UI1:               // unsigned char
					case ADSVT_KBYTE:             // 单关键字B类型
						nIndex = (unsigned char)retVal.ucval;
						break;

					case ADSVT_UI4:               // unsigned int
						nIndex = (int)retVal.uival;
						break;

					default:
						return "";
				}

				if(nIndex>=listTable.size())
					return "----";
				else
					return listTable[nIndex];
			}
			else
				return "";
		}
	}
	else if(binExpList.size()<=MAX_EXPRESSION_COUNT) //最多只能处理4个表达式
	{
		for(i=0;i<binExpList.size();i++)
		{
			if(ComputeExpItem(binExpList[i],RcvFrame,retVal)) //计算成功
			{
				//如果结果类型是7，8，9，10则出错。
				if(retVal.vt>=7 && retVal.vt<=10)
					return "";
				else
				{
					listRetVal.push_back(retVal);

					//记住每个结果的值
					if(retVal.vt<7) // 整数型
					{
						int value;
						switch(retVal.vt)
						{
							case ADSVT_I1:               // char
							case ADSVT_KCHAR:            // 单关键字C类型
								value = (int)retVal.cval;
								break;

							case ADSVT_I4:                // int
								value = (int)retVal.ival;
								break;

							case ADSVT_UI1:               // unsigned char
							case ADSVT_KBYTE:             // 单关键字B类型
								value = (unsigned char)retVal.ucval;
								break;

							case ADSVT_UI4:               // unsigned int
								value = (int)retVal.uival;
								break;
						}

						listIntVal[i]=value;
						listFloatVal[i]=0.0;
					}
					else if(retVal.vt >10)//浮点运算
					{
						double value;
						if(retVal.vt == ADSVT_R4)            // float
						{
							value = (double)retVal.fval;
						}
						else if(retVal.vt == ADSVT_R8)       // double
						{
							value = retVal.dval;
						}

						listFloatVal[i]=value;
						listIntVal[i]=0;
					}
				}
			}
			else
				return "";
		}

		nSize=listRetVal.size();

		if(nSize)
		{
			int strFmtLength = binFormat.GetSize();
			char *pchFormat = (char*)new unsigned char[strFmtLength+1];
			char *pchValue	= (char*)new unsigned char[strFmtLength+16*nSize];

			// 复制数据流格式到缓冲
			memset(pchFormat, 0, strFmtLength+1);
			memcpy(pchFormat, binFormat.GetBuffer(), strFmtLength);

			switch(nSize)
			{
			case 1:

				retVal=listRetVal[0];

				if(retVal.vt<7) // 整数型
				{
						if(strchr(pchFormat,'%') != NULL && 
							( strchr(pchFormat,'f') != NULL || strchr(pchFormat,'F') != NULL) )
							sprintf(pchValue, pchFormat, (double)listIntVal[0]);
						else
							sprintf(pchValue, pchFormat, listIntVal[0]);
				}
				else if(retVal.vt >10)//浮点运算
				{
					if(strchr(pchFormat,'%') != NULL && 
							( strchr(pchFormat,'f') != NULL || strchr(pchFormat,'F') != NULL) )
						sprintf(pchValue, pchFormat, listFloatVal[0]);
					else
						sprintf(pchValue, pchFormat, (long)listFloatVal[0]);
				}

				strRet=pchValue;

				break;

			case 2:

				if(listRetVal[0].vt<7 && listRetVal[1].vt<7)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listIntVal[1]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt>10)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listFloatVal[1]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt<7)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listIntVal[1]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt>10)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listFloatVal[1]);
				}
				
				strRet=pchValue;

				break;

			case 3:

				if(listRetVal[0].vt<7 && listRetVal[1].vt<7 && listRetVal[2].vt<7)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listIntVal[1],listIntVal[2]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt<7 && listRetVal[2].vt>10)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listIntVal[1],listFloatVal[2]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt>10 && listRetVal[2].vt<7)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listFloatVal[1],listIntVal[2]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt>10 && listRetVal[2].vt>10)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listFloatVal[1],listFloatVal[2]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt<7 && listRetVal[2].vt<7)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listIntVal[1],listIntVal[2]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt<7 && listRetVal[2].vt>10)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listIntVal[1],listFloatVal[2]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt>10 && listRetVal[2].vt<7)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listFloatVal[1],listIntVal[2]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt>10 && listRetVal[2].vt>10)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listFloatVal[1],listFloatVal[2]);
				}	
				strRet=pchValue;

				break;

			case 4:

				if(listRetVal[0].vt<7 && listRetVal[1].vt<7 && listRetVal[2].vt<7 && listRetVal[3].vt<7)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listIntVal[1],listIntVal[2],listIntVal[3]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt<7 && listRetVal[2].vt<7 && listRetVal[3].vt>10)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listIntVal[1],listIntVal[2],listFloatVal[3]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt<7 && listRetVal[2].vt>10 && listRetVal[3].vt<7)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listIntVal[1],listFloatVal[2],listIntVal[3]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt<7 && listRetVal[2].vt>10 && listRetVal[3].vt>10)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listIntVal[1],listFloatVal[2],listFloatVal[3]);
				}
				
				else if(listRetVal[0].vt<7 && listRetVal[1].vt>10 && listRetVal[2].vt<7 && listRetVal[3].vt<7)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listFloatVal[1],listIntVal[2],listIntVal[3]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt>10 && listRetVal[2].vt<7 && listRetVal[3].vt>10)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listFloatVal[1],listIntVal[2],listFloatVal[3]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt>10 && listRetVal[2].vt>10 && listRetVal[3].vt<7)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listFloatVal[1],listFloatVal[2],listIntVal[3]);
				}
				else if(listRetVal[0].vt<7 && listRetVal[1].vt>10 && listRetVal[2].vt>10 && listRetVal[3].vt>10)
				{
					sprintf(pchValue, pchFormat, listIntVal[0],listFloatVal[1],listFloatVal[2],listFloatVal[3]);
				}

				else if(listRetVal[0].vt>10 && listRetVal[1].vt<7 && listRetVal[2].vt<7 && listRetVal[3].vt<7)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listIntVal[1],listIntVal[2],listIntVal[3]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt<7 && listRetVal[2].vt<7 && listRetVal[3].vt>10)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listIntVal[1],listIntVal[2],listFloatVal[3]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt<7 && listRetVal[2].vt>10 && listRetVal[3].vt<7)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listIntVal[1],listFloatVal[2],listIntVal[3]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt<7 && listRetVal[2].vt>10 && listRetVal[3].vt>10)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listIntVal[1],listFloatVal[2],listFloatVal[3]);
				}
				
				else if(listRetVal[0].vt>10 && listRetVal[1].vt>10 && listRetVal[2].vt<7 && listRetVal[3].vt<7)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listFloatVal[1],listIntVal[2],listIntVal[3]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt>10 && listRetVal[2].vt<7 && listRetVal[3].vt>10)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listFloatVal[1],listIntVal[2],listFloatVal[3]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt>10 && listRetVal[2].vt>10 && listRetVal[3].vt<7)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listFloatVal[1],listFloatVal[2],listIntVal[3]);
				}
				else if(listRetVal[0].vt>10 && listRetVal[1].vt>10 && listRetVal[2].vt>10 && listRetVal[3].vt>10)
				{
					sprintf(pchValue, pchFormat, listFloatVal[0],listFloatVal[1],listFloatVal[2],listFloatVal[3]);
				}

				strRet=pchValue;

				break;
			}

			delete pchFormat;
			delete pchValue;

			return strRet;
		}
		else
			return "";
	}
	else
		return "";
}


int CArithmetic::GetExpList(CBinary binExp, vector<CBinary>& listBinExp)
{
	int nSize=binExp.GetSize();

	int nIndex;
	int nStart;
	int i;

	if(nSize<7)
		return 0;
	else
	{	
		nStart=0;
		nIndex=0;

		while(nIndex<nSize)
		{	
			//逗号分隔多个表达式
			if(binExp[nIndex]==',')
			{
				CBinary binItem;
				for(i=0;i<nIndex-nStart;i++)
				{
					binItem.Add(binExp[nStart + i]);
				}

				listBinExp.push_back(binItem);

				nStart=nIndex+1;
			}

			nIndex++;
		}

		if(listBinExp.size()==0)
		{
			listBinExp.push_back(binExp);
		}
		else
		{
			if(nStart<nSize) //把剩余部分作为一个表达式添加
			{
				CBinary binItem;
				for(i=0;i<nSize-nStart;i++)
				{
					binItem.Add(binExp[nStart + i]);
				}

				listBinExp.push_back(binItem);
			}
		}

		return listBinExp.size();
	}
}
