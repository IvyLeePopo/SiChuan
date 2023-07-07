/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：AesECB.h
 * 摘    要：实现AES加密/解密功能类（仅仅支持128位的ECB模式），声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月18日
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
	//加密数据
	const CString EncryptData(IN const CString& strSrc);
	//解密
	const CString DecryptData(IN const CString& strSrc);
//Inner Functions
private:
	//16进制转2进制
	void Hex2Bin(IN LPTSTR pHex,OUT PBYTE pBin,IN DWORD dwHex_len);
	//2进制转16进制
	CString Bin2Hex(IN PBYTE pBin,IN DWORD dwBin_Len);

	// PKCS5Padding: aaa(0x61,0x61,0x61) -> (0x61,0x61,0x61,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d,0x0d)      add hex(16 - n)
	// PKCS7Padding: aaa(0x61,0x61,0x61) -> (0x61,0x61,0x61,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00)	   add 0
	// these two functions only work for ECB mode, PKCS5Padding, 128 bit
	//内部实现加密的函数
	std::string Encrypt( std::string strSrc, std::string strKey );
	//内部实现解密的函数
	std::string Decrypt( std::string strSrc, std::string strKey );
	//内部数据转换函数
	void StringToHex( const char* pSrc, unsigned char* pDest );
	//内部数据转换辅助函数
	void Padding( unsigned char* pSrc, int nSrcLen );
//Attributes
private:
	//密钥
	CString	m_strKey;
};

