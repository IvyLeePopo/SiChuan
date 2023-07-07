#include "chtsddevicedll.h"
#include "twsdnetpay.h"

CHTSDDeviceDll::CHTSDDeviceDll(QObject *parent) :
    QObject(parent)
  ,m_hModule(NULL)
  ,m_bInitDll(false)
  ,m_bInitDev(false)
  ,m_strLastError("")
  ,fnIF_InitDev(NULL)
  ,fnIF_Destroy(NULL)
  ,fnIF_GetLastErrorDesc(NULL)
  ,fnIF_QueryDevice(NULL)
  ,fnIF_SetDevice(NULL)
  ,fnIF_DebitMoney(NULL)
  ,fnIF_DebitCancel(NULL)
  ,fnIF_InitDeviecCMD(NULL)
  ,fnIF_CheckDeviceState(NULL)
{
}

CHTSDDeviceDll::~CHTSDDeviceDll(void)
{

    if(NULL != m_hModule)
    {
        delete m_hModule;
        m_hModule = NULL;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSDNetPayDll::Init
//
// 功能描述：初始化动态库
//
// 输入参数：IN const CString& strDllFilePath
// 输出参数：const
// 编写人员：
// 编写时间：2016年7月23日
// 修改人员 : FFFF
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CHTSDDeviceDll::Init( const QString& strDllFilePath)
{
    try{
        if(NULL == m_hModule)
        {
            m_hModule = new QLibrary(strDllFilePath);
        }

        if( m_hModule->load())
        {
            fnIF_InitDev				= (defIF_InitDev)m_hModule->resolve("IF_InitDev");
            fnIF_Destroy				= (defIF_Destroy)m_hModule->resolve("IF_Destroy");
            fnIF_GetLastErrorDesc		= (defIF_GetLastErrorDesc)m_hModule->resolve("IF_GetLastError");

            fnIF_QueryDevice			= (defIF_QueryDevice)m_hModule->resolve("IF_QueryDevice");
            fnIF_SetDevice				= (defIF_SetDevice)m_hModule->resolve("IF_SetDevice");
            fnIF_DebitMoney				= (defIF_StartDebit)m_hModule->resolve("IF_StartDebit");
            fnIF_DebitCancel			= (defIF_StartDebitCancle)m_hModule->resolve("IF_StartDebitCancle");
            fnIF_InitDeviecCMD          = (defIF_InitPara)m_hModule->resolve("IF_InitPara");

            fnIF_BreakDebit          = (defIF_BreakDebit)m_hModule->resolve("IF_BreakDebit");
            fnIF_CheckDeviceState = (defIF_CheckDeviceStatus) m_hModule->resolve("IF_CheckDeviceStatus");

            if((NULL != fnIF_InitDev)
                    && (NULL != fnIF_Destroy)
                    && (NULL != fnIF_GetLastErrorDesc)

                    && (NULL != fnIF_QueryDevice)
                    && (NULL != fnIF_SetDevice)
                    && (NULL != fnIF_DebitMoney)
                    && (NULL != fnIF_DebitCancel)
                    && (NULL != fnIF_InitDeviecCMD)

                    && (NULL != fnIF_BreakDebit)
                    &&(NULL != fnIF_CheckDeviceState)
                    )
            {
                m_bInitDll = true;
                GetGlobalApp()->WriteLogFile(QString("[硬件设备]：通讯动态库加载成功"));
            }
        }
        else
        {
            GetGlobalApp()->WriteLogFile(QString("[硬件设备]：通讯动态库加载失败: %1").arg(m_hModule->errorString()));
        }

    }
    catch(int& nError)
    {
        switch(nError)
        {
        case -1:
            GetGlobalApp()->WriteLogFile("[硬件设备]：通讯动态库内部资源错误，原因：资源申请失败");
            break;
        }
    }

    //返回执行状态
    return m_bInitDll;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CHTSDDeviceDll::InitDev
//
// 功能描述：组件初始化
//
// 输入参数：strPortName:串口名称
//                      baudRate:波特率
//                      参数3，回调指针
// 输出参数：BOOL
// 编写人员：FFFF
// 编写时间：2017年9月28日
// 修改人员 :
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CHTSDDeviceDll::InitDev(const std::string& strPortName, const int& baudRate ,void (*NotifyFun)(int type,const QByteArray& sendStr))
{
    if(m_bInitDll)
    {
        m_bInitDev = fnIF_InitDev(strPortName, baudRate, NotifyFun);
        return m_bInitDev;
    }
    return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::Destroy
//
// 功能描述：清理资源
//
// 输入参数：void
// 输出参数：BOOL
// 编写人员：
// 编写时间：2017年5月3日
// 修改人员 : FFFF
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CHTSDDeviceDll::Destroy(void)
{
    if(m_bInitDll)
        return fnIF_Destroy();
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::GetLastErrorDesc
//
// 功能描述：获取上一次错误信息
//
// 输入参数：void
// 输出参数：std::string strLastDesc  错误描述
// 编写人员：
// 编写时间：2017年5月3日
// 修改人员 : FFFF
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CHTSDDeviceDll::GetLastErrorDesc(  std::string& strLastDesc)
{
    if(m_bInitDll)
    {
        if(fnIF_GetLastErrorDesc(strLastDesc))
        {
            return true;
        }
    }

    return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::CheckDeviceStatus
//
// 功能描述：执行检测硬件状态操作
//
// 输入参数：IN OUT DWORD& dwState
// 输出参数：BOOL
// 编写人员：
// 编写时间：2017年5月3日
// 修改人员 : FFFF
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CHTSDDeviceDll::CheckDeviceStatus(  unsigned int & dwState)
{
    if(m_bInitDll && m_bInitDev)
    {
        return fnIF_CheckDeviceState(dwState);
    }

    return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::DebitMoney
//
// 功能描述：执行扣款操作
//
// 输入参数：CString& strDebitInfo
//         ：int iType
// 输出参数：BOOL
// 编写人员：
// 编写时间：2017年5月3日
// 修改人员 :FFFF
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CHTSDDeviceDll::DebitMoney( const QString& strDebitInfo,  QString& strResultInfo, const int& inType)
{
    bool bRet = false;
    if(m_bInitDll && m_bInitDev)
    {
        QByteArray  strDebit  = DebitJsonToBlock(strDebitInfo);
        std::string  tmpstrResultInfo;
        bRet =  fnIF_DebitMoney(strDebit,inType,tmpstrResultInfo);
        strResultInfo = QString(tmpstrResultInfo.c_str());
    }
    return bRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::DebitCancel
//
// 功能描述：执行退款操作
//
// 输入参数：IN const CString& strCancelInfo
//         ：IN const int& inType
// 输出参数：BOOL
// 编写人员：
// 编写时间：2017年5月3日
// 修改人员 :FFFF
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CHTSDDeviceDll::DebitCancel( const QString& strCancelInfo, const int& inType)
{
    if(m_bInitDll && m_bInitDev)
    {
        QString strCancel  = strCancelInfo;
        return fnIF_DebitCancel(strCancel.toStdString(),inType);
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::InitConfigDevice
//
// 功能描述：执行设备初始化操作
//
// 输入参数：IN const CString& strInitInfo
//         ：IN const int& inType
// 输出参数：BOOL
// 编写人员：
// 编写时间：2017年5月3日
// 修改人员 :FFFF
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CHTSDDeviceDll::InitConfigDevice( const QString& strInitInfo, const int& inType)
{
    if(m_bInitDll && m_bInitDev)
    {
        std::string strInit  = strInitInfo.toStdString();
        return fnIF_InitDeviecCMD(strInit,inType);
    }
    return false;
}


bool CHTSDDeviceDll::SetDevice( const QString& para, int nType)
{
    if(m_bInitDll && m_bInitDev)
    {
        std::string strInit  = para.toStdString();
        return fnIF_SetDevice(strInit,nType);
    }
    return false;
}


bool CHTSDDeviceDll::BreakDebit(const QString& payID)
{
    if(m_bInitDll && m_bInitDev)
    {
        std::string strPayid = payID.toStdString();
        return fnIF_BreakDebit(strPayid);
    }
    return false;
}






QByteArray CHTSDDeviceDll::DebitJsonToBlock(const QString &strData)
{
    QString strLog;
        QByteArray TotalArray;
        TotalArray.clear();

        QTextCodec *gbk = QTextCodec::codecForName("GBK");

        Json::Reader reader;
        Json::Value root;
        Json::Value DebitParamContext;
        Json::Value Data;
        Json::Value Debit;
        Json::Value Entry;
        Json::Value Operation;
        Json::Value Vehicle;

        if(!reader.parse(strData.toLocal8Bit().data(),root))
        {
            strLog = "解析业务组件传入扣款Json出错";
            GetGlobalApp()->WriteLogFile(strLog);

            return TotalArray;
        }

        DebitParamContext = root["DebitParamContext"];
        Data = DebitParamContext["Data"];
        Debit = Data["Debit"];
        Entry = Data["Entry"];
        Operation = Data["Operation"];
        Vehicle = Data["Vehicle"];

        QString qPayId = Debit["PayIdentifier"].asCString();
        strLog = QObject::tr("交易识别码: %1").arg(qPayId);
        GetGlobalApp()->WriteLogFile(strLog);

        QByteArray PayIdArray = qPayId.toLocal8Bit();
        int len = PayIdArray.length();
        if(len < 40)
        {
            for(int i=0;i<(40-len);i++)
                PayIdArray[len+i] = 0x00;
        }
        TotalArray.append(PayIdArray);

        int iMoney = Debit["Money"].asInt();
        strLog = QObject::tr("金额: %1").arg(iMoney);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray MoneyArray = Int4Array(iMoney);
        TotalArray.append(MoneyArray);

        int iClassAndType;
        int iType = Vehicle["Class"].asInt();  ////西公所把两个变量弄反了
        int iClass= Vehicle["Type"].asInt();
        if(0 == iClass)
            iClassAndType = iType;
        else
            iClassAndType = iType+10;
        strLog = QObject::tr("车种车型: %1").arg(iClassAndType);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray ClassTypeArray;
        ClassTypeArray[0] = (uchar)iClassAndType;
        TotalArray.append(ClassTypeArray);

        int iCarColor  = Vehicle["VLColor"].asInt();
        strLog = QObject::tr("车牌颜色: %1").arg(iCarColor);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray ColorArray ;
        ColorArray[0] = (uchar)iCarColor;
        TotalArray.append(ColorArray);

        QString qCarPlate  = Vehicle["License"].asCString();
        strLog = QObject::tr("车牌: %1").arg(qCarPlate);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray CarPlateArray = gbk->fromUnicode(qCarPlate);
        len = CarPlateArray.length();
        if(len < 16)
        {
            for(int i=0;i<(16-len);i++)
                CarPlateArray[len+i] = 0x00;
        }
        TotalArray.append(CarPlateArray);

        QString qStationId   = Entry["StationID"].asCString();
        strLog = QObject::tr("入口站点编码: %1").arg(qStationId);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray StationIdArray =  qStationId.toLocal8Bit();
        len = StationIdArray.length();
        if(len < 20)
        {
            for(int i=0;i<(20-len);i++)
                StationIdArray[len+i] = 0x00;
        }
        TotalArray.append(StationIdArray);

        QString qEntryName=Entry["StationName"] .asCString();
        strLog = QObject::tr("入口站点名称: %1").arg(qEntryName);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray EntryNameArray = gbk->fromUnicode(qEntryName);
        len = EntryNameArray.length();
        if(len < 24)
        {
            for(int i=0;i<(24-len);i++)
                EntryNameArray[len+i] = 0x00;
        }
        TotalArray.append(EntryNameArray);

        QString qEntryTime   = Entry["Time"].asCString();
        strLog = QObject::tr("入口时间: %1").arg(qEntryTime);
        GetGlobalApp()->WriteLogFile(strLog);
        int iEntryTime;
        QDateTime dateTime = QDateTime::fromString(qEntryTime,"yyyy-MM-dd hh:mm:ss");
        iEntryTime = QDateTime(dateTime).toTime_t();
        QByteArray EntryTimeArray = Int4Array(iEntryTime);
        TotalArray.append(EntryTimeArray);

        int iTollData  = Operation["TollDate"].asInt();
        strLog = QObject::tr("统计日期: %1").arg(iTollData);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray TollDateArray = Int4Array(iTollData);
        TotalArray.append(TollDateArray);

        int iShiftId  = Operation["ShiftID"].asInt();
        strLog = QObject::tr("统计班次: %1").arg(iShiftId);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray ShiftIdArray;
        ShiftIdArray[0] = (uchar)iShiftId ;
        TotalArray.append(ShiftIdArray);

        QString qCardId  = Operation["CardID"].asCString();
        len  = qCardId.toLocal8Bit().length();

        strLog = QObject::tr("卡号长度: %1").arg(len);
        GetGlobalApp()->WriteLogFile(strLog);
        strLog = QObject::tr("通行卡号: %1").arg(qCardId);
        GetGlobalApp()->WriteLogFile(strLog);

        QByteArray IdLenArray;
        IdLenArray[0] = uchar(len);
        TotalArray.append(IdLenArray);

        QByteArray CardIdArray = StrCopyToHex(qCardId);
        len = CardIdArray.length();

        QByteArray _CardIdArray;
        if(len < 10)
        {
            for(int i=0;i<(10-len);i++)
                _CardIdArray[i] = 0x00;
            _CardIdArray.append(CardIdArray);
        }
        else
            _CardIdArray = CardIdArray.mid(0,10);

        TotalArray.append(_CardIdArray);

        int iPassStationNum = 0;
        strLog = QObject::tr("经过标识站个数: %1").arg(iPassStationNum);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray PassNumArray;
        PassNumArray[0] = (uchar)iPassStationNum;
        TotalArray.append(PassNumArray);

        int iAxis  = Vehicle["AxisCount"].asInt();
        strLog = QObject::tr("轴数: %1").arg(iAxis);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray AxisArray;
        if(iAxis < 256)
        {
            AxisArray[0]=0x00;AxisArray[1]=(uchar)iAxis;
        }
        else
        {
            AxisArray[0]=iAxis/256;AxisArray[1]=iAxis%256;
        }
        TotalArray.append(AxisArray);

        int iWeight = Vehicle["Weight"].asInt();
        strLog = QObject::tr("重量: %1").arg(iWeight);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray WeightArray = Int4Array(iWeight);
        TotalArray.append(WeightArray);

        QString qCashierId = Operation["OperatorID"].asCString();
        strLog = QObject::tr("收费员工号: %1").arg(qCashierId);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray CashierIdArray = qCashierId.toLocal8Bit();
        len = CashierIdArray.length();
        if(len < 20)
        {
            for(int i=0;i<20-len;i++)
                CashierIdArray[len+i]=0x00;
        }
        else
            CashierIdArray = CashierIdArray.mid(0,20);
        TotalArray.append(CashierIdArray);

        QString qCashierName = Operation["OperatorName"].asCString();
        strLog = QObject::tr("收费员姓名: %1").arg(qCashierName);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray CashierNameArray = gbk->fromUnicode(qCashierName);
        len = CashierNameArray.length();
        if(len < 10)
        {
            for(int i=0;i<10-len;i++)
                CashierNameArray[len+i]=0x0;
        }
        else
            CashierNameArray = CashierNameArray.mid(0,10);
        TotalArray.append(CashierNameArray);

        int iBalance = 0;
        strLog = QObject::tr("支付卡余额: %1").arg(iBalance);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray BalanceArray = Int4Array(iBalance);
        TotalArray.append(BalanceArray);

        int iChargeMode = Operation["PassCertificateType"].asInt();
        strLog = QObject::tr("扣费模式: %1").arg(iChargeMode);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray ChargeModeArray;
        ChargeModeArray[0] = (uchar)iChargeMode;
        TotalArray.append(ChargeModeArray);

        int iOverTime = (Data["OverTime"].asInt()) /1000;

        strLog = QObject::tr("扣款超时: %1").arg(iOverTime);
        GetGlobalApp()->WriteLogFile(strLog);
        QByteArray OverTimeArray;
        OverTimeArray[0] = (uchar)(iOverTime);
        TotalArray.append(OverTimeArray);

        QString qSubTime  = Debit["SubTime"].asCString();
        strLog = QObject::tr("出口时间: %1").arg(qSubTime);
        GetGlobalApp()->WriteLogFile(strLog);
        int iSubTime;
        QDateTime SubDateTime = QDateTime::fromString(qSubTime,"yyyy-MM-dd hh:mm:ss");
        iSubTime = QDateTime(SubDateTime).toTime_t();
        QByteArray SubTimeArray = Int4Array(iSubTime);
        TotalArray.append(SubTimeArray);

        strLog = QObject::tr("扣款数据块长度: %1").arg(TotalArray.length());
        GetGlobalApp()->WriteLogFile(strLog);

        return TotalArray;
    }

QByteArray CHTSDDeviceDll::StrCopyToHex(const QString &strData)
{
    QString str = strData;
       int len = str.length();
       if(0 != (len%2))
           str = "0"+str;
       QByteArray Array;
       for(int i=0;i<len;i+=2)
       {
           bool ok;
           QString str0 = str.mid(i,2);
           QByteArray array;
           array[0]= (uchar)str0.toInt(&ok,16);
           Array.append(array);
       }
       return Array;
}

QByteArray CHTSDDeviceDll::Int4Array(int m)
{
    QByteArray array;
       if(m <= 0)
       {
           array[0]=0x00; array[1]=0x00;
           array[2]=0x00; array[3]=0x00;
       }
       else if(m < 256)
       {
           array[0]=0x00; array[1]=0x00;
           array[2]=0x00; array[3]=(uchar)m;
       }
       else if(m < 65536)
       {
           array[0]=0x00; array[1]=m/65536;
           array[2]=m/256;array[3]=m%256;
       }
       else
       {
           array[0]=m/16777216;array[1]=m/65536;
           array[2]=m/256;array[3]=m%256;
       }
       return array;
}














