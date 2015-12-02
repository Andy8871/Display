#ifndef	__MULTISELECTSHOW_H_
#define	__MULTISELECTSHOW_H_


#include <string>

using namespace std;
#include "adsStd.h"
#include "basedisplay.h"
#include "Binary.h"
#include "Database.h"


class CMultiSelectShow : public CBaseDisplay
{
public:
	CMultiSelectShow();
	~CMultiSelectShow();

	enum
	{
		DT_MUTI_SELECT          = 40
	};

public:

	class CSelectedItemData 
	{
	friend class CMultiSelectShow;
	
    public: 
		enum
		{
			SID_ALL_ITEM = -1
		};

	public:
		CSelectedItemData();

		void SetMaxSelectedNumber(unsigned short uiMaxSelectedNumber = 0xFFFF) ;
		bool IsSelected(short iSequence);

		short GetItemNumber();
		short SetItemNumber(short nNumber);
		short SetSelectedItem(short iSequence, bool IsSelected);
		short UserClickButtonKeyValue();

	protected: 
		CBinary m_binMaskCode;
		short m_i16ScreenFirstLineItemSequence;
		short m_i16ItemNumber;
		short m_iUserClickButtonKeyValue;
		unsigned short m_uiMaxSelectedNumber;

	};

public:
	void Init(const char* pTitle = NULL);
	void Init(string strTitle);
	void Init(CBinary idTitle);
	bool SetFlag(unsigned char iFlag);
	short Add(CBinary idDataStream);
	short Add(string strMutiSelectedItem);
	short AddMsg(string strMsg);
//	short SetFlag(short iFlag=0);
	bool Show(CSelectedItemData& SelectedData,short &iSelectedIndex,unsigned char iFlag=0);
	bool Show(CSelectedItemData& SelectedData);

private:
	CDatabase *m_pDataBase;
	short m_iItemNumber;  // 实际的多选项目数量
	string	m_strMsg;
};

#endif	//__MULTISELECTSHOW_H_
