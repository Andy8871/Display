#ifndef	__VEHICLEINFO_H_
#define	__VEHICLEINFO_H_

#include <string>
using namespace std;
#include "basedisplay.h"
#include "adsStd.h"
#include "Binary.h"


class CVehicleInfo : public CBaseDisplay
{
public:
	enum 
	{
		DT_VEHICLE_INFORMATION  = 50
    };


	void Init (const char *pTitle = NULL);	
	void Init (string strTitle);
	void Init (CBinary idTitle);
	
	short Add (string strExplain, string strContain);
	short Add (CBinary idExplain, string strContain);
	bool SetFlag(unsigned char iFlag);
	short Show ();

};

#endif	//__VEHICLEINFO_H_