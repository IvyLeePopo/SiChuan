#include "cdvjsoncommanddebitresult.h"
#include "paydbinf.h"

CDVJsonCommandDebitResult::CDVJsonCommandDebitResult()
{

}



/****************************************
 *函数名称：fnAnalysisStringForData
 *函数描述：分析数据
 *函数参数：QString
 *函数返回：bool
*****************************************/
bool CDVJsonCommandDebitResult::fnAnalysisStringForData(const QString &strResponseResult)
{
    try
    {
    //进行数据格式转换
    Json::Reader reader;
    Json::Value  root;
    Json::Value  DebitResultContext;
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

    //获取用户信息区
    if(Json::nullValue == root["DebitResultContext"].type())
        throw -4;
    DebitResultContext = root["DebitResultContext"];

    //获取版本
    if(Json::stringValue != DebitResultContext["Version"].type())
        throw -5;
    m_Version = DebitResultContext["Version"].asCString();

    //获取数据信息区
    if(Json::nullValue == DebitResultContext["Data"].type())
        throw -6;
    Data = DebitResultContext["Data"];

    //获取结果类型
    if(Json::intValue != Data["ResultType"].type())
        throw -7;
    m_ResultType = Data["ResultType"].asInt();

    //获取执行状态
    if(Json::intValue != Data["ExecuteCode"].type())
        throw -8;
    m_ExecuteCode = Data["ExecuteCode"].asInt();

    //获取执行结果描述
    if(Json::nullValue == Data["ExecuteDesc"].type())
        throw -9;
    m_ExecuteDesc = Data["ExecuteDesc"].asCString();

    //获取数据信息区
    if(Json::nullValue == Data["TradeKeyItem"].type())
        throw -10;
    TradeKeyItem = Data["TradeKeyItem"];

    //获取交易识别码
    if(Json::stringValue != TradeKeyItem["PayIdentifier"].type())
        throw -11;
    m_PayIdentifier = TradeKeyItem["PayIdentifier"].asCString();

    //获取第三方支付平台类型
    if(Json::intValue != TradeKeyItem["PayPlatformType"].type())
        throw -12;
    m_PayPlatformType = TradeKeyItem["PayPlatformType"].asInt();

    //獲取交易時間
    if(Json::stringValue != TradeKeyItem["DebitTime"].type())
        throw -13;
    m_DebitTime = TradeKeyItem["DebitTime"].asCString();

    //獲取交易訂單號
    if(Json::stringValue != TradeKeyItem["DebitOrder"].type())
        throw -14;
    m_DebitOrder = TradeKeyItem["DebitOrder"].asCString();

    //獲取交易安全碼
    if(Json::stringValue != TradeKeyItem["TradeSecurityCode"].type())
        throw -15;
    m_TradeSecurityCode = TradeKeyItem["TradeSecurityCode"].asCString();

    // get consume time
    if(Json::intValue != TradeKeyItem["ConsumeTime"].type())
        throw -16;
    m_ConsumeTime = TradeKeyItem["ConsumeTime"].asInt();

    qDebug()<<"交易识别码:"<<m_PayIdentifier;

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            break;
        }

        qDebug()<<"CDVJsonCommandDebitResult::fnAnalysisStringForData:"<<iErrorCode;
    }
}

