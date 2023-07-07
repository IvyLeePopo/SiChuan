#include "cdvxmlbase.h"

CDVXMLBase::CDVXMLBase(QObject *parent) : QObject(parent)
{

    m_error = "";
    m_ReRow = 0;
    m_ReColumn = 0;

}



void CDVXMLBase::ResetChildPos(void)
{
    m_ReRoot = m_ReDoc.documentElement();
}



bool CDVXMLBase::FindChildElem(QString t_data)
{
    bool t_result = false;

   m_ReChildElement = m_ReRoot.firstChildElement(t_data);

   if(!m_ReChildElement.isNull())
       t_result = true;

     return t_result;
}


bool CDVXMLBase::FindTwoChildElem(QString t_data)
{
    bool t_result = false;

   m_ReTwoChildElement = m_ReChildElement.firstChildElement(t_data);

   if(!m_ReTwoChildElement.isNull())
       t_result = true;

     return t_result;

}
bool CDVXMLBase::FindThreeChildElem(QString t_data)
{
    bool t_result = false;

   m_ReThreeChildElement = m_ReTwoChildElement.firstChildElement(t_data);

   if(! m_ReThreeChildElement.isNull())
       t_result = true;

     return t_result;

}


bool CDVXMLBase::FindChildElemTemp(QDomElement t_Element,QString t_data)
{

    bool t_result = false;

    m_ReElementTemp = t_Element.firstChildElement(t_data);

   if(! m_ReElementTemp.isNull())
       t_result = true;

     return t_result;

}




QString CDVXMLBase::GetChildElem()
{
    return m_ReChildElement.text();

}
QString CDVXMLBase::GetTwoChildElem()
{
    return m_ReTwoChildElement.text();

}
QString CDVXMLBase::GetThreeChildElem()
{
    return m_ReThreeChildElement.text();

}

QString CDVXMLBase::GetChildElemTemp()
{

 return m_ReElementTemp.text();
}


