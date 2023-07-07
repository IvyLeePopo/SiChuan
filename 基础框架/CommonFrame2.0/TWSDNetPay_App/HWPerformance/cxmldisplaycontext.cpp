#include "cxmldisplaycontext.h"

CXmlDisplayContext::CXmlDisplayContext(void)
{
}


/*
 <?xml version="1.0" encoding="UTF-8"?>
 <DisplayContext>
 <Version>1.0</Version>
 <ProvinceID>63</ProvinceID>
 <VerifyCode>1</VerifyCode>
 <Data>
 <Type>1</Type>
 <Money>122</Money>
 <Weight>122</Weight>
 <VehicleType>1</VehicleType>
 <VehicleClass>1</VehicleClass>
 <Balance>12</Balance>
 <CurrentTime>2016-07-22 12:23:11</CurrentTime>
 <EntrySTName>xx</EntrySTName>
 </Data>
 </DisplayContext>
*/

const bool CXmlDisplayContext::Init( const QString& strXMLString)
{
    bool bReturn = false;

    try
    {
        m_ReDoc.setContent(strXMLString, false,&m_error,&m_ReRow,&m_ReColumn);

        if(m_ReDoc.isNull())
            throw -1;

        /****************************************************************数据格式判断****************************************************************/
        ResetChildPos();
        if(!FindChildElem("Version"))
            throw -2;
        XMLVersion = GetChildElem();


        /**************************************************************************/
        if(!FindChildElem("Data"))
            throw -3;

        if(!FindTwoChildElem("Type"))
            throw -4;
        m_sScanTemp.Type = GetTwoChildElem().toInt();

        if(!FindTwoChildElem("Money"))
            throw -5;
        m_sScanTemp.Money = GetTwoChildElem().toInt();

        if(!FindTwoChildElem("Weight"))
            throw -6;
        m_sScanTemp.Weight = GetTwoChildElem().toInt();

        if(!FindTwoChildElem("VehicleType"))
            throw -7;
        m_sScanTemp.VehicleType = GetTwoChildElem().toInt();

        if(!FindTwoChildElem("VehicleClass"))
            throw -8;
        m_sScanTemp.VehicleClass = GetTwoChildElem().toInt();

        if(!FindTwoChildElem("Balance"))
            throw -9;
        m_sScanTemp.Balance = GetTwoChildElem().toInt();

        if(!FindTwoChildElem("CurrentTime"))
            throw -10;
        // m_sScanTemp. = GetTwoChildElem();
        if(!FindTwoChildElem("EntrySTName"))
            throw -11;
        memcpy(m_sScanTemp.EntrySTName, GetTwoChildElem().toLocal8Bit().data(),sizeof(m_sScanTemp.EntrySTName));
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据头 DebitParamContext";
            break;
        case -2:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 Version";
            break;
        case -3:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 Data";
            break;
        case -4:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 Type";
            break;
        case -5:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 Money";
            break;
        case -6:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 Weight";
            break;
        case -7:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 VehicleType";
            break;
        case -8:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 VehicleClass";
            break;
        case -9:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 Balance";
            break;
        case -10:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 CurrentTime";
            break;
        case -11:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 EntrySTName";
            break;
        }
        qDebug()<<m_strLastErrorDesc;
    }

    return bReturn;
}





const void  CXmlDisplayContext::GetTagScanDisplay( tagScanDisplay&	t_sScanTemp)
{
    t_sScanTemp = m_sScanTemp;
}
