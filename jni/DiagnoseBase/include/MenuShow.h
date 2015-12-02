#ifndef __MENUSHOW_H_
#define __MENUSHOW_H_


#include <string>
using namespace std;
#include "adsStd.h"
#include "Binary.h"
#include "BaseDisplay.h"


class CMenuShow : public CBaseDisplay
{
public:
	CMenuShow();
	~CMenuShow();

	enum 
	{
		DT_MENU = 0
    };

public:
	class CMenuStruct 
	{
	public:
		short  m_i16MenuSelected;
		short  m_i16MenuScreenFirstLineItem;

		CMenuStruct() 
		{ 
			m_i16MenuSelected            = -1; 
			m_i16MenuScreenFirstLineItem = 0; 
		}

		virtual ~CMenuStruct() 
		{ 

		}
	};

public:
	void Init(const char *pTitle = NULL);
	void Init(string strTitle);
	void Init(CBinary idTitle);
	bool SetFlag (unsigned char byFlag);
	bool Add(string strMenuItem);
	bool Add(CBinary idMenuItem);

	short Show(CMenuStruct &MenuParameter);

};

#endif // __MENUSHOW_H_