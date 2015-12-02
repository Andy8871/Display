/******************************************************************************

 Copyright (c) 2005, AUTOBOSS Inc.
 All rights reserved.

 �ļ����ƣ�Database.cpp
 �ĵ���ʶ��007�������ƽ̨��ϸ���˵����
 ժ    Ҫ���������ڸ����ṩ������ID�����ݿ���Ҷ�Ӧ������

 ��ʷ��¼��
 ----------------------------------------------------------------------------
 ʱ     ��   ����		�汾��		��    ��    ��    ��
 ----------------------------------------------------------------------------
 2005.11.28  zhangsh		1.0AX		�������ࡣ
 2006.7.3    wcx         0.0908      ����IsOpen(), ����CMenuTree�Ż����Ҳ�
 ���ı�ʹ��

 ******************************************************************************/
#include <android/log.h>
#include	<math.h>
#include	"Database.h"
#include	"Information.h"
#include	"RunEnvironment.h"
#include    "assert.h"
#include	"SecurityBaseClass.h"

extern CDatabase m_MenuTxtDb; //FOR MENU DISPLAY

#define OFFSET_LENGTH					4

#define DB_VERSION_2_0                0x00020000
#define DB_VERSION_3_0                0x00030000

unsigned char g_mapName[][2] =
{
{ 'A', 'd' },
{ 'B', 'h' },
{ 'C', 's' },
{ 'D', 'j' },
{ 'E', 'i' },
{ 'F', 'q' },
{ 'G', 'w' },
{ 'H', 'g' },
{ 'I', 'x' },
{ 'J', 'u' },
{ 'K', 'b' },
{ 'L', 'n' },
{ 'M', 'l' },
{ 'N', 'k' },
{ 'O', 'p' },
{ 'P', 'e' },
{ 'Q', 'y' },
{ 'R', 'v' },
{ 'S', 'o' },
{ 'T', 'c' },
{ 'U', 'a' },
{ 'V', 'r' },
{ 'W', 't' },
{ 'X', 'z' },
{ 'Y', 'f' },
{ 'Z', 'm' },
{ '_', '_' } };

#define MAP_SIZE 27

class CEncryptDecrypt
{
public:
	CEncryptDecrypt(void)
	{
		m_uiDbVersion = 0;
	}
	;
	~CEncryptDecrypt(void)
	{
	}
	;
public:
	void SetDbVersion(unsigned int uiDbVersion);
	void SetDbFlag(unsigned char byDbFLag);
	CBinary Decrypt(unsigned char*, int);
protected:
	unsigned int m_uiDbVersion;
	unsigned char m_byteDbFlag;
};

void CEncryptDecrypt::SetDbVersion(unsigned int uiDbVersion)
{
	m_uiDbVersion = uiDbVersion;
}

void CEncryptDecrypt::SetDbFlag(unsigned char byteDbFlag)
{
	m_byteDbFlag = byteDbFlag;
}

CBinary CEncryptDecrypt::Decrypt(unsigned char* pucContain, int iLength)
{
	//hpy: dbcompile.cpp/2020
	CBinary bin;
	unsigned char* pTemp = pucContain;

	//unsigned char aucString[]= "WWW.AUTOBOSS.NET Copyright (c) 2009, AUTOBOSS Inc.an SPX";
	//unsigned char aucString[]= "www.ucandas.com, All Rights Reserved. Copyright (c) 2013";//hpy changed 20130827
	unsigned char aucString[] = "{CE8E31D2-F5FE-469b-A69A-299595D3F40E}"; //Andy changed 20150511
	for (int i = 0; i < iLength; i++)
	{
		(*pucContain) ^= aucString[i % (sizeof(aucString) - 1)];
		(*pucContain) = ~(((*pucContain) << 4) | (((*pucContain) >> 4) & 0x0F));
		++pucContain;
	}
	bin.WriteBuffer(pTemp, iLength);

	return bin;
}

static CEncryptDecrypt s_Crypt;

