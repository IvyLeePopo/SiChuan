#include "cdvjsoncommanddebitmoney.h"

CDVJsonCommandDebitMoney::CDVJsonCommandDebitMoney()
{

}



/****************************************
 *函数名称：fnAnalysisStringForData
 *函数描述：支付数据解析
 *函数参数：QString
 *函数返回：bool
*****************************************/
bool  CDVJsonCommandDebitMoney::fnAnalysisStringForData(const QString& strResponseResult)
{

//    qDebug()<<"into fnAnalysisStringForData";

   bool breturn = false;

   QString strlog;

       try
       {

           //进行数据格式转换
           Json::Reader reader;
           Json::Value  root;
           Json::Value  DebitParamContext;
           Json::Value  Data;
           Json::Value  Debit;
           Json::Value  Vehicle;
           Json::Value  Operation;
           Json::Value	Entry;

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
           // m_OverTime = Data["OverTime"].asInt();

           //20170414 縮短3秒
           m_OverTime = abs(Data["OverTime"].asInt() - 3000);

           //获取扣款操作模式
           if(Json::intValue != Data["OperationMode"].type())
               throw -7;
            m_OperationMode = Data["OperationMode"].asInt();

           //获取扣款信息区
           if(Json::nullValue == Data["Debit"].type())
               throw -8;
           Debit = Data["Debit"];


           //获取扣款金额
           if(Json::intValue != Debit["Money"].type())
               throw -9;
           m_Money = Debit["Money"].asInt();

           //获取交易识别码
           if(Json::stringValue != Debit["PayIdentifier"].type())
               throw -10;
           m_PayIdentifier = Debit["PayIdentifier"].asCString();

           //获取扣款主分类
           if(Json::intValue != Debit["Type"].type())
               throw -13;
           m_DeType = Debit["Type"].asInt();

           //获取过车时间
           if(Json::stringValue != Debit["SubTime"].type())
               throw -14;
            m_SubTime = Debit["SubTime"].asCString();

           //获取车辆信息区
           if(Json::nullValue == Data["Vehicle"].type())
               throw -15;
             Vehicle = Data["Vehicle"];

           //获取车种
           if(Json::intValue != Vehicle["Type"].type())
               throw -16;
             m_VehicleType = Vehicle["Type"].asInt();

           //获取车型
           if(Json::intValue != Vehicle["Class"].type())
               throw -17;
            m_VehicleClass = Vehicle["Class"].asInt();

           //获取车牌
           if(Json::stringValue != Vehicle["License"].type())
               throw -18;
            m_VehicleLicense = Vehicle["License"].asCString();

           //获取车牌颜色
           if(Json::intValue != Vehicle["VLColor"].type())
               throw -19;
            m_VehicleColor = Vehicle["VLColor"].asInt();

           //获取轴数
           if(Json::intValue != Vehicle["AxisCount"].type())
               throw -20;
            m_VehicleAxisCount = Vehicle["AxisCount"].asInt();

           //获取装载重量
           if(Json::intValue != Vehicle["Weight"].type())
               throw -21;
            m_VehicleWeight = Vehicle["Weight"].asInt();

           //获取限重
//           if(Json::intValue == Vehicle["LimitWeight"].type())
//           {
//             m_VehicleWeight = Vehicle["LimitWeight"].asInt();
//           }

           //获取交易信息区
           if(Json::nullValue == Data["Operation"].type())
               throw -22;
           Operation = Data["Operation"];


           //获取通行凭证类型
           if(Json::intValue != Operation["PassCertificateType"].type())
               throw -23;
            m_PassCertificateType = Operation["PassCertificateType"].asInt();

            // OperatorID and OperatorName
            if(Json::stringValue == Operation["OperatorID"].type())
                m_OperatorID = Operation["OperatorID"].asCString();
            else
                m_OperatorID = "";

            if(Json::stringValue == Operation["OperatorName"].type())
                m_OperatorName = Operation["OperatorName"].asCString();
            else
                m_OperatorName = "";

           //获取通行凭证卡号
           if(Json::stringValue != Operation["CardID"].type())
               throw -24;
            m_CardID = Operation["CardID"].asCString();

           //获取统计日
           if(Json::intValue == Operation["TollDate"].type())
           {
            m_TollDate = Operation["TollDate"].asInt();
           }


           //获取统计班次
           if(Json::intValue != Operation["ShiftID"].type())
               throw -26;
            m_ShiftID = Operation["ShiftID"].asInt();


           //获取入口信息区
           if(Json::nullValue == Data["Entry"].type())
               throw -27;
           Entry = Data["Entry"];

           //获取入口区域编码
           if(Json::intValue != Entry["AreaID"].type())
               throw -28;
            m_EntryAreaID = Entry["AreaID"].asInt();

           //获取入口路段编码
           if(Json::intValue != Entry["RoadID"].type())
               throw -29;
            m_EntryRoadID = Entry["RoadID"].asInt();

           //获取入口站点编码
           if(Json::stringValue != Entry["StationID"].type())
               throw -30;

            m_EntryStationID = Entry["StationID"].asCString();

           //获取入口站点名称
           if(Json::stringValue != Entry["StationName"].type())
               throw -31;
            m_EntryStationName = Entry["StationName"].asCString();

           //获取入口车道编码
           if(Json::intValue != Entry["LaneID"].type())
               throw -32;
            m_EntryLaneID = Entry["LaneID"].asInt();

           //获取入口车种
           if(Json::intValue != Entry["VType"].type())
               throw -33;
            m_EntryType = Entry["VType"].asInt();

           //获取入口车型
           if(Json::intValue != Entry["VClass"].type())
               throw -34;
             m_EntryClass = Entry["VClass"].asInt();

           //获取入口车牌
           if(Json::stringValue != Entry["License"].type())
               throw -35;
           m_EntryLicense = Entry["License"].asCString();

           //获取入口车牌颜色
           if(Json::intValue != Entry["VColor"].type())
               throw -36;
            m_EntryColor = Entry["VColor"].asInt();

           //获取入口时间
           if(Json::stringValue != Entry["Time"].type())
               throw -37;
           m_EntryTime = Entry["Time"].asCString();

           //数据正常，启动扫码支付
           qDebug()<<"Start_GetFeeID():";

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
           case -11:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： PayCertificateType";     break;
           case -12:            strlog = "[业务线程],JSON_DT_PARAM_DEBIT_MONEY数据解析,异常找不到： PayCertificateCode";     break;
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
           qDebug()<<strlog;
           qDebug()<<"JSON_DT_PARAM_DEBIT_MONEY异常2: "<<iErrorCode;
       }


   return breturn;

}




