#ifndef	__ARITHMETIC_H_
#define	__ARITHMETIC_H_

#include "adsStd.h"
#include <string>
#include <vector>
#include <stack>
using namespace std;
#include "Binary.h"
#include "ReceiveFrame.h"

#ifndef CBinaryGroup
#define CBinaryGroup	vector<CBinary>
#endif

class CArithmetic
{
public:
	CArithmetic(void);
	~CArithmetic(void);

protected:
	typedef struct _DataStreamTag
	{
		CBinary idDataStream; // ������ID
		CBinary binFormat; // ��������ʽ
		CBinary binArithMetic; // �������㷨
	} DATASTREAM;

	typedef struct _StackStructTag
	{
		unsigned char vt; // ��������

		char cval;
		unsigned char ucval;
		int ival;
		unsigned int uival;
		float fval;
		double dval;
	} STACKSTRUCT;

	vector<DATASTREAM> m_vDataStream; // �ܹ�����������ID��ת��������㷨����ʾ�����ַ���������

	static unsigned char m_ui8Debug; // �����0λΪ1����ʾ�������㷨���㷨��ʹ�õ�ECU�ظ�ֵ

public:
	enum
	{
		//AD_  Arithmetic Debug
		AD_DataStream = 0x01,
		AD_TroubleCode = 0x02
	};

	// ������궨��			 
	enum
	{
		CONDITION_QS = 0, //�������ʽ: �ʺ�
		CONDITION_CO = 1, //�������ʽ���ֺ�
		LOGICAL_OR = 2, //�߼���
		LOGICAL_AND = 3, //�߼���
		BIT_OR = 4, //��λ��
		BIT_XOR = 5, //��λ���
		BIT_AND = 6, //��λ��
		NOT_EQUAL = 7, //������
		EQUAL = 8, //����
		GREATER_EQ = 9, //���ڵ���
		GREATER_THAN = 10, //����
		LESS_EQ = 11, //С�ڵ���
		LESS_THAN = 12, //С��
		SHFT_RIGHT = 13, //��λ����
		SHFT_LEFT = 14, //��λ����
		SUB = 15, //��
		ADD = 16, //��
		MODULUS = 17, //ȡģ����
		DIVIDE = 18, //��
		MULTIPLY = 19, //��
		UNBINARY = 20, //һԪ��
		LOGICAL_NOT = 21, //�߼���
		BIT_NOT = 22, //��λ��
		SUFFIX = 23, //�±�
		MEMBER_SEL = 24 //��Աѡ��
	};

	// ջ�������Ͷ���
	enum
	{
		ADSVT_NULL = 0, ADSVT_I1 = 1, // char
		ADSVT_UI1 = 2, // unsigned char
		ADSVT_I4 = 3, // int
		ADSVT_UI4 = 4, // unsigned int
		ADSVT_KCHAR = 5, //���ؼ���C����
		ADSVT_KBYTE = 6, //���ؼ���B����

		ADSVT_KG = 7, //���ؼ���G����
		ADSVT_KGF = 8, //���Ϲؼ���G.F����
		ADSVT_KF = 9, //���ؼ���F����
		ADSVT_KBIT = 10, //���ؼ���b����

		ADSVT_R4 = 11, // float
		ADSVT_R8 = 12 // double
	};

public:
	// �Ƿ������Ե�����Ϣ����ʽ��ʾ
	static void SetDebugState(unsigned char uiDebugItem, bool bEnable);

	// ��ռ�¼ID���㷨֮��Ķ�Ӧ��ϵ�ĳ�Ա�������ͷ��ڴ�
	void Clear(void);

	// ���һ���������㷨���������
	int AddDataStreamItem(CBinary idDataStream, vector<CBinary>& aDsItem);

	// ����������ֵ
	string DataStream(const CBinary idDataStream, CReceiveFrame& rcvfrmFromEcu);

private:
	bool ComputeExpress(stack<STACKSTRUCT>& strExpress, char* meta,
			CReceiveFrame& rcvfrmFromEcu);

	/*
	 public:
	 string ComputeTest(CBinary binExp, CBinary binFormat, CReceiveFrame RcvFrame);
	 */

public:
	int ComputeExpItem(CBinary binDSExp, CReceiveFrame rcvfrmFromEcu,
			STACKSTRUCT &retVal);
	string ComputeExpressEx(vector<CBinary> binExpList, CBinary binFormat,
			CReceiveFrame RcvFrame);
	string DataStreamEx(const CBinary idDataStream,
			CReceiveFrame& rcvfrmFromEcu);
private:
	int GetTableList(CBinary binFormat, vector<string>& listTable);
	int GetExpList(CBinary binExp, vector<CBinary>& listBinExp);
};

#endif	//__ARITHMETIC_H_