/****************************************
 *函数名称：FormatParamData2Json
 *函数描述：构造数据
 *函数参数：void
 *函数返回：QString
*****************************************/
QString	CDVJsonCommandDebitResult::FormatParamData2Json()
{
    QString strReturn;
    try
    {
        // 表示整个 json 对象
        Json::Value root;
        // 标识用户参数
        Json::Value DebitResultContext;
        // 标识详细数据项
        Json::Value Data;

        Json::FastWriter t_RootDocument;

        //标识版本
        DebitResultContext["Version"]=std::string(m_Version.toLocal8Bit());

        Data["ExecuteCode"]=m_ExecuteCode;
        Data["ExecuteDesc"]=std::string(m_ExecuteDesc.toLocal8Bit());
        Data["ResultType"]=m_ResultType;
        Data["ConsumeTime"]=m_ConsumeTime;
        Data["DebitOrder"]=std::string(m_DebitOrder.toLocal8Bit());
        Data["DebitTime"]=std::string(m_DebitTime.toLocal8Bit());
        Data["PayIdentifier"]=std::string(m_PayIdentifier.toLocal8Bit());
        Data["PayPlatformType"]=m_PayPlatformType;
        Data["TradeSecurityCode"]=std::string(m_TradeSecurityCode.toLocal8Bit());

        DebitResultContext["Data"]=Data;
        root["DebitResultContext"]=DebitResultContext;



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

    qDebug() << "CDVJsonCommandDebitResult::FormatParamData2Json";

    return strReturn;
}


/****************************************
 *函数名称：DebitResultSuccess()
 *函数描述：构造支付成功数据
 *函数参数：void
 *函数返回：QString
*****************************************/
QString CDVJsonCommandDebitResult::DebitResultSuccess()
{

    QString strReturn;
    try
    {
        // 表示整个 json 对象
        Json::Value root;
        // 标识用户参数
        Json::Value DebitResultContext;
        // 标识详细数据项
        Json::Value Data;
        //详细信息
        Json::Value TradeKeyItem;

        Json::FastWriter t_RootDocument;

        //标识版本
        DebitResultContext["Version"]=std::string(m_Version.toLocal8Bit());

        Data["ExecuteCode"]=m_ExecuteCode;
        Data["ExecuteDesc"]=std::string(m_ExecuteDesc.toLocal8Bit());

        //20170331
        m_ResultType = 1;
        Data["ResultType"]=m_ResultType;


        TradeKeyItem["ConsumeTime"]=m_ConsumeTime;
        TradeKeyItem["DebitOrder"]=std::string(m_DebitOrder.toLocal8Bit());
        TradeKeyItem["DebitTime"]=std::string(m_DebitTime.toLocal8Bit());
        TradeKeyItem["PayIdentifier"]=std::string(m_PayIdentifier.toLocal8Bit());
        TradeKeyItem["PayPlatformType"]=m_PayPlatformType;
        TradeKeyItem["TradeSecurityCode"]=std::string(m_TradeSecurityCode.toLocal8Bit());

        Data["TradeKeyItem"]=TradeKeyItem;
        DebitResultContext["Data"]=Data;
        root["DebitResultContext"]=DebitResultContext;

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




/****************************************
 *函数名称：DebitResultfail
 *函数描述：构造支付失败数据
 *函数参数：void
 *函数返回：QString
*****************************************/
QString CDVJsonCommandDebitResult::DebitResultfail(int t_ResultType,int t_ExecuteCode)
{

    QString strReturn;
    try
    {
        // 表示整个 json 对象
        Json::Value root;
        // 标识用户参数
        Json::Value DebitResultContext;
        // 标识详细数据项
        Json::Value Data;
        //详细信息
        Json::Value TradeKeyItem;

        Json::FastWriter t_RootDocument;

        //标识版本
        DebitResultContext["Version"]=std::string(m_Version.toLocal8Bit());

        Data["ExecuteCode"]=t_ExecuteCode;
        Data["ExecuteDesc"]=std::string(m_ExecuteDesc.toLocal8Bit());
        Data["ResultType"]=t_ResultType;

        TradeKeyItem["ConsumeTime"]=m_ConsumeTime;
        TradeKeyItem["DebitOrder"]=std::string(m_DebitOrder.toLocal8Bit());
        TradeKeyItem["DebitTime"]=std::string(m_DebitTime.toLocal8Bit());
        TradeKeyItem["PayIdentifier"]=std::string(m_PayIdentifier.toLocal8Bit());
        TradeKeyItem["PayPlatformType"]=m_PayPlatformType;
        TradeKeyItem["TradeSecurityCode"]=std::string(m_TradeSecurityCode.toLocal8Bit());

        Data["TradeKeyItem"]=TradeKeyItem;
        DebitResultContext["Data"]=Data;
        root["DebitResultContext"]=DebitResultContext;

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


/****************************************
 *函数名称：DebitResultCommand
 *函数描述：构造支付数据命令
 *函数参数：void
 *函数返回：QByteArray
*****************************************/
QByteArray CDVJsonCommandDebitResult::DebitResultCommand(char t_NumericalOrderH, char t_NumericalOrderL, QString ResultJson)
{

    QByteArray  t_Data = ResultJson.toAscii();
    QByteArray  t_InitCommand;
    ushort t_VerifyCode;
    ushort t_DataLenth  = t_Data.length();


    t_InitCommand.append(COMMAND_START);
    t_InitCommand.append((uchar)(t_DataLenth>>8));
    t_InitCommand.append((uchar)t_DataLenth);


    t_InitCommand.append(t_NumericalOrderH);
    t_InitCommand.append(t_NumericalOrderL);

    t_InitCommand.append(0XB3);
    t_InitCommand.append(0X01);
    t_InitCommand.append(t_Data);

    t_VerifyCode = GetVerifyCode(t_InitCommand,t_InitCommand.length());

    t_InitCommand.append((uchar)(t_VerifyCode>>8));
    t_InitCommand.append((uchar)t_VerifyCode);


    t_InitCommand.append(COMMAND_END);

    return  t_InitCommand;
}





/****************************************
 *函数名称：CencelResultCommand
 *函数描述：支付撤单数据命令
 *函数参数：void
 *函数返回：QByteArray
*****************************************/
QString CDVJsonCommandDebitResult::CencelResultCommand(char t_result, QString payId)
{

    QString strReturn;
    try
    {
        // 表示整个 json 对象
        Json::Value root;
        // 标识用户参数
        Json::Value DebitCancelParamContext;
        // 标识详细数据项
        Json::Value Data;


        Json::FastWriter t_RootDocument;

        //标识版本
        DebitCancelParamContext["Version"]=std::string(m_Version.toLocal8Bit());

        Data["PayIdentifier"] = payId.toStdString();
        Data["ResultType"]=t_result;
        DebitCancelParamContext["Data"]=Data;
        root["DebitCancelParamContext"]=DebitCancelParamContext;

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
