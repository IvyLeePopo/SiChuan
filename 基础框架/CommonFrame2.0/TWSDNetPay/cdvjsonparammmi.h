#ifndef CDVJSONPARAMMMI_H
#define CDVJSONPARAMMMI_H

#include <QObject>
#include "cdvjsonbase.h"
#include "basedefine.h"

class CDVJsonParamMMI :  public CDVJsonBase
{
    Q_OBJECT
public:
    explicit CDVJsonParamMMI(void);

    //运算符重载
    CDVJsonParamMMI& operator=(const CDVJsonParamMMI& cSrc);

    //重置数据
    void  Reset( const int& iType=0);
    //生成数据校验码
    quint32 CreateInnerVerifyCode(void){return 0;}
    //内部数据码校验
    bool  InnerVerify(void){return true;}
signals:

public slots:

public:
    //交互界面特殊处理标记
    int ClearMMI;
    //文字显示数据
    QVector<tagMMIItemText>		Text;
    //声音数据
  //  QVector<tagMMIItemVoice,tagMMIItemVoice&>	Voice;
    QVector<tagMMIItemVoice>	Voice;
    //图形数据
    QVector<tagMMIItemGraph>	Graph;
};

#endif // CDVJSONPARAMMMI_H
