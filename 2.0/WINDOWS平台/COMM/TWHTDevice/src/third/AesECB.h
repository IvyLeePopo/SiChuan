/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�AesECB.h
 * ժ    Ҫ��ʵ��AES����/���ܹ����ࣨ����֧��128λ��ECBģʽ���������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��18��
 */
#pragma once
#include <string>
#include "aes.h"

using namespace AES_ECB_ALG;

#define KEYCODELENGTH 16

class CAesECB
{
public:
	CAesECB(IN const CString& strKey);
	~CAesECB(void);
//Interface
public:
	//��������
	const CString EncryptData(IN const CString& strSrc);
	//����
	const CString DecryptData(IN const CString& strSrc);
//Inner Functions
private:
	//16����ת2����
	void Hex2Bin(IN LPTSTR pHex,OUT PBYTE pBin,IN DWORD dwHex_len);
	//2����ת16����
	CString Bin2Hex(IN PBYTE pBin,IN DWORD dwBin_Len);

	// PKCS5Padding: aaa(0x61,0x61,0x61) -> (0x61,0x61,0x61,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d)      add hex(16 - n)
	// PKCS7Padding: aaa(0x61,0x61,0x61) -> (0x61,0x61,0x61,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00)	   add 0
	// these two functions only work for ECB mode, PKCS5Padding, 128 bit
	//�ڲ�ʵ�ּ��ܵĺ���
	std::string Encrypt( std::string strSrc, std::string strKey );
	//�ڲ�ʵ�ֽ��ܵĺ���
	std::string Decrypt( std::string strSrc, std::string strKey );
	//�ڲ�����ת������
	void StringToHex( const char* pSrc, unsigned char* pDest );
	//�ڲ�����ת����������
	void Padding( unsigned char* pSrc, int nSrcLen );
//Attributes
private:
	//��Կ
	CString	m_strKey;
};

