#ifndef	__SHORTTESTSHOW_H_
#define	__SHORTTESTSHOW_H_

#include <string>
using namespace std;
#include "adsStd.h"
#include "basedisplay.h"
#include "Binary.h"
#include "Database.h"



class CShortTestShow : public CBaseDisplay
{
public:
	enum
	{
		DT_SHORT_TEST           = 100
	};

	//added by johnnyling
	short	m_nEnterIndex;
	short GetEnterIndex(void);

	void Init (const char* pTitle = NULL,short hide_btn_stop = 0x0001);
	void Init (CBinary idTitle);
	void Init (string strTitle);	

	short AddItem (const char* pContain);
	short AddItem (string strContain);
	short AddItem (CBinary idContain);


	short ChangeValue (CBinary idValue);
	short ChangeValue (const char* pValue);
	short ChangeValue (string strValue);
	bool SetFlag(unsigned char iFlag);
	short Show ();

};

#endif	//__SHORTTESTSHOW_H_