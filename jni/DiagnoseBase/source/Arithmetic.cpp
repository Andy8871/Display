#include <math.h>
#include "arithmetic.h"


// ����״̬ʱ������ʾ������ECU���յ���������
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
��    �ܣ�	�Ƿ������Ե�����Ϣ����ʽ��ʾ
����˵����	UINT16 uiDebugItem ������Ŀ��
			bool bEnable ����״̬ʹ�ܣ�����C true 
									   ��ֹ �C false
�� �� ֵ��	��
˵    ����	CDataStreamShow��CActiveTestShow���Show()����
			�趨����������״̬
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
��    �ܣ�	��ռ�¼ID���㷨֮��Ķ�Ӧ��ϵ�ĳ�Ա�������ͷ��ڴ�
����˵����	��
�� �� ֵ��	��
˵    ����	��
***********************************************************/
void CArithmetic::Clear (void)
{
	m_vDataStream.clear();
}


/*********************************************************************************
��    �ܣ�	���һ���������㷨���������
����˵����	CBinary idDataStream ������ID 
			vecter<CBinary> &aDsItem ID��Ӧ��������������
�� �� ֵ��	�������
˵    ����	������Ҫ��������������㷨�Ƿ���ڡ��Ϸ����粻
			�ܼ����򷵻�С��0��ֵ�����ܼ��㣬�򽫱��ʽת��
			�ɼ���������ʹ�õ�����ٵ��м���ʽ��������֮��
**********************************************************************************/
// ���������ݶ���
#define DATASTREAMITEMNUM	 5	// ���������ݵ�����
#define	DSEXPRESSPOS		 3	// ��������ʽ���ʽ��λ��
#define DSFORMATPOS			 2	// ��������ʾ��ʽ��λ��

