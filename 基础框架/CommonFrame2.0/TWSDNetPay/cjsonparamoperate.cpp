#include "cjsonparamoperate.h"
#include <QDateTime>
#include <QDebug>
#include "cdvjsonparammmi.h"
#include "cdvjsonparamdebitresult.h"

#include "twsdnetpay.h"


CJsonParamOperate::CJsonParamOperate(void)
{

}


CJsonParamOperate::~CJsonParamOperate(void)
{
}

//将接收到的数据结构转化为标准的JSON格式 -- 支付请求参数数据变换(扣费结果)
QString CJsonParamOperate::FormatParamData2Json( const CDVJsonParamDebitResult* pDVJsonParamDebitResult)
{
    QString strReturn;
    try
    {
        if(NULL == pDVJsonParamDebitResult)
            throw -1;
        // 表示整个 json 对象
        Json::Value root;
        // 标识用户参数
        Json::Value ResultContext;
        // 标识详细数据项
        Json::Value Data;
        // 标识关键项
        Json::Value KeyItem;
        /*****构造JSON数据结构***********************************************************/

        //标识版本
        ResultContext["Version"]        = Json::Value(std::string(pDVJsonParamDebitResult->Version.toLocal8Bit()));
        //标识扣款类型
        Data["ResultType"]		= Json::Value(pDVJsonParamDebitResult->ResultType);
        //标识执行状态
        Data["ExecuteCode"]		= Json::Value(pDVJsonParamDebitResult->ExecuteCode);
        //标识执行结果描述
        Data["ExecuteDesc"]		= Json::Value(std::string(pDVJsonParamDebitResult->ExecuteDesc.toLocal8Bit()));

        //关键数据项 -- 交易安全码
        KeyItem["TradeSecurityCode"]	= Json::Value(std::string(pDVJsonParamDebitResult->TradeSecurityCode.toLocal8Bit()));
        //关键数据项 -- 第三方支付平台类型
        KeyItem["PayPlatformType"]	= Json::Value(pDVJsonParamDebitResult->PayPlatformType);
        //关键数据项 -- 交易识别码
        KeyItem["PayIdentifier"]	= Json::Value(std::string(pDVJsonParamDebitResult->PayIdentifier.toLocal8Bit()));
        //关键数据项 -- 订单号
        KeyItem["DebitOrder"]		= Json::Value(std::string(pDVJsonParamDebitResult->DebitOrder.toLocal8Bit()));
        //关键数据项 -- 服务端扣款时间
        //tiaoshi
        KeyItem["DebitTime"]		= Json::Value(std::string(pDVJsonParamDebitResult->DebitTime.toLocal8Bit().data()));
        //.Format(_T("%Y-%m-%d %H:%M:%S")));
        //关键数据项 -- 消耗时间
        KeyItem["ConsumeTime"]		= Json::Value(pDVJsonParamDebitResult->ConsumeTime);

        Data["TradeKeyItem"]		= KeyItem;
        ResultContext["Data"]		= Data;
        root["DebitResultContext"]	= ResultContext;
        //转化为字符串
        Json::FastWriter fast_writer;
        strReturn = QString(fast_writer.write(root).c_str());
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



//将接收到的数据格式转化为标准的JSON格式 -- 用户数据参数数据变换
QString CJsonParamOperate::FormatParamData2Json( const CDVJsonParamAccount* pDVJsonParamAccount)
{
    QString strReturn;
    try
    {
        if(NULL == pDVJsonParamAccount)
            throw -1;
        // 表示整个 json 对象
        Json::Value root;
        // 标识用户参数
        Json::Value ParamAccount;
        // 标识详细数据项
        Json::Value Data;
        /**构造JSAO数据结构***********************************************************/

        //标识版本
        ParamAccount["Version"] = Json::Value(std::string(pDVJsonParamAccount->Version.toLocal8Bit()));
        //标识数据类型
        Data["Type"]		= Json::Value(pDVJsonParamAccount->DataType);
        //标识数据来源
        Data["Source"]		= Json::Value(pDVJsonParamAccount->DataSrc);
        //标识数据内容
        Data["Context"]		= Json::Value(std::string(pDVJsonParamAccount->DataContext.toLocal8Bit()));

        ParamAccount["Data"]	= Data;
        root["AccountContext"]	= ParamAccount;
        //转化为字符串
        Json::FastWriter fast_writer;
        strReturn = QString(fast_writer.write(root).c_str());
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



CDVJsonBase* CJsonParamOperate::FormatResponseResult2Class( const QString& strResponseResult, const int& iDataType)
{
    CDVJsonBase* pDVJsonBase = NULL;
    QString  strlog;

    switch(iDataType)
    {

    case JSON_DT_PARAM_ACCOUNT:
    {
        try
        {
            pDVJsonBase = new CDVJsonParamAccount();
            if(NULL == pDVJsonBase)
            {
                throw -1;
            }
            //进行数据格式转换
            Json::Reader reader;
            Json::Value  root;
            Json::Value  AccountContext;
            Json::Value  Data;

            QString strResult(strResponseResult);
            if(!reader.parse(strResult.toLocal8Bit().data(),root))
            {
                strResult.clear();
                throw -2;
            }

            if(Json::nullValue == root.type())
                throw -3;


            //获取用户信息区
            if(Json::nullValue == root["AccountContext"].type())
                throw -4;
            AccountContext = root["AccountContext"];

            //获取版本
            if(Json::stringValue != AccountContext["Version"].type())
                throw -5;
            pDVJsonBase->Version = AccountContext["Version"].asCString();

            //获取数据信息区
            if(Json::nullValue == AccountContext["Data"].type())
                throw -6;
            Data = AccountContext["Data"];

            //获取用户数据类型
            if(Json::intValue != Data["Type"].type())
                throw -7;
            ((CDVJsonParamAccount*)pDVJsonBase)->DataType = Data["Type"].asInt();

            //获取用户数据来源
            if(Json::intValue != Data["Source"].type())
                throw -8;
            ((CDVJsonParamAccount*)pDVJsonBase)->DataSrc = Data["Source"].asInt();

            //获取用户数据内容
            if(Json::stringValue != Data["Context"].type())
                throw -9;
            ((CDVJsonParamAccount*)pDVJsonBase)->DataContext = Data["Context"].asCString();
        }
        catch(int& iErrorCode)
        {
            switch(iErrorCode)
            {
            case -1:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：";
                break;
            case -2:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：";
                break;
            case -3:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：root";
                break;
            case -4:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：AccountContext";
                break;
            case -5:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：Version";
                break;
            case -6:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：Data";
                break;
            case -7:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：Type";
                break;
            case -8:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：Source";
                break;
            case -9:
                strlog = "[业务线程],JSON_DT_PARAM_ACCOUNT数据解析,异常找不到：Context";
                break;

            }
            GetGlobalApp()->WriteLogFile(strlog);
        }
    }
        break;

    case JSON_DT_PARAM_DEBIT_MONEY:
    {
        try
        {
            pDVJsonBase = new CDVJsonParamDebitMoney();
            if(NULL == pDVJsonBase)
            {
                throw -1;
            }
            //进行数据格式转换
            Json::Reader reader;
            Json::Value  root;
            Json::Value  DebitParamContext;
            Json::Value  Data;
            Json::Value  Debit;
            Json::Value  Vehicle;
            Json::Value  Operation;
            Json::Value	Entry;
            Json::Value Paystation;


            QString strResult(strResponseResult);
            if(!reader.parse(strResult.toLocal8Bit().data(),root))
            {
                strResult.clear();
                throw -2;
            }

            if(Json::nullValue == root.type())
                throw -3;

            //获取交易数据区
            if(Json::nullValue == root["DebitParamContext"].type())
                throw -4;
            DebitParamContext = root["DebitParamContext"];

            //获取数据明细区
            if(Json::nullValue == DebitParamContext["Data"].type())
                throw -5;
            Data = DebitParamContext["Data"];

            //获取超时时间
            if(Json::intValue != Data["OverTime"].type())
                throw -6;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->OverTime = Data["OverTime"].asInt();

            //获取扣款操作模式
            if(Json::intValue != Data["OperationMode"].type())
                throw -7;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->OperationMode = Data["OperationMode"].asInt();

            //获取扣款信息区
            if(Json::nullValue == Data["Debit"].type())
                throw -8;
            Debit = Data["Debit"];


            //获取扣款金额
            if(Json::intValue != Debit["Money"].type())
                throw -9;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->Money = Debit["Money"].asInt();

            //获取交易识别码
            if(Json::stringValue != Debit["PayIdentifier"].type())
                throw -10;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->PayIdentifier = Debit["PayIdentifier"].asCString();

            //            //获取支付凭证类型
            //            if(Json::intValue != Debit["PayCertificateType"].type())
            //                throw -11;
            //            ((CDVJsonParamDebitMoney*)pDVJsonBase)->PayCertificateType = Debit["PayCertificateType"].asInt();

            //获取支付凭证内容
            //            if(Json::stringValue != Debit["PayCertificateCode"].type())
            //                throw -12;
            //            ((CDVJsonParamDebitMoney*)pDVJsonBase)->PayCertificateCode = Debit["PayCertificateCode"].asCString();

            //获取扣款主分类
            if(Json::intValue != Debit["Type"].type())
                throw -13;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->DebitMainType = Debit["Type"].asInt();


            //获取过车时间
            if(Json::stringValue != Debit["SubTime"].type())
                throw -14;
            //tiaoshi
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->SubTime = Debit["SubTime"].asCString();

            //获取车辆信息区
            if(Json::nullValue == Data["Vehicle"].type())
                throw -15;
            Vehicle = Data["Vehicle"];

            //获取车种
            if(Json::intValue != Vehicle["Type"].type())
                throw -16;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleType = Vehicle["Type"].asInt();

            //获取车型
            if(Json::intValue != Vehicle["Class"].type())
                throw -17;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleClass = Vehicle["Class"].asInt();

            //获取车牌
            if(Json::stringValue != Vehicle["License"].type())
                throw -18;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleLicense = Vehicle["License"].asCString();


            //获取车牌颜色
            if(Json::intValue != Vehicle["VLColor"].type())
                throw -19;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->VLColor = Vehicle["VLColor"].asInt();

            //获取轴数
            if(Json::intValue != Vehicle["AxisCount"].type())
                throw -20;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->AxisCount = Vehicle["AxisCount"].asInt();

            //获取装载重量
            if(Json::intValue != Vehicle["Weight"].type())
                throw -21;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->Weight = Vehicle["Weight"].asInt();

            //获取限重
            if(Json::intValue == Vehicle["LimitWeight"].type())
            {
                ((CDVJsonParamDebitMoney*)pDVJsonBase)->LimitWeight = Vehicle["LimitWeight"].asInt();
            }

            //获取交易信息区
            if(Json::nullValue == Data["Operation"].type())
                throw -22;
            Operation = Data["Operation"];


            //获取通行凭证类型
            if(Json::intValue != Operation["PassCertificateType"].type())
                throw -23;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->PassCertificateType = Operation["PassCertificateType"].asInt();

            //获取通行凭证卡号
            if(Json::stringValue != Operation["CardID"].type())
                throw -24;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->CardID = Operation["CardID"].asCString();

            //获取统计日
            if(Json::intValue != Operation["TollDate"].type())
                throw -25;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->TollDate = Operation["TollDate"].asInt();

            //获取统计班次
            if(Json::intValue == Operation["ShiftID"].type())
                ((CDVJsonParamDebitMoney*)pDVJsonBase)->ShiftID = Operation["ShiftID"].asInt();

            //打印票号
            if(Json::stringValue != Operation["TicketNo"].type())
                throw -40;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->TicketNo = Operation["TicketNo"].asCString();

            //收费员编号
            if(Json::stringValue == Operation["OperatorID"].type())
                ((CDVJsonParamDebitMoney*)pDVJsonBase)->OperatorID = Operation["OperatorID"].asCString();

            //收费员姓名
            if(Json::stringValue == Operation["OperatorName"].type())
                ((CDVJsonParamDebitMoney*)pDVJsonBase)->OperatorName = Operation["OperatorName"].asCString();


            //获取入口信息区
            if(Json::nullValue == Data["Entry"].type())
                throw -27;
            Entry = Data["Entry"];

            //获取入口区域编码
            if(Json::intValue != Entry["AreaID"].type())
                throw -28;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryAreaID = Entry["AreaID"].asInt();

            //获取入口路段编码
            if(Json::intValue != Entry["RoadID"].type())
                throw -29;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryRoadID = Entry["RoadID"].asInt();

            //获取入口站点编码
            if(Json::stringValue == Entry["StationID"].type())
            {
                ((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryStationID = Entry["StationID"].asCString();
            }
            else if(Json::intValue == Entry["StationID"].type())
            {
                ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryStationID = Entry["StationID"].asInt();
                int tmpInt = Entry["StationID"].asInt();
                ((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryStationID = QString("%1").arg(tmpInt);
            }
            else
            {
                throw -30;
            }

            //获取入口站点名称
            if(Json::stringValue != Entry["StationName"].type())
                throw -31;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryStationName = Entry["StationName"].asCString();


            //获取入口车道编码
            if(Json::intValue != Entry["LaneID"].type())
                throw -32;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryLane = Entry["LaneID"].asInt();

            //获取入口车种
            if(Json::intValue != Entry["VType"].type())
                throw -33;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleType = Entry["VType"].asInt();

            //获取入口车型
            if(Json::intValue != Entry["VClass"].type())
                throw -34;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleClass = Entry["VClass"].asInt();

            //获取入口车牌
            if(Json::stringValue != Entry["License"].type())
                throw -35;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleLicense = Entry["License"].asCString();

            //获取入口车牌颜色
            if(Json::intValue != Entry["VColor"].type())
                throw -36;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVLColor = Entry["VColor"].asInt();

            //获取入口时间
            if(Json::stringValue != Entry["Time"].type())
                throw -37;
            ((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryTime = Entry["Time"].asCString();



            //操作员信息，江苏版
            if(Json::nullValue != Data["Paystation"].type())
            {
                Paystation = Data["Paystation"];

                if(Json::stringValue == Paystation["ExitOperatorId"].type())
                {
                    ((CDVJsonParamDebitMoney*)pDVJsonBase)->OperatorID = Paystation["ExitOperatorId"].asCString();

                }
                if(Json::stringValue == Paystation["ExitOperatorName"].type())
                {
                    ((CDVJsonParamDebitMoney*)pDVJsonBase)->OperatorName = Paystation["ExitOperatorName"].asCString();

                }
                if(Json::intValue == Paystation["ExitShift"].type())
                {
                    ((CDVJsonParamDebitMoney*)pDVJsonBase)->ShiftID = Paystation["ExitShift"].asInt();

                }
            }

        }
        catch(int& iErrorCode)
        {
            switch(iErrorCode)
            {
            case -1:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到：";                     break;
            case -2:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到：";                     break;
            case -3:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： root";                break;
            case -4:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： DebitParamContext";   break;
            case -5:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Data";                break;
            case -6:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： OverTime";            break;
            case -7:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： OperationMode";       break;
            case -8:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Debit";               break;
            case -9:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Money";               break;

            case -10:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： PayIdentifier";          break;
                //     case -11:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： PayCertificateType";     break;
                //      case -12:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： PayCertificateCode";     break;
            case -13:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Type";                   break;
            case -14:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： SubTime";                break;
            case -15:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Vehicle";                break;
            case -16:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Type";                   break;
            case -17:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Class";                  break;
            case -18:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： License";                break;
            case -19:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： VLColor";                break;

            case -20:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： AxisCount";              break;
            case -21:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Weight";                 break;
            case -22:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Operation";              break;
            case -23:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： PassCertificateType";    break;
            case -24:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： CardID";                 break;
            case -25:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： TollDate";               break;
            case -26:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： ShiftID";                break;
            case -27:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Entry";                  break;
            case -28:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： AreaID";                 break;
            case -29:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： RoadID";                 break;

            case -30:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： StationID";              break;
            case -31:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： StationName";            break;
            case -32:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： LaneID";                 break;
            case -33:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： VType";                  break;
            case -34:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： VClass";                 break;
            case -35:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： License";                break;
            case -36:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： VColor";                 break;
            case -37:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Time";                   break;
            }

            GetGlobalApp()->WriteLogFile(strlog);



        }


    }
        break;

    case JSON_DT_PARAM_DEBIT_CANCLE:
    {
        try
        {
            pDVJsonBase = new CDVJsonParamDebitCancel();
            if(NULL == pDVJsonBase)
            {
                throw -1;
            }
            //进行数据格式转换
            Json::Reader reader;
            Json::Value  root;
            Json::Value  DebitCancelParamContext;
            Json::Value  Data;
            Json::Value  TradeKeyItem;
            /*tiaoshi*/


            QString strResult(strResponseResult);
            if(!reader.parse(strResult.toLocal8Bit().data(),root))
            {
                strResult.clear();
                throw -2;
            }

            if(Json::nullValue == root.type())
                throw -3;

            //获取用户信息区
            if(Json::nullValue == root["DebitCancelParamContext"].type())
                throw -4;
            DebitCancelParamContext = root["DebitCancelParamContext"];

            //获取版本
            if(Json::stringValue != DebitCancelParamContext["Version"].type())
                throw -5;
            ((CDVJsonParamDebitCancel*)pDVJsonBase)->Version = DebitCancelParamContext["Version"].asCString();

            //获取数据信息区
            if(Json::nullValue == DebitCancelParamContext["Data"].type())
                throw -6;
            Data = DebitCancelParamContext["Data"];

            //获取交易数据信息区
            if(Json::nullValue == Data["TradeKeyItem"].type())
                throw -7;
            TradeKeyItem = Data["TradeKeyItem"];

            //获取交易识别码
            if(Json::stringValue != TradeKeyItem["PayIdentifier"].type())
                throw -8;
            ((CDVJsonParamDebitCancel*)pDVJsonBase)->PayIdentifier = TradeKeyItem["PayIdentifier"].asCString();
        }
        catch(int& iErrorCode)
        {
            switch(iErrorCode)
            {
            case -1:
                strlog = "[业务线程],JSON_DT_PARAM_DEBIT_CANCLE数据解析,异常找不到：";
                break;
            case -2:
                strlog = "[业务线程],JSON_DT_PARAM_DEBIT_CANCLE数据解析,异常找不到：";
                break;
            case -3:
                strlog = "[业务线程],JSON_DT_PARAM_DEBIT_CANCLE数据解析,异常找不到：root";
                break;
            case -4:
                strlog = "[业务线程],JSON_DT_PARAM_DEBIT_CANCLE数据解析,异常找不到： DebitCancelParamContext";
                break;
            case -5:
                strlog = "[业务线程],JSON_DT_PARAM_DEBIT_CANCLE数据解析,异常找不到： Version";
                break;
            case -6:
                strlog = "[业务线程],JSON_DT_PARAM_DEBIT_CANCLE数据解析,异常找不到： Data";
                break;
            case -7:
                strlog = "[业务线程],JSON_DT_PARAM_DEBIT_CANCLE数据解析,异常找不到： TradeKeyItem";
                break;
            case -8:
                strlog = "[业务线程],JSON_DT_PARAM_DEBIT_CANCLE数据解析,异常找不到： PayIdentifier";
                break;
            }
            GetGlobalApp()->WriteLogFile(QString("JSON_DT_PARAM_DEBIT_CANCLE异常: ").arg(iErrorCode));
        }

    }
        break;

    case JSON_DT_PARAM_DEBIT_RESULT:
    {
        try
        {

            pDVJsonBase = new CDVJsonParamDebitResult();
            if(NULL == pDVJsonBase)
            {
                throw -1;
            }

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
            pDVJsonBase->Version = DebitResultContext["Version"].asCString();

            //获取数据信息区
            if(Json::nullValue == DebitResultContext["Data"].type())
                throw -6;
            Data = DebitResultContext["Data"];

            //获取结果类型
            if(Json::intValue != Data["ResultType"].type())
                throw -7;
            ((CDVJsonParamDebitResult*)pDVJsonBase)->ResultType = Data["ResultType"].asInt();

            //获取执行状态
            if(Json::intValue != Data["ExecuteCode"].type())
                throw -8;
            ((CDVJsonParamDebitResult*)pDVJsonBase)->ExecuteCode = Data["ExecuteCode"].asInt();


            //获取执行结果描述
            if(Json::nullValue == TradeKeyItem["ExecuteDesc"].type())
                throw -9;
            ((CDVJsonParamDebitResult*)pDVJsonBase)->ExecuteDesc = TradeKeyItem["ExecuteDesc"].asCString();

            //获取数据信息区
            if(Json::nullValue == Data["TradeKeyItem"].type())
                throw -10;
            TradeKeyItem = Data["TradeKeyItem"];

            //获取交易识别码
            if(Json::stringValue != TradeKeyItem["PayIdentifier"].type())
                throw -11;
            ((CDVJsonParamDebitResult*)pDVJsonBase)->PayIdentifier = TradeKeyItem["PayIdentifier"].asCString();

            //GetGlobalApp()->WriteLogFile("pDVJsonBase)->PayIdentifier:"<<((CDVJsonParamDebitResult*)pDVJsonBase)->PayIdentifier;


            //获取第三方支付平台类型
            if(Json::intValue != TradeKeyItem["PayPlatformType"].type())
                throw -12;
            ((CDVJsonParamDebitResult*)pDVJsonBase)->PayPlatformType = TradeKeyItem["PayPlatformType"].asInt();

            //获取交易安全码
            if(Json::stringValue != TradeKeyItem["TradeSecurityCode"].type())
                throw -13;
            ((CDVJsonParamDebitResult*)pDVJsonBase)->TradeSecurityCode = TradeKeyItem["TradeSecurityCode"].asCString();


            //获取交订单号
            if(Json::stringValue != TradeKeyItem["DebitOrder"].type())
                throw -14;
            ((CDVJsonParamDebitResult*)pDVJsonBase)->DebitOrder = TradeKeyItem["DebitOrder"].asCString();

            //获取服务器扣款时间
            if(Json::stringValue != TradeKeyItem["DebitTime"].type())
                throw -15;
            //tiaoshi
            ((CDVJsonParamDebitResult*)pDVJsonBase)->DebitTime = TradeKeyItem["DebitTime"].asCString();

            //获取消耗时间
            if(Json::intValue != TradeKeyItem["ConsumeTime"].type())
                throw -16;
            ((CDVJsonParamDebitResult*)pDVJsonBase)->ConsumeTime = TradeKeyItem["ConsumeTime"].asInt();

        }
        catch(int& iErrorCode)
        {
            switch(iErrorCode)
            {
            case -1:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到：";                     break;
            case -2:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到：";                     break;
            case -3:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： root";                break;
            case -4:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： DebitResultContext";  break;
            case -5:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Version";             break;
            case -6:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： Data";                break;
            case -7:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： ResultType";          break;
            case -8:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： ExecuteCode";         break;
            case -9:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： ExecuteDesc";         break;

            case -10:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： TradeKeyItem";       break;
            case -11:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： PayIdentifier";      break;
            case -12:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： PayPlatformType";    break;
            case -13:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： TradeSecurityCode";  break;
            case -14:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： DebitOrder";         break;
            case -15:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： DebitTime";          break;
            case -16:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： ConsumeTime";        break;
            }
            GetGlobalApp()->WriteLogFile(QString("JSON_DT_PARAM_DEBIT_RESULT 异常: ").arg(iErrorCode));
        }

    }
        break;

    case JSON_DT_PARAM_MMI:
    {

        try
        {
            pDVJsonBase = new CDVJsonParamMMI();
            if(NULL == pDVJsonBase)
            {
                throw -1;
            }
            //进行数据格式转换
            Json::Reader reader;
            Json::Value  root;
            Json::Value  MMIParamContext;
            Json::Value  Data;
            Json::Value  ItemData;
            Json::Value  aryTextView;
            Json::Value  aryVoiceView;
            Json::Value  aryGraphView;


            QString strResult(strResponseResult);
            if(!reader.parse(strResult.toLocal8Bit().data(),root))
            {
                strResult.clear();
                throw -2;
            }

            if(Json::nullValue == root.type())
                throw -3;


            //获取交互界面设置信息区
            if(Json::nullValue == root["MMIParamContext"].type())
                throw -4;
            MMIParamContext = root["MMIParamContext"];

            //获取版本号
            if(Json::stringValue != MMIParamContext["Version"].type())
                throw -4;
            ((CDVJsonParamMMI*)pDVJsonBase)->Version = MMIParamContext["Version"].asCString();

            //获取数据区域
            if(Json::nullValue == MMIParamContext["Data"].type())
                throw -4;
            Data = MMIParamContext["Data"];

            //获取清屏标识
            if( (Json::booleanValue != Data["ClearMMI"].type()) && (Json::intValue != Data["ClearMMI"].type()))
                throw -4;
            if(Json::booleanValue == Data["ClearMMI"].type())
            {
                ((CDVJsonParamMMI*)pDVJsonBase)->ClearMMI = (int)Data["ClearMMI"].asBool();
            }
            else if(Json::intValue == Data["ClearMMI"].type())
            {
                ((CDVJsonParamMMI*)pDVJsonBase)->ClearMMI = Data["ClearMMI"].asInt();
            }

            //非清屏指令
            if(0 == ((CDVJsonParamMMI*)pDVJsonBase)->ClearMMI)
            {
                //获取设置项内容
                if(Json::nullValue == Data["ItemData"].type())
                    throw -4;
                ItemData = Data["ItemData"];

                //获取文字显示项
                if(Json::nullValue == ItemData["Text"].type())
                    throw -4;
                aryTextView = ItemData["Text"];
                for (int i=0; i < aryTextView.size();++i)
                {
                    tagMMIItemText sText = {0};
                    if(Json::intValue == aryTextView[i]["Type"].type())
                    {
                        sText.Type = aryTextView[i]["Type"].asInt();
                    }
                    if(Json::intValue == aryTextView[i]["Sub"].type())
                    {
                        sText.Sub = aryTextView[i]["Sub"].asInt();
                    }
                    if(Json::intValue == aryTextView[i]["Row"].type())
                    {
                        sText.Row = aryTextView[i]["Row"].asInt();
                    }
                    if(Json::intValue == aryTextView[i]["IsScroll"].type())
                    {
                        sText.IsScroll = aryTextView[i]["IsScroll"].asBool();
                    }
                    if(Json::stringValue == aryTextView[i]["Context"].type())
                    {
                        QString tmpstr = QString(aryTextView[i]["Context"].asCString());
                        memcpy(sText.Context, tmpstr.toStdString().c_str()  ,tmpstr.toStdString().length());

                        //    _tcscpy_s(sText.Context,_countof(sText.Context),aryTextView[i]["Context"].asCString());
                    }
                    //判断数据是否合法
                    if(sText.Sub>0)
                    {
                        ((CDVJsonParamMMI*)pDVJsonBase)->Text.append(sText);
                    }

                }

                //获取声音显示项
                if(Json::nullValue == ItemData["Voice"].type())
                    throw -4;
                aryVoiceView = root["Voice"];
                for (int i=0; i < aryVoiceView.size();++i)
                {
                    tagMMIItemVoice sVoice = {0};
                    if(Json::intValue == aryVoiceView[i]["Type"].type())
                    {
                        sVoice.Type = aryVoiceView[i]["Type"].asInt();
                    }
                    if(Json::intValue == aryVoiceView[i]["No"].type())
                    {
                        sVoice.No = aryVoiceView[i]["No"].asInt();
                    }

                    ((CDVJsonParamMMI*)pDVJsonBase)->Voice.append(sVoice);
                }

                //获取图形显示项
                if(Json::nullValue == ItemData["Voice"].type())
                    throw -4;
                aryGraphView = root["Voice"];
                for (int i=0; i < aryGraphView.size();++i)
                {
                    tagMMIItemGraph sGraph = {0};
                    if(Json::intValue == aryGraphView[i]["Type"].type())
                    {
                        sGraph.Type = aryGraphView[i]["Type"].asInt();
                    }
                    if(Json::intValue == aryGraphView[i]["No"].type())
                    {
                        sGraph.No = aryGraphView[i]["No"].asInt();
                    }

                    ((CDVJsonParamMMI*)pDVJsonBase)->Graph.append(sGraph);
                }


            }
        }
        catch(int& iErrorCode)
        {
            switch(iErrorCode)
            {
            case -1:
                strlog = "";
                break;
            case -2:
                break;
            case -3:
                break;
            case -4:
                break;
            case -5:
                break;
            }
        }

    }
        break;

    }//begin with switch
    return pDVJsonBase;
}


