#ifndef CDVJSONCOMMANDSETMMI_H
#define CDVJSONCOMMANDSETMMI_H

#include <QObject>
#include "cdvjsoncommanddebit.h"
#include "TWSDNetPay_Dll/basedefine.h"


class CDVJsonCommandSetMMI : public CDVJsonCommandDebit
{
    Q_OBJECT
public:
    explicit CDVJsonCommandSetMMI();
    bool fnAnalysisStringForData(const QString &strResponseResult);
    QString	FormatParamData2Json();


    QString    strSetClearScreen();
    QByteArray byteSetClearScreen();

signals:

public slots:

private:
    //交互界面特殊处理标记
    int m_ClearMMI;
    //文字显示数据
    QVector<tagMMIItemText>		m_Text;
    //声音数据
    QVector<tagMMIItemVoice>	m_Voice;
    //图形数据
    QVector<tagMMIItemGraph>	m_Graph;

};

#endif // CDVJSONCOMMANDSETMMI_H
