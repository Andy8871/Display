/*
 * CommBaseClass.h
 *
 *  Created on: 2015-11-2
 *      Author: andy
 */

#ifndef COMMBASECLASS_H_
#define COMMBASECLASS_H_

#include <jni.h>
#include <string>
using namespace std;

class Lock{
public:
	Lock():mutex(PTHREAD_MUTEX_INITIALIZER){
		pthread_mutex_init(&mutex, NULL);
	}
	void GetLock()
	{
		pthread_mutex_lock(&mutex);
	}
	void ReleaseLock()
	{
		pthread_mutex_unlock(&mutex);
	}
	~Lock()
	{
		pthread_mutex_destroy(&mutex);
	}
private:
	pthread_mutex_t mutex;
};

class CCommBase
{
public:
	static CCommBase* GetInstance();
public:
	~CCommBase();
	bool Connect(JNIEnv* env);
	void Disconnect();
	bool Read(char* buff, int nSize);
	char* WriteForWait(const char* buff, int nLen, int & outLen);
	bool WriteForImmediate(const char* buff, int nLen);
protected:
	CCommBase();
protected:
	static CCommBase* m_pInstance;
private:
	void LogExceptionFromJava();
	void ThrowExceptionToJava(string strNote);

private:
	/*jstring str2jstring(const char*);
	string jstring2str(jstring);*/
private:
	jmethodID m_guiCallbackWaitForResponse;
	jmethodID m_guiCallbackImmediate;
	jclass m_clazz;
	JNIEnv* m_env;
	jbyteArray m_byteData;
	char* m_comBuff;
};

#endif /* COMMBASECLASS_H_ */
