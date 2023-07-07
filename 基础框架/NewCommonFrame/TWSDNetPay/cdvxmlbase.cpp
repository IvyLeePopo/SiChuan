#include "cdvxmlbase.h"

CDVXMLBase::CDVXMLBase(QObject *parent) : QObject(parent)
{
    m_error = "";
    m_ReRow = 0;
    m_ReColumn = 0;
}



void CDVXMLBase::ResetChildPos(void)
{
    m_ReRoot = m_domDoc.documentElement();
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

/*******************************************
* 函数名称： addTextNode
* 函数功能：添加xml节点值
* 参   数：  QDomElement& root  根节点
*         ：QString first
*         ：QString second
*         ：QString third
*         ：QString four
*         ：QString five
*         ：函数描述：root根节点非空，其他节点默认为空，通过判断传入的最后一个参数是否为空作为构造节点的依据。
*         ：                    参数从右到左，最右一个非空参数最为xml的节点内容，其他非空参数作为节点。
*         ： 特别说明：由于没有考虑兄弟节点，所以兄弟节点的添加回被覆盖。为了避免这种情况，可以在添加兄弟节点时
*         ：                    做折中处理。如需要兄弟节点 StuName，可以通过Stuame1,StuName2,StuName3,，依次添加，
*         ：                    最后通过字符串处理，得到需要的结果。
*         ：
* 返   回：bool
********************************************/
bool CDVXMLBase::addTextNode(QDomElement& root, QString first, QString second, QString third, QString four , QString five)
{
    bool bRet = false;
    QDomElement retElement;
    if(five.isEmpty() && four.isEmpty() && third.isEmpty() && second.isEmpty() && first.isEmpty())
    {
        return bRet;
    }
    else  if(five.isEmpty() && four.isEmpty() && third.isEmpty() && second.isEmpty() && (!first.isEmpty()))
    {
        QDomElement element = m_domDoc.createElement(first);
        if(!isElementExist(root,first, retElement))
        {
            root.appendChild(element);
        }
        bRet = true;
    }
    else  if(five.isEmpty() && four.isEmpty() && third.isEmpty() && (!second.isEmpty()))
    {
        QDomText text = m_domDoc.createTextNode(second);
        QDomElement element = m_domDoc.createElement(first);
        element.appendChild(text);
        root.appendChild(element);
        bRet = true;
    }
    else  if(five.isEmpty() && four.isEmpty() && (!third.isEmpty()))
    {
        QDomText text = m_domDoc.createTextNode(third);
        QDomElement element = m_domDoc.createElement(second);
        element.appendChild(text);
        QDomElement first_element = m_domDoc.createElement(first);//创建该节点为判断是否存在做参考
        if(!isElementExist(root, first, retElement) )
        {
            root.appendChild(first_element);
            first_element.appendChild(element);
        }
        else
        {
            retElement.appendChild(element);
        }
        bRet = true;
    }
    else  if(five.isEmpty() && !four.isEmpty())
    {
        QDomText text = m_domDoc.createTextNode(four);
        QDomElement element = m_domDoc.createElement(third);
        element.appendChild(text);
        QDomElement second_element = m_domDoc.createElement(second);
        QDomElement first_element = m_domDoc.createElement(first);
        if(!isElementExist(root, first, retElement))
        {
            root.appendChild(first_element);
            first_element.appendChild(second_element);
            second_element.appendChild(element);
        }
        else
        {
            QDomElement secRetElement;
            if(!isElementExist(retElement, second, secRetElement))
            {
                retElement.appendChild(second_element);
                second_element.appendChild(element);
            }
            else
            {
                secRetElement.appendChild(element);
            }
        }
        bRet = true;
    }
    else if(!five.isEmpty())
    {
        QDomText text = m_domDoc.createTextNode(five);
        QDomElement element = m_domDoc.createElement(four);
        element.appendChild(text);
        QDomElement third_element     = m_domDoc.createElement(third);
        QDomElement second_element = m_domDoc.createElement(second);
        QDomElement first_element = m_domDoc.createElement(first);
        if(!isElementExist(root, first, retElement))
        {
            root.appendChild(first_element);
            first_element.appendChild(second_element);
            second_element.appendChild(third_element);
            third_element.appendChild(element);
        }
        else
        {
            QDomElement secRetElement;
            if(!isElementExist(retElement, second, secRetElement))
            {
                retElement.appendChild(second_element);
                second_element.appendChild(third_element);
                third_element.appendChild(element);
            }
            else
            {
                QDomElement thirdRetElement;
                if(!isElementExist(secRetElement, third, thirdRetElement))
                {
                    secRetElement.appendChild(third_element);
                    third_element.appendChild(element);

                }
                else
                {
                    thirdRetElement.appendChild(element);
                }

            }
        }
        bRet = true;

    }


    return bRet;
}

//判断xml节点是否存在，如果存在则传出该节点
bool CDVXMLBase::isElementExist(const QDomElement& element, QString childTagetName, QDomElement& retElement)
{
    bool bReturn = false;
    QDomNode node =  element.firstChild();
    while(!node.isNull())
    {
        if(node.toElement().tagName() == childTagetName)
        {
            retElement = node.toElement();
            bReturn = true;
            break;
        }

        node = node.nextSibling();
    }
    return bReturn;
}



