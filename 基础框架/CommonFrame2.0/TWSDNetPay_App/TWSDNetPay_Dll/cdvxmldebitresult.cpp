#include "cdvxmldebitresult.h"

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
    QDomText text;
    QDomText childtext;
    QDomText Towchildtext;
    QDomElement element;
    QDomElement childelement;
    QDomElement Towchildelement;
    QDomProcessingInstruction instruction;

    instruction = doc.createProcessingInstruction( "xml","version=\'1.0\' encoding=\'GB2312\'" );
    doc.appendChild( instruction );

    QDomElement root = doc.createElement( XMLFunctionDesc );
    doc.appendChild(root);

    element = doc.createElement( "Version" );
    text = doc.createTextNode( XMLVersion );
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement( "Data" );
    root.appendChild(element);


    childelement = doc.createElement( "ResultType" );
    childtext = doc.createTextNode( QString::number(ResultType) );
    childelement.appendChild(childtext);
    element.appendChild(childelement);

    childelement = doc.createElement( "ExecuteCode" );
    childtext = doc.createTextNode( QString::number(ExecuteCode) );
    childelement.appendChild(childtext);
    element.appendChild(childelement);

    childelement = doc.createElement( "TradeKeyItem" );
    element.appendChild(childelement);

    Towchildelement = doc.createElement( "PayIdentifier" );
    Towchildtext = doc.createTextNode( PayIdentifier);
    Towchildelement.appendChild(Towchildtext);
    childelement.appendChild(Towchildelement);

    strXMLString = doc.toString();

    strXMLString.replace(QString("\n"), QString(""));
    return strXMLString;
}
