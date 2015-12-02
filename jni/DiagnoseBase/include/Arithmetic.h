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
		CBinary idDataStream; // 数据流ID
		CBinary binFormat; // 数据流格式
		CBinary binArithMetic; // 数据流算法
	} DATASTREAM;

	typedef struct _StackStructTag
	{
		unsigned char vt; // 数据类型

		char cval;
		unsigned char ucval;
		int ival;
		unsigned int uival;
		float fval;
		double dval;
	} STACKSTRUCT;

	vector<DATASTREAM> m_vDataStream; // 能够保存数据流ID、转换后计算算法和显示控制字符串的数组

	static unsigned char m_ui8Debug; // 如果第0位为1，显示数据流算法、算法中使用的ECU回复值

public:
	enum
	{
		//AD_  Arithmetic Debug
		AD_DataStream = 0x01,
		AD_TroubleCode = 0x02
	};

	// 运算符宏定义			 
	enum
	{
		CONDITION_QS = 0, //条件表达式: 问号
		CONDITION_CO = 1, //条件表达式：分号
		LOGICAL_OR = 2, //逻辑或
		LOGICAL_AND = 3, //逻辑与
		BIT_OR = 4, //按位或
		BIT_XOR = 5, //按位异或
		BIT_AND = 6, //按位与
		NOT_EQUAL = 7, //不等于
		EQUAL = 8, //等于
		GREATER_EQ = 9, //大于等于
		GREATER_THAN = 10, //大于
		LESS_EQ = 11, //小于等于
		LESS_THAN = 12, //小于
		SHFT_RIGHT = 13, //按位右移
		SHFT_LEFT = 14, //按位左移
		SUB = 15, //减
		ADD = 16, //加
		MODULUS = 17, //取模求余
		DIVIDE = 18, //除
		MULTIPLY = 19, //乘
		UNBINARY = 20, //一元减
		LOGICAL_NOT = 21, //逻辑非
		BIT_NOT = 22, //按位非
		SUFFIX = 23, //下标
		MEMBER_SEL = 24 //成员选择
	};

	// 栈数据类型定义
	enum
	{
		ADSVT_NULL = 0, ADSVT_I1 = 1, // char
		ADSVT_UI1 = 2, // unsigned char
		ADSVT_I4 = 3, // int
		ADSVT_UI4 = 4, // unsigned int
		ADSVT_KCHAR = 5, //单关键字C类型
		ADSVT_KBYTE = 6, //单关键字B类型

		ADSVT_KG = 7, //单关键字G类型
		ADSVT_KGF = 8, //复合关键字G.F类型
		ADSVT_KF = 9, //单关键字F类型
		ADSVT_KBIT = 10, //单关键字b类型

		ADSVT_R4 = 11, // float
		ADSVT_R8 = 12 // double
	};

public:
	// 是否允许以调试信息的形式显示
	static void SetDebugState(unsigned char uiDebugItem, bool bEnable);

	// 清空记录ID与算法之间的对应关系的成员变量，释放内存
	void Clear(void);

	// 添加一项数据流算法相关项内容
	int AddDataStreamItem(CBinary idDataStream, vector<CBinary>& aDsItem);

	// 计算数据流值
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