CDatabase::CTextCache CDatabase::m_TextCache;

CDatabase::CTextCache::CTextCache(void)
{
}

CDatabase::CTextCache::~CTextCache(void)
{
}

/**************************************************************
 ��    �ܣ��ɸ���������ID��list�в��ң����ҵ��򷵻����Ӧ���ı�����ʼ��ַ��
 û�ҵ��򷵻ؿգ�NULL��
 ����˵����idText-   Ҫ���ҵ��������
 �� �� ֵ����Ӧ���ı�����ʼ��ַ
 ˵    ������
 **************************************************************/
string* CDatabase::CTextCache::GetTextPointer(CBinary idText)
{
	string *pstrTemp = NULL;

	list<struct tagTextCache>::iterator cIter;

	for (cIter = m_lstTextCache.begin(); cIter != m_lstTextCache.end(); cIter++)
	{
		if (cIter->idText == idText)
		{
			// ��������λ
			if (cIter != m_lstTextCache.begin())
			{
				m_lstTextCache.insert(m_lstTextCache.begin(), *cIter);
				m_lstTextCache.erase(cIter);
			}

			pstrTemp = &(m_lstTextCache.begin()->strTextContain);
			return pstrTemp;
		}
	}

	return NULL;
}

/**************************************************************
 ��    �ܣ����¸���������ID��Ӧ��LIST���ַ�������
 ����˵����idText-Ҫ���ҵ�������ţ�strText-�µ��ַ�������
 �� �� ֵ��true-���³ɹ���false-����ʧ��
 ˵    ������
 **************************************************************/
bool CDatabase::CTextCache::UpdateText(CBinary idText, string strText)
{
	struct tagTextCache updata;

	updata.idText = idText;
	updata.strTextContain = strText;

	list<struct tagTextCache>::iterator cIter;
	for (cIter = m_lstTextCache.begin(); cIter != m_lstTextCache.end(); cIter++)
	{
		if (cIter->idText == idText)
		{
			break;
		}
	}

	// ����ڻ�������ڣ�����������ײ�
	if (cIter != m_lstTextCache.end())
	{
		cIter->strTextContain = strText;
		m_lstTextCache.insert(m_lstTextCache.begin(), *cIter);

		m_lstTextCache.erase(cIter);
	}
	else
	{
		// ���û���ҵ�������ӵ��ײ�
		m_lstTextCache.push_front(updata);

		// ���������������LISTCACHEMAXITEM������ɾ��β����һ��
		if (m_lstTextCache.size() > LISTCACHEMAXITEM)
		{
			m_lstTextCache.pop_back();
		}
	}

	return true;
}

/**************************************************************
 ��    �ܣ�ȱʡ���캯��
 ����˵������
 �� �� ֵ����
 ˵    ������ʼ����Ա����
 **************************************************************/
CDatabase::CDatabase(void)
{
	m_pfText = NULL;
	m_pIndex = NULL;

	char* pchTemp[FILEFUNCTIONNUM] =
	{ "_CMD.DB", "_TEXT.DB", "_DTC.DB", "_DS.DB", "_DTC_H.DB" }; //hpy need changed here

	for (int i = 0; i < FILEFUNCTIONNUM; i++)
	{
		m_astrFileFunction[i] = pchTemp[i];
	}
}

/*************************************************************
 �������ܣ���������
 ����˵������
 �� �� ֵ����
 ˵    �����ж��ļ�ָ���Ƿ�Ϊ�գ���Ϊ���ٹرգ������ļ�ָ���ÿ�
 **************************************************************/
CDatabase::~CDatabase(void)
{
	if (NULL != m_pfText)
	{
		fclose(m_pfText);
		m_pfText = NULL;
	}

	if (m_pIndex)
	{
		delete[] m_pIndex;
		m_pIndex = NULL;
	}
}

/*************************************************************
 ��    �ܣ����ļ�
 ����˵����unsigned short uiFileCodeName �ļ�����
 �� �� ֵ���ɹ���true ʧ�ܡ�false
 ˵    ������
 *************************************************************/