int CArithmetic::AddDataStreamItem (CBinary idDataStream, vector<CBinary> &aDsItem)
{
	// �յ����������ݻ����������ݲ���
	if (aDsItem.size() < DATASTREAMITEMNUM)
	{
		return DATASTEAM_ERRORDSITEM;
	}

	// ���ʽΪ��
	if (aDsItem[DSEXPRESSPOS].GetSize() == 0)
	{
		return DATASTEAM_NOEXPRESS;
	}

	// ��ʽ���ƴ�Ϊ��
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
��    �ܣ�	����������ֵ
����˵����	const CBinary idDataStream          ������ID
			const CReceiveFrame & rcvfrmFromEcu ����֡ 
�� �� ֵ��	������ֵ
˵    ����	1����RELEASE�棬���ڴ���������ID�ֳ��ּ������
			  ʱ������ֵΪ��N/A������DEBUG�棬����ֵ��DS ERROR����
			2�����������������״̬��(m_ui8Debug& AD_DataStream
			  Ϊ��),��������Ϊ������Ա��д���㷨���ʽ�� +��2����
			  �ǿո�+��ʮ�����Ʊ�ʾ�Ľ���֡���ݡ���
***********************************************************************************/
string CArithmetic::DataStream (const CBinary idDataStream, CReceiveFrame& rcvfrmFromEcu)
{
	string strRet;	// �����ַ���

	stack <STACKSTRUCT> sComputeStack;	// ����ջ

	CBinary binDSExp;	// �������㷨	
	vector<DATASTREAM>::iterator vIter;
	for (vIter = m_vDataStream.begin(); vIter != m_vDataStream.end(); vIter++)
	{
		// ����������ID�ҵ���Ӧ�ı��ʽ
		if(vIter->idDataStream == idDataStream)
		{
			binDSExp = vIter->binArithMetic;

			if(0 == binDSExp.GetSize())
			{
				return strRet;  // ���ؿմ�
			}
			break;
		}
	}

	if(vIter == m_vDataStream.end())
	{
		// ���ؿմ�
		return strRet;
	}

#ifdef ADS_DEBUG	
	strRet = "DS ERROR";  // DEBUG�汾���
#else	
	strRet = "N/A";  // RELEASE�汾���
#endif

	// ���������������״̬��(m_ui8Debug&AD_DataStreamΪ��)
	if(m_ui8Debug&AD_DataStream)
	{
		int iLength = binDSExp.GetSize();

		char* pchBuf = (char*)new unsigned char[iLength+1];
		memset(pchBuf, 0, iLength+1);
		memcpy(pchBuf, binDSExp.GetBuffer(), iLength);
		pchBuf[iLength] = '\0';
		strRet = pchBuf;
		strRet += "  ";		// �����ո�
		delete []pchBuf;

		for(int iGroupOrder = 0; iGroupOrder < rcvfrmFromEcu.size(); iGroupOrder++)
		{
			CBinaryGroup binGroup = rcvfrmFromEcu[iGroupOrder];
			
			vector<CBinary>::iterator vBinIter;
			for(vBinIter = binGroup.begin(); vBinIter != binGroup.end(); vBinIter++)
			{
				// ����״̬ʱ������ʾ������ECU���յ���������
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

	// ȡ��RPN���ʽ
	int expLength = iDataLength + 2;
	
	char *pchExpChar = (char*)new unsigned char[expLength];
	memset(pchExpChar, 0, expLength);

	// ��ǰ����Ͽո�
	pchExpChar[0] = ' ';
	memcpy(pchExpChar+1, binDSExp.GetBuffer(), iDataLength);

	int begin, end;
	begin = 0;
	for (int i = 1; i<expLength; i++)  // ��0���ֽں�Ϊ" "
	{
		if(pchExpChar[i] != ' ')
			continue;
		
		end = i;  // ��¼��2���ո����ڵ�λ��

		// ����������ָ�����������
		if ((end-begin)<2)
		{
			delete [] pchExpChar;
			return strRet;  // ���ؿմ�
		}

		// ��¼�����㷨�Ļ���
		char *pchMeta = (char*)new unsigned char[end-begin]; 
		memset(pchMeta, 0, end-begin);
		memcpy(pchMeta, pchExpChar + begin + 1, end - begin - 1);

		// �ж��Ƿ�����ĸ������(A~Z, a~z, 0~9)
		if(isalnum(pchMeta[0]))
		{
			STACKSTRUCT value;
			memset(&value, 0, sizeof(STACKSTRUCT));

			// ���������
			if(isdigit(pchMeta[0]))
			{
				// û���ҵ�С���㣬˵��������
				char *pdest = strchr(pchMeta, '.');
				if(NULL == pdest)
				{
					value.vt = ADSVT_I4;  // ����
					value.ival = atoi(pchMeta);  
				}
				else 
				{
					value.vt = ADSVT_R8;  // ˫����
					value.dval = atol(pchMeta);
				}
			}
			else  // �������ĸ�����ǹؼ���G��F��C��B��b
			{	
				switch(pchMeta[0])
				{
					case 'G':
						value.vt = ADSVT_KG;     // ���ؼ���G����, ���ʶ�������ڱ�ʾ���������еĶ�Ӧĳ���͵�֡���ݵ�һ��FRAME�ļ���
						break; 
			
					case 'C':
						value.vt = ADSVT_KCHAR;	 // ���ؼ���C����, CHAR�з��ŵ��ֽ����ݱ�ʶ
						break;
					
					case 'F':
						value.vt = ADSVT_KF;	 //���ؼ���F����, FRAME��֡��ʶ�����������ݼ��ϣ�ͨ������һ���շ����������
						break;
				
					case 'B':
						value.vt = ADSVT_KBYTE;	 //���ؼ���B����, unsigned char���޷��ŵ��ֽ����ݱ�ʶ
						break;

					case 'b':
						value.vt = ADSVT_KBIT;	 //���ؼ���b����,  BIT���ֽ��е�����λ��ʶ
						break;

					default:
						delete [] pchMeta;
						delete [] pchExpChar;
						return strRet;  // ���ؿմ�
				}
			}
			
			// ѹ��������ʽջ
			sComputeStack.push(value);	
		}
		else  // ����������
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
		return strRet;  // ���ؿմ�
	}
	
	// ��������ʽΪ��
	if (0 == vIter->binFormat.GetSize())
	{
		delete [] pchExpChar;
		return strRet;
	}

	int strFmtLength = vIter->binFormat.GetSize();
	char *pchFormat = (char*)new unsigned char[strFmtLength+1];
	char *pchValue	= (char*)new unsigned char[strFmtLength+16];

	// ������������ʽ������
	memset(pchFormat, 0, strFmtLength+1);
	memcpy(pchFormat, vIter->binFormat.GetBuffer(), strFmtLength);

	// ��ջ��ȡ��������
	STACKSTRUCT Val = sComputeStack.top();
	if(Val.vt<7) // ������
	{
		int value;
		switch(Val.vt)
		{
			case ADSVT_I1:               // char
			case ADSVT_KCHAR:            // ���ؼ���C����
				value = (int)Val.cval;
				break;

			case ADSVT_I4:                // int
				value = (int)Val.ival;
				break;

			case ADSVT_UI1:               // unsigned char
			case ADSVT_KBYTE:             // ���ؼ���B����
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
	else if(Val.vt >10)//��������
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
			return strRet;  // ���ؿմ�
		}

		sprintf(pchValue, pchFormat, value);
	}
	else // �������Ϊ7��8��9��10�������
	{
		delete [] pchExpChar;
		return strRet;
	}

	strRet.append(pchValue);

	delete [] pchValue;
	delete [] pchFormat;
	delete [] pchExpChar;

	return strRet; // ��ȷ����
}

//added by johnnyling 
//��չ�Ķ���������Ա����

string CArithmetic::DataStreamEx(const CBinary idDataStream, CReceiveFrame& rcvfrmFromEcu)
{
	string strRet;	// �����ַ���

	stack <STACKSTRUCT> sComputeStack;	// ����ջ

	CBinary binDSExp;
	CBinary binFormat;
	vector<CBinary> listBinExp;

	vector<DATASTREAM>::iterator vIter;

	for (vIter = m_vDataStream.begin(); vIter != m_vDataStream.end(); vIter++)
	{
		// ����������ID�ҵ���Ӧ�ı��ʽ
		if(vIter->idDataStream == idDataStream)
		{
			binDSExp  = vIter->binArithMetic;
			binFormat = vIter->binFormat;

			int nCount=GetExpList(binDSExp,listBinExp);

			if(nCount>0)
			{
				//if(binFormat.GetSize()<2)
				//	strRet = "USERHANDLE"; //��ʽ������ַ���Ϊ�գ���Ҫ�û��Լ������������㷨�����崨
				//else
					strRet = ComputeExpressEx(listBinExp,binFormat,rcvfrmFromEcu);
			}
			else
			{
				strRet = "@@??@@"; //���ʽΪ�գ���Ҫ�û��Լ������������㷨�����崨

				for(int i=0;i<binFormat.GetSize();i++)
					strRet+=(char)binFormat[i];
			}

			break;
		}
	}

	return strRet; // ��ȷ����
}

/***********************************************************************************
��    �ܣ�	��������������ʽ��ջ������֡�������ݼ���������ֵ
����˵����	stack<string>& strExpress �������ջ
			char* meta	�����
			const CReceiveFrame & rcvfrmFromEcu ����֡ 
�� �� ֵ��	true	-��������ȷ������
			false	-������̳���
˵    ����	��
***********************************************************************************/
bool CArithmetic::ComputeExpress (stack<STACKSTRUCT>& sComputeStack, char* meta, CReceiveFrame& rcvfrmFromEcu)
{
	// ��֧�ֵ������
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
		// �ҵ�����������
		if(strcmp(sOperator[iIndex], meta) == 0)
		{
			op = iIndex;
			break;
		}
	}
	
	if(iIndex == 25)
	{
		return false;  // û���ҵ������
	}
	
	bool bSingle = false;	
	
	// ��Ŀ����� '!'�� '~'
	if(iIndex == 21 || iIndex == 22) 
	{
		bSingle = true;  
	}

	// �������
	STACKSTRUCT v1, v2, vResult,vTop;

	//added by johnnyling start
	memset((unsigned char *)&vResult,0,sizeof(STACKSTRUCT));
	//added by johnnyling end

	// �����˫Ŀ�����
	if(!bSingle)
	{
		// ȡ��һ��������
		v2 = sComputeStack.top();
		sComputeStack.pop();
	}

	// �ڶ���������
	v1 = sComputeStack.top();

	sComputeStack.pop();



	// ˫Ŀ�����
	if(!bSingle)
	{
		// ��������
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
				case ADSVT_KG:		// ���ؼ���G����
				case ADSVT_KGF:		// ���Ϲؼ���G.F����
				case ADSVT_KF:		// ���ؼ���F����
				case ADSVT_KCHAR:	// ���ؼ���C����
				case ADSVT_KBYTE:   // ���ؼ���B����
				case ADSVT_KBIT:    // ���ؼ���b����
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
				case CONDITION_QS: //     0  //�������ʽ: �ʺ�
				{
					bConditionExp = false;	//����Ϊfalse
					if(i1 != 0)			//���ʽΪ��
					{
						bConditionExp = true;
					}
					sComputeStack.push(v2);
					return true;
				}

				case CONDITION_CO: //	 1	//�������ʽ���ֺ� ���������κδ���
				{
					if(bConditionExp)		//������ʽΪ����ȡǰֵ
					{
						sComputeStack.push(v1);
					}
					else					//���ʽΪ��ȡ��ֵ
					{
						sComputeStack.push(v2);					
					}
				
					// �ָ��������ʽ��ֵ
					bConditionExp = false;
					return true;
				}

			case LOGICAL_OR: //      2  //�߼���	
				vResult.ucval = i1 || i2;
				vResult.vt = ADSVT_UI1;
				break;

			case LOGICAL_AND: //     3  //�߼���
				vResult.ucval = i1 && i2;
				vResult.vt = ADSVT_UI1;
				break;

			case BIT_OR: //          4  //��λ��																																							
				vResult.ival = i1|i2;
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case BIT_XOR: //         5  //��λ���																																							
				vResult.ival = i1 ^ i2;
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case BIT_AND: //         6  //��λ��																																							
				vResult.ival = i1 & i2;
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case NOT_EQUAL: //       7  //������																																							
				vResult.ucval = (i1 != i2);
				vResult.vt = ADSVT_UI1;
				break;

			case EQUAL: //           8  //����																																								
				vResult.ucval = (i1 == i2);
				vResult.vt = ADSVT_UI1;
				break;

			case GREATER_EQ	: // 	 9 //���ڵ���																																							
				vResult.ucval = (i1 >= i2);
				vResult.vt = ADSVT_UI1;
				break;

			case GREATER_THAN: //	10  //����																																								
				vResult.ucval = (i1 > i2);
				vResult.vt = ADSVT_UI1;
				break;

			case LESS_EQ: //		11  //С�ڵ���																																							
				vResult.ucval = (i1 <= i2);
				vResult.vt = ADSVT_UI1;
				break;

			case LESS_THAN: //		12  //С��																																								
				vResult.ucval = (i1 < i2);
				vResult.vt = ADSVT_UI1;
				break;

			case SHFT_RIGHT: //		13  //��λ����																																							
				vResult.ival =  (i1 >> (unsigned int)i2);
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case SHFT_LEFT: //		14  //��λ����																																							
				vResult.ival =  (i1 << (unsigned int)i2);
				vResult.vt = ADSVT_I4;//ADSVT_UI1;
				break;

			case SUB: //			15  //��																																								
				vResult.ival =  i1 - i2;
				vResult.vt = ADSVT_I4;
				break;

			case ADD: //			16  //��																																								
				vResult.ival = i1 + i2;
				vResult.vt = ADSVT_I4;
				break;

			case MODULUS: //		17  //ȡģ����																																							
				if(!i2)
				{
					return false;
				}

				vResult.ival = i1 % i2;
				vResult.vt = ADSVT_I4;
				break;

			case DIVIDE: //			18  //��																																								
				if(!i2)
				{
					return false;
				}

				vResult.ival =  i1 / i2;
				vResult.vt = ADSVT_I4;
				break;

			case MULTIPLY: //		19  //��                																																				
				vResult.ival =  i1 * i2;
				vResult.vt = ADSVT_I4;
				break;

			case SUFFIX: //			23  //�±�����	

				//���ջΪ�ջ���ջ������FRAME[],ֱ��ȡĬ��֡��Ӧ���ֽڣ�����ȡ������ֵ
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

			case MEMBER_SEL: //		24  //��Աѡ��(���ڵ������G.F��KGF.B��KGF.C��F.B��F.C��B.b �� ����.b)
				if ((v1.vt == ADSVT_KG) && (v2.vt == ADSVT_KF))	//G.F
				{
					vResult.vt = ADSVT_KGF;		//�м���������
					vResult.ival = (v1.ival<<16) + v2.ival;//��16λ����ţ���16λ��֡��
				}
				else if ((v1.vt == ADSVT_KGF) && (v2.vt == ADSVT_KCHAR))	//KGF.C
				{
					int group = (v1.ival>>16)&0XFFFF;//��16λ�����
					int frame = (v1.ival)&0xFFFF;	//��16λ��֡��

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
					int group = (v1.ival>>16)&0XFFFF;//��16λ�����
					int frame = (v1.ival)&0xFFFF;	//��16λ��֡��

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
				else if ((v1.vt == ADSVT_UI1) && (v2.vt == ADSVT_KBIT))		//����.b
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
			// ����˫Ŀ����
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
				case NOT_EQUAL: //       7  //������																																							
					//vResult.ucval = (abs(d1-d2)>(double)0.000000001);
					vResult.ucval = (fabs(d1-d2)>(double)0.000000001);
					vResult.vt = ADSVT_UI1;
					break;

				case EQUAL: //           8  //����																																								
					//vResult.ucval = (abs(d1-d2)<=(double)0.000000001);
					vResult.ucval = (fabs(d1-d2)<=(double)0.000000001);
					vResult.vt = ADSVT_UI1;
					break;

			    case GREATER_EQ	: // 	 9 //���ڵ���																																							
					vResult.ucval = (d1 >= d2);
					vResult.vt = ADSVT_UI1;
					break;

				case GREATER_THAN: //	10  //����																																								
					vResult.ucval = (d1 > d2);
					vResult.vt = ADSVT_UI1;
					break;

				case LESS_EQ: //		11  //С�ڵ���																																							
					//vResult.ucval = (d1 < d2) || (abs(d1-d2)<=(double)0.000000001);
					vResult.ucval = (d1 < d2) || (fabs(d1-d2)<=(double)0.000000001);
					vResult.vt = ADSVT_UI1;
					break;

				case LESS_THAN: //		12  //С��																																								
					//vResult.ucval = (d1 < d2) && (abs(d1-d2)>(double)0.000000001);
					vResult.ucval = (d1 < d2) && (fabs(d1-d2)>(double)0.000000001);
					vResult.vt = ADSVT_UI1;
					break;

				case SUB: //			15  //��																																								
					vResult.dval =  d1 - d2;
					vResult.vt = ADSVT_R8;
					break;

				case ADD: //			16  //��																																								
					vResult.dval =  d1 + d2;
					vResult.vt = ADSVT_R8;
					break;

				case DIVIDE: //			18  //��																																								
					//if( abs(d2) <= 0.000000001)//d2Ϊ0ֵ
					if(fabs(d2) <= 0.000000001)//d2Ϊ0ֵ
					{
						return false;
					}

					vResult.dval =  d1/d2;
					vResult.vt = ADSVT_R8;
					break;

				case MULTIPLY: //		19  //��                																																				
					vResult.dval = d1 * d2;
					vResult.vt = ADSVT_R8;
					break;

				default:
					return false;
			}
		}//		else{//����˫Ŀ����
	}
	else  //��Ŀ�����
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

			case ADSVT_R4:	//���Ǹ��㣬��ǿ��Ϊ����
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
			case LOGICAL_NOT: //	21  //�߼���	��Ŀ�����
				vResult.ucval =  !i1;
				vResult.vt = ADSVT_UI1;  // unsigned char
				break;

			case BIT_NOT: //		22  //��λȡ��	��Ŀ�����
				//modified by johnnyling start
				//v1.ival =  ~i1;
				vResult.ucval =  ~(unsigned char)i1;
				vResult.vt = ADSVT_UI1;
				//modified by johnnyling end
				break;

			default:
				return false;
		}

	}//	else{//��Ŀ�����

	sComputeStack.push(vResult);
	return true;
} 