/****************************************
 *函数名称：FormatParamData2Json()
 *函数描述：构造支付数据
 *函数参数：void
 *函数返回：QString
*****************************************/
QString CDVJsonCommandDebitMoney::FormatParamData2Json()
{
    QString strReturn;
    try
    {
        // 表示整个 json 对象
        Json::Value root;
        // 标识用户参数
        Json::Value DebitParamContext;
        // 标识详细数据项
        Json::Value Data;
        // 标识扣费数据项
        Json::Value Debit;
        // 标识车辆数据项
        Json::Value Vehicle;
        // 标识业务数据项
        Json::Value Operation;
        // 标识入口数据项
        Json::Value Entry;

        Json::FastWriter t_RootDocument;

        //标识版本
        m_Version = "1.0";
        DebitParamContext["Version"]=std::string(m_Version.toLocal8Bit());

        Data["OverTime"]=m_OverTime;
        Data["OperationMode"]=m_OperationMode;

        Debit["PayIdentifier"]=std::string(m_PayIdentifier.toLocal8Bit());
        Debit["Money"]=m_Money;
        Debit["SubTime"]=std::string(m_SubTime.toLocal8Bit());
        Debit["Type"]=m_DeType;
        //Debit["PayCertificateCode",pDVJsonParamDebitMoney->PayCertificateCode);
        //Debit["PayCertificateType",pDVJsonParamDebitMoney->PayCertificateType);

        Vehicle["Type"]=m_VehicleType;
        Vehicle["Class"]=m_VehicleClass;
        Vehicle["License"]=std::string(m_VehicleLicense.toLocal8Bit());
        Vehicle["VLColor"]=m_VehicleColor;
        Vehicle["AxisCount"]=m_VehicleAxisCount;
        Vehicle["Weight"]=m_VehicleWeight;
        Vehicle["limitWeight"]=m_VehicleWeight;

        Operation["PassCertificateType"]=m_PassCertificateType;
        Operation["CardID"]=std::string(m_CardID.toLocal8Bit());

        // OperatorID & OperatorName for shaxi province
        Operation["OperatorID"] = m_OperatorID.toStdString();
        Operation["OperatorName"] = m_OperatorName.toStdString();

        Operation["TollDate"]=m_TollDate;
        Operation["ShiftID"]=m_ShiftID;
        Operation["TicketNo"]=std::string(QString::number(QDateTime::currentDateTime().toTime_t()).toLocal8Bit());


        Entry["AreaID"]=m_EntryAreaID;
        Entry["RoadID"]=m_EntryRoadID;
        Entry["StationID"]=m_EntryStationID.toStdString();
        Entry["StationName"]=std::string(m_EntryStationName.toLocal8Bit());
        Entry["Time"]=std::string(m_EntryTime.toLocal8Bit());
        Entry["LaneID"]=m_EntryLaneID;
        Entry["License"]=std::string(m_EntryEnLicense.toLocal8Bit());
        Entry["VColor"]=m_EntryColor;
        Entry["VType"]=m_EntryType;
        Entry["VClass"]=m_EntryClass;

        Data["Debit"]=Debit;
        Data["Vehicle"]=Vehicle;
        Data["Operation"]=Operation;
        Data["Entry"]=Entry;

        DebitParamContext["Data"]=Data;
        root["DebitParamContext"]=DebitParamContext;

        //转化为字符串
      //  strReturn = QString::fromStdString(t_RootDocument.write(root)).toUtf8();
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

//    qDebug() << "\nCDVJsonCommandDebitMoney::FormatParamData2Json: " << strReturn;

    return strReturn;
}


