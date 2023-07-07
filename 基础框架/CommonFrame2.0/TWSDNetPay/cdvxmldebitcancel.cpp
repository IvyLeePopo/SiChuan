#include "cdvxmldebitcancel.h"
#include <QDebug>
#include"twsdnetpay.h"

CDVXMLDebitCancel::CDVXMLDebitCancel(void)
    :PayIdentifier("")
{
    XMLFunctionDesc = "DebitCancelParamContext";
}


CDVXMLDebitCancel::~CDVXMLDebitCancel(void)
{

}


bool CDVXMLDebitCancel::Init( const QString& strXMLString)
{
    bool bReturn = false;
    try
    {
        m_domDoc.setContent(strXMLString, false,&m_error,&m_ReRow,&m_ReColumn);
        if(m_domDoc.isNull())
            throw -1;

        ResetChildPos();
        if(!FindChildElem("Version"))
            throw -2;
        XMLVersion = GetChildElem();

        if(!FindChildElem("Data"))
            throw -3;

        if(!FindTwoChildElem("TradeKeyItem"))
            throw -4;

        if(!FindThreeChildElem("PayIdentifier"))
            throw -5;
        PayIdentifier = GetThreeChildElem();
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            m_strLastErrorDesc = "撤单传入参数解析失败,无法找到指定数据头 DebitCancelParamContext";
            break;
        case -2:
            m_strLastErrorDesc = "撤单传入参数解析失败,无法找到指定数据版本 Version";
            break;
        case -3:
            m_strLastErrorDesc = "撤单传入参数解析失败,无法找到指定数据区 Data";
            break;
        case -4:
            m_strLastErrorDesc = "撤单传入参数解析失败,无法找到指定数据项(超时时间) TradeKeyItem";
            break;
        case -5:
            m_strLastErrorDesc = "撤单传入参数解析失败,无法找到指定数据项(扣款模式) PayIdentifier";
            break;
        }
        GetGlobalApp()->WriteLogFile(m_strLastErrorDesc);
    }

    return bReturn ;

}











