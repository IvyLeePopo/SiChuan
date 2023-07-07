#include "cdvxmlaccountcontext.h"

CDVXMLAccountContext::CDVXMLAccountContext(void)
{

}


const bool CDVXMLAccountContext::Init(const QString& strXMLString)
{
    bool bReturn = false;
    try
    {

        m_ReDoc.setContent(strXMLString, false,&m_error,&m_ReRow,&m_ReColumn);

        if(m_ReDoc.isNull())
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
    QDomText text;
    QDomText childtext;
    QDomElement element;
    QDomElement childelement;
    QDomProcessingInstruction instruction;

    instruction = doc.createProcessingInstruction( "xml","version=\'1.0\' encoding=\'GB2312\'" );
    doc.appendChild( instruction );

    QDomElement root = doc.createElement( XMLFunctionDesc );
    doc.appendChild(root);

    element = doc.createElement( "Version" );
    text = doc.createTextNode( "1.0" );
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement( "Data" );
    root.appendChild(element);

    childelement = doc.createElement( "Type" );
    childtext = doc.createTextNode( QString::number(iDataType) );
    childelement.appendChild(childtext);
    element.appendChild(childelement);

    childelement = doc.createElement( "Source" );
    childtext = doc.createTextNode( QString::number(iSource) );
    childelement.appendChild(childtext);
    element.appendChild(childelement);

    childelement = doc.createElement( "Context" );
    childtext = doc.createTextNode( strData );
    childelement.appendChild(childtext);
    element.appendChild(childelement);

    strXMLString = doc.toString();

    return strXMLString;
}
