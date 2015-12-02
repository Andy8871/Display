#ifndef	__WARRANT_H__
#define	__WARRANT_H__



class CWarrant
{
public:
	CWarrant(void) { };
	~CWarrant(void) { };
	int Lisence(const char *pSerialNumber,const char* pVecleName,const char* pLanguage,double fVersion);

};

#endif	//__WARRANT_H_