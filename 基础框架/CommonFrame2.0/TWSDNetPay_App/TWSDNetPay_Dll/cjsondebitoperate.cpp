#include "cjsondebitoperate.h"
#include <QDateTime>
#include <QDebug>
#include "upay/upayutils.h"

extern QString G_AppVersion;

CJsonDebitOperate::CJsonDebitOperate(void)
{
   // m_AppVersion = ReadAppVersion();
  //  m_AppVersion.replace(QString("\r"),QString(""));
  //  m_AppVersion.replace(QString("\n"),QString(""));
}


CJsonDebitOperate::~CJsonDebitOperate(void)
{
}


/**************************************************
 * 函数名称：FormatSendData2Json
 * 函数描述：将接收到的数据结构转化为标准的JSON格式
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
QString	CJsonDebitOperate::FormatSendData2Json( const CDVJsonDebit* pDVJsonDebit)
{
    QString strReturn;

    if(NULL != pDVJsonDebit)
    {
        // 表示整个 json 对象
        Json::Value root;
        // 临时JSON对象
        Json::Value DebitCondition;

        Json::Value DebitConditionBase;

        Json::Value DebitConditionAddtional;

        Json::Value AddtionalEntryStation;

        Json::Value AddtionalPayStation;

        Json::Value AddtionalVehicleInfo;


        //标识版本
        root["Version"]			= Json::Value(std::string(pDVJsonDebit->Version.toLocal8Bit()));
        //标识数据模式(正式数据或测试数据)
        if(pDVJsonDebit->TestingData)
            root["IsTestData"]		= Json::Value(1);
        else
            root["IsTestData"]		= Json::Value(0);
        //标识数据类型
        root["PackageType"]		= Json::Value(pDVJsonDebit->PackageType);
        //安全认证
        root["Token"]			= Json::Value(std::string(pDVJsonDebit->Token.toLocal8Bit()));
        //标识省份
        root["ProvinceID"]		= Json::Value(pDVJsonDebit->ProvinceID);
        //递交次数
        root["SubmitCount"]		= Json::Value(pDVJsonDebit->SubmitCount);
        //递交时间
        root["SubmitTime"]		= Json::Value(std::string(pDVJsonDebit->SubmitTime.toLocal8Bit()));
        //交易标记
        root["RecordMatchCode"]		= Json::Value(std::string(pDVJsonDebit->RecordMatchCode.toLocal8Bit()));
        //扣款凭证
        root["PayIdentifier"]		= Json::Value(std::string(pDVJsonDebit->PayIdentifier.toLocal8Bit()));
        //交易安全认证码
        root["TradeSecurityCode"]	= Json::Value(std::string(pDVJsonDebit->RecordMatchCode.toLocal8Bit()));
        //是否测试数据
        root["TestingData"]		= Json::Value((int)pDVJsonDebit->TestingData);
        //业务动态库版本号
        root["NetPayDllVersion"]        = Json::Value(std::string(pDVJsonDebit->NetPayDllVersion.toLocal8Bit()));
        //设备动态库版本号
        root["DeviceDllVersion"]        = Json::Value(std::string(pDVJsonDebit->DeviceDllVersion.toLocal8Bit()));
        //APP版本号
        root["AppVersion"] = Json::Value(G_AppVersion.toStdString());
        //用户数据数据来源
        root["AccountInfoSrcType"]	= Json::Value(pDVJsonDebit->AccountInfoSrcType);
        //用户数据具体设备号
        root["AccountInfoDeviceID"]	= Json::Value(pDVJsonDebit->AccountInfoDeviceID);
        //用户数据具体类型
        root["AccountInfoDataType"]	= Json::Value(pDVJsonDebit->AccountInfoDataType);

        //填充基本信息
        DebitConditionBase["ChannelUserId"]	= Json::Value(std::string(pDVJsonDebit->IDBindInPayPlatform.toLocal8Bit()));
        DebitConditionBase["ChannelType"]	= Json::Value(pDVJsonDebit->IDBindInPayPlatformType);
        DebitConditionBase["PayPlatformQRCode"]	= Json::Value(std::string(pDVJsonDebit->QRCodeData.toLocal8Bit()));
        DebitConditionBase["VirtualCardID"]	= Json::Value(std::string(pDVJsonDebit->SMVirtualCardID.toLocal8Bit()));
        DebitConditionBase["EntryInfoUniqueFlag"] = Json::Value(std::string(pDVJsonDebit->EntryInfoUniqueFlag.toLocal8Bit()));
        DebitConditionBase["DebitMoney"]	= Json::Value(pDVJsonDebit->DebitMoney);
        DebitConditionBase["DebitType"]		= Json::Value(pDVJsonDebit->DebitType);
        DebitConditionBase["DebitTime"]		= Json::Value(std::string(pDVJsonDebit->DebitTime.toLocal8Bit()));


        //统计日转换--20170103
        DebitConditionBase["TollDate"]		= Json::Value(pDVJsonDebit->DebitTollDate);
        DebitConditionBase["TollShift"]		= Json::Value(pDVJsonDebit->DebitTollShift);


        //填充附加信息 -- 入站信息
        AddtionalEntryStation["EntryProvinceId"]     = Json::Value(pDVJsonDebit->EntryProvinceID);
        AddtionalEntryStation["EntryStationId"]      = Json::Value(pDVJsonDebit->EntryStationID.toStdString());
        AddtionalEntryStation["EntryStationName"]    = Json::Value(std::string(pDVJsonDebit->EntryStationName.toLocal8Bit()));
        AddtionalEntryStation["EntryLane"]           = Json::Value(pDVJsonDebit->EntryLane);


        //AddtionalEntryStation["EntryTime"]           = Json::Value(std::string(pDVJsonDebit->EntryTime.toLocal8Bit().data()));
        //20170330
        AddtionalEntryStation["EntryTime"]           = Json::Value(std::string(pDVJsonDebit->EntryTime.toLocal8Bit()));


        AddtionalEntryStation["EntryRoadNo"]         = Json::Value(pDVJsonDebit->EntryRoadID);
        AddtionalEntryStation["EntryAreaNo"]         = Json::Value(pDVJsonDebit->EntryAreaID);

        //填充附加信息 -- 扣费站信息
        AddtionalPayStation["PayProvinceId"]         = Json::Value(pDVJsonDebit->PayProvinceID);
        AddtionalPayStation["PayStationId"]          = Json::Value(pDVJsonDebit->PayStationID.toStdString());
        AddtionalPayStation["PayStationName"]        = Json::Value(std::string(pDVJsonDebit->PayStationName.toLocal8Bit()));
        AddtionalPayStation["PayLane"]               = Json::Value(pDVJsonDebit->PayLane);
        AddtionalPayStation["PayRoadNo"]             = Json::Value(pDVJsonDebit->PayRoadID);
        AddtionalPayStation["PayOperatorId"]         = Json::Value(pDVJsonDebit->PayOperatorID.toStdString());
        AddtionalPayStation["PayOperatorName"]       = Json::Value(pDVJsonDebit->PayOperatorName.toStdString());

        //填充附加信息 -- 扣费车辆信息
        AddtionalVehicleInfo["VehicleClass"]         = Json::Value(pDVJsonDebit->VehicleClass);
        AddtionalVehicleInfo["VehicleType"]          = Json::Value(pDVJsonDebit->VehicleType);
        AddtionalVehicleInfo["VehicleLicense"]       = Json::Value(std::string(pDVJsonDebit->VehicleLicense.toLocal8Bit()));
        AddtionalVehicleInfo["VehicleColor"]         = Json::Value(pDVJsonDebit->VehicleColor);
        AddtionalVehicleInfo["AxisCount"]            = Json::Value(pDVJsonDebit->AxisCount);
        AddtionalVehicleInfo["TotalWeight"]          = Json::Value(pDVJsonDebit->TotalWeight);
        AddtionalVehicleInfo["LimitWeight"]          = Json::Value(pDVJsonDebit->LimitWeight);

        //NFCPayInfo
        int payPlatformType = pDVJsonDebit->AccountInfoDataType;
        if (0x07 == payPlatformType) {
            QString nfcQRData = pDVJsonDebit->QRCodeData;
            QString nfcPayInfo = UPayUtils::getNFCPayInfo(nfcQRData);

            Json::Reader reader;
            Json::Value nfcInfo;

            reader.parse(nfcPayInfo.toStdString(), nfcInfo);
            DebitCondition["NFCPayInfo"] = nfcInfo;

            QString qrcodeNFC = "111111";
            DebitConditionBase["PayPlatformQRCode"]	= Json::Value(std::string(qrcodeNFC.toLocal8Bit()));
        }

        //格式化
        DebitConditionAddtional["EntryStation"]  = AddtionalEntryStation;
        DebitConditionAddtional["PayStation"]	= AddtionalPayStation;
        DebitConditionAddtional["VehicleInfo"]	= AddtionalVehicleInfo;
        DebitCondition["Base"]			= DebitConditionBase;
        DebitCondition["Addtional"]		= DebitConditionAddtional;

        root["Data"] = DebitCondition;

        //转化为字符串
        Json::FastWriter fast_writer;

        strReturn = QString(fast_writer.write(root).c_str());
    }
    return strReturn;
}



/**************************************************
 * 函数名称：FormatSendData2Json
 * 函数描述：将接收到的数据结构转化为标准的JSON格式 -- 扣费状态查询
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
QString	CJsonDebitOperate::FormatSendData2Json( const CDVJsonDebitCheck* pDVJsonDebitCheck)
{
    QString strReturn;

    if(NULL != pDVJsonDebitCheck)
    {
        // 表示整个 json 对象
        Json::Value root;
        // 临时JSON对象
        Json::Value DebitCheckCondition;

        Json::Value DebitCheckBase;
        /*****构造JSAO你数据结构***********************************************************/

        //标识版本
        root["Version"]		= Json::Value(std::string(pDVJsonDebitCheck->Version.toLocal8Bit()));
        //标识数据类型
        root["PackageType"]	= Json::Value(pDVJsonDebitCheck->PackageType);
        //安全认证
        root["Token"]		= Json::Value(std::string(pDVJsonDebitCheck->Token.toLocal8Bit()));
        //标识省份
        root["ProvinceID"]	= Json::Value(pDVJsonDebitCheck->ProvinceID);
        //递交次数
        root["SubmitCount"]	= Json::Value(pDVJsonDebitCheck->SubmitCount);
        //递交时间
        root["SubmitTime"]	= Json::Value(std::string(pDVJsonDebitCheck->SubmitTime.toLocal8Bit()));
        //扣款凭证
        root["PayIdentifier"]	= Json::Value(std::string(pDVJsonDebitCheck->PayIdentifier.toLocal8Bit()));

        //转化为字符串
        Json::FastWriter fast_writer;
        strReturn = QString(fast_writer.write(root).c_str());
    }
    return strReturn;
}



