#include "StdAfx.h"
#include "DVXMLUnipay.h"
#include "TWSDNetPay_Dll.h"

CDVXMLUnipay::CDVXMLUnipay(void)
	:Module(0)
	,Type(0)
	,Context(_T(""))
{
	XMLFunctionDesc.Format(_T("IssueData"));
}

CDVXMLUnipay::~CDVXMLUnipay(void)
{
}


const BOOL CDVXMLUnipay::Init(IN const CString& strXMLString)
{
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVXMLUnipay::ConstructNFCFrameData
//     
// ��������������NFC��صĸ�ʽ�����ݣ��·���POS�ǽ�ģ�飩
//     
// ���������IN const CString& strData
//         ��IN const int& iType
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CDVXMLUnipay::ConstructNFCFrameData(IN const CString& strData,IN const int& iType)
{
	CString strXMLString;
	CMarkup xml;
	xml.SetDoc(XMLHead);
	xml.AddElem(XMLFunctionDesc);
	xml.IntoElem(); 
	xml.AddElem(_T("Version"),XMLVersion);
	xml.AddElem(_T("VerifyCode"),_T("0"));
	xml.AddElem(_T("Data"));

	xml.IntoElem(); 
	xml.AddElem(_T("Module"),0x01);
	xml.AddElem(_T("Type"),iType);
	xml.AddElem(_T("Context"),strData);
	xml.OutOfElem();

	xml.OutOfElem();

	strXMLString = xml.GetDoc();

	return strXMLString;
}