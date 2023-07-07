#include "cdvxmldebitparamcontext.h"
#include <QDebug>
#include"twsdnetpay.h"

CDVXMLDebitParamContext::CDVXMLDebitParamContext(void)
    :OverTime(0)
    ,OperationMode(0)

    ,PayIdentifier("")
    ,PayCertificateType(0)
    ,PayCertificateCode("")
    ,Money(0)
    ,DebitMainType(0)

    ,VehicleType(0)
    ,VehicleClass(0)
    ,VLColor(0)
    ,VehicleLicense("")
    ,AxisCount(0)
    ,Weight(0)
    ,LimitWeight(0)

    ,PassCertificateType(0)
    ,CardID("")
    ,TollDate(0)
    ,ShiftID(0)
    ,TicketNo("")

    ,EntryRoadID(0)
    ,EntryAreaID(0)
    ,EntryStationID(0)
    ,EntryStationName("")
    ,EntryTime("")
    ,EntryLane(0)
    ,EntryVehicleLicense("")
    ,EntryVLColor(0)
    ,EntryVehicleType(0)
    ,EntryVehicleClass(0)
    ,OperatorName("")
    ,OperatorID("")
{

}


CDVXMLDebitParamContext::~CDVXMLDebitParamContext(void)
{
}



bool CDVXMLDebitParamContext::Init( const QString& strXMLString)
{
    //GetGlobalApp()->WriteLogFile("into CDVXMLDebitParamContext::Init:");
    bool bReturn = false;
    try
    {
        m_domDoc.setContent(strXMLString, false,&m_error,&m_ReRow,&m_ReColumn);
        if(m_domDoc.isNull())
            throw -1;

        /****************************************************************数据格式判断****************************************************************/
        ResetChildPos();
        if(!FindChildElem("Version"))
            throw -2;
        XMLVersion = GetChildElem();

        /**************************************************************************/
        if(!FindChildElem("Data"))
            throw -3;

        if(!FindTwoChildElem("OverTime"))
            throw -4;
        OverTime = GetTwoChildElem().toInt();

        if(!FindTwoChildElem("OperationMode"))
            throw -5;
        OperationMode = GetTwoChildElem().toInt();

        /*****************Debit详细项************************/

        if(!FindTwoChildElem("Debit"))
            throw -6;

        if(!FindThreeChildElem(("PayIdentifier")))
            throw -7;
        PayIdentifier =  GetThreeChildElem();

        if(!FindThreeChildElem("Money"))
            throw -8;
        Money = GetThreeChildElem().toInt();

        if(!FindThreeChildElem(("SubTime")))
            throw -9;
        SubTime = GetThreeChildElem();

        if(!FindThreeChildElem("Type"))
            throw -10;
        DebitMainType = GetThreeChildElem().toInt();

        /*****************Vehicle详细项************************/

        if(!FindTwoChildElem("Vehicle"))
            throw -11;

        if(!FindThreeChildElem("Type"))
            throw -12;
        VehicleType =  GetThreeChildElem().toInt();


        if(!FindThreeChildElem("Class"))
            throw -13;
        VehicleClass =  GetThreeChildElem().toInt();


        if(!FindThreeChildElem("License"))
            throw -14;
        VehicleLicense =  GetThreeChildElem();


        if(!FindThreeChildElem("VLColor"))
            throw -15;
        VLColor =  GetThreeChildElem().toInt();


        if(!FindThreeChildElem("AxisCount"))
            throw -32;
        AxisCount = GetThreeChildElem().toInt();


        if(!FindThreeChildElem("Weight"))
            throw -33;
        Weight =  GetThreeChildElem().toInt();


        if(FindThreeChildElem("LimitWeight"))
        {
            LimitWeight =  GetThreeChildElem().toInt();
        }


        /*****************Operation详细项************************/

        if(!FindTwoChildElem("Operation"))
            throw -16;

        if(!FindThreeChildElem("PassCertificateType"))
            throw -17;
        PassCertificateType =  GetThreeChildElem().toInt();

        if(!FindThreeChildElem("CardID"))
            throw -18;
        CardID =  GetThreeChildElem();

        if(!FindThreeChildElem("TollDate"))
            throw -19;
        TollDate =  GetThreeChildElem().toInt();

        //    if(!FindThreeChildElem("ShiftID"))
        //           throw -20;
        if(FindThreeChildElem("ShiftID"))
            ShiftID =  GetThreeChildElem().toInt();

        if(FindThreeChildElem("TicketNo"))
        {
            TicketNo =  GetThreeChildElem();
        }

        if(FindThreeChildElem("OperatorID"))
        {
            OperatorID =  GetThreeChildElem();
        }

        if(FindThreeChildElem("OperatorName"))
        {
            OperatorName =  GetThreeChildElem();
        }


        /*****************Entry详细项************************/

        if(!FindTwoChildElem(("Entry")))
            throw -21;

        if(FindThreeChildElem("StationID"))
            str_EntryStationID = GetThreeChildElem();

        if(FindThreeChildElem("StationName"))
            EntryStationName = GetThreeChildElem();

        if(!FindThreeChildElem("Time"))
            throw -24;
        EntryTime = GetThreeChildElem();

        if(!FindThreeChildElem("LaneID"))
            throw -25;
        EntryLane =  GetThreeChildElem().toInt();


        if(!FindThreeChildElem("License"))
            throw -26;
        EntryVehicleLicense = GetThreeChildElem();


        if(!FindThreeChildElem("VColor"))
            throw -27;
        EntryVLColor =  GetThreeChildElem().toInt();


        if(!FindThreeChildElem("VClass"))
            throw -28;
        EntryVehicleClass =  GetThreeChildElem().toInt();


        if(!FindThreeChildElem("VType"))
            throw -29;
        EntryVehicleType =  GetThreeChildElem().toInt();


        if(!FindThreeChildElem("RoadID"))
            throw -34;
        EntryRoadID =   GetThreeChildElem().toInt();


        if(!FindThreeChildElem("AreaID"))
            throw -35;
        EntryAreaID =    GetThreeChildElem().toInt();

        /*****************操作员信息** 江苏版本**********************/
        if(FindTwoChildElem(("Paystation")))
        {
            if(FindThreeChildElem("ExitOperatorId"))
                OperatorID =  GetThreeChildElem();

            if(FindThreeChildElem("ExitOperatorName"))
                OperatorName =  GetThreeChildElem();

            if(FindThreeChildElem("ExitShift"))
                ShiftID =  GetThreeChildElem().toInt();
        }


        /****************************************************************数据逻辑判断****************************************************************/
        if(OperationMode>2 || OperationMode<1)
            throw -30;

        if(PayIdentifier.isEmpty())
            throw -31;

        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据头 DebitParamContext";
            break;
        case -2:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据版本 Version";
            break;
        case -3:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据区 Data";
            break;
        case -4:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(超时时间) OverTime";
            break;
        case -5:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(扣款模式) OperationMode";
            break;
        case -6:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据区 Debit";
            break;
        case -7:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(交易识别码) PayIdentifier";
            break;
        case -8:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(扣款金额) Money";
            break;
        case -9:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(递交时间) SubTime";
            break;
        case -10:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(扣款类型) Type";
            break;
        case -11:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据区 Vehicle";
            break;
        case -12:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(车辆种类-客货分类) Type";
            break;
        case -13:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(车辆类型-国标分类) Class";
            break;
        case -14:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(车辆牌照) License";
            break;
        case -15:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(车牌颜色) VLColor";
            break;
        case -16:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据区 Operation";
            break;
        case -17:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(通行凭证类型) PassCertificateType";
            break;
        case -18:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(通行凭证编码) CardID";
            break;
        case -19:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(统计日期) TollDate";
            break;
        case -20:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(统计班次) ShiftID";
            break;
        case -21:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据区 Entry";
            break;
        case -22:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(入口站点编码) StationID";
            break;
        case -23:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(入口站点名称) StationName";
            break;
        case -24:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(入口时间) Time";
            break;
        case -25:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(入口车道编码) LaneID";
            break;
        case -26:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(入口车牌) License";
            break;
        case -27:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(入口车牌颜色) VColor";
            break;
        case -28:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(入口车型) VClass";
            break;
        case -29:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(入口车种) VType";
            break;
        case -30:
            m_strLastErrorDesc = "扣款传入参数解析失败,业务操作码错误，不在范围内 PayIdentifier";
            break;
        case -31:
            m_strLastErrorDesc = "扣款传入参数解析失败,二维码为空 PayIdentifier";
            break;
        case -32:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(车辆轴数) AxisCount";
            break;
        case -33:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(车辆总重) Weight";
            break;
        case -34:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(区域编码) AreaID";
            break;
        case -35:
            m_strLastErrorDesc = "扣款传入参数解析失败,无法找到指定数据项(路段编码) RoadID";
            break;
        }

        GetGlobalApp()->WriteLogFile(m_strLastErrorDesc);

    }

    return bReturn;
}