/**************************************************
 * 函数名称：FormatSendData2Json
 * 函数描述：将接收到的数据结构转化为标准的JSON格式 -- 支付请求(撤单)
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
QString	CJsonDebitOperate::FormatSendData2Json( const CDVJsonDebitCancel* pDVJsonDebitCancel)
{
    QString strReturn;

    if(NULL != pDVJsonDebitCancel)
    {
        // 表示整个 json 对象
        Json::Value root;
        // 临时JSON对象
        Json::Value DebitCancelCondition;

        Json::Value DebitCancelBase;
        /*******构造JSON数据结构***********************************************************/

        //标识版本
        root["Version"]		= Json::Value(std::string(pDVJsonDebitCancel->Version.toLocal8Bit()));
        //标识数据类型
        root["PackageType"]	= Json::Value(pDVJsonDebitCancel->PackageType);
        //安全认证
        root["Token"]		= Json::Value(std::string(pDVJsonDebitCancel->Token.toLocal8Bit()));
        //标识省份
        root["ProvinceID"]	= Json::Value(pDVJsonDebitCancel->ProvinceID);
        //递交次数
        root["SubmitCount"]	= Json::Value(pDVJsonDebitCancel->SubmitCount);
        //递交时间
        root["SubmitTime"]	= Json::Value(std::string(pDVJsonDebitCancel->SubmitTime.toLocal8Bit()));
        //扣款凭证
        root["PayIdentifier"]	= Json::Value(std::string(pDVJsonDebitCancel->PayIdentifier.toLocal8Bit()));

        //NFCPayInfo
        int payPlatformType = pDVJsonDebitCancel->m_payPlatformType;
        if (0x07 == payPlatformType) {
            QString nfcContData = pDVJsonDebitCancel->m_contdata;
            QString nfcPayInfo = UPayUtils::getNFCPayInfo(nfcContData);

            Json::Reader reader;
            Json::Value nfcInfo;

            reader.parse(nfcPayInfo.toStdString(), nfcInfo);
            root["NFCPayInfo"] = nfcInfo;
        }

        //转化为字符串
        Json::FastWriter fast_writer;
        strReturn = QString(fast_writer.write(root).c_str());
    }
    return strReturn;
}




