#include "cdvxmldebitresult.h"
#include<QDateTime>
CDVXMLDebitResult::CDVXMLDebitResult(void)
    :ResultType(0)
    ,ExecuteCode(-1)
    ,ExecuteDesc("")

    ,PayIdentifier("")
    ,PayPlatformType(0)
    ,TradeSecurityCode("")
    ,DebitOrder("")
{
    XMLFunctionDesc = "DebitResultContext";

}




CDVXMLDebitResult::~CDVXMLDebitResult(void)
{
}



QString CDVXMLDebitResult::GetXMLString(void)
{
    QString strXMLString;

    QDomDocument doc;
    QDomProcessingInstruction instruction;

    instruction = doc.createProcessingInstruction( "xml","version=\'1.0\' encoding=\'GB2312\'" );
    doc.appendChild( instruction );

    QDomElement root = doc.createElement( XMLFunctionDesc );
    doc.appendChild(root);

    addTextNode(root, "Version", XMLVersion);
    QString strData = "Data";
    addTextNode(root , strData, "ResultType", QString::number(ResultType));
    addTextNode(root, strData, "ExecuteCode", QString::number(ExecuteCode));

    QString strItem = "TradeKeyItem";
    addTextNode(root, strItem, "PayIdentifier", PayIdentifier);
    addTextNode(root, strItem, "TradeSecurityCode", TradeSecurityCode);
    addTextNode(root , strItem, "PayPlatformType", QString::number(PayPlatformType));
    addTextNode(root, strItem, "DebitOrder", DebitOrder);
    addTextNode(root, strItem, "DebitTime", DebitTime);

    strXMLString = doc.toString();

    strXMLString.replace(QString("\n"), QString(""));
    return strXMLString;
}
