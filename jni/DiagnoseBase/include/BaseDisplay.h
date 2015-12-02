#ifndef __BASEDISPLAY_H_
#define __BASEDISPLAY_H_


#include "CommWithDisplay.h"


class CBaseDisplay
{
public:
	CBaseDisplay();
	virtual ~CBaseDisplay();

protected:
	CCommunicationWithDisplay* m_pSendAndReceive;	

public:
	void SetCommunicationPointer(CCommunicationWithDisplay* pSendAndReceive);


};

#endif // __BASEDISPLAY_H_