bool CDatabase::Open(unsigned short uiFileCodeName)
{
	// �������Ԥ����ķ�Χ�������
	if (uiFileCodeName < 0 || uiFileCodeName >= FILEFUNCTIONNUM)
	{
		return false;
	}

	// ȡ�ù���Ŀ¼
	string strFilename = CRunEnvironment::GetDiagnosisDirectory();

	// ����Ŀ¼�����һ���ַ���������'\0'
	char chEnd = *(strFilename.end() - 1);

	// �ж�Ŀ¼ĩβ�Ƿ����'\\'��'/'�ַ�
	if (chEnd != '\\' && chEnd != '/')
	{
		strFilename += '\\';
	}

	if (DB_COMMAND == uiFileCodeName)
	{
		// ��ϵ��
		string strName = CInformation::GetVehiclesSystemName();
		strName = strName.substr(0, 4);
		strFilename += strName;
	}
	else
	{
		// ���Դ���
		strFilename += CRunEnvironment::GetLanguage();
	}

	// �ļ���׺
	strFilename += m_astrFileFunction[uiFileCodeName];

	string szNewName = GetNameMapping(strFilename); //hpy add

	const char *pchFileName = szNewName.data(); //const char *pchFileName = strFilename.data();//hpy

	try
	{
		m_pfText = fopen(pchFileName, "rb");
		if (NULL == m_pfText)
		{
			return false;
		}
	} catch (...)
	{
		return false;
	}
	bool bFlag = false;
	//if( strstr(pchFileName,"_TXT.DB") != NULL || strstr(pchFileName,"_DS.DB") != NULL || strstr(pchFileName,"_DTC.DB") != NULL )//hpy need changed here
	//if( strstr(pchFileName,"_UYU.PNT") != NULL || strstr(pchFileName,"_ET.PNT") != NULL || strstr(pchFileName,"_EUD.PNT") != NULL )
	if (strstr(pchFileName, "gyCy.0000") != NULL
			|| strstr(pchFileName, "gMt.0000") != NULL
			|| strstr(pchFileName, "gMyV.0000") != NULL)
		bFlag = true;

	if (!CompareDBVersion(bFlag)) //�Ƚ���ϰ汾; ��txt/dtc/ds�ļ���Ҫ
	{
		return false;
	}

	LoadIndex();

	return true;
}

/*************************************************************
 ��    �ܣ����ļ�
 ����˵����string strFileName �ļ�����(����·������
 Ĭ��ΪӦ�ó���ǰ·��)
 �� �� ֵ���ɹ���true ʧ�ܡ�false
 ˵    ������
 *************************************************************/
bool CDatabase::Open(string strFile)
{
	string strFilename;

	//ȡ�ù���Ŀ¼��
	strFilename += CRunEnvironment::GetDiagnosisDirectory();
	char chEnd = *(strFilename.end() - 1);
	if (chEnd != '\\' && chEnd != '/')
	{
		strFilename += '/';
	}

	strFilename += strFile;

	string szNewName = GetNameMapping(strFilename); //hpy add

	const char *file = szNewName.data(); //const char *file = strFilename.data();//hpy

	try
	{
		m_pfText = fopen(file, "rb");
		if (NULL == m_pfText)
		{
			__android_log_print(ANDROID_LOG_DEBUG, "CDatabase",
					"Unable to open database file %s", file);
			return false;
		}
		__android_log_print(ANDROID_LOG_DEBUG, "CDatabase",
				"Successfully open database file %s", file);
	} catch (...)
	{
		return false;
	}

	bool bFlag = false;
	//if( strstr(file,"_TXT.DB") != NULL || strstr(file,"_DS.DB") != NULL || strstr(file,"_DTC.DB") != NULL )//hpy need changed here
	//if( strstr(file,"_UYU.PNT") != NULL || strstr(file,"_ET.PNT") != NULL || strstr(file,"_EUD.PNT") != NULL )
	if (strstr(file, "gyCy.0000") != NULL || strstr(file, "gMt.0000") != NULL
			|| strstr(file, "gMyV.0000") != NULL)
		bFlag = true;

	if (!CompareDBVersion(bFlag)) //�Ƚ���ϰ汾
	{
		__android_log_print(ANDROID_LOG_DEBUG, "CDatabase",
				"Failed to CompareDBVersion");
		return false;
	}

	LoadIndex();

	return true;
}

