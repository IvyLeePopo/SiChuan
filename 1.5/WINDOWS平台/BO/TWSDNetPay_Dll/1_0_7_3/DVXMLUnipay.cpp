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
// 函数名称：CDVXMLUnipay::ConstructNFCFrameData
//     
// 功能描述：构造NFC相关的格式化数据（下发到POS非接模块）
//     
// 输入参数：IN const CString& strData
//         ：IN const int& iType
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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