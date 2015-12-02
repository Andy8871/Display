#ifndef	__DIAGNOSISAPPLICATION_H_
#define	__DIAGNOSISAPPLICATION_H_
#include <jni.h>

class CDiagnosisFrame
{
public:
	CDiagnosisFrame(void);
	~CDiagnosisFrame(void);

public:
	int InitInstance(JNIEnv* env);
};

#endif	//__DIAGNOSISAPPLICATION_H_