/*************************************************************
 ��    �ܣ��رմ򿪵��ļ�
 ����˵������
 �� �� ֵ����
 ˵    ������
 *************************************************************/
void CDatabase::Close()
{
	if (NULL != m_pfText)
	{
		fclose(m_pfText);
		m_pfText = NULL;
	}
	if (m_pIndex)
	{
		delete[] m_pIndex;
		m_pIndex = NULL;
	}
}

/*********************************************************
 ��    �ܣ��õ����ݿ���ϰ汾���ݣ���Ӧ�ó���汾���бȽ�
 ����˵������
 �� �� ֵ���Ƚ���ȣ�true��������ȣ�false
 ˵    ����˽�к���
 *********************************************************/
bool CDatabase::CompareDBVersion(bool bFlag)
{
	unsigned char ucEncrptChar[35];
	memset(ucEncrptChar, 0, 35);

	unsigned char ucChar[35];
	memset(ucChar, 0, 35);

	try
	{
		// ��λ����ͷ
		fseek(m_pfText, 0, SEEK_SET);

		// ���ļ�ͷ����
		int result = fread(ucEncrptChar, sizeof(unsigned char), HEADER_LEN,
				m_pfText);
		if (32 != result)
		{
			__android_log_print(ANDROID_LOG_DEBUG, "CDatabase", "Failed to read database");
			return false;
		}
	} catch (...)
	{
		return false;
	}

	CSecurityBaseClass m_Safe;
	m_Safe.DecryptBuffer((const char*)ucEncrptChar,32,(char*)ucChar);
	char cLangageCode[3] = "";
	memcpy(cLangageCode, ucChar + 16, 2);
	if (bFlag)
	{
		if (strcmp(CRunEnvironment::GetLanguage().c_str(), cLangageCode) != 0)
		{
			__android_log_print(ANDROID_LOG_DEBUG, "CDatabase", "CRunEnvironment::GetLanguage()=%s", CRunEnvironment::GetLanguage().c_str());
			__android_log_print(ANDROID_LOG_DEBUG, "CDatabase", "Database language=%s", cLangageCode);
			return false;
		}
	}
	// �ж����ݿ��־

	if (ucChar[0] != 'P' || ucChar[1] != 'T')
	{
		__android_log_print(ANDROID_LOG_DEBUG, "CDatabase", "Error for database flag ucChar[0] = %d, ucChar[1] = %d", ucChar[0], ucChar[1]);
		return false;
	}

	s_Crypt.SetDbVersion(
			(ucChar[2] << 24) | (ucChar[3] << 16) | (ucChar[4] << 8)
					| ucChar[5]);
	s_Crypt.SetDbFlag(ucChar[7]);
	char pchVersion[10];
	pchVersion[0] = (char) ((ucChar[8] >> 4) + 0x30);
	pchVersion[1] = (char) ((ucChar[8] & 0x0f) + 0x30);
	pchVersion[2] = (char) ((ucChar[9] >> 4) + 0x30);
	pchVersion[3] = (char) ((ucChar[9] & 0x0f) + 0x30);
	pchVersion[4] = '.';
	pchVersion[5] = (char) ((ucChar[10] >> 4) + 0x30);
	pchVersion[6] = (char) ((ucChar[10] & 0x0f) + 0x30);
	pchVersion[7] = (char) ((ucChar[11] >> 4) + 0x30);
	pchVersion[8] = (char) ((ucChar[11] & 0x0f) + 0x30);
	pchVersion[9] = '\0';

	// ���ݿ�汾
	double fDBVer = atof(pchVersion);

	// ��ϳ���汾
	double fDiagVer = CInformation::GetVersion();

	__android_log_print(ANDROID_LOG_DEBUG, "CDatabase", "fDBVer == %f----------fDiagVer = %f", fDBVer, fDiagVer);

	// �ж���ϰ汾
	//if(abs(fDBVer-fDiagVer) > 0.00001 )
	if (fabs(fDBVer - fDiagVer) > 0.00001)
	{
		//���ڱ�������BUG����ʱ���Ƚϰ汾���ȼ��ܹ��ܼ������˵��
		//2006-11-07,johnnyling
		return false; //11/23/2006 by wcx 
	}

	// ���ݿ������ֽڳ���
	m_ucIDLength = ucChar[6];

	// ID����
	m_ulIDNumber = (unsigned long) ((((unsigned long) ucChar[12]) << 24)
			+ (((unsigned long) ucChar[13]) << 16)
			+ (((unsigned long) ucChar[14]) << 8) + (unsigned long) ucChar[15]);

	return true;
}

/*********************************************************
 ��    �ܣ��������ݿ⣬ͨ��ID�ҵ���Ӧ������
 ����˵����CBinary Id �ڼ���������ID
 �� �� ֵ�����ݿ������
 ˵    ������
 *********************************************************/
const vector<CBinary> CDatabase::SearchId(CBinary& Id)
{
	vector<CBinary> binVector;
	binVector.clear();

	// ����ļ�δ�򿪣��򷵻�
	if (!m_pfText)
	{
		return binVector;
	}

	try
	{
		if (m_pIndex)
		{
			long lOffset;
			if (-1 != (lOffset = GetOffsetBisearch(Id)))
				fseek(m_pfText, lOffset, SEEK_SET);
			else
				return binVector;
		}
		else
		{
			int result = 0;

			// ��λ��������ͷ�ĵط�
			fseek(m_pfText, HEADER_LEN, SEEK_SET);

			CBinary binID;
			char pchID[20];

			// �������õ�ƫ����
			unsigned long i;
			for (i = 0; i < m_ulIDNumber; i++)
			{
				// ����ַ�����
				memset(pchID, 0, sizeof(pchID));

				result = (int) fread(pchID, sizeof(unsigned char), m_ucIDLength,
						m_pfText);
				if (m_ucIDLength != result)
				{
					return binVector;
				}

				binID.WriteBuffer(pchID, m_ucIDLength);
				if (binID == Id)
				{
					break;
				}

				// ��λ����һ��������ƫ����ռ4���ֽ�
				fseek(m_pfText, 4, SEEK_CUR);
			}

			// û���ҵ���Ӧ������
			if (i == m_ulIDNumber)
			{
				return binVector;
			}

			// ����ƫ��λ��
			unsigned char ucPos[4];
			result = (int) fread(ucPos, sizeof(unsigned char), 4, m_pfText);
			if (4 != result)
			{
				return binVector;
			}

			unsigned long ulContentPos =
					(unsigned long) ((((unsigned long) ucPos[0]) << 24)
							+ (((unsigned long) ucPos[1]) << 16)
							+ (((unsigned long) ucPos[2]) << 8)
							+ (unsigned long) ucPos[3]);

			// ��λ������
			fseek(m_pfText, ulContentPos, SEEK_SET);
		}

		unsigned char unItemNum;
		fread(&unItemNum, sizeof(unsigned char), 1, m_pfText);
		if (0 == unItemNum)
		{
			return binVector;
		}

		//	CBinary binItem;  //  = new CBinary;
		for (int i = 0; i < unItemNum; i++)
		{
			unsigned char ucPos[4];
			memset(ucPos, 0, sizeof(ucPos));
			int result = (int) fread(ucPos, sizeof(unsigned char), 2, m_pfText); // �������
			if (result != 2)
			{
				binVector.clear();
				return binVector;
			}

			// ĳ��ĳ���
			unsigned short usItemLength =
					(unsigned short) ((((unsigned short) ucPos[0]) << 8)
							+ (unsigned short) ucPos[1]);

			// 11/23/2006
			//		char *pchBuffer = (char*)new unsigned char[usItemLength];
			unsigned char *pchBuffer = new unsigned char[usItemLength];

			assert(pchBuffer != NULL);
			memset(pchBuffer, 0, usItemLength);

			// ����������
			result = (int) fread(pchBuffer, sizeof(unsigned char), usItemLength,
					m_pfText);
			if (result != usItemLength)
			{
				binVector.clear();
				delete[] pchBuffer;

				return binVector;
			}

			// 11/23/2006 add by wcx
			//		binItem.WriteBuffer(pchBuffer, usItemLength);
			//		binVector.push_back(binItem);
			//		binItem =s_Crypt.Decrypt(pchBuffer, usItemLength);
			//binVector.push_back(s_Crypt.Decrypt(pchBuffer, usItemLength));

			binVector.push_back(s_Crypt.Decrypt(pchBuffer, usItemLength));
		}
	} catch (...)
	{

	}

	return binVector;
}

/*********************************************************
 ��    �ܣ��õ�����ID�ĳ���
 ����˵������
 �� �� ֵ������ID�ĳ���
 ˵    ���������ýӿ�
 *********************************************************/
unsigned char CDatabase::GetDBIDLength(void)
{
	return m_ucIDLength;
}

/*********************************************************
 ��    �ܣ��õ�����ID������
 ����˵������
 �� �� ֵ������ID������
 ˵    ���������ýӿ�
 *********************************************************/
unsigned long CDatabase::GetDBIDNumber(void)
{
	return m_ulIDNumber;
}

/*********************************************************
 ��    �ܣ�����CACHE�в��ң����ҵ�����CACHEʹ�ô��򲢷�������;
 ��δ�ҵ�����ı��⣬ȡ���ı�����������CACHE
 ����˵����CBinary binTextID �ַ���ID
 �� �� ֵ���ı�����
 ˵    ����ȫ�ֽӿ�
 *********************************************************/
string adsGetTextString(CBinary binTextID)
{
	string strContent;

	// ���ڻ����в���
	CDatabase::CTextCache *pCache = &(CDatabase::m_TextCache);

	string *strTemp = pCache->GetTextPointer(binTextID);
	if (NULL == strTemp)
	{
		if (!m_MenuTxtDb.IsOpen())
		{
			string strFilename;
			strFilename += CRunEnvironment::GetLanguage();
			strFilename += "_TXT.DB";

			if (!m_MenuTxtDb.Open(strFilename))
			{
				return strContent;
			}
		}

		vector<CBinary> vtBinText;
		vtBinText = m_MenuTxtDb.SearchId(binTextID);
		if (vtBinText.capacity() != 1)
		{
			return strContent;
		}

		vector<CBinary>::iterator cIter;
		cIter = vtBinText.begin();

		unsigned short length = cIter->GetSize();
		char* pchBuffer = (char*) new unsigned char[length];
		assert(pchBuffer != NULL);

		cIter->ReadBuffer(pchBuffer);
		strContent = pchBuffer;

		// �����ַ�����
		pCache->UpdateText(binTextID, strContent);

		delete[] pchBuffer;
		//dataBase.Close();//FOR MENU DISPLAY
	}
	else
	{
		strContent = strTemp->data();
	}

	return strContent;
}

/////////////////////////////////////////////////////////////////

