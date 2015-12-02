#ifndef		__DATABASE_H_
#define		__DATABASE_H_

#include "adsStd.h"
#include <string>
#include <list>
#include <vector>
using namespace std;
#include "Binary.h"

#define	FILEFUNCTIONNUM		5	// �����ļ���Ŀ
#define LISTCACHEMAXITEM	100	// �ı�������������Ŀ
#define HEADER_LEN	32	// �ı�������������Ŀ

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
	// ���ļ�
	bool Open(unsigned short uiFileCodeName);

	// ���ļ�
	bool Open(string strFile);

	// �ر��ļ�
	void Close();

	// �������ݿ⣬ͨ��ID�ҵ���Ӧ������
	const vector<CBinary> SearchId(CBinary& Id);

	// ���ڲ���
	unsigned char GetDBIDLength();
	unsigned long GetDBIDNumber();

private:
	bool CompareDBVersion(bool bFlag);

private:
	unsigned char m_ucIDLength;			// ���ݿ������ֽڳ���
	unsigned long m_ulIDNumber;			// ���ݿ�������
	char * m_pIndex;
};

// ����ID���ı��ⷵ���ַ���
string adsGetTextString (CBinary binTextID);
string GetNameMapping(string szOldName);//hpy add

#endif		//__DATABASE_H_