/**************************************************
 * 函数名称：CJsonDebitOperate::FormatSendData2Json
 * 函数描述：将接收到的数据结构转化为标准的JSON格式 -- 支付请求（二次处理）
 * 参   数: const CDVJsonDebitSecond* pDVJsonDebitSecond
 * 返   回：QString
 * ************************************************/
QString CJsonDebitOperate::FormatSendData2Json(const CDVJsonDebitSecond* pDVJsonDebitSecond)
{
    QString strReturn;

    if(NULL != pDVJsonDebitSecond)
    {
        // 表示整个 json 对象
        Json::Value root;
        // 临时JSON对象
        Json::Value DebitCondition;

        Json::Value DebitConditionBase;

        Json::Value DebitConditionAddtional;

        Json::Value AddtionalEntryStation;

        Json::Value AddtionalPayStation;

        Json::Value AddtionalVehicleInfo;
        /******构造JSAO你数据结构***********************************************************/

        //标识版本
        root["Version"]			= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.Version.toLocal8Bit()));
        //标识数据类型
        root["PackageType"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PackageType);
        //安全认证
        root["Token"]			= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.Token.toLocal8Bit()));
        //标识省份
        root["ProvinceID"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.ProvinceID);
        //递交次数
        root["SubmitCount"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.SubmitCount);
        //递交时间
        root["SubmitTime"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.SubmitTime.toLocal8Bit()));
        //交易标记
        root["RecordMatchCode"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.RecordMatchCode.toLocal8Bit()));
        //上次扣款凭证
        root["ReversePayIdentifier"]    = Json::Value(std::string(pDVJsonDebitSecond->m_strLastPayIdentifier.toLocal8Bit()));
        //扣款凭证
        root["PayIdentifier"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.PayIdentifier.toLocal8Bit()));
        //交易安全认证码
        root["TradeSecurityCode"]	= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.TradeSecurityCode.toLocal8Bit()));


        //填充基本信息
        DebitConditionBase["ChannelUserId"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.IDBindInPayPlatform.toLocal8Bit()));
        DebitConditionBase["ChannelType"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.IDBindInPayPlatformType);
        DebitConditionBase["PayPlatformQRCode"]         = Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.QRCodeData.toLocal8Bit()));
        DebitConditionBase["VirtualCardID"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.SMVirtualCardID.toLocal8Bit()));
        DebitConditionBase["EntryInfoUniqueFlag"]	= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.EntryInfoUniqueFlag.toLocal8Bit()));
        DebitConditionBase["DebitMoney"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.DebitMoney);
        DebitConditionBase["DebitType"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.DebitType);
        //tiaoshi
        DebitConditionBase["DebitTime"]	                = Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.DebitTime.toLocal8Bit().data()));


        //填充附加信息 -- 入站信息
        AddtionalEntryStation["EntryProvinceId"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryProvinceID);
        AddtionalEntryStation["EntryStationId"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryStationID.toStdString());
        AddtionalEntryStation["EntryStationName"]	= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.EntryStationName.toLocal8Bit()));
        AddtionalEntryStation["EntryLane"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryLane);
        //tiaoshi

        // AddtionalEntryStation["EntryTime"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.EntryTime.toLocal8Bit().data()));
        //.Format(_T("%Y-%m-%d %H:%M:%S")));
        //20170405
        AddtionalEntryStation["EntryTime"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.EntryTime.toLocal8Bit()));


        //填充附加信息 -- 扣费站信息
        AddtionalPayStation["PayProvinceId"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayProvinceID);
        AddtionalPayStation["PayStationId"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayStationID.toStdString());
        AddtionalPayStation["PayStationName"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.PayStationName.toLocal8Bit()));
        AddtionalPayStation["PayLane"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayLane);

        //填充附加信息 -- 扣费车辆信息
        AddtionalVehicleInfo["VehicleClass"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleClass);
        AddtionalVehicleInfo["VehicleType"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleType);
        AddtionalVehicleInfo["VehicleLicense"]		= Json::Value(std::string(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleLicense.toLocal8Bit()));
        AddtionalVehicleInfo["VehicleColor"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleColor);


        //格式化
        DebitConditionAddtional["EntryStation"] = AddtionalEntryStation;
        DebitConditionAddtional["PayStation"]	= AddtionalPayStation;
        DebitConditionAddtional["VehicleInfo"]	= AddtionalVehicleInfo;
        DebitCondition["Base"]			= DebitConditionBase;
        DebitCondition["Addtional"]		= DebitConditionAddtional;

        root["Data"] = DebitCondition;

        //转化为字符串
        Json::FastWriter fast_writer;
        strReturn = QString(fast_writer.write(root).c_str());
    }
    return strReturn;
}



