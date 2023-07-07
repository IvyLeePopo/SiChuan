/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：AesECB.cpp
 * 摘    要：实现AES加密/解密功能类（仅仅支持128位的ECB模式），实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月18日
 */
#include "stdafx.h"
#include "AesECB.h"

CAesECB::CAesECB(IN const CString& strKey)
	:m_strKey(strKey)
{
}


CAesECB::~CAesECB(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CAesECB::EncryptData
//     
// 功能描述：加密数据
//     
// 输入参数：IN const CString& strSrc
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口函数
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CAesECB::EncryptData(IN const CString& strSrc)
{
	std::string strData = Encrypt((LPCSTR)strSrc,(LPCSTR)m_strKey);
	CString strReturn;
	strReturn.Format(_T("%s"),strData.c_str());
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CAesECB::DecryptData
//     
// 功能描述：解密数据
//     
// 输入参数：IN const CString& strSrc
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口函数
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CAesECB::DecryptData(IN const CString& strSrc)
{
	std::string strData = Decrypt((LPCSTR)strSrc,(LPCSTR)m_strKey);
	CString strReturn;
	strReturn.Format(_T("%s"),strData.c_str());
	return strReturn;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CAesECB::Encrypt
//     
// 功能描述：内部实现加密的函数
//     
// 输入参数：std::string strSrc
//         ：std::string strKey
// 输出参数：std::string
// 编写人员：ROCY
// 编写时间：2016年8月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
std::string CAesECB::Encrypt( std::string strSrc, std::string strKey )
{
	aes_context ctx;
	const char* pSrc = 0;
	const char* pTmpSrc = 0;
	unsigned char* pDest = 0;
	unsigned char* pTmpDest = 0;
	int nSrcLen = 0;
	int nDestLen = 0;
	unsigned char buf[KEYCODELENGTH];
	unsigned char key[KEYCODELENGTH];
	StringToHex( strKey.c_str(), key );
    aes_set_key( &ctx, key, 128);
	pSrc = strSrc.c_str();
	nSrcLen = strSrc.size();
	nDestLen = (nSrcLen / KEYCODELENGTH) * KEYCODELENGTH + KEYCODELENGTH;
	pDest = new unsigned char[nDestLen+1];

	std::string strRet;
	if(NULL != pDest)
	{
		memset( pDest, 0, nDestLen+1 );
		pTmpSrc  = pSrc;
		pTmpDest = pDest;
		while( (pTmpSrc - pSrc) < nSrcLen )
		{
			StringToHex(pTmpSrc, buf);
			aes_encrypt( &ctx, buf, buf );
			memcpy( pTmpDest, buf, KEYCODELENGTH );
			pTmpSrc  += KEYCODELENGTH;
			pTmpDest += KEYCODELENGTH;
		}
		if( (pTmpDest - pDest) < nDestLen )	// if the source size % KEYCODELENGTH == 0 then need to add an extra buffer 
		{
			StringToHex(0, buf);
			aes_encrypt( &ctx, buf, buf );
			memcpy( pTmpDest, buf, KEYCODELENGTH );
		}
		strRet = (LPCSTR)Bin2Hex(pDest,nDestLen);
		delete [] pDest;
	}
	return strRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CAesECB::Decrypt
//     
// 功能描述：内部实现解密的函数
//     
// 输入参数：std::string strSrc
//         ：std::string strKey
// 输出参数：std::string
// 编写人员：ROCY
// 编写时间：2016年8月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
std::string CAesECB::Decrypt( std::string strSrc, std::string strKey )
{
	aes_context ctx;
	const char* pSrc		= 0;
	const char* pTmpSrc		= 0;
	unsigned char* pDest	= 0;
	unsigned char* pTmpDest = 0;
	int nSrcLen = 0;
	int nDestLen = 0;
	unsigned char buf[KEYCODELENGTH] = {0};
	unsigned char key[KEYCODELENGTH] = {0};
	StringToHex( strKey.c_str(), key );
    aes_set_key( &ctx, key, 128);

	unsigned char byDataBuffer[20480]={0};
	Hex2Bin((LPSTR)strSrc.c_str(),byDataBuffer,strSrc.size());
	nSrcLen = strSrc.size()/2;

	pSrc = (const char*)&byDataBuffer;
	nDestLen = nSrcLen;

	std::string strRet;
	pDest = new unsigned char[nDestLen];
	if(NULL != pDest)
	{
		memset( pDest, 0, nDestLen );
		pTmpSrc  = pSrc;
		pTmpDest = pDest;
		while( (pTmpSrc - pSrc) < nSrcLen )
		{
			memcpy(buf, pTmpSrc, KEYCODELENGTH);
			aes_decrypt( &ctx, buf, buf );
			memcpy(pTmpDest, buf, KEYCODELENGTH);
			pTmpSrc  += KEYCODELENGTH;
			pTmpDest += KEYCODELENGTH;
		}
		unsigned char ucTest = 0;
		while(ucTest = *(pTmpDest - 1))
		{
			if( ucTest > 0 && ucTest <= 0x10 )
				*(pTmpDest-1) = 0;
			else
				break;
			pTmpDest--;
		}
		strRet = (char*)pDest;
		delete [] pDest;
	}
	return strRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CAesECB::StringToHex
//     
// 功能描述：数据转换
//     
// 输入参数：const char* pSrc
//         ：unsigned char* pDest
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CAesECB::StringToHex( const char* pSrc, unsigned char* pDest )
{
	int nSrcLen = 0;
	if( pSrc != 0 )
	{
		nSrcLen = strlen(pSrc);
		if(nSrcLen>16)
		{
			nSrcLen = 16;
		}
		memcpy(pDest, pSrc, nSrcLen);
	}
	Padding( pDest, nSrcLen );
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CAesECB::Padding
//     
// 功能描述：数据转换辅助
//     
// 输入参数：unsigned char* pSrc
//         ：int nSrcLen
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CAesECB::Padding( unsigned char* pSrc, int nSrcLen )
{
	if( nSrcLen < KEYCODELENGTH )
	{
		unsigned char ucPad = KEYCODELENGTH - nSrcLen;
		for( int nID = KEYCODELENGTH; nID > nSrcLen; --nID )
		{
			pSrc[nID - 1] = ucPad;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:fnHexToBin
////函数功能:将16进制的字符串转换成2进制的数字 
////输入参数:IN PBYTE	pHex		-- 需要转换的16进制字符串
////		 OUT PBYTE	pbin		-- 输出的2进制数字数组
////		 IN DWORD	dwHex_len	-- 需要转换的16进制数据长度,必须为2的倍数
////输出参数:无
// 编写人员：
// 编写时间：2013年1月5日
// 修改人员：
// 修改时间：
////函数版本:1.0.0.1
////备注说明:所输入的16进制字符串长度必须是2的倍数
//////////////////////////////////////////////////////////////////////////////////////////
void CAesECB::Hex2Bin(IN LPTSTR lpHex,OUT PBYTE pBin,IN DWORD dwHex_len)
{
	if(dwHex_len%2)
		return;

	CString strTemp(lpHex);	
	strTemp.MakeUpper();

	USES_CONVERSION;
	for(int i=0;i<strTemp.GetLength()/2;i++)
	{
		BYTE a = (BYTE)(strTemp.GetAt(i*2)>='A')?(BYTE)(strTemp.GetAt(i*2)-'A'+10):(BYTE)(strTemp.GetAt(i*2)-'0');
		BYTE b = (BYTE)(strTemp.GetAt(i*2+1)>='A')?(BYTE)(strTemp.GetAt(i*2+1)-'A'+10):(BYTE)(strTemp.GetAt(i*2+1)-'0');
		pBin[i] = (BYTE)(a * 0x10 + b);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:BinToHex
////函数功能:将2进制的数字转换成16进制的字符串
////输入参数:IN PBYTE	pBin		-- 需要转换的2进制数字数组
////		 IN DWORD	dwHex_len	-- 需要转换的2进制数字数组长度
////输出参数:CString	非空 -- 转换成功后输出的16进制字符串
////		 CString	NULL -- 转换失败
// 编写人员：
// 编写时间：2013年1月5日
// 修改人员：
// 修改时间：
////函数版本:1.0.0.1
////备注说明:如果调用成功后,不需要调用者释放内存
//////////////////////////////////////////////////////////////////////////////////////////
CString CAesECB::Bin2Hex(IN PBYTE pBin,IN DWORD dwBin_Len)
{
	CString strHex;
	CString strTemp;
	for ( DWORD i = 0; i < dwBin_Len; ++i )
	{
		strTemp.Format(_T("%02X"), pBin[i] );
		strHex += strTemp;
	}
	return strHex;
}