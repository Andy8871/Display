/*
 * CommBase.cpp
 *
 *  Created on: 2015-11-2
 *      Author: andy
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <android/log.h>
#include <CommBaseClass.h>

#define TAG "CCommBase"

CCommBase::CCommBase()
{
	env = NULL;
	m_comBuff = NULL;
	m_guiCallbackWaitForResponse = NULL;
	m_guiCallbackImmediate = NULL;
}

/**
 *	����C++��Java֮���ͨѶ
 *	��ȡJava�еĻص�����
 */
bool CCommBase::Connect(JNIEnv* pEnv)
{
	if (NULL == pEnv)
		return false;

	/* �����ڴ�Java��C++֮������ݽ��� */
	if (NULL == m_comBuff)
	{
		m_comBuff = new char[0xFFFF];
		if (NULL == m_comBuff)
			return false;
		memset(m_comBuff, 0, 0xFFFF);
	}

	env = pEnv;
	/* ��ȡJava�ص��������ڵ��� */
	jclass clazz = env->FindClass("com/TD/Model/Diagnose");
	if (NULL == clazz)
		return false;
	m_clazz = clazz;

	/* ��ȡJava�ص�����guiCallbackImmediate */
	if (NULL == m_guiCallbackImmediate && NULL != m_clazz)
	{
		m_guiCallbackImmediate = env->GetStaticMethodID(m_clazz,
				"guiCallbackImmediate", "([B)Z");
		if (NULL == m_guiCallbackImmediate)
		{
			if (NULL != m_comBuff)
			{
				delete []m_comBuff;
				m_comBuff = NULL;
			}
			return false;
		}
	}

	/* ��ȡJava�ص�����guiCallbackWaitForResponse */
	if (NULL == m_guiCallbackWaitForResponse && NULL != m_clazz)
	{
		m_guiCallbackWaitForResponse = env->GetStaticMethodID(m_clazz,
				"guiCallbackWaitForResponse", "([B)[B");
		if (NULL == m_guiCallbackWaitForResponse)
		{
			if (NULL != m_comBuff)
			{
				delete[] m_comBuff;
				m_comBuff = NULL;
			}
			return false;
		}
	}
	return true;
}

CCommBase::~CCommBase()
{
	Disconnect();
}

void CCommBase::Disconnect()
{
	if (NULL != m_clazz)
	{
		env->DeleteLocalRef(m_clazz);
		m_clazz = NULL;
	}

	if (NULL != m_comBuff)
	{
		delete []m_comBuff;
		m_comBuff = NULL;
	}
}

/**
 *	��UI�������ݺ�ֱ�ӷ���
 */
bool CCommBase::WriteForImmediate(const char* buff, int nLen)
{
	if (NULL == m_guiCallbackImmediate && NULL == m_clazz)
		return false;
	/* ��C�ַ���ת��ΪJava��byte�������� */
	jbyte* pBuff = (jbyte*) buff;
	jbyteArray arrBuff = env->NewByteArray(nLen);
	env->SetByteArrayRegion(arrBuff, 0, nLen, pBuff);
	bool bRes = env->CallStaticBooleanMethod(m_clazz, m_guiCallbackImmediate,
			arrBuff);

	/* �ͷŷ�����ڴ� */
	env->ReleaseByteArrayElements(arrBuff, pBuff, 0);
	env->DeleteLocalRef(arrBuff);
	return bRes;
}

/**
 * 	��UI�������ݶ���ֱ��UI�н���������ز��˳�
 */
char* CCommBase::WriteForWait(const char* buff, int nLen, int & outLen)
{
	if (NULL == m_clazz && NULL == m_guiCallbackWaitForResponse)
		return NULL;

	/* ��C�ַ���ת��ΪJava��byte�������� */
	jbyte* pBuff = (jbyte*) buff;
	jbyteArray arrBuff = env->NewByteArray(nLen);

	env->SetByteArrayRegion(arrBuff, 0, nLen, pBuff);

	/* ����Java����:�����ݷ��͸�UI,���ȴ�UI�Ĳ������ */
	jbyteArray objRes = (jbyteArray) env->CallStaticObjectMethod(m_clazz,
			m_guiCallbackWaitForResponse, arrBuff);

	/* �ͷŷ�����ڴ� */
	env->ReleaseByteArrayElements(arrBuff, pBuff, JNI_ABORT);
	env->DeleteLocalRef(arrBuff);
	arrBuff = NULL;
	if (NULL == objRes)
	{
		return NULL;
	}

	/* ��UI���صĲ��������Java byte[]ת����C���Ե��ַ�ָ�� */
	outLen = env->GetArrayLength(objRes);
	memset(m_comBuff, 0, 0xFFFF);
	jbyte* pRes =  env->GetByteArrayElements(objRes, JNI_FALSE);
	memcpy(m_comBuff, (char*)pRes, outLen);
	/* �ͷŷ�����ڴ� */
	env->ReleaseByteArrayElements(objRes, pRes, JNI_ABORT);
	env->DeleteLocalRef(objRes);
	objRes = NULL;
	return m_comBuff;
}

void CCommBase::LogExceptionFromJava()
{
	if (env->ExceptionOccurred() != NULL)
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}

void CCommBase::ThrowExceptionToJava(string strNote)
{
	jclass clazzException = env->FindClass("java/lang/RuntimeException");
	env->ThrowNew(clazzException, strNote.c_str());
}

/*jstring CCommBase::str2jstring(const char* pBuff)
{
	//��ȡJava String��
	jclass clazz = env->FindClass("java/lang/String");

	//��ȡJava String��Ĺ��캯��String(byte[], String),��������ת��
	jmethodID ctorID = env->GetMethodID(clazz, "<init>",
			"([BLjava/lang/String;)V");

	//����byte����
	jbyteArray arrByte = env->NewByteArray(strlen(pBuff));

	//��char*ת��Ϊbyte����
	env->SetByteArrayRegion(arrByte, 0, strlen(pBuff), (jbyte*) pBuff);

	//���뷽ʽ�����ڽ�byteת��ΪString
	jstring encoding = env->NewStringUTF("GB2312");

	return (jstring) env->NewObject(clazz, ctorID, arrByte, encoding);
}

string CCommBase::jstring2str(jstring jstr)
{
	char* pRes = NULL;
	jclass clazz = env->FindClass("java/lang/String");
	jstring encoding = env->NewStringUTF("GB2312");
	jmethodID ctorID = env->GetMethodID(clazz, "getBytes",
			"(Ljava/lang/String;)[B");
	jbyteArray arrByte = (jbyteArray) env->CallObjectMethod(jstr, ctorID,
			encoding);
	jsize len = env->GetArrayLength(arrByte);
	jbyte* bytes = env->GetByteArrayElements(arrByte, JNI_FALSE);
	if (len > 0)
	{
		pRes = (char*) malloc(len + 1);
		memset(pRes, 0, len + 1);
		memcpy(pRes, bytes, len);
		pRes[len] = 0;
	}
	env->ReleaseByteArrayElements(arrByte, bytes, 0);
	string stemp(pRes);
	free(pRes);
	pRes = NULL;
	return stemp;
}*/