/**************************************************
 * 函数名称：FormatSendData2Json
 * 函数描述：将接收到的数据结构转化为标准的JSON格式 -- 支付请求（二次处理）
 * 参   数:  const CDVJsonDebitSecond* pDVJsonDebitSecond
 * 返   回：QString
 * ************************************************/
CDVJsonBase* CJsonDebitOperate::FormatResponseResult2Class(const QString& strResponseResult, const int& iDataType)
{
    CDVJsonBase*  pDVJsonBase = NULL;
    switch(iDataType)
    {
    case JSON_DT_SEND_DIBIT:
        pDVJsonBase = fnAnalysisStringForDebit(strResponseResult);
        break;
    case JSON_DT_SEND_DIBIT_CHECK:
        pDVJsonBase = fnAnalysisStringForDebitCheck(strResponseResult);
        break;
    case JSON_DT_SEND_DIBIT_CANCEL:
        pDVJsonBase = fnAnalysisStringForDebitCancel(strResponseResult);
        break;
    case JSON_DT_SEND_DIBIT_SECOND:
        pDVJsonBase = fnAnalysisStringForDebitSecond(strResponseResult);
        break;
    }
    return pDVJsonBase;
}
/**************************************************
 * 函数名称：fnAnalysisStringForDebit
 * 函数描述：解析字符串，获得扣款操作回馈数据
 * 参   数: QString
 * 返   回：QString
 * ************************************************/