#if 0
string CArithmetic::ComputeTest(CBinary binDSExp, CBinary binFormat, CReceiveFrame rcvfrmFromEcu)
{
	//vector<DATASTREAM>::iterator vIter;
	string strRet;	// �����ַ���
	stack <STACKSTRUCT> sComputeStack;	// ����ջ
	int iDataLength = binDSExp.GetSize();

	// ȡ��RPN���ʽ
	int expLength = iDataLength + 2;
	
	char *pchExpChar = (char*)new unsigned char[expLength];
	memset(pchExpChar, 0, expLength);

	// ��ǰ����Ͽո�
	pchExpChar[0] = ' ';
	memcpy(pchExpChar+1, binDSExp.GetBuffer(), iDataLength);

	int begin, end;
	begin = 0;
	for (int i = 1; i<expLength; i++)  // ��0���ֽں�Ϊ" "
	{
		if(pchExpChar[i] != ' ')
			continue;
		
		end = i;  // ��¼��2���ո����ڵ�λ��

		// ����������ָ�����������
		if ((end-begin)<2)
		{
			delete [] pchExpChar;
			return strRet;  // ���ؿմ�
		}

		// ��¼�����㷨�Ļ���
		char *pchMeta = (char*)new unsigned char[end-begin]; 
		memset(pchMeta, 0, end-begin);
		memcpy(pchMeta, pchExpChar + begin + 1, end - begin - 1);

		// �ж��Ƿ�����ĸ������(A~Z, a~z, 0~9)
		if(isalnum(pchMeta[0]))
		{
			STACKSTRUCT value;
			memset(&value, 0, sizeof(STACKSTRUCT));

			// ���������
			if(isdigit(pchMeta[0]))
			{
				// û���ҵ�С���㣬˵��������
				char *pdest = strchr(pchMeta, '.');
				if(NULL == pdest)
				{
					value.vt = ADSVT_I4;  // ����
					value.ival = atoi(pchMeta);  
				}
				else 
				{
					value.vt = ADSVT_R8;  // ˫����

					//modified by johnnyling start
					//value.dval = atol(pchMeta);
					value.dval = atof(pchMeta);
					//modified by johnnyling end
				}
			}
			else  // �������ĸ�����ǹؼ���G��F��C��B��b
			{	
				//added by johnnying start
				memset((unsigned char *)&value,0,sizeof(STACKSTRUCT));
				//added by johnnyling end

				switch(pchMeta[0])
				{
					case 'G':
						value.vt = ADSVT_KG;     // ���ؼ���G����, ���ʶ�������ڱ�ʾ���������еĶ�Ӧĳ���͵�֡���ݵ�һ��FRAME�ļ���
						break; 
			
					case 'C':
						value.vt = ADSVT_KCHAR;	 // ���ؼ���C����, CHAR�з��ŵ��ֽ����ݱ�ʶ
						break;
					
					case 'F':
						value.vt = ADSVT_KF;	 //���ؼ���F����, FRAME��֡��ʶ�����������ݼ��ϣ�ͨ������һ���շ����������
						break;
				
					case 'B':
						value.vt = ADSVT_KBYTE;	 //���ؼ���B����, unsigned char���޷��ŵ��ֽ����ݱ�ʶ
						break;

					case 'b':
						value.vt = ADSVT_KBIT;	 //���ؼ���b����,  BIT���ֽ��е�����λ��ʶ
						break;

					default:
						delete [] pchMeta;
						delete [] pchExpChar;
						return strRet;  // ���ؿմ�
				}
			}
			
			// ѹ��������ʽջ
			sComputeStack.push(value);	
		}
		else  // ����������
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
		return strRet;  // ���ؿմ�
	}
	
	// ��������ʽΪ��
	if (0 == binFormat.GetSize())
	{
		delete [] pchExpChar;
		return strRet;
	}

	int strFmtLength = binFormat.GetSize();
	char *pchFormat = (char*)new unsigned char[strFmtLength+1];
	char *pchValue	= (char*)new unsigned char[strFmtLength+16];

	// ������������ʽ������
	memset(pchFormat, 0, strFmtLength+1);
	memcpy(pchFormat, binFormat.GetBuffer(), strFmtLength);

	// ��ջ��ȡ��������
	STACKSTRUCT Val = sComputeStack.top();
	if(Val.vt<7) // ������
	{
		int value;
		switch(Val.vt)
		{
			case ADSVT_I1:               // char
			case ADSVT_KCHAR:            // ���ؼ���C����
				value = (int)Val.cval;
				break;

			case ADSVT_I4:                // int
				value = (int)Val.ival;
				break;

			case ADSVT_UI1:               // unsigned char
			case ADSVT_KBYTE:             // ���ؼ���B����
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
	else if(Val.vt >10)//��������
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
			return strRet;  // ���ؿմ�
		}

		sprintf(pchValue, pchFormat, value);
	}
	else // �������Ϊ7��8��9��10�������
	{
		delete [] pchExpChar;
		return strRet;
	}

	strRet.append(pchValue);

	delete [] pchValue;
	delete [] pchFormat;
	delete [] pchExpChar;

	return strRet; // ��ȷ����
}
#endif

