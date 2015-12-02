
#ifndef	__PROFILE_H_
#define __PROFILE_H_

#include "string"
using namespace std;

#include "tinyxml.h"
class CProfile  
{
protected:
	class CTextFile  
	{
	public:
		string SetValueString(string strSrc , string strValue);
		string GetValueString(string strTemp);
		bool LoadFile(string strFileName);
		bool IsEof();
		bool KeyNameIs(string strSrcText, string strKeyName);
		bool SectionNameIs(string strSrcText, string strSectionName);
		bool IsKeyLine(string strText);
		bool IsSectionLine(string strText);
		char * GetCurrentPointer();
		string GetLine();
		CTextFile();
		virtual ~CTextFile();

	protected:
		char * m_pFileContain;
		int m_iCurrentPosition;
	};

public:
	static string GetString (string strFileName, string strSectionName, string strKeyName, string strDefaultString, bool bCaseSensitive=false); 
	static int GetInteger (string strFileName, string strSectionName, string strKeyName, int intDefaultValue, bool bCaseSensitive=false); 
	static bool WriteString (string strFileName, string strSectionName, string strKeyName, string strString, bool bCaseSensitive=false); 
	static bool WriteInteger (string strFileName, string strSectionName, string strKeyName, int iValue, bool bCaseSensitive=false);

	//Add by andy 2015.6.27
	//brief:following functions use tinyxml for configuration file
	static string GetString(string strElemName, string strAttribute, string strDefaultString);
	static int GetInteger(string strElemName, string strAttribute, int iDefaultValue);
	static bool WriteString(string strElemName, string strAttribute, string strValue);
	static bool WriteInteger(string strElemName, string strAttribute, int iValue);
private:
	static bool GetElementByName(TiXmlElement* pRoot, string strElemName, TiXmlElement*& xmlNode);
};

//hpy 以下是网络通信部分，暂不理会
#define DISPLAY_PROFILE			\
(CRunEnvironment::GetDisplayDirectory()[CRunEnvironment::GetDisplayDirectory().length()-1]=='\\'?CRunEnvironment::GetDisplayDirectory()+"display.ini":CRunEnvironment::GetDisplayDirectory()+"\\"+"display.ini")

#define SEC_COMM_PORT			"CommPort"
#define KEY_COMM_PORT			"CommPort"
#define KEY_LAST_SERIAL_PORT	"LastSerialPort"
#define KEY_VCI_IP				"VCI_IP"
#define KEY_VCI_PORT			"VCI_Port"


#endif // !defined(AFX_TEXTFILE_H__0B34BBD2_9145_4797_9DD1_BF7A9B8B2151__INCLUDED_)

