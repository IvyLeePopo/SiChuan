/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�AesECB.cpp
 * ժ    Ҫ��ʵ��AES����/���ܹ����ࣨ����֧��128λ��ECBģʽ����ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��18��
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
// �������ƣ�CAesECB::EncryptData
//     
// ������������������
//     
// ���������IN const CString& strSrc
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿں���
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
// �������ƣ�CAesECB::DecryptData
//     
// ������������������
//     
// ���������IN const CString& strSrc
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿں���
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
// �������ƣ�CAesECB::Encrypt
//     
// �����������ڲ�ʵ�ּ��ܵĺ���
//     
// ���������std::string strSrc
//         ��std::string strKey
// ���������std::string
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CAesECB::Decrypt
//     
// �����������ڲ�ʵ�ֽ��ܵĺ���
//     
// ���������std::string strSrc
//         ��std::string strKey
// ���������std::string
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CAesECB::StringToHex
//     
// ��������������ת��
//     
// ���������const char* pSrc
//         ��unsigned char* pDest
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CAesECB::Padding
//     
// ��������������ת������
//     
// ���������unsigned char* pSrc
//         ��int nSrcLen
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
////��������:fnHexToBin
////��������:��16���Ƶ��ַ���ת����2���Ƶ����� 
////�������:IN PBYTE	pHex		-- ��Ҫת����16�����ַ���
////		 OUT PBYTE	pbin		-- �����2������������
////		 IN DWORD	dwHex_len	-- ��Ҫת����16�������ݳ���,����Ϊ2�ı���
////�������:��
// ��д��Ա��
// ��дʱ�䣺2013��1��5��
// �޸���Ա��
// �޸�ʱ�䣺
////�����汾:1.0.0.1
////��ע˵��:�������16�����ַ������ȱ�����2�ı���
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
////��������:BinToHex
////��������:��2���Ƶ�����ת����16���Ƶ��ַ���
////�������:IN PBYTE	pBin		-- ��Ҫת����2������������
////		 IN DWORD	dwHex_len	-- ��Ҫת����2�����������鳤��
////�������:CString	�ǿ� -- ת���ɹ��������16�����ַ���
////		 CString	NULL -- ת��ʧ��
// ��д��Ա��
// ��дʱ�䣺2013��1��5��
// �޸���Ա��
// �޸�ʱ�䣺
////�����汾:1.0.0.1
////��ע˵��:������óɹ���,����Ҫ�������ͷ��ڴ�
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