int	CArithmetic::ComputeExpItem(CBinary binDSExp,CReceiveFrame rcvfrmFromEcu,STACKSTRUCT &retVal)
{
	stack <STACKSTRUCT> sComputeStack;	// ����ջ
	int iDataLength = binDSExp.GetSize();

	// ȡ��RPN���ʽ
	int expLength = iDataLength + 2;
	
	char *pchExpChar = (char*)new unsigned char[expLength];
	memset(pchExpChar, 0, expLength);

	// ��ǰ����Ͽո�
	pchExpChar[0] = ' ';
	memcpy(pchExpChar+1, binDSExp.GetBuffer(), iDataLength);

	int begin, end;
	begin = 0;
	for (int i = 1; i<expLength; i++)  // ��0���ֽں�Ϊ" "
	{
		if(pchExpChar[i] != ' ')
			continue;
		
		end = i;  // ��¼��2���ո����ڵ�λ��

		// ����������ָ�����������
		if ((end-begin)<2)
		{
			delete [] pchExpChar;
			return 0;  // ���ؿմ�
		}

		// ��¼�����㷨�Ļ���
		char *pchMeta = (char*)new unsigned char[end-begin]; 
		memset(pchMeta, 0, end-begin);
		memcpy(pchMeta, pchExpChar + begin + 1, end - begin - 1);

		// �ж��Ƿ�����ĸ������(A~Z, a~z, 0~9)
		if(isalnum(pchMeta[0]))
		{
			STACKSTRUCT value;
			memset(&value, 0, sizeof(STACKSTRUCT));

			// ���������
			if(isdigit(pchMeta[0]))
			{
				// û���ҵ�С���㣬˵��������
				char *pdest = strchr(pchMeta, '.');
				if(NULL == pdest)
				{
					value.vt = ADSVT_I4;  // ����
					value.ival = atoi(pchMeta);  
				}
				else 
				{
					value.vt = ADSVT_R8;  // ˫����

					//modified by johnnyling start
					//value.dval = atol(pchMeta);
					value.dval = atof(pchMeta);
					//modified by johnnyling end
				}
			}
			else  // �������ĸ�����ǹؼ���G��F��C��B��b
			{	
				//added by johnnying start
				memset((unsigned char *)&value,0,sizeof(STACKSTRUCT));
				//added by johnnyling end

				switch(pchMeta[0])
				{
					case 'G':
						value.vt = ADSVT_KG;     // ���ؼ���G����, ���ʶ�������ڱ�ʾ���������еĶ�Ӧĳ���͵�֡���ݵ�һ��FRAME�ļ���
						break; 
			
					case 'C':
						value.vt = ADSVT_KCHAR;	 // ���ؼ���C����, CHAR�з��ŵ��ֽ����ݱ�ʶ
						break;
					
					case 'F':
						value.vt = ADSVT_KF;	 //���ؼ���F����, FRAME��֡��ʶ�����������ݼ��ϣ�ͨ������һ���շ����������
						break;
				
					case 'B':
						value.vt = ADSVT_KBYTE;	 //���ؼ���B����, unsigned char���޷��ŵ��ֽ����ݱ�ʶ
						break;

					case 'b':
						value.vt = ADSVT_KBIT;	 //���ؼ���b����,  BIT���ֽ��е�����λ��ʶ
						break;

					default:
						delete [] pchMeta;
						delete [] pchExpChar;
						return 0;  // ���ؿմ�
				}
			}
			
			// ѹ��������ʽջ
			sComputeStack.push(value);	
		}
		else  // ����������
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
		return 0;  // ���ؿմ�
	}
	
	// ��ջ��ȡ��������
	STACKSTRUCT Val = sComputeStack.top();
	if(Val.vt<7) // ������
	{
		int value;
		switch(Val.vt)
		{
			case ADSVT_I1:               // char
			case ADSVT_KCHAR:            // ���ؼ���C����
				value = (int)Val.cval;
				break;

			case ADSVT_I4:                // int
				value = (int)Val.ival;
				break;

			case ADSVT_UI1:               // unsigned char
			case ADSVT_KBYTE:             // ���ؼ���B����
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
	else if(Val.vt >10)//��������
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
			return 0;  // ���ؿմ�
		}
	}
	else // �������Ϊ7��8��9��10�������
	{
		delete [] pchExpChar;
		return 0;
	}

	delete [] pchExpChar;

	memcpy(&retVal,&Val,sizeof(STACKSTRUCT));

	return 1; // ��ȷ����

}

