#include "cdvxmlaccountcontext.h"

CDVXMLAccountContext::CDVXMLAccountContext(void)
{

}


bool CDVXMLAccountContext::Init(const QString& strXMLString)
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

        if(!FindTwoChildElem("Type"))
            throw -4;

        if(!FindTwoChildElem("Source"))
            throw -5;
        Src = GetTwoChildElem().toInt();

        if(!FindTwoChildElem("Context"))
            throw -6;
        Context = GetTwoChildElem();

        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            m_strLastErrorDesc = "用户信息解析失败,无法找到指定数据头 DebitCancelParamContext";
            break;
        case -2:
            m_strLastErrorDesc = "用户信息解析失败,无法找到指定数据版本 Version";
            break;
        case -3:
            m_strLastErrorDesc = "用户信息解析失败,无法找到指定数据区 Data";
            break;
        case -4:
            m_strLastErrorDesc = "用户信息解析失败,无法找到指定数据项(数据类型) Type";
            break;
        case -5:
            m_strLastErrorDesc = "用户信息解析失败,无法找到指定数据项(数据来源) Source";
            break;
        case -6:
            m_strLastErrorDesc = "用户信息解析失败,无法找到指定数据项(数据内容) Context";
            break;
        }
    }
    return bReturn;
}


const QString CDVXMLAccountContext::GetXMLString(const QString& strData, const int& iDataType, const int& iSource)
{
    QString strXMLString;

    //重构XML数据
    QDomDocument doc;
    QDomProcessingInstruction instruction;

    instruction = doc.createProcessingInstruction( "xml","version=\'1.0\' encoding=\'GB2312\'" );
    doc.appendChild( instruction );

    QDomElement root = doc.createElement( XMLFunctionDesc );
    doc.appendChild(root);

    addTextNode(root, "Version", "1.0");
    addTextNode(root,"Data", "Type", QString::number(iDataType));
    addTextNode(root, "Data", "Source", QString::number(iSource));
    addTextNode(root, "Data", "Context", strData);
    strXMLString = doc.toString();

    return strXMLString;
}
