#include "cdvxmlmmi.h"
#include <QDebug>

CDVXMLMMI::CDVXMLMMI(void)
{

}



bool CDVXMLMMI::Init(const QString& strXMLString)
{
    bool bReturn = false;
    QString strTemp;

    try
    {
        Text.clear();
        Voice.clear();
        Graph.clear();

        m_domDoc.setContent(strXMLString, false,&m_error,&m_ReRow,&m_ReColumn);

        if(m_domDoc.isNull())
            throw -1;

        ResetChildPos();
        if(!FindChildElem("Version"))
            throw -2;
        XMLVersion = GetChildElem();

        if(!FindChildElem("Data"))
            throw -3;

        if(!FindTwoChildElem("ClearMMI"))
            throw -4;
        ClearMMI = GetTwoChildElem().toInt();

        /*****************Item详细项************************/
        if(ClearMMI == 0)
        {
            if(!FindTwoChildElem("ItemData"))
                throw -5;

            QDomNodeList t_Ttemlist = m_ReTwoChildElement.childNodes();
            int t_count = t_Ttemlist.count();
            QDomElement t_element;

            for(int i=0;i<t_count;i++)
            {
                t_element = t_Ttemlist.item(i).toElement();

                if(!FindChildElemTemp(t_element,"Type"))
                    throw -6;
                int iType = GetChildElemTemp().toInt();
                switch(iType)
                {
                case 1:
                {
                    tagMMIItemText sItem = {0};
                    sItem.Type = iType;

                    if(!FindChildElemTemp(t_element,("Sub")))
                        throw -7;
                    sItem.Sub = GetChildElemTemp().toInt();

                    if(!FindChildElemTemp(t_element,("RowNo")))
                        throw -8;
                    sItem.Row = GetChildElemTemp().toInt();

                    if(!FindChildElemTemp(t_element,("Context")))
                        throw -9;
                    strTemp = GetChildElemTemp();
                    memcpy(sItem.Context,strTemp.toStdString().c_str(),sizeof(sItem.Context));
                    strTemp.clear();

                    if(!FindChildElemTemp(t_element,("IsScroll")))
                        throw -10;
                    sItem.IsScroll = (bool)GetChildElemTemp().toInt();

                    Text.append(sItem);

                }
                    break;
                case 2:
                {
                    tagMMIItemVoice sItem = {0};
                    sItem.Type = iType;


                    if(!FindChildElemTemp(t_element,("VoiceNo")))
                        throw -11;
                    sItem.No = GetChildElemTemp().toInt();

                    Voice.append(sItem);

                }
                    break;
                case 3:
                {
                    tagMMIItemGraph sItem = {0};
                    sItem.Type = iType;


                    if(!FindChildElemTemp(t_element,("GraphNo")))
                        throw -12;
                    sItem.No = GetChildElemTemp().toInt();

                    Graph.append(sItem);
                }
                    break;
                }
            }
        }
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据头 MMIParamContext";
            break;
        case -2:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据版本 Version";
            break;
        case -3:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据区 Data";
            break;
        case -4:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据项(清屏标记) ClearMMI";
            break;
        case -5:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据区 Item";
            break;
        case -6:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据项(设置交互类型) Type";
            break;
        case -7:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据项(设置文字数据项分类) Sub";
            break;
        case -8:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据项(设置文字行号) RowNo";
            break;
        case -9:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据项(设置文字内容) Context";
            break;
        case -10:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据项(设置文字播放方式) IsScroll";
            break;
        case -11:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据项(设置声音编码) VoiceNo";
            break;
        case -12:
            m_strLastErrorDesc = "交互界面设置传入参数解析失败,无法找到指定数据项(设置图形编码) GraphNo";
            break;
        }
    }

    return bReturn;
}