//������б�ѡ�����ʽ���£�
//@@TABLE:XXX@XXX@XXX
//@@TABLE:��ͷ
//ÿһ��Ŀ֮����@�ָ�
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
		//�ж�ǰ���Ƿ��Ǳ�־��@@TABLE:
		CBinary binFlag;

		for(i=0;i<8;i++)
			binFlag+=binFormat[i];

		if(!(binFlag==CBinary("@@TABLE:",8)))
			return 0;

		//�����м���ѡ����Ŀ

		nStart=8;
		nIndex=8;
		strTable="";

		while(nIndex<nSize)
		{	
			//@�ָ�������ʽ
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

		if(listTable.size()==0) //������Ҫ��������
		{
			return 0;
		}
		else
		{
			if(nStart<nSize) //��ʣ�ಿ����Ϊһ�����ʽ���
			{
				strTable="";
				for(i=0;i<nSize-nStart;i++)
				{
					//�ַ���������
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

	// ��������ʽΪ��
	if (0 == binFormat.GetSize())
		return "";

	if(GetTableList(binFormat,listTable)) //��ʽ����ֵ��
	{
		if(binExpList.size()!=1)
			return "";
		else
		{
			if(ComputeExpItem(binExpList[0],RcvFrame,retVal)) //����ɹ�
			{
				//ֻ����ֵ�����Ͳ��ܴӱ���ȡֵ
				switch(retVal.vt)
				{
					case ADSVT_I1:               // char
					case ADSVT_KCHAR:            // ���ؼ���C����
						nIndex = (int)retVal.cval;
						break;

					case ADSVT_I4:                // int
						nIndex = (int)retVal.ival;
						break;

					case ADSVT_UI1:               // unsigned char
					case ADSVT_KBYTE:             // ���ؼ���B����
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
	else if(binExpList.size()<=MAX_EXPRESSION_COUNT) //���ֻ�ܴ���4�����ʽ
	{
		for(i=0;i<binExpList.size();i++)
		{
			if(ComputeExpItem(binExpList[i],RcvFrame,retVal)) //����ɹ�
			{
				//������������7��8��9��10�����
				if(retVal.vt>=7 && retVal.vt<=10)
					return "";
				else
				{
					listRetVal.push_back(retVal);

					//��סÿ�������ֵ
					if(retVal.vt<7) // ������
					{
						int value;
						switch(retVal.vt)
						{
							case ADSVT_I1:               // char
							case ADSVT_KCHAR:            // ���ؼ���C����
								value = (int)retVal.cval;
								break;

							case ADSVT_I4:                // int
								value = (int)retVal.ival;
								break;

							case ADSVT_UI1:               // unsigned char
							case ADSVT_KBYTE:             // ���ؼ���B����
								value = (unsigned char)retVal.ucval;
								break;

							case ADSVT_UI4:               // unsigned int
								value = (int)retVal.uival;
								break;
						}

						listIntVal[i]=value;
						listFloatVal[i]=0.0;
					}
					else if(retVal.vt >10)//��������
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

			// ������������ʽ������
			memset(pchFormat, 0, strFmtLength+1);
			memcpy(pchFormat, binFormat.GetBuffer(), strFmtLength);

			switch(nSize)
			{
			case 1:

				retVal=listRetVal[0];

				if(retVal.vt<7) // ������
				{
						if(strchr(pchFormat,'%') != NULL && 
							( strchr(pchFormat,'f') != NULL || strchr(pchFormat,'F') != NULL) )
							sprintf(pchValue, pchFormat, (double)listIntVal[0]);
						else
							sprintf(pchValue, pchFormat, listIntVal[0]);
				}
				else if(retVal.vt >10)//��������
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
			//���ŷָ�������ʽ
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
			if(nStart<nSize) //��ʣ�ಿ����Ϊһ�����ʽ���
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
