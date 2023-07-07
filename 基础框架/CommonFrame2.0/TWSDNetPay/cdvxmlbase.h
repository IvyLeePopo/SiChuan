/********************************************
 *
 * 类   名：CDVXMLBase
 * 摘   要：所有XML数据格式的公用基类，声明文件
 * 作   者：CWX
 * 修   改：FFFF
 * 创建日期：2016-11-21
 * 修改日期：2017-09-03
 *
*********************************************/
#ifndef CDVXMLBASE_H
#define CDVXMLBASE_H

#include <QObject>
#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QDomAttr>


//完
class CDVXMLBase : public QObject
{
    Q_OBJECT
public:
    explicit CDVXMLBase(QObject *parent = 0);
    //获取错误信息
    const QString GetLastErrorDesc(){return m_strLastErrorDesc;}
    //解析XML字符串
    virtual  bool Init(const QString& strXMLString)=0;
    //重设内容
    void ResetChildPos(void);
    //寻找字元素
    bool FindChildElem(QString t_data);
    bool FindTwoChildElem(QString t_data);
    bool FindThreeChildElem(QString t_data);
    bool FindChildElemTemp(QDomElement t_Element,QString t_data);

    QString GetChildElem();
    QString GetTwoChildElem();
    QString GetThreeChildElem();
    QString GetChildElemTemp();

    //添加节点元素
    bool addTextNode(QDomElement& root, QString first = "", QString second = "", QString third = "", QString four = "",   QString five = "");
private:
    //if true ,you need use retElement append element node
    bool isElementExist(const QDomElement& element, QString childTagetName, QDomElement& retElement);


signals:

public slots:

public:
    //所有XML文件的文件头
    QString	  XMLHead;
    //继承类是XML文件结构描述体，所具现的内容分类描述
    QString     XMLFunctionDesc;
    //XMl文件协议版本
    QString     XMLVersion;
    //校验码
    QString     XMLVerifyCode;
    //错误描述
    QString     m_strLastErrorDesc;

protected:
    //解释器
    QDomDocument m_domDoc;
    QString m_error;
    int m_ReRow;
    int m_ReColumn;
    //主元素
    QDomElement m_ReRoot ;
    //字节点转内容
    QDomElement m_ReElement;
    QDomElement m_ReChildElement;
    QDomElement m_ReTwoChildElement;
    QDomElement m_ReThreeChildElement;
    QDomElement m_ReElementTemp;

};

#endif // CDVXMLBASE_H