CDVJsonDebit* CJsonDebitOperate::fnAnalysisStringForDebit( const QString& strResponseResult)
{
    CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();

    try
    {
        if(NULL == pDVJsonDebit)
            throw -1;

        //进行数据格式转换
        Json::Reader reader;
        Json::Value  root;
        /*陈老大*/
        QString strResult(strResponseResult);
        if(!reader.parse(strResult.toLocal8Bit().data(),root))
            throw -2;
        strResult.clear();



        //        //进行数据格式转换
        //        QJsonParseError json_error;
        //        QJsonDocument reader;
        //        QJsonObject root;
        //        QJsonValue value;


        //        reader = QJsonDocument::fromJson(strResponseResult.toUtf8(), &json_error);

        //        if(!reader.isObject())
        //            throw -2;
        //        root = reader.object();
        //        */


        //获取版本
        if(Json::stringValue == root["Version"].type())
        {
            pDVJsonDebit->Version       = root["Version"].asCString();
        }
        //获取数据包类型
        if(Json::intValue == root["PackageType"].type())
        {
            pDVJsonDebit->PackageType	= root["PackageType"].asInt();
        }
        //获取响应状态代码
        if(Json::intValue == root["Status"].type())
        {
            pDVJsonDebit->ServerResponseCode    = root["Status"].asInt();
        }
        //获取响应状态描述
        if(Json::stringValue == root["Message"].type())
        {
            pDVJsonDebit->ServerResponseResultDesc  = root["Message"].asCString();
        }
        //获取服务器接收数据时间
        if(Json::stringValue == root["ReceveTime"].type())
        {
            pDVJsonDebit->ServerReceiveDataTime     = root["ReceveTime"].asCString();
        }

        //获取服务器响应数据时间
        if(Json::stringValue == root["ResponseTime"].type())
        {
            pDVJsonDebit->ServerResponseDataTime    = root["ResponseTime"].asCString();
        }
        //获取服务器实际扣款时间
        if(Json::stringValue == root["ServerDebitTime"].type())
        {
            pDVJsonDebit->ServerDebitTime = root["ServerDebitTime"].asCString();
        }

        //获取扣款详细状态代码
        if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
        {
            pDVJsonDebit->DebitStatusCode   = root["ErrorCodeByPayPlatform"].asInt();
        }
        //获取扣款详细状态描述
        if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
        {
            pDVJsonDebit->DebitStatusDesc   = root["ErrorDescByPayPlatform"].asCString();
        }
        //获取扣款订单号
        if(Json::stringValue == root["DebitOrderNumber"].type())
        {
            pDVJsonDebit->DebitOrderNumber  = root["DebitOrderNumber"].asCString();
        }

        //获取交易安全码
        if(Json::stringValue == root["TradeSecurityCode"].type())
        {
            pDVJsonDebit->TradeSecurityCode = root["TradeSecurityCode"].asCString();
        }
        //获取交易识别码
        if(Json::stringValue == root["PayIdentifier"].type())
        {
            pDVJsonDebit->PayIdentifier     = root["PayIdentifier"].asCString();
        }
        //获取支付平台类型
        if(Json::intValue == root["PayChannelType"].type())
        {
            pDVJsonDebit->QRCodeType        = root["PayChannelType"].asInt();
        }
        //获取虚拟卡号
        if(Json::stringValue == root["PayVirtualCardID"].type())
        {
            QString strTempID(root["PayVirtualCardID"].asCString());
            //tiaoshi            pDVJsonDebit->PayVirtualCardID		= strTempID.IsEmpty()?_T(""):GetGlobalApp()->FmtStr(_T("%s%s"),GetGlobalApp()->m_strVirtualCardPrefix,root["PayVirtualCardID"].asCString());
        }

        //调试用
        //	pDVJsonDebit->ServerResponseCode    = HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1://申请资源失败
            break;
        case -2://数据解析失败
            break;
        }
    }
    return pDVJsonDebit;
}


