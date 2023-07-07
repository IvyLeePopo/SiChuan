/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonHeart.cpp
 * ժ    Ҫ�������ӿ����ݽ��������ṹ�壬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��12��14��
 */
#include "StdAfx.h"
#include "DVJsonHeart.h"

CDVJsonHeart::CDVJsonHeart(void)
	:StationID(_T(""))
	,StationName(_T(""))
	,LaneID(_T(""))
{
}

CDVJsonHeart::~CDVJsonHeart(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonHeart::Reset
//     
// ������������������
//     
// ���������IN const int& iType -- ��������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CDVJsonHeart::Reset(IN const int& iType)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonHeart::operator=
//     
// �������������������
//     
// ���������IN const CDVJsonHeart& cSrc
// ���������CDVJsonHeart&
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��14��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonHeart& CDVJsonHeart::operator=(IN const CDVJsonHeart& cSrc)
{
	if(&cSrc != this)
	{
		//�������ݸ���
		Version					=	cSrc.Version;
		PackageType				=	cSrc.PackageType;
		ProvinceID				=	cSrc.ProvinceID;
		Token					=	cSrc.Token;
		SubmitCount				=	cSrc.SubmitCount;
		SubmitTime				=	cSrc.SubmitTime;
		QRCodeData				=	cSrc.QRCodeData;
		QRCodeType				=	cSrc.QRCodeType;
		m_hCallerWnd			=	cSrc.m_hCallerWnd;
		m_dwThreadID			=	cSrc.m_dwThreadID;
		ServerResponseCode		=	cSrc.ServerResponseCode;
		ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
		ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
		//���������ݸ���
		StationID	= cSrc.StationID;
		StationName	= cSrc.StationName;
		LaneID		= cSrc.LaneID;
		HeartTime	= cSrc.HeartTime;
	}
	return *this;
}