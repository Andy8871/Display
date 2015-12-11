/******************************************************************************

 Copyright (c) 2005, AUTOBOSS Inc.
 All rights reserved.

 文件名称：Database.cpp
 文档标识：007汽车诊断平台详细设计说明书
 摘    要：本类用于根据提供的索引ID从数据库查找对应的内容

 历史记录：
 ----------------------------------------------------------------------------
 时     间   作者		版本号		操    作    内    容
 ----------------------------------------------------------------------------
 2005.11.28  zhangsh		1.0AX		创建此类。
 2006.7.3    wcx         0.0908      加入IsOpen(), 用于CMenuTree优化查找菜
 单文本使用

 ******************************************************************************/
#include <android/log.h>
#include	<math.h>
#include	"Database.h"
#include	"Information.h"
#include	"RunEnvironment.h"
#include    "assert.h"
#include	"../../Security/include/SecurityBaseClass.h"

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
 功    能：由给定的索引ID在list中查找，如找到则返回其对应的文本串起始地址，
 没找到则返回空（NULL）
 参数说明：idText-   要查找的索引编号
 返 回 值：对应的文本串起始地址
 说    明：无
 **************************************************************/
string* CDatabase::CTextCache::GetTextPointer(CBinary idText)
{
	string *pstrTemp = NULL;

	list<struct tagTextCache>::iterator cIter;

	for (cIter = m_lstTextCache.begin(); cIter != m_lstTextCache.end(); cIter++)
	{
		if (cIter->idText == idText)
		{
			// 调整到首位
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
 功    能：更新给定的索引ID对应的LIST中字符串内容
 参数说明：idText-要查找的索引编号，strText-新的字符串内容
 返 回 值：true-更新成功，false-更新失败
 说    明：无
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

	// 如果在缓冲里存在，则将其调整到首部
	if (cIter != m_lstTextCache.end())
	{
		cIter->strTextContain = strText;
		m_lstTextCache.insert(m_lstTextCache.begin(), *cIter);

		m_lstTextCache.erase(cIter);
	}
	else
	{
		// 如果没有找到，则添加到首部
		m_lstTextCache.push_front(updata);

		// 如果缓冲条数大于LISTCACHEMAXITEM条，则删除尾部的一条
		if (m_lstTextCache.size() > LISTCACHEMAXITEM)
		{
			m_lstTextCache.pop_back();
		}
	}

	return true;
}

/**************************************************************
 功    能：缺省构造函数
 参数说明：无
 返 回 值：无
 说    明：初始化成员变量
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
 功　　能：析构函数
 参数说明：无
 返 回 值：无
 说    明：判断文件指针是否为空，不为空再关闭，并将文件指针置空
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
 功    能：打开文件
 参数说明：unsigned short uiFileCodeName 文件类型
 返 回 值：成功—true 失败—false
 说    明：无
 *************************************************************/
bool CDatabase::Open(unsigned short uiFileCodeName)
{
	// 如果超出预定义的范围，则出错
	if (uiFileCodeName < 0 || uiFileCodeName >= FILEFUNCTIONNUM)
	{
		return false;
	}

	// 取得工作目录
	string strFilename = CRunEnvironment::GetDiagnosisDirectory();

	// 工作目录的最后一个字符，不包括'\0'
	char chEnd = *(strFilename.end() - 1);

	// 判断目录末尾是否加了'\\'或'/'字符
	if (chEnd != '\\' && chEnd != '/')
	{
		strFilename += '\\';
	}

	if (DB_COMMAND == uiFileCodeName)
	{
		// 车系名
		string strName = CInformation::GetVehiclesSystemName();
		strName = strName.substr(0, 4);
		strFilename += strName;
	}
	else
	{
		// 语言代码
		strFilename += CRunEnvironment::GetLanguage();
	}

	// 文件后缀
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

	if (!CompareDBVersion(bFlag)) //比较诊断版本; 仅txt/dtc/ds文件需要
	{
		return false;
	}

	LoadIndex();

	return true;
}

/*************************************************************
 功    能：打开文件
 参数说明：string strFileName 文件名串(不带路径名，
 默认为应用程序当前路径)
 返 回 值：成功—true 失败—false
 说    明：无
 *************************************************************/
bool CDatabase::Open(string strFile)
{
	string strFilename;

	//取得工作目录名
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

	if (!CompareDBVersion(bFlag)) //比较诊断版本
	{
		__android_log_print(ANDROID_LOG_DEBUG, "CDatabase",
				"Failed to CompareDBVersion");
		return false;
	}

	LoadIndex();

	return true;
}

/*************************************************************
 功    能：关闭打开的文件
 参数说明：无
 返 回 值：无
 说    明：无
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
 功    能：得到数据库诊断版本内容，与应用程序版本进行比较
 参数说明：无
 返 回 值：比较相等－true，　不相等－false
 说    明：私有函数
 *********************************************************/
bool CDatabase::CompareDBVersion(bool bFlag)
{
	unsigned char ucEncrptChar[35];
	memset(ucEncrptChar, 0, 35);

	unsigned char ucChar[35];
	memset(ucChar, 0, 35);

	try
	{
		// 定位到开头
		fseek(m_pfText, 0, SEEK_SET);

		// 读文件头内容
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
	// 判断数据库标志

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

	// 数据库版本
	double fDBVer = atof(pchVersion);

	// 诊断程序版本
	double fDiagVer = CInformation::GetVersion();

	__android_log_print(ANDROID_LOG_DEBUG, "CDatabase", "fDBVer == %f----------fDiagVer = %f", fDBVer, fDiagVer);

	// 判断诊断版本
	//if(abs(fDBVer-fDiagVer) > 0.00001 )
	if (fabs(fDBVer - fDiagVer) > 0.00001)
	{
		//由于编译程序的BUG，暂时不比较版本，等加密功能加入后再说。
		//2006-11-07,johnnyling
		return false; //11/23/2006 by wcx 
	}

	// 数据库索引字节长度
	m_ucIDLength = ucChar[6];

	// ID数量
	m_ulIDNumber = (unsigned long) ((((unsigned long) ucChar[12]) << 24)
			+ (((unsigned long) ucChar[13]) << 16)
			+ (((unsigned long) ucChar[14]) << 8) + (unsigned long) ucChar[15]);

	return true;
}

/*********************************************************
 功    能：检索数据库，通过ID找到对应的内容
 参数说明：CBinary Id 在检索的内容ID
 返 回 值：数据库的内容
 说    明：无
 *********************************************************/
const vector<CBinary> CDatabase::SearchId(CBinary& Id)
{
	vector<CBinary> binVector;
	binVector.clear();

	// 如果文件未打开，则返回
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

			// 定位到索引开头的地方
			fseek(m_pfText, HEADER_LEN, SEEK_SET);

			CBinary binID;
			char pchID[20];

			// 由索引得到偏移量
			unsigned long i;
			for (i = 0; i < m_ulIDNumber; i++)
			{
				// 清空字符缓冲
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

				// 定位到下一条索引，偏移量占4个字节
				fseek(m_pfText, 4, SEEK_CUR);
			}

			// 没有找到对应的索引
			if (i == m_ulIDNumber)
			{
				return binVector;
			}

			// 内容偏移位置
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

			// 定位到内容
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
			int result = (int) fread(ucPos, sizeof(unsigned char), 2, m_pfText); // 读单项长度
			if (result != 2)
			{
				binVector.clear();
				return binVector;
			}

			// 某项的长度
			unsigned short usItemLength =
					(unsigned short) ((((unsigned short) ucPos[0]) << 8)
							+ (unsigned short) ucPos[1]);

			// 11/23/2006
			//		char *pchBuffer = (char*)new unsigned char[usItemLength];
			unsigned char *pchBuffer = new unsigned char[usItemLength];

			assert(pchBuffer != NULL);
			memset(pchBuffer, 0, usItemLength);

			// 读单项内容
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
 功    能：得到索引ID的长度
 参数说明：无
 返 回 值：索引ID的长度
 说    明：调试用接口
 *********************************************************/
unsigned char CDatabase::GetDBIDLength(void)
{
	return m_ucIDLength;
}

/*********************************************************
 功    能：得到索引ID的数量
 参数说明：无
 返 回 值：索引ID的数量
 说    明：调试用接口
 *********************************************************/
unsigned long CDatabase::GetDBIDNumber(void)
{
	return m_ulIDNumber;
}

/*********************************************************
 功    能：先在CACHE中查找，如找到更新CACHE使用次序并返回内容;
 如未找到则打开文本库，取得文本串，并更新CACHE
 参数说明：CBinary binTextID 字符串ID
 返 回 值：文本内容
 说    明：全局接口
 *********************************************************/
string adsGetTextString(CBinary binTextID)
{
	string strContent;

	// 先在缓冲中查找
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

		// 更新字符缓冲
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

		// 定位到索引开头的地方
		fseek(m_pfText, HEADER_LEN, SEEK_SET);
		// 读文件索引内容
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
	if (szOldName.find(".dat") != -1) //暂不处理dat文件
		return szOldName;
	if (szOldName.find(".DAT") != -1) //暂不处理dat文件
		return szOldName;

	szFileName = szOldName.substr(szOldName.rfind('/') + 1); //获得不带路径的文件名

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

// 	if( szFileName.find(".db") != -1 )//替换.db为.pnt
// 		szNewFileName.replace(szFileName.find(".db"),3,".PNT");
// 	if( szFileName.find(".DB") != -1 )//替换.db为.pnt
// 		szNewFileName.replace(szFileName.find(".DB"),3,".PNT");

	if (szFileName.find(".db") != -1) //替换.db为.pnt
		szNewFileName.replace(szFileName.find(".db"), 3, ".0000");
	if (szFileName.find(".DB") != -1) //替换.db为.pnt
		szNewFileName.replace(szFileName.find(".DB"), 3, ".0000");

	szOldName.erase(szOldName.rfind('/') + 1);

	szNewFullName = szOldName + szNewFileName;

	return szNewFullName;
}
//hpy added end