/**************************************************
 * 函数名称：fnAnalysisStringForDebitSecond
 * 函数描述：解析字符串，获得支付请求（二次处理）服务端回馈数据
 * 参   数: QString
 * 返   回：QString
 * ************************************************/
CDVJsonDebit* CJsonDebitOperate::fnAnalysisStringForDebitSecond(const QString& strResponseResult)
{
    CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();

    try
    {
        if(NULL == pDVJsonDebit)
            throw -1;

        //进行数据格式转换
        Json::Reader reader;
        Json::Value  root;
        /*陈老大
        CString strResult(strResponseResult);
        if(!reader.parse(strResult.GetBuffer(0),root))
            throw -2;
        strResult.ReleaseBuffer();
        */


        //获取版本
        if(Json::stringValue == root["Version"].type())
        {
            pDVJsonDebit->Version   = root["Version"].asCString();
        }
        //获取数据包类型
        if(Json::intValue == root["PackageType"].type())
        {
            pDVJsonDebit->PackageType   = root["PackageType"].asInt();
        }
        //获取响应状态代码
        if(Json::intValue == root["Status"].type())
        {
            pDVJsonDebit->ServerResponseCode    = root["Status"].asInt();
        }
        //获取响应状态描述
        if(Json::stringValue == root["Message"].type())
        {
            pDVJsonDebit->ServerResponseResultDesc  = root["Message"].asCString();
        }
        //获取服务器接收数据时间
        if(Json::stringValue == root["ReceveTime"].type())
        {
            //陈老大
            pDVJsonDebit->ServerReceiveDataTime     = root["ReceveTime"].asCString();
        }

        //获取服务器响应数据时间
        if(Json::stringValue == root["ResponseTime"].type())
        {
            //陈老大
            pDVJsonDebit->ServerResponseDataTime    = root["ResponseTime"].asCString();
        }

        //获取扣款详细状态代码
        if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
        {
            pDVJsonDebit->DebitStatusCode   = root["ErrorCodeByPayPlatform"].asInt();
        }
        //获取扣款详细状态描述
        if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
        {
            pDVJsonDebit->DebitStatusDesc   = root["ErrorDescByPayPlatform"].asCString();
        }
        //获取扣款订单号
        if(Json::stringValue == root["DebitOrderNumber"].type())
        {
            pDVJsonDebit->DebitOrderNumber  = root["DebitOrderNumber"].asCString();
        }
        //获取交易安全码
        if(Json::stringValue == root["TradeSecurityCode"].type())
        {
            pDVJsonDebit->TradeSecurityCode = root["TradeSecurityCode"].asCString();
        }

        //获取支付平台类型
        if(Json::intValue == root["PayChannelType"].type())
        {
            pDVJsonDebit->ServerJudgeDebitIDType    = root["PayChannelType"].asInt();
        }
        //获取支付平台类型
        if(Json::stringValue == root["PayVirtualCardID"].type())
        {
            //tiaoshi
            QString strTempID(root["PayVirtualCardID"].asCString());
            //tiaoshi
            pDVJsonDebit->PayVirtualCardID		= "122343";
            //strTempID.IsEmpty()?_T(""):GetGlobalApp()->FmtStr(_T("%s%s"),GetGlobalApp()->m_strVirtualCardPrefix,root["PayVirtualCardID"].asCString());
        }
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1://申请资源失败
            break;
        case -2://数据解析失败
            break;
        }
    }
    return pDVJsonDebit;
}




