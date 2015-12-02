#ifndef		__DATABASE_H_
#define		__DATABASE_H_

#include "adsStd.h"
#include <string>
#include <list>
#include <vector>
using namespace std;
#include "Binary.h"

#define	FILEFUNCTIONNUM		5	// 类型文件数目
#define LISTCACHEMAXITEM	100	// 文本缓冲区缓冲数目
#define HEADER_LEN	32	// 文本缓冲区缓冲数目

//#pragma comment (lib,"D:\\v30\\adsStd\\w32lib\\SecurityBaseClass.lib")

class CDatabase
{
public: 
	class CTextCache
	{
	protected: 
		struct tagTextCache
		{
			CBinary idText;
			string strTextContain;
		};

	public:
		CTextCache(void);
		~CTextCache(void);

	public: 
		string* GetTextPointer(CBinary idText);
		bool UpdateText(CBinary idText, string strText);

	protected: 
		list <struct tagTextCache> m_lstTextCache;
	};

	enum 
	{	// DB_ DATA BASE
		DB_COMMAND           = 0x00,
		DB_TEXT              = 0x01,
		DB_TROUBLE_CODE      = 0x02,
        DB_DATA_STREAM       = 0x03,
		DB_TROUBLE_CODE_HELP = 0x04
	}; 

public: 
	static CTextCache m_TextCache;

protected:
	bool LoadIndex();
	long GetOffsetBisearch(CBinary ID);
	FILE *m_pfText;
	string m_astrFileFunction[FILEFUNCTIONNUM];

public:
	CDatabase(void);
	~CDatabase(void);

public:
	bool IsOpen();
	// 打开文件
	bool Open(unsigned short uiFileCodeName);

	// 打开文件
	bool Open(string strFile);

	// 关闭文件
	void Close();

	// 检索数据库，通过ID找到对应的内容
	const vector<CBinary> SearchId(CBinary& Id);

	// 用于测试
	unsigned char GetDBIDLength();
	unsigned long GetDBIDNumber();

private:
	bool CompareDBVersion(bool bFlag);

private:
	unsigned char m_ucIDLength;			// 数据库索引字节长度
	unsigned long m_ulIDNumber;			// 数据库索引数
	char * m_pIndex;
};

// 根据ID从文本库返回字符串
string adsGetTextString (CBinary binTextID);
string GetNameMapping(string szOldName);//hpy add

#endif		//__DATABASE_H_