long CDatabase::GetOffsetBisearch(CBinary ID)
{
	CBinary bin;
	long lOffset = -1;
	if (m_pIndex)
	{
		//bisearch	
		int iBegin = 0;
		int iEnd = m_ulIDNumber - 1;
		int iMiddle = (iBegin + iEnd) / 2;
		while (1)
		{
			int iResult = memcmp(
					m_pIndex + iMiddle * (m_ucIDLength + OFFSET_LENGTH),
					ID.GetBuffer(), m_ucIDLength);
			if (0 == iResult)
			{
				unsigned char * pTemp = (unsigned char *) m_pIndex
						+ iMiddle * (m_ucIDLength + OFFSET_LENGTH)
						+ m_ucIDLength;
				lOffset = (pTemp[0] << 24) | (pTemp[1] << 16) | (pTemp[2] << 8)
						| pTemp[3];
				break;
			}
			else if (0 < iResult)
			{
				if (iEnd == iMiddle)
					break;
				iEnd = iMiddle;
			}
			else
			{
				if (iBegin == iMiddle)
				{
					if (iEnd == iMiddle)
						break;
					else
						iBegin = iEnd;
				}
				else
				{
					iBegin = iMiddle;
				}
			}

			iMiddle = (iBegin + iEnd) / 2;
		}
	}

	return lOffset;
}

bool CDatabase::LoadIndex()
{
	m_pIndex = NULL;

	try
	{
		m_pIndex = new char[(m_ucIDLength + OFFSET_LENGTH) * m_ulIDNumber];

		if (NULL == m_pIndex)
			return false;

		// ��λ��������ͷ�ĵط�
		fseek(m_pfText, HEADER_LEN, SEEK_SET);
		// ���ļ���������
		if ((m_ucIDLength + OFFSET_LENGTH) * m_ulIDNumber
				!= fread(m_pIndex, sizeof(char),
						(m_ucIDLength + OFFSET_LENGTH) * m_ulIDNumber,
						m_pfText))
		{
			delete[] m_pIndex;
			m_pIndex = NULL;
			return false;
		}
	} catch (...)
	{
		if (m_pIndex)
			delete[] m_pIndex;
		m_pIndex = NULL;
	}

	return true;
}

bool CDatabase::IsOpen()
{
	return (NULL != m_pfText);
}

//hpy added begin
string GetNameMapping(string szOldName)
{
	string szFileName;
	string szNewFullName;
	string szNewFileName;
	char ch;
	char cMap;
	if (szOldName.find(".dat") != -1) //�ݲ�����dat�ļ�
		return szOldName;
	if (szOldName.find(".DAT") != -1) //�ݲ�����dat�ļ�
		return szOldName;

	szFileName = szOldName.substr(szOldName.rfind('/') + 1); //��ò���·�����ļ���

	int nBuffsize = sizeof(g_mapName);
	int nRows = sizeof(g_mapName) / (2 * sizeof(unsigned char));
	for (int i = 0; i < szFileName.length(); i++)
	{
		ch = szFileName.at(i);
		if (ch >= 'a' && ch <= 'z')
			ch = ch - 32;
		cMap = ch;
		if (cMap >= 'A' && cMap <= 'Z')
		{
			int iLoop = 0;

			for (iLoop = 0; iLoop < nRows; ++iLoop)
			{
				if (g_mapName[iLoop][0] == ch)
				{
					cMap = g_mapName[iLoop][1];
					break;
				}
			}
		}
		szNewFileName += cMap;
	}

// 	if( szFileName.find(".db") != -1 )//�滻.dbΪ.pnt
// 		szNewFileName.replace(szFileName.find(".db"),3,".PNT");
// 	if( szFileName.find(".DB") != -1 )//�滻.dbΪ.pnt
// 		szNewFileName.replace(szFileName.find(".DB"),3,".PNT");

	if (szFileName.find(".db") != -1) //�滻.dbΪ.pnt
		szNewFileName.replace(szFileName.find(".db"), 3, ".0000");
	if (szFileName.find(".DB") != -1) //�滻.dbΪ.pnt
		szNewFileName.replace(szFileName.find(".DB"), 3, ".0000");

	szOldName.erase(szOldName.rfind('/') + 1);

	szNewFullName = szOldName + szNewFileName;

	return szNewFullName;
}
//hpy added end