/**************************************************
 * 函数名称：CJsonDebitOperate::fnAnalysisStringForDebitCheck
 * 函数描述：解析字符串，获得扣款状态查询回馈数据
 * 参   数: QString
 * 返   回：QString
 * ************************************************/
CDVJsonDebitCheck* CJsonDebitOperate::fnAnalysisStringForDebitCheck(const QString& strResponseResult)
{
    qDebug() << "CJsonDebitOperate::fnAnalysisStringForDebitCheck";

    CDVJsonDebitCheck* pDVJsonDebitCheck = new CDVJsonDebitCheck();

    try
    {
        if(NULL == pDVJsonDebitCheck)
            throw -1;

        //进行数据格式转换
        Json::Reader reader;
        Json::Value  root;
        /*tiaoshi*/
        QString strResult(strResponseResult);
        if(!reader.parse(strResult.toLocal8Bit().data(),root))
            throw -2;
        strResult.clear();

        //获取版本
        if(Json::stringValue == root["Version"].type())
        {
            pDVJsonDebitCheck->Version  = root["Version"].asCString();
        }
        //获取数据包类型
        if(Json::intValue == root["PackageType"].type())
        {
            pDVJsonDebitCheck->PackageType  = root["PackageType"].asInt();
        }
        //获取响应状态代码
        if(Json::intValue == root["Status"].type())
        {
            pDVJsonDebitCheck->ServerResponseCode   = root["Status"].asInt();
        }
        //获取响应状态描述
        if(Json::stringValue == root["Message"].type())
        {
            pDVJsonDebitCheck->ServerResponseResultDesc	= root["Message"].asCString();
        }
        //获取服务器接收数据时间
        if(Json::stringValue == root["ReceveTime"].type())
        {
            //陈老大            pDVJsonDebitCheck->ServerReceiveDataTime    = root["ReceveTime"].asCString();
        }

        //获取服务器响应数据时间
        if(Json::stringValue == root["ResponseTime"].type())
        {
            //陈老大            pDVJsonDebitCheck->ServerResponseDataTime	= root["ResponseTime"].asCString();
        }
        //获取服务器实际扣款时间
        if(Json::stringValue == root["ServerDebitTime"].type())
        {
            pDVJsonDebitCheck->ServerDebitTime	= root["ServerDebitTime"].asCString();
        }
        //获取扣款详细状态代码
        if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
        {
            pDVJsonDebitCheck->DebitStatusCode  = root["ErrorCodeByPayPlatform"].asInt();
        }
        //获取扣款详细状态描述
        if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
        {
            pDVJsonDebitCheck->DebitStatusDesc  = root["ErrorDescByPayPlatform"].asCString();
        }
        //获取扣款订单号
        if(Json::stringValue == root["DebitOrderNumber"].type())
        {
            pDVJsonDebitCheck->DebitOrderNumber = root["DebitOrderNumber"].asCString();
        }

        //获取交易识别码
        if(Json::stringValue == root["PayIdentifier"].type())
        {
            pDVJsonDebitCheck->PayIdentifier    = root["PayIdentifier"].asCString();
        }
        //获取支付凭证所属第三方支付平台类型
        if(Json::intValue == root["PayChannelType"].type())
        {
            pDVJsonDebitCheck->QRCodeType   = root["PayChannelType"].asInt();
        }

        //调试用
        //	pDVJsonDebitCheck->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1://申请资源失败
            break;
        case -2://数据解析失败
            break;
        }
    }
    return pDVJsonDebitCheck;
}



