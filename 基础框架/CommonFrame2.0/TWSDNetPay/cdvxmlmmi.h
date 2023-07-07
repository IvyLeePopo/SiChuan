#ifndef CDVXMLMMI_H
#define CDVXMLMMI_H

#include <QObject>
#include "cdvxmlbase.h"
#include "basedefine.h"
#include <QVector>



class CDVXMLMMI:public CDVXMLBase
{
    Q_OBJECT
public:
    CDVXMLMMI(void);

//Interface
public:
        //解析XML字符串
        bool  Init( const QString& strXMLString);


//Attributes
public:
        //交互界面特殊处理标记
        int ClearMMI;  //0：非清屏，～0：清屏
        //文字显示数据
        QVector<tagMMIItemText> Text;
        //声音数据
        QVector<tagMMIItemVoice> Voice;
        //图形数据
        QVector<tagMMIItemGraph> Graph;
};

#endif // CDVXMLMMI_H
