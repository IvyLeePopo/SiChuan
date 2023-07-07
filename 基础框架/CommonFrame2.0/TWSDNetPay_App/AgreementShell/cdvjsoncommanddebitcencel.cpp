#include "cdvjsoncommanddebitcencel.h"

CDVJsonCommandDebitCencel::CDVJsonCommandDebitCencel()
{
    m_type = 1;
}




/****************************************
 *函数名称：FormatParamData2Json()
 *函数描述：撤单数据解析
 *函数参数：void
 *函数返回：QString
*****************************************/
bool  CDVJsonCommandDebitCencel::fnAnalysisStringForData(const QString& strResponseResult)
{

   bool breturn = false;

   QString strlog;

       try
       {

           //进行数据格式转换
           Json::Reader reader;
           Json::Value  root;
           Json::Value  DebitCancelParamContext;
           Json::Value  Data;
           Json::Value  TradeKeyItem;

           QString strResult(strResponseResult);
           if(!reader.parse(strResult.toLocal8Bit().data(),root))
           {
               strResult.clear();
               throw -2;
           }

           if(Json::nullValue == root.type())
               throw -3;

           //获取交易数据区
           if(Json::nullValue == root["DebitCancelParamContext"].type())
               throw -4;
           DebitCancelParamContext = root["DebitCancelParamContext"];

           //获取数据明细区
           if(Json::nullValue == DebitCancelParamContext["Data"].type())
               throw -5;
           Data = DebitCancelParamContext["Data"];

           // Version
           if (Json::nullValue != DebitCancelParamContext["Version"].type())
                m_Version = DebitCancelParamContext["Version"].asCString();
           else
                m_Version = "1.0";

           //get TradeKeyItem
           if(Json::nullValue == DebitCancelParamContext["Data"].type())
               throw -6;
           TradeKeyItem = Data["TradeKeyItem"];

           //PayIdentifier
           if(Json::stringValue != TradeKeyItem["PayIdentifier"].type())
               throw -7;
           m_PayIdentifier = TradeKeyItem["PayIdentifier"].asCString();

           //Type
           if(Json::intValue == TradeKeyItem["Type"].type())
               m_type = TradeKeyItem["Type"].asInt();
           else
               m_type = 1;

           //数据正常，撤单退款
           qDebug()<<"Start_Cancel";
       }
       catch(int& iErrorCode)
       {
           switch(iErrorCode)
           {
           case -1:
               strlog = "[业务线程],JSON_DT_PARAM_Cencel_MONEY数据解析,异常找不到：";
               break;
           case -2:
               strlog = "[业务线程],JSON_DT_PARAM_Cencel_MONEY数据解析,异常找不到：";
               break;
           case -3:
               strlog = "[业务线程],JSON_DT_PARAM_Cencel_MONEY数据解析,异常找不到： root";
               break;
           case -4:
               strlog = "[业务线程],JSON_DT_PARAM_Cencel_MONEY数据解析,异常找不到： DebitParamContext";
               break;
           case -5:
               strlog = "[业务线程],JSON_DT_PARAM_Cencel_MONEY数据解析,异常找不到： Data";
               break;
           case -6:
               strlog = "[业务线程],JSON_DT_PARAM_Cencel_MONEY数据解析,异常找不到： TradeKeyItem";
               break;

           case -7:
               strlog = "[业务线程],JSON_DT_PARAM_Cencel_MONEY数据解析,异常找不到： PayIdentifier";
               break;
           }

           qDebug()<<strlog;
           qDebug()<<"JSON_DT_PARAM_Cencel_MONEY异常1: "<<iErrorCode;
       }


   return breturn;

}




/****************************************
 *函数名称：FormatParamData2Json()
 *函数描述：撤单数据构造
 *函数参数：void
 *函数返回：QString
*****************************************/
QString CDVJsonCommandDebitCencel::FormatParamData2Json()
{
    QString strReturn;
    try
    {

        Json::FastWriter t_RootDocument;
        // 表示整个 json 对象
        Json::Value root;
        // 标识用户参数
        Json::Value DebitCancel;
        // 标识详细数据项
        Json::Value Data;
        // 标识关键数据项
        Json::Value KeyItem;

        //标识版本
        DebitCancel["Version"]=std::string(m_Version.toLocal8Bit());

        KeyItem["Type"]= m_type;
        KeyItem["PayIdentifier"]=std::string(m_PayIdentifier.toLocal8Bit());

        Data["TradeKeyItem"]=KeyItem;
        DebitCancel["Data"]= Data;
        root["DebitCancelParamContext"]=DebitCancel;

        //转化为字符串
        strReturn = QString::fromStdString(t_RootDocument.write(root));



    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            break;
        }
    }
    return strReturn;
}