/**************************************************
 * 函数名称：CJsonDebitOperate::fnAnalysisStringForDebitCancel
 * 函数描述：解析字符串，获得支付请求（撤单）服务端回馈数据
 * 参   数: QString
 * 返   回：QString
 * ************************************************/
CDVJsonDebitCancel* CJsonDebitOperate::fnAnalysisStringForDebitCancel( const QString& strResponseResult)
{
    //    qDebug() << "CJsonDebitOperate::fnAnalysisStringForDebitCancel";

    CDVJsonDebitCancel* pDVJsonDebitCancel = new CDVJsonDebitCancel();

    try
    {
        if(NULL == pDVJsonDebitCancel)
            throw -1;

        //进行数据格式转换
        Json::Reader reader;
        Json::Value  root;

        QString strResult(strResponseResult);
        if(!reader.parse(strResult.toLocal8Bit().data(),root))
            throw -2;
        strResult.clear();



        //获取版本
        if(Json::stringValue == root["Version"].type())
        {
            pDVJsonDebitCancel->Version = root["Version"].asCString();
        }
        //获取数据包类型
        if(Json::intValue == root["PackageType"].type())
        {
            pDVJsonDebitCancel->PackageType = root["PackageType"].asInt();
        }
        //获取响应状态代码
        if(Json::intValue == root["Status"].type())
        {
            pDVJsonDebitCancel->ServerResponseCode  = root["Status"].asInt();
        }
        //获取响应状态描述
        if(Json::stringValue == root["Message"].type())
        {
            pDVJsonDebitCancel->ServerResponseResultDesc    = root["Message"].asCString();
        }
        //获取服务器接收数据时间
        if(Json::stringValue == root["ReceveTime"].type())
        {
            //tiaoshi
            pDVJsonDebitCancel->ServerReceiveDataTime   = root["ReceveTime"].asCString();
        }

        //获取服务器响应数据时间
        if(Json::stringValue == root["ResponseTime"].type())
        {
            //tiaoshi
            pDVJsonDebitCancel->ServerResponseDataTime	= root["ResponseTime"].asCString();
        }

        //获取撤单的订单号
        if(Json::stringValue == root["DebitOrderNumber"].type())
        {
            pDVJsonDebitCancel->ServerReceiveDataTime   = root["DebitOrderNumber"].asCString();
        }

        //payId
        if(Json::stringValue == root["PayIdentifier"].type())
        {
            pDVJsonDebitCancel->PayIdentifier   = root["PayIdentifier"].asCString();
        }

        //调试用
        //	pDVJsonDebitCancel->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1://申请资源失败
            break;
        case -2://数据解析失败
            break;
        }
    }
    return pDVJsonDebitCancel;
}










