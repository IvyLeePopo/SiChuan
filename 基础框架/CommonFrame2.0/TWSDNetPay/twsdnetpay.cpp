#include "twsdnetpay.h"
#include <qstring.h>
#include <QDebug>

//唯一全局变量
TWSDNetPay theApp;

TWSDNetPay* GetGlobalApp()  { return &theApp; }

/***********************变量初始化*********************************/
CThreadManage*                      TWSDNetPay::m_pThreadManage							= NULL;
bool                                          TWSDNetPay::m_bIsInitEnvironment						= false;
bool                                           TWSDNetPay:: m_bAuthorizedLegal					        = false;
bool                                TWSDNetPay::m_bSaveLog							= false;
quint32                             TWSDNetPay::m_dwThreadCheckSpanTime						= 30;
QString                             TWSDNetPay::m_strWorkDir							= "";
QString                             TWSDNetPay::m_strLastErrorDesc						= "";
QString                             TWSDNetPay::m_strVirtualCardPrefix						= "";
tagTranstParam                      TWSDNetPay::m_sTranstParam							= {0};
bool                                TWSDNetPay::m_bCurrentDebitMode						= false;
bool                                TWSDNetPay::m_bIsTrading							= false;
tagPayKeyItems                      TWSDNetPay::m_sCurrentDebitInfo						= {0};
tagDebitResultItems                 TWSDNetPay::m_sCurrentDebitResult                = {0};
quint32                                   TWSDNetPay::m_dwUpdateDevieTimeSpanTime                                     = 0;
bool                                      TWSDNetPay::m_bIsTestData							= false;
QString                             TWSDNetPay::m_strNetPayDllVersion						= "";
QString                             TWSDNetPay::m_strDeviceDllVersion						= "";
int                             TWSDNetPay::m_iDeleteLogStartHour						= 0;
int                             TWSDNetPay::m_iDeleteLogEndHour						= 0;
int                              TWSDNetPay::m_iDeleteLogSpanTime						= 60;
int                             TWSDNetPay::m_iDeleteLogSaveDays						= 0;
int                             TWSDNetPay::m_iDebitTaskNo							= 0;
QString                             TWSDNetPay::m_JsonVersion                                                   = "";
QString                             TWSDNetPay::m_XMLVersion                                                    = "";
char                                TWSDNetPay::m_PortName[20]                                                  ={0};
int                                             TWSDNetPay::m_BaudRate                                                      = 0;
QString                             TWSDNetPay::m_CurrentServerAddress                                          ="";
int                                   TWSDNetPay::m_CurrentServerPort                                             = 0;
C_logtxt                              TWSDNetPay::m_Logtxt;
int                                      TWSDNetPay::m_iNotifyMode                                                   = 1;
NotifyCallerFun                TWSDNetPay::m_fnCallbackNotifyFun						= NULL;
NotifyCallerFun45         TWSDNetPay::m_fnCallbackNotifyFun45					= NULL;
void*                               TWSDNetPay::m_pIndPtrFor45								= NULL;


TWSDNetPay::TWSDNetPay(void)
{
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    //QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
}

TWSDNetPay::~TWSDNetPay()
{
    if(NULL != m_pThreadManage)
    {
        delete m_pThreadManage;
        m_pThreadManage = NULL;
    }

}

void  TWSDNetPay::run()
{
    ReadConfigInfo();
    //启动业务管理线程
    if(NULL == m_pThreadManage)
    {
        m_pThreadManage = new CThreadManage();
        if(NULL == m_pThreadManage)
        {
            QString strLog = "[主进程响应] :无法启动业务管理线程";
            m_strLastErrorDesc  = strLog;
            GetGlobalApp()->WriteLogFile(m_strLastErrorDesc);
        }
    }

    if(NULL != m_pThreadManage)
    {
        QString strConfigFilePath;
        strConfigFilePath = m_strWorkDir+"\\"+FILE_NAME_APP_CONFIG;
        m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadManage->SetParam(&m_sTranstParam);
        m_pThreadManage->start();

    }

    exec();
}



void TWSDNetPay:: WriteLogFile(QString   strLog)
{
    m_Logtxt.WriteLogFile(strLog);
}



/***************************************************
 * 函数名称：QString2time_t
 * 函数功能：将时间的字符串形式转化为time_t格式
 * 参   数：void
 * 返   回：void
 * *************************************************/
time_t TWSDNetPay::QString2time_t(QString lpstime)
{

    QDateTime t_time;

    t_time = QDateTime::fromString(lpstime, "yyyy-MM-dd hh:mm:ss");

    return t_time.toTime_t();

}



/***************************************************
 * 函数名称：GetCurrentTimeString
 * 函数功能：获取当前的时间值
 * 参   数：void
 * 返   回：void
 * *************************************************/
const QString TWSDNetPay::GetCurrentTimeString(const int& iTimeType)
{
    QDateTime t_time;
    t_time = QDateTime::currentDateTime();
    QString strTime;
    if(0 == iTimeType)
    {
        strTime = t_time.toString("yyyy-MM-dd hh:mm:ss");
    }
    else
    {
        strTime = t_time.toString("yyyy-MM-dd hh:mm:ss.zzz");
    }
    return strTime;
}



/*
 *参数列表
    1.串口号
    2.串口波特率
    3.代理服务器地址，可留空
    4.服务器链接端口，默认80（公网测试为8081）
    5.服务器是否开放端口，默认0（公网测试为1）
    6.是否产生日志标记（0或1），默认为1
    7.日志保存天数，整型，默认20
    8.区域信息
    9.本地站点信息
    10.本地车道号信息
    11.本地服务器信息
    12.省份编码
    13.回调函数声明
* ***************************************/
bool Inner_IF_InitEnvironment(
        const char  *tty,
        int         baudRate,
        const char  *serverIP,
        int         serverPort,
        bool        isOpenServerPort,
        bool        logFlag,
        int         logSaveTime,
        const char  *szAreaInfo,
        const char  *szLoaclStation,
        const char  *szLoaclLaneID,
        const char  *szServerInfo,
        const int   &iProvinceID,
        void (*fun)(int option, int result))
{
    bool bReturn = false;
    QString strLog;
    QString t_StrLog;
    try
    {

        if( GetGlobalApp()->m_bIsInitEnvironment )
            throw -11;

        //获取软件版本号
        GetGlobalApp()->m_strNetPayDllVersion = "2.0.0.1";
        GetGlobalApp()->m_strDeviceDllVersion = "2.0.0.1";
        GetGlobalApp()->m_strWorkDir = QDir::currentPath();

        strLog = QString("/*************************************程序启动(业务%1 - 设备%2)*************************************/").arg( GetGlobalApp()->m_strNetPayDllVersion).arg(GetGlobalApp()->m_strDeviceDllVersion);
        GetGlobalApp()->WriteLogFile(strLog);


        strLog = QString(tty)+":"+QString::number(baudRate)+":"+QString(szAreaInfo)+":"+QString(szLoaclStation)+":"+QString(szServerInfo);


        //20170510
        t_StrLog = "[主进程响应],程序发布时间:"+QString( __DATE__ )+"-"+ QString( __TIME__);
        GetGlobalApp()->WriteLogFile(t_StrLog);

        t_StrLog = "[主进程响应],组件初始化接口(IF_InitEnvironment)调用开始";
        GetGlobalApp()->WriteLogFile(t_StrLog);
        /*******************检索参数是否正常*******************/
        // if( (0 == nThreadID))
        //     throw -1;
        if( 0 == baudRate)
            throw -2;
        if( (NULL == szAreaInfo) || (0 == szAreaInfo))
            throw -3;
        if( (NULL == szLoaclStation) || (0 == strlen(szLoaclStation)))
            throw -4;
        if( (NULL == szLoaclLaneID) || (0 == strlen(szLoaclLaneID)))
            throw -5;
        if( (NULL == szServerInfo) || (0 == strlen(szServerInfo)))
            throw -6;
        if( 0 == iProvinceID)
            throw -7;

        //填充配置信息
        QString strTemp;
        theApp.m_JsonVersion             = QString("1.0");
        theApp.m_XMLVersion              = QString("1.0");

        //7.日志保存天数，整型，默认20
        theApp.m_bSaveLog                = logFlag;
        theApp.m_iDeleteLogSaveDays      = logSaveTime;
        //当前服务器地址
        theApp.m_CurrentServerAddress    = serverIP;
        //当前服务器端口
        theApp.m_CurrentServerPort       = serverPort;

        //省份等数据
        theApp.m_sTranstParam.ProvinceID                            = iProvinceID;
        theApp.m_sTranstParam.LaneConfigInfo.LaneID         = QString(szLoaclLaneID).toInt();

        strTemp = szAreaInfo;
        if(-1 == strTemp.indexOf("#", 0, Qt::CaseInsensitive))
            throw -9;

        theApp.m_sTranstParam.LaneConfigInfo.AreaID = strTemp.left(strTemp.indexOf("#", 0, Qt::CaseInsensitive)).toInt();
        QString strRoadID = strTemp.right(strTemp.length()-strTemp.indexOf("#", 0, Qt::CaseInsensitive)-1);
        theApp.m_sTranstParam.LaneConfigInfo.RoadID = strRoadID.toInt();

        //本地站点信息
        strTemp = szLoaclStation;
        if(-1 == strTemp.indexOf("#", 0, Qt::CaseInsensitive))
            throw -10;

        QString strSTID;
        theApp.m_sTranstParam.LaneConfigInfo.StationID =  strTemp.left(strTemp.indexOf("#", 0, Qt::CaseInsensitive)).toLongLong();
        strSTID = QString::number(theApp.m_sTranstParam.LaneConfigInfo.StationID);
        memcpy(theApp.m_sTranstParam.LaneConfigInfo.str_StationID, strSTID.toStdString().c_str(), strSTID.toStdString().length());

        QString t_StrEnterStation = strTemp.right(strTemp.length()-strTemp.indexOf("#", 0, Qt::CaseInsensitive)-1);
        memcpy(theApp.m_sTranstParam.LaneConfigInfo.StationTab,t_StrEnterStation.toStdString().c_str(),t_StrEnterStation.toStdString().length());

        //特殊处理代码区域
        switch(theApp.m_sTranstParam.ProvinceID)
        {
        case 44:
        {

            switch(theApp.m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //广东华快的出口站号特殊处理
            case 30:
            case 31:
            {
                strSTID = strSTID.sprintf("%04d%03d%02lld", theApp.m_sTranstParam.LaneConfigInfo.AreaID, theApp.m_sTranstParam.LaneConfigInfo.RoadID, theApp.m_sTranstParam.LaneConfigInfo.StationID);
                theApp.m_sTranstParam.LaneConfigInfo.StationID = strSTID.toLongLong();
                memcpy(theApp.m_sTranstParam.LaneConfigInfo.str_StationID, strSTID.toStdString().c_str(), strSTID.toStdString().length());

            }
                break;
            }
        }
            break;
        }


        memcpy(theApp.m_sTranstParam.PathCommDir,theApp.m_strWorkDir.toStdString().c_str(),sizeof(theApp.m_sTranstParam.PathCommDir));

        //获取云端通讯IP地址以及开放端口
        QString szTempIP;
        QString strReflectWebFilePath;
        strReflectWebFilePath.append(theApp.m_strWorkDir);
        strReflectWebFilePath.append("/");
        strReflectWebFilePath.append(FILE_NAME_APP_CONFIG);


        //为了与是否提供配置文件兼容，如此处理 IP 和 Port
        QSettings  qsettings(strReflectWebFilePath, QSettings::IniFormat);
        szTempIP = qsettings.value("ReflectWeb/IP", theApp.m_CurrentServerAddress).toString();
        memcpy(theApp.m_sTranstParam.LaneConfigInfo.WebServerIP,szTempIP.toStdString().c_str() ,szTempIP.toStdString().length());
        theApp.m_sTranstParam.LaneConfigInfo.WebServerPort  = qsettings.value("ReflectWeb/PORT", theApp.m_CurrentServerPort ).toInt();

        //自动更新端口号
        theApp.m_sTranstParam.LaneConfigInfo.AutoUpdatePort = qsettings.value("ReflectWeb/UpdatePort",8089).toInt();

        GetGlobalApp()->WriteLogFile("[主进程响应],组件初始化接口(IF_InitEnvironment)，内部参数配置:"+
                                     t_StrEnterStation+"(区域编码:"+
                                     QString::number(theApp.m_sTranstParam.LaneConfigInfo.AreaID)+
                                     ",路段编码:"+QString::number(theApp.m_sTranstParam.LaneConfigInfo.RoadID)+
                                     ",站点编码:"+QString::number(theApp.m_sTranstParam.LaneConfigInfo.StationID));

        //进行合法性校验
        theApp.m_bAuthorizedLegal = true;
        //获取串口參數
        theApp.m_BaudRate = baudRate;
        memcpy(theApp.m_PortName,tty,sizeof(theApp.m_PortName));

        //标记通知方式
        theApp.m_iNotifyMode = 2;
        //消息回调函数
        if(NULL != fun)
        {
            theApp.m_fnCallbackNotifyFun = fun;
        }

        //标记初始化状态
        theApp.m_bIsInitEnvironment = true;
        //返回结果
        bReturn = theApp.m_bIsInitEnvironment?true:false;

        GetGlobalApp()->start(); //从这里开始启动业务管理线程

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入线程ID与窗体句柄均为0";
            break;
        case -2:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入通知调用者消息ID为0";
            break;
        case -3:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入区域信息非法";
            break;
        case -4:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入站点编码信息非法";
            break;
        case -5:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入车道编码信息非法";
            break;
        case -6:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入服务器信息非法";
            break;
        case -7:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入省份编码为0";
            break;
        case -8:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误,传入调用者线程ID和窗体句柄不能同时为空";
            break;
        case -9:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误,传入区域参数格式不正确";
            break;
        case -10:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误,传入站点信息参数格式不正确";
            break;
        case -11:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),组件不允许多次初始化";
            break;
        case -12:
            strLog = "[主进程响应],组件初始化接口(IF_InitEnvironment),内部初始化错误,无法启动业务管理线程";
            break;
        }

        //记录日志
        theApp.m_strLastErrorDesc  = strLog;
        GetGlobalApp()->WriteLogFile(theApp.m_strLastErrorDesc);
    }


    if(bReturn)
        GetGlobalApp()->WriteLogFile(QString("[主进程响应],组件初始化接口(IF_InitEnvironment)调用结束--true"));
    else
        GetGlobalApp()->WriteLogFile(QString("[主进程响应],组件初始化接口(IF_InitEnvironment)调用结束--false"));

    return bReturn;
}



/***************************************************
 * 函数名称：IF_Destroy
 * 函数功能：清理资源
 * 参   数：void
 * 返   回：void
 * *************************************************/
bool Inner_IF_Destroy(void)
{
    bool bReturn = false;
    try
    {
        QString strLog;
        /**************************清理线程资源*********************/
        strLog = "[主进程响应],组件资源清理接口(IF_Destroy),正在开始退出后台线程...";
        GetGlobalApp()->WriteLogFile(strLog);

        //停止业务管理线程
        if(NULL != theApp.m_pThreadManage)
        {
            theApp.m_pThreadManage->ResourceDestroy();
            theApp.m_pThreadManage->quit();
            theApp.m_pThreadManage->wait();
        }
        GetGlobalApp()->WriteLogFile(QString("[主进程响应],组件退出..."));

        theApp.quit();
        theApp.wait();
        bReturn = true;
    }
    catch(int& )
    {
    }

    return bReturn;
}
/***************************************************
 * 函数名称：IF_Authority
 * 函数功能：权限验证
 * 参   数：void
 * 返   回：void
 * *************************************************/
bool  TWSDNetPay::IF_Authority( const char* szAuthKey, const int& iAuthKeySize)
{
    bool bReturn = false;

    return bReturn;

}

void   TWSDNetPay::ReadConfigInfo(void)
{
    QString strConfigFilePath;
    QString dirPath = QDir::currentPath();
    strConfigFilePath = QString("%1/%2").arg(dirPath).arg(FILE_NAME_APP_CONFIG);

    QSettings qsettings(strConfigFilePath, QSettings::IniFormat);
    //是否保存日志
    m_bSaveLog = qsettings.value("Log/Save", 0 ).toBool();
    //获取日志删除限定时间段起点
    m_iDeleteLogStartHour = qsettings.value("Log/DeleteStartHour", 2).toInt();
    //获取日志删除限定时间段终点
    m_iDeleteLogEndHour = qsettings.value("Log/DeleteEndHour", 2).toInt();
    //日志删除操作轮询时间
    m_iDeleteLogSpanTime = qsettings.value("Log/DeleteSpanTime", 60).toInt();
    //日志本地保存天数时间
    m_iDeleteLogSaveDays = qsettings.value("Log/DeleteSaveDays", 60).toInt();
    //标记是否测试数据
    m_bIsTestData = qsettings.value("Setup/TestData", 0).toBool();
    //线程存活日志检测时间间隔
    m_dwThreadCheckSpanTime = qsettings.value("Setup/ThreadCheckSpanTime", 100).toInt();
    //获取虚拟卡前缀
    m_strVirtualCardPrefix = qsettings.value("Setup/VirtualCardPrefix", "4412").toString();
    //获取往智能硬件下发时间的间隔
    m_dwUpdateDevieTimeSpanTime = qsettings.value("Setup/UpdateDeviceTimeSpanTime", 30*60).toUInt();
    m_dwUpdateDevieTimeSpanTime *= 1000;

}

/***************************************************
 * 函数名称：IF_GetLastErrorDesc
 * 函数功能：获取上一次错误信息
 * 参   数：void
 * 返   回：char*
 * *************************************************/
const char* Inner_IF_GetLastErrorDesc(void)
{
    return theApp.m_strLastErrorDesc.toStdString().c_str();
}


/***************************************************
 * 函数名称：IF_StartGetAccountInfo
 * 函数功能：启动获取用户信息操作
 * 参   数：void
 * 返   回：char*
 * *************************************************/
bool Inner_IF_StartGetAccountInfo( const int& iOvertime)
{

    bool bReturn = false;
    QString strLog;
    GetGlobalApp()->WriteLogFile("[主进程响应],启动获取用户信息操作接口(IF_StartGetAccountInfo)调用开始");
    try
    {
        strLog = QString("暂未提供该项功能,参数：%1").arg(iOvertime);
        GetGlobalApp()->WriteLogFile(strLog);
        //标记成功
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        GetGlobalApp()->WriteLogFile("[主进程响应],获取用户信息操作预处理失败，将结果通知调用者");
    }

    GetGlobalApp()->WriteLogFile("[主进程响应],启动获取用户信息操作接口(IF_StartGetAccountInfo)调用结束");

    return bReturn;
}


/***************************************************
 * 函数名称：IF_StopGetAccountInfo
 * 函数功能：停止获取用户信息操作
 * 参   数：void
 * 返   回：char*
 * *************************************************/
bool  Inner_IF_StopGetAccountInfo(void)
{
    bool bReturn = false;
    QString strLog;
    GetGlobalApp()->WriteLogFile(("[主进程响应],停止获取用户信息操作接口(IF_StopGetAccountInfo)调用开始"));

    try
    {

        strLog = QString("暂不提供该项功能");
        GetGlobalApp()->WriteLogFile(strLog);
        //标记成功
        bReturn = true;
    }
    catch(int& iErrorCode)
    {

    }
    GetGlobalApp()->WriteLogFile(("[主进程响应],停止获取用户信息操作接口(IF_StopGetAccountInfo)调用结束"));

    return bReturn;
}



/***************************************************
 * 函数名称：IF_GetAccountInfo
 * 函数功能：获取用户信息
 * 参   数：  char* szParamContext
 *         ：int& iParamSize
 *         ：const int& iFormatType
 * 返   回：char*
 * *************************************************/
bool Inner_IF_GetAccountInfo( char* szParamContext, int& iParamSize, const int& iFormatType)
{
    bool bReturn = false;
    QString strLog;
    QString strDataString;
    GetGlobalApp()->WriteLogFile(("[主进程响应],获取用户信息接口(IF_GetAccountInfo)调用开始,参数1:"+QString(szParamContext)+",参数2:"+QString::number(iParamSize)+",参数3:"+QString::number(iFormatType)));

    try
    {
        //判断组件是否已经被初始化
        if(!theApp.m_bIsInitEnvironment)
            throw -1;
        //判断组件是否成功授权
        if(!theApp.m_bAuthorizedLegal)
            throw -2;
        //判断业务处理线程是否异常
        if(NULL == theApp.m_pThreadManage)
            throw -3;
        //判断参数是否异常
        if((NULL == szParamContext) || (iParamSize<1024))
            throw -4;
        if( (iFormatType>2) || (iFormatType<1))
            throw -5;
        //判断数据是否已经获取成功
        if(!theApp.m_pThreadManage->GetAccountDataReadyState())
            throw -6;
        //获取数据
        tagAccountInfo sAccountInfo = theApp.m_pThreadManage->GetLastAccountInfo();
        if(1 == iFormatType)
        {
            //XML
            CDVXMLAccountContext xmlAccount;
            strDataString = xmlAccount.GetXMLString(sAccountInfo.DataContext,sAccountInfo.DataType,sAccountInfo.SrcType);

            strDataString.replace(QString("\r"), QString(""));
            strDataString.replace(QString("\n"), QString(""));

        }
        else
        {
            //JSON解析
            CDVJsonParamAccount jsonAccount;
            jsonAccount.DataType	= sAccountInfo.DataType;
            jsonAccount.DataSrc		= sAccountInfo.SrcType;
            jsonAccount.DataContext = sAccountInfo.DataContext;

            CJsonParamOperate jsonParamOperation;
            strDataString = jsonParamOperation.FormatParamData2Json(&jsonAccount);

        }
        if(strDataString.isEmpty())
            throw -7;

        GetGlobalApp()->WriteLogFile(("[主进程响应],获取用户信息接口(IF_GetAccountInfo)输出数据:"+strDataString));

        memcpy(szParamContext,strDataString.data(),strDataString.length()>=iParamSize?(iParamSize-1):strDataString.length());
        strDataString.clear();
        //标记成功
        bReturn = true;
        //设置用户数据准备状态为假
        theApp.m_pThreadManage->SetAccountDataReadyState(false);
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[主进程响应],获取用户信息操作(IF_GetAccountInfo)出错,组件未成功初始化";
            break;
        case -2:
            strLog = "[主进程响应],获取用户信息操作(IF_GetAccountInfo)出错,组件未被授权";
            break;
        case -3:
            strLog = "[主进程响应],获取用户信息操作(IF_GetAccountInfo)出错,组件内部资源异常";
            break;
        case -4:
            strLog = "[主进程响应],获取用户信息操作(IF_GetAccountInfo)出错,用户数据未能获取成功";
            break;
        }
        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            GetGlobalApp()->WriteLogFile(theApp.m_strLastErrorDesc);
        }
    }
    GetGlobalApp()->WriteLogFile(("[主进程响应],获取用户信息接口(IF_GetAccountInfo)调用结束"));
    return bReturn;
}




/***************************************************
 * 函数名称：IF_DebitMoney
 * 函数功能：执行扣款操作
 * 参   数： IN const char* szParamContext
 *         ：IN const int& iParamSize
 *         ：IN const int& iFormatType
 * 返   回：char*
 * *************************************************/
bool Inner_IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bReturn = false;
    QString strLog = "[主进程响应],执行扣款操作接口(IF_DebitMoney)调用开始,参数1"+QString(szParamContext)+
            ",参数2:"+QString::number(iParamSize)+",参数3:"+QString::number(iFormatType);

    GetGlobalApp()->WriteLogFile(strLog);

    QString strOperationMode;
    QString strDataString;
    QString strAnalyError;

    try
    {

        /*****************************校核过程***********************************/
        //判断组件是否初始化成功
        if(!theApp.m_bIsInitEnvironment)
            throw -1;
        //判断合法性
        if(!theApp.m_bAuthorizedLegal)
            throw -2;
        //判断参数是否异常
        if((NULL == szParamContext) || (0 == iParamSize) || (iFormatType>2 || iFormatType<1))
            throw -3;

        strDataString = QString(szParamContext);

        if(1 == iFormatType)
        {
            CDVXMLDebitParamContext xml;
            if(!xml.Init(strDataString))
            {
                strAnalyError = xml.m_strLastErrorDesc;
                throw -4;
            }

            //支付金额为空
            if(xml.Money <= 0 )
            {
                xml.Money = 0;
                GetGlobalApp()->WriteLogFile(("into IF_DebitMoney:  XML,支付金额为0或负数"));
            }

            //复制数据
            theApp.m_sCurrentDebitInfo.OverTime			= xml.OverTime;
            theApp.m_sCurrentDebitInfo.OperationMode		= xml.OperationMode;   //1、整合 2、单体
            theApp.m_sCurrentDebitInfo.PayIdentifier                  = xml.PayIdentifier;
            theApp.m_sCurrentDebitInfo.Money				= xml.Money;
            theApp.m_sCurrentDebitInfo.SubTime				= xml.SubTime;
            theApp.m_sCurrentDebitInfo.DebitMainType		= xml.DebitMainType;
            theApp.m_sCurrentDebitInfo.VehicleType			= xml.VehicleType;
            theApp.m_sCurrentDebitInfo.VehicleClass		        = xml.VehicleClass;
            theApp.m_sCurrentDebitInfo.VLColor				= xml.VLColor;
            theApp.m_sCurrentDebitInfo.VehicleLicense		= xml.VehicleLicense;
            theApp.m_sCurrentDebitInfo.AxisCount			= xml.AxisCount;
            theApp.m_sCurrentDebitInfo.TotalWeight			= xml.Weight;
            theApp.m_sCurrentDebitInfo.LimitWeight			= xml.LimitWeight;
            theApp.m_sCurrentDebitInfo.PassCertificateType        = xml.PassCertificateType;
            theApp.m_sCurrentDebitInfo.CardID				= xml.CardID;
            theApp.m_sCurrentDebitInfo.TollDate			        = xml.TollDate;
            theApp.m_sCurrentDebitInfo.ShiftID				= xml.ShiftID;
            theApp.m_sCurrentDebitInfo.EntryStationID		= xml.EntryStationID;
            theApp.m_sCurrentDebitInfo.str_EntryStationID         = xml.str_EntryStationID;
            theApp.m_sCurrentDebitInfo.EntryStationName	        = xml.EntryStationName;
            theApp.m_sCurrentDebitInfo.EntryTime			= xml.EntryTime;
            theApp.m_sCurrentDebitInfo.EntryLane			= xml.EntryLane;
            theApp.m_sCurrentDebitInfo.EntryVehicleLicense       = xml.EntryVehicleLicense;
            theApp.m_sCurrentDebitInfo.EntryVLColor		        = xml.EntryVLColor;
            theApp.m_sCurrentDebitInfo.EntryVehicleType	        = xml.EntryVehicleType;
            theApp.m_sCurrentDebitInfo.EntryVehicleClass	        = xml.EntryVehicleClass;
            theApp.m_sCurrentDebitInfo.EntryRoadID			= xml.EntryRoadID;
            theApp.m_sCurrentDebitInfo.EntryAreaID			= xml.EntryAreaID;
            theApp.m_sCurrentDebitInfo.TicketNo			        = xml.TicketNo;

            theApp.m_sCurrentDebitInfo.OperatorID = xml.OperatorID;
            theApp.m_sCurrentDebitInfo.OperatorName = xml.OperatorName;

            theApp.m_sCurrentDebitInfo.Valid				= true;
        }
        else
        {
            CDVJsonParamDebitMoney* pDebitMoney = NULL;
            CJsonParamOperate jsonParamOperation;
            pDebitMoney = (CDVJsonParamDebitMoney*)jsonParamOperation.FormatResponseResult2Class(strDataString,JSON_DT_PARAM_DEBIT_MONEY);
            if(NULL == pDebitMoney)
            {
                strAnalyError = "";
                throw -5;
            }

            // 支付金额为空，
            if(pDebitMoney->Money <=0)
            {
                pDebitMoney->Money = 0;
                GetGlobalApp()->WriteLogFile(("into IF_DebitMoney:  JSON,支付金额为0或负数"));
            }

            //复制数据
            theApp.m_sCurrentDebitInfo.OverTime                         = pDebitMoney->OverTime;
            theApp.m_sCurrentDebitInfo.OperationMode               = pDebitMoney->OperationMode;
            theApp.m_sCurrentDebitInfo.PayIdentifier                    = pDebitMoney->PayIdentifier;
            theApp.m_sCurrentDebitInfo.Money			          = pDebitMoney->Money;
            theApp.m_sCurrentDebitInfo.SubTime				   = pDebitMoney->SubTime;
            theApp.m_sCurrentDebitInfo.DebitMainType                 = pDebitMoney->DebitMainType;
            theApp.m_sCurrentDebitInfo.VehicleType			= pDebitMoney->VehicleType;
            theApp.m_sCurrentDebitInfo.VehicleClass                   = pDebitMoney->VehicleClass;

            theApp.m_sCurrentDebitInfo.VLColor				= pDebitMoney->VLColor;
            theApp.m_sCurrentDebitInfo.VehicleLicense               = pDebitMoney->VehicleLicense;
            theApp.m_sCurrentDebitInfo.AxisCount			= pDebitMoney->AxisCount;
            theApp.m_sCurrentDebitInfo.TotalWeight			= pDebitMoney->Weight;
            theApp.m_sCurrentDebitInfo.LimitWeight			= pDebitMoney->LimitWeight;
            theApp.m_sCurrentDebitInfo.PassCertificateType       = pDebitMoney->PassCertificateType;
            theApp.m_sCurrentDebitInfo.CardID				= pDebitMoney->CardID;
            theApp.m_sCurrentDebitInfo.TollDate                         = pDebitMoney->TollDate;
            theApp.m_sCurrentDebitInfo.ShiftID				= pDebitMoney->ShiftID;
            theApp.m_sCurrentDebitInfo.EntryStationID               = pDebitMoney->EntryStationID;
            theApp.m_sCurrentDebitInfo.str_EntryStationID         = pDebitMoney->str_EntryStationID;
            theApp.m_sCurrentDebitInfo.EntryStationName         = pDebitMoney->EntryStationName;
            theApp.m_sCurrentDebitInfo.EntryTime			= pDebitMoney->EntryTime;
            theApp.m_sCurrentDebitInfo.EntryLane			= pDebitMoney->EntryLane;
            theApp.m_sCurrentDebitInfo.EntryVehicleLicense       = pDebitMoney->EntryVehicleLicense;
            theApp.m_sCurrentDebitInfo.EntryVLColor                 = pDebitMoney->EntryVLColor;

            theApp.m_sCurrentDebitInfo.EntryVehicleType            = pDebitMoney->EntryVehicleType;
            theApp.m_sCurrentDebitInfo.EntryVehicleClass           = pDebitMoney->EntryVehicleClass;
            theApp.m_sCurrentDebitInfo.EntryRoadID			= pDebitMoney->EntryRoadID;
            theApp.m_sCurrentDebitInfo.EntryAreaID			= pDebitMoney->EntryAreaID;
            theApp.m_sCurrentDebitInfo.TicketNo                         = pDebitMoney->TicketNo;

            theApp.m_sCurrentDebitInfo.OperatorID = pDebitMoney->OperatorID;
            theApp.m_sCurrentDebitInfo.OperatorName = pDebitMoney->OperatorName;

            theApp.m_sCurrentDebitInfo.Valid				= true;

            delete pDebitMoney;
            pDebitMoney = NULL;
        }

        //特殊处理代码区域
        switch(theApp.m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            QString strSTID;
            switch(theApp.m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //广东华快的出口站号特殊处理
            case 30:
            case 31:
            {
                strSTID = strSTID.sprintf("%04d%03d%02lld", theApp.m_sTranstParam.LaneConfigInfo.AreaID,
                                          theApp.m_sTranstParam.LaneConfigInfo.RoadID,
                                          theApp.m_sTranstParam.LaneConfigInfo.StationID);
                theApp.m_sTranstParam.LaneConfigInfo.StationID = strSTID.toLongLong();
                memcpy(theApp.m_sTranstParam.LaneConfigInfo.str_StationID, strSTID.toStdString().c_str(), strSTID.toStdString().length());
            }
                break;
            }
        }
            break;
        }

        strLog = "OverTime:"+QString::number(theApp.m_sCurrentDebitInfo.OverTime);
        GetGlobalApp()->WriteLogFile(strLog);

        //检测业务管理线程是否正常
        if(NULL == theApp.m_pThreadManage)
            throw -6;

        //判断上次交易是否完成
        if(theApp.m_bIsTrading)
            throw -7;

        //标记当前交易正在进行
        theApp.m_bIsTrading = true;

        /*****************************响应操作***********************************/

        switch(theApp.m_sCurrentDebitInfo.OperationMode)
        {
        case 1://整合操作
        {
            strOperationMode = "整合模式";
            theApp.m_bCurrentDebitMode = true;

            //重构扣款xml/json
            //内部使用json格式传输
            QString  debitString =  GetGlobalApp()->ConstructDebitInfo(2, GetGlobalApp()->m_sCurrentDebitInfo);
            debitString.replace(QString("\n"), QString(""));
            debitString.replace(QString("\r"), QString(""));

            //启动扣款流程
            GetGlobalApp()->m_pThreadManage->SEND_WM_THREAD_MANAGE_START_DEBIT_FLOW(debitString, iFormatType);

            //添加描述
            theApp.m_strLastErrorDesc = "[主进程响应],执行扣款操作(IF_DebitMoney),业务正在处理(整合操作):"+QString::number(theApp.m_sCurrentDebitInfo.OverTime);
        }
            break;
        }

        GetGlobalApp()->WriteLogFile(theApp.m_strLastErrorDesc);
        //标记成功
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        uchar byFunCode		    = 0x01;
        uchar byExecuteState	= RCC_INNER_ERROR;
        switch(iErrorCode)
        {
        case -1:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),组件未成功初始化";
            byExecuteState	= RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),合法性校验失败";
            byExecuteState	= RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),传入参数异常";
            byExecuteState	= RCC_PARAM_ERROR;
            break;
        case -4:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),传入参数(XML格式)解析出错，详细描述:"+strAnalyError ;
            byExecuteState	= RCC_PARAM_ERROR;
            break;
        case -5:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),传入参数(JSON格式)解析出错";
            byExecuteState	= RCC_PARAM_ERROR;
            break;
        case -6:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),内部处理异常:业务管理模块未能正常工作";
            byExecuteState	= RCC_PARAM_ERROR;
            break;
        case -7:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),内部处理异常:上次交易未完成，不允许频繁调用";
            byExecuteState	= RCC_LAST_TRADE_NOT_END;
            break;
        case -8:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),内部处理异常:资源申请失败";
            byExecuteState = RCC_INNER_ERROR;
            break;
        }
        GetGlobalApp()->WriteLogFile(strLog);
        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            GetGlobalApp()->WriteLogFile(theApp.m_strLastErrorDesc);
        }

        //预处理失败，回馈调用者结果
        GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);  //消息回调函数
        //保存日志
        GetGlobalApp()->WriteLogFile(("[主进程响应],扣款操作预处理失败，将结果通知调用者(功能代码:"+QString::number(byFunCode)+strLog+",执行状况:"+QString::number(byExecuteState)+")"));

    }
    GetGlobalApp()->WriteLogFile(("[主进程响应],执行扣款操作接口(IF_DebitMoney)调用结束"));
    return bReturn;
}




/***************************************************
 * 函数名称：IF_DebitCancel
 * 函数功能：执行撤单操作
 * 参   数： IN const char* szParamContext
          ：IN const int& iParamSize
          ：IN const int& iFormatType
 * 返   回：char*
 * *************************************************/

bool Inner_IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bReturn = false;
    QString strLog ;
    strLog = "[主进程响应],执行撤单操作接口(IF_DebitCancel)调用开始,参数1:"+
            QString(szParamContext)+",参数2:"+
            QString::number(iParamSize)+",参数3:"+
            QString::number(iFormatType);

    GetGlobalApp()->WriteLogFile(strLog);

    QString strDataString;
    QString strPayIdentifier;
    CDVXMLDebitCancel xml;
    try
    {
        /*****************************校核过程***********************************/
        //判断组件是否初始化成功
        if(!theApp.m_bIsInitEnvironment)
            throw -1;
        //判断合法性
        if(!theApp.m_bAuthorizedLegal)
            throw -2;
        //判断参数是否异常
        if((NULL == szParamContext) || (0 == iParamSize) || (iFormatType>2 || iFormatType<1))
            throw -3;

        strDataString = QString(szParamContext);
        int nType = iFormatType;
        if(1 == iFormatType)
        {
            if(!xml.Init(strDataString))
                throw -4;
            strPayIdentifier = xml.PayIdentifier;
        }
        else
        {
            CDVJsonParamDebitCancel* pDebitCancel = NULL;
            CJsonParamOperate jsonParamOperation;
            pDebitCancel = (CDVJsonParamDebitCancel*)jsonParamOperation.FormatResponseResult2Class(strDataString,JSON_DT_PARAM_DEBIT_CANCLE);
            if(NULL == pDebitCancel)
            {
                throw -5;
            }
            strPayIdentifier = pDebitCancel->PayIdentifier;
        }
        //检测业务管理线程是否正常
        if(NULL == theApp.m_pThreadManage)
            throw -6;
        //检测撤单交易标识是否合法
        //临时调试
        if(strPayIdentifier.isEmpty())
            throw -7;

        QString cancelInfo = strPayIdentifier;
        GetGlobalApp()->m_pThreadManage->SEND_WM_THREAD_MANAGE_DEBIT_CANCEL(cancelInfo, nType);
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        uchar byFunCode		= 0x02;
        uchar byExecuteState	= RCC_INNER_ERROR;

        switch(iErrorCode)
        {
        case -1:
            strLog  = "[主进程响应],执行撤单操作(IF_DebitCancel)出错,组件未成功初始化";
            byExecuteState	= RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog = "[主进程响应],执行撤单操作(IF_DebitCancel)出错,合法性校验失败";
            byExecuteState	= RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog = "[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数异常";
            byExecuteState	= RCC_PARAM_ERROR;
            break;
        case -4:
            strLog = "[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数解析出错，详细描述"+xml.m_strLastErrorDesc;
            byExecuteState	= RCC_PARAM_ERROR;
            break;
        case -5:
            strLog = "[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数解析出错(JSON)";
            byExecuteState	= RCC_PARAM_ERROR;
            break;
        case -6:
            strLog = "[主进程响应],执行撤单操作(IF_DebitCancel)出错,内部处理异常:业务管理模块未能正常工作";
            break;
        case -7:
            strLog = "[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数有误,需要撤销的订单对应的交易识别码为空";
            byExecuteState	= RCC_PARAM_ERROR;
            break;
        case -8:
            strLog = "[主进程响应],执行撤单操作(IF_DebitCancel)出错,内部处理异常:资源申请失败";
            break;
        }
        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            GetGlobalApp()->WriteLogFile(theApp.m_strLastErrorDesc);
        }
        //预处理失败，回馈调用者结果
        GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);

        //保存日志
        GetGlobalApp()->WriteLogFile(("[主进程响应],撤单操作预处理失败，将结果通知调用者(功能代码:"+QString::number(byFunCode)+",执行状况:"+QString::number(byExecuteState)+")"));

    }

    GetGlobalApp()->WriteLogFile(("[主进程响应],执行撤单操作接口(IF_DebitCancel)调用结束"));
    return bReturn;
}


/***************************************************
 * 函数名称：IF_GetDebitResult
 * 函数功能：获取扣款结果
 * 参   数： IN OUT char* szParamContext
         ：IN OUT int& iParamSize
         ：IN const int& iFormatType
 * 返   回：char*
 * *************************************************/
bool Inner_IF_GetDebitResult( char* szParamContext, int& iParamSize, const int& iFormatType)
{
    bool bReturn = false;
    QString strLog;
    QString strDataString;

    try
    {
        //判断组件是否初始化成功
        if(!theApp.m_bIsInitEnvironment)
            throw -1;
        //判断合法性
        if(!theApp.m_bAuthorizedLegal)
            throw -2;
        //判断参数是否异常
        if((iFormatType>2) || (iFormatType<1) || (NULL == szParamContext))
            throw -3;
        //判断结果是否已经准备好
        if(!theApp.m_sCurrentDebitResult.Valid)
            throw -4;

        switch(iFormatType)
        {
        case 1:
        {
            CDVXMLDebitResult xml;
            xml.XMLFunctionDesc     = "COMMAND";
            xml.XMLVersion              = theApp.m_XMLVersion;
            xml.ResultType		   = theApp.m_sCurrentDebitResult.ResultType;
            xml.ExecuteCode	 	   = theApp.m_sCurrentDebitResult.ExecuteCode;
            xml.PayPlatformType      = theApp.m_sCurrentDebitResult.PayPlatformType;
            xml.DebitTime		   = theApp.m_sCurrentDebitResult.DebitTime;
            xml.ConsumeTime	   = theApp.m_sCurrentDebitResult.ConsumeTime;
            xml.ExecuteDesc             = theApp.m_sCurrentDebitResult.ExecuteDesc;
            xml.TradeSecurityCode    = theApp.m_sCurrentDebitResult.TradeSecurityCode;
            xml.PayIdentifier             = theApp.m_sCurrentDebitResult.PayIdentifier;
            xml.DebitOrder                = theApp.m_sCurrentDebitResult.DebitOrder;

            strDataString = xml.GetXMLString();

            strDataString.replace(QString("\r"), QString(""));
            strDataString.replace(QString("\n"), QString(""));

        }
            break;
        case 2:
        {
            CDVJsonParamDebitResult jsonDebitResult;
            jsonDebitResult.ResultType                  = theApp.m_sCurrentDebitResult.ResultType;
            jsonDebitResult.ExecuteCode		= theApp.m_sCurrentDebitResult.ExecuteCode;
            jsonDebitResult.PayPlatformType         = theApp.m_sCurrentDebitResult.PayPlatformType;

            jsonDebitResult.DebitTime		= theApp.m_sCurrentDebitResult.DebitTime;
            jsonDebitResult.ConsumeTime	          = theApp.m_sCurrentDebitResult.ConsumeTime;
            jsonDebitResult.ExecuteDesc                = theApp.m_sCurrentDebitResult.ExecuteDesc;
            jsonDebitResult.TradeSecurityCode      = theApp.m_sCurrentDebitResult.TradeSecurityCode;
            jsonDebitResult.PayIdentifier               = theApp.m_sCurrentDebitResult.PayIdentifier;
            jsonDebitResult.DebitOrder                 = theApp.m_sCurrentDebitResult.DebitOrder;

            CJsonParamOperate jsonParamOperation;
            strDataString = jsonParamOperation.FormatParamData2Json(&jsonDebitResult);
            if(strDataString.isEmpty())
            {
                throw -4;
            }
        }
            break;
        default:
            break;
        }

        if(strDataString.length()>(iParamSize+1))
            throw -5;

        GetGlobalApp()->WriteLogFile(("[主进程响应],获取扣款结果接口(IF_GetDebitResult)输出结果如下: "+strDataString));

        iParamSize = strDataString.length();
        memcpy(szParamContext,strDataString.toStdString().c_str(),strDataString.toStdString().length());
        //重置交易结果
        theApp.m_sCurrentDebitResult.Valid = false;

        //标记成功
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,组件未成功初始化";
            break;
        case -2:
            strLog = "[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,组件未被授权";
            break;
        case -3:
            strLog = "[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,传入参数异常";
            break;
        case -4:
            strLog = "[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,扣款结果未准备好";
            break;
        case -5:
            strLog = "[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,容器太小";
            break;
        }
        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            GetGlobalApp()->WriteLogFile(theApp.m_strLastErrorDesc);
        }
    }

    GetGlobalApp()->WriteLogFile(("[主进程响应],获取扣款结果接口(IF_GetDebitResult)调用结束"));

    return bReturn;
}




/***************************************************
 * 函数名称：IF_SetMMI
 * 函数功能：设置交互界面
 * 参   数：  IN const char* szParamContext
//         ：IN const int& iParamSize
//         ：IN const int& iFormatType
 * 返   回：char*
 * *************************************************/
bool  Inner_IF_SetMMI( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bReturn = false;
    QString strLog = "[主进程响应],设置交互界面接口(IF_SetMMI)调用开始,参数1:"+QString(szParamContext)+",参数2:"+QString::number(iParamSize)+",参数3:"+QString::number(iFormatType);
    GetGlobalApp()->WriteLogFile(strLog);

    QString strParamString(szParamContext);
    QString strSubError;
    QString strDataString;
    int nMMItype = -1;
    try
    {
        //判断组件是否已经被初始化
        if(!theApp.m_bIsInitEnvironment)
            throw -1;
        //判断组件是否成功授权
        if(!theApp.m_bAuthorizedLegal)
            throw -2;
        //判断业务处理线程是否异常
        if(NULL == theApp.m_pThreadManage)
            throw -3;
        if(NULL == theApp.m_pThreadManage->m_pThreadDevice)
            throw -4;
        //判断参数是否异常
        if((NULL == szParamContext) || (0 == iParamSize))
            throw -5;
        //判断参数类型
        if( (iFormatType>2) || (iFormatType<1))
            throw -6;
        int iType = iFormatType;


        strDataString  = GetGlobalApp()->ConstructMMIInfo(iType,strParamString,  nMMItype);
        strDataString.replace(QString("\r"), QString(""));
        strDataString.replace(QString("\n"), QString(""));

        if(0 == nMMItype )
        {
            //费显
            GetGlobalApp()->m_pThreadManage->SEND_WM_THREAD_MANAGE_SHOW_DEAL_INFO(strDataString, 203);
        }
        else
        {
            //清屏
            GetGlobalApp()->m_pThreadManage->SEND_WM_THREAD_MANAGE_SHOW_DEAL_INFO(strDataString, 204);
        }
        //标记成功
        bReturn = true;

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,组件未成功初始化";
            break;
        case -2:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,组件未被授权";
            break;
        case -3:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,theApp.m_pThreadManage异常";
            break;
        case -4:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,theApp.m_pThreadManage->m_pThreadDevice异常";
            break;
        case -5:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,NULL == szParamContext) || (0 == iParamSize";
            break;
        case -6:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,(iFormatType>2) || (iFormatType<1)异常";
            break;
        case -9:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,构造的结果XML为空";
            break;
        case -7:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(XML格式)有误，详细描述:"+strSubError;
            break;
        case -8:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(JSON格式)有误，详细描述:"+strSubError;
            break;
        }

        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            GetGlobalApp()->WriteLogFile(theApp.m_strLastErrorDesc);
        }
    }
    GetGlobalApp()->WriteLogFile(("[主进程响应],设置交互界面接口(IF_SetMMI)调用结束"));

    return bReturn;
}
/***************************************************
 * 函数名称：IF_GetParam
 * 函数功能：获取硬件参数
 * 参   数： IN OUT char* szParamContext
         ：IN OUT int& iParamSize
         ：IN const int& iFormatType
 * 返   回：char*
 * *************************************************/
bool Inner_IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize)
{
    bool bReturn = false;
    QString strLog = "[主进程响应],获取硬件参数接口(IF_GetParam)暂不提供此项功能";

    if(strLog != theApp.m_strLastErrorDesc)
    {
        theApp.m_strLastErrorDesc = strLog;
        GetGlobalApp()->WriteLogFile(strLog);
    }
    return bReturn;
}


/***************************************************
 * 函数名称：IF_GetComponentStatus
 * 函数功能：检测组件状态
 * 参   数： 0x00 正常, 其他异常
 * 返   回：char*
 * *************************************************/
bool Inner_IF_GetComponentStatus( unsigned int& uStatus)
{

    bool bReturn = false;
    quint64 dwState = 0x00;
    QString strLog;
    try
    {
        //判断组件是否已经被初始化
        if(!theApp.m_bIsInitEnvironment)
            throw -1;
        //判断组件是否成功授权
        if(!theApp.m_bAuthorizedLegal)
            throw -2;
        //判断业务处理线程是否异常
        if(NULL == theApp.m_pThreadManage)
            throw -3;
        //判断硬件控制线程是否正常
        if(NULL == theApp.m_pThreadManage->m_pThreadDevice)
            throw -4;

        //获取硬件状态
        quint64 dwHDState = theApp.m_pThreadManage->m_pThreadDevice->GetDeviceState();
        uStatus = (dwState | dwHDState);
        //标记成功
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件未成功初始化";
            break;
        case -2:
            strLog = "[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件未被授权";
            break;
        case -3:
        case -4:
            strLog = "[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件内部资源异常";
            break;
        }
        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
        }
    }

    return bReturn;
}



/***************************************************
 * 函数名称：IF_TranslateData
 * 函数功能：执行传输关键信息数据操作
 * 参   数： IN OUT char* szParamContext
         ： IN OUT int& iParamSize
         ： IN const int& iFormatType
 * 返   回：char*
 * *************************************************/
bool Inner_IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bReturn = false;
    QString strLog = "[主进程响应],执行传输关键信息数据接口(IF_TranslateData)暂不提供此项功能";
    if(strLog != theApp.m_strLastErrorDesc)
    {
        theApp.m_strLastErrorDesc = strLog;
        GetGlobalApp()->WriteLogFile((theApp.m_strLastErrorDesc));

    }
    return bReturn;
}


//重构扣款xml/json
QString TWSDNetPay::ConstructDebitInfo(int nFormateType, tagPayKeyItems &pkItems )
{
    QString strReturn;
    if(1 == nFormateType)
    {
        QDomDocument doc;
        QDomProcessingInstruction instruction;

        CDVXMLDebitParamContext useAddFun;
        instruction = doc.createProcessingInstruction( "xml","version=\'1.0\' encoding=\'GB2312\'" );
        doc.appendChild( instruction );
        QDomElement root = doc.createElement("DebitParamContext");
        doc.appendChild(root);

        useAddFun.addTextNode(root, "Version", "1.0");
        QString strData = "Data";

        useAddFun.addTextNode(root, strData, "OverTime", QString::number(pkItems.OverTime));
        useAddFun.addTextNode(root, strData, "OperationMode", QString::number(pkItems.OperationMode));

        QString strDebit = "Debit";
        useAddFun.addTextNode(root, strData, strDebit, "PayIdentifier", pkItems.PayIdentifier);
        useAddFun.addTextNode(root, strData, strDebit, "Money", QString::number(pkItems.Money));
        useAddFun.addTextNode(root, strData, strDebit, "SubTime", pkItems.SubTime);
        useAddFun.addTextNode(root, strData, strDebit, "Type", QString::number(pkItems.DebitMainType));

        QString strVehicle = "Vehicle";
        useAddFun.addTextNode(root, strData, strVehicle, "Type", QString::number(pkItems.VehicleType));
        useAddFun.addTextNode(root, strData, strVehicle, "Class", QString::number(pkItems.VehicleClass));
        useAddFun.addTextNode(root, strData, strVehicle, "License", pkItems.VehicleLicense);
        useAddFun.addTextNode(root, strData, strVehicle, "VLColor", QString::number(pkItems.VLColor));
        useAddFun.addTextNode(root, strData, strVehicle, "AxisCount", QString::number(pkItems.AxisCount));
        useAddFun.addTextNode(root, strData, strVehicle, "Weight", QString::number(pkItems.TotalWeight));
        useAddFun.addTextNode(root, strData, strVehicle, "LimitWeight", QString::number(pkItems.LimitWeight));

        QString strOperate = "Operation";
        useAddFun.addTextNode(root, strData, strOperate, "PassCertificateType", QString::number(pkItems.PassCertificateType));
        useAddFun.addTextNode(root, strData, strOperate, "CardID", pkItems.CardID);
        useAddFun.addTextNode(root, strData, strOperate, "TollDate", QString::number(pkItems.TollDate));
        useAddFun.addTextNode(root, strData, strOperate, "ShiftID", QString::number(pkItems.ShiftID));
        useAddFun.addTextNode(root, strData, strOperate, "TicketNo", pkItems.TicketNo);
        useAddFun.addTextNode(root, strData, strOperate, "OperatorName", pkItems.OperatorName);
        useAddFun.addTextNode(root, strData, strOperate, "OperatorID", pkItems.OperatorID);

        QString strEntry = "Entry";
        useAddFun.addTextNode(root, strData, strEntry, "StationID" , pkItems.str_EntryStationID);
        useAddFun.addTextNode(root, strData, strEntry, "StationName" , pkItems.EntryStationName);
        useAddFun.addTextNode(root, strData, strEntry, "Time" , pkItems.EntryTime);
        useAddFun.addTextNode(root, strData, strEntry, "LaneID" , QString::number(pkItems.EntryLane));
        useAddFun.addTextNode(root, strData, strEntry, "License" , pkItems.EntryVehicleLicense);
        useAddFun.addTextNode(root, strData, strEntry, "VColor" , QString::number(pkItems.EntryVLColor));
        useAddFun.addTextNode(root, strData, strEntry, "VClass" , QString::number(pkItems.EntryVehicleClass));
        useAddFun.addTextNode(root, strData, strEntry, "VType" , QString::number(pkItems.EntryVehicleType));
        useAddFun.addTextNode(root, strData, strEntry, "RoadID" , QString::number(pkItems.EntryRoadID));
        useAddFun.addTextNode(root, strData, strEntry, "AreaID" , QString::number(pkItems.EntryAreaID));

        strReturn =   doc.toString();
        strReturn.replace("\n", "");

    }
    else  //重构扣款json
    {
        Json::Value               root;
        Json::Value		DebitParamContext;
        Json::Value		Data;
        Json::Value		Debit;
        Json::Value		Entry;
        Json::Value		Operation;
        Json::Value		Vehicle;

        /***********************************************************构造JSAO数据结构***********************************************************/
        Debit["Money"] =					Json::Value(pkItems.Money);
        Debit["PayIdentifier"] =			Json::Value(pkItems.PayIdentifier.toStdString()) ;
        Debit["SubTime"] =					Json::Value(pkItems.SubTime.toStdString());
        Debit["Type"] =						1;

        Entry["AreaID"] =					Json::Value(pkItems.EntryAreaID) ;
        Entry["LaneID"] =					Json::Value(pkItems.EntryLane) ;
        Entry["License"] =					Json::Value(pkItems.EntryVehicleLicense.toStdString()) ;
        Entry["RoadID"] =					Json::Value(pkItems.EntryRoadID) ;
        //        Entry["StationID"] =				Json::Value(pkItems.EntryStationID) ;
        Entry["StationID"] =				Json::Value(pkItems.str_EntryStationID.toStdString()) ;
        Entry["StationName"] =                       Json::Value(pkItems.EntryStationName.toStdString()) ;
        Entry["Time"] =						Json::Value(pkItems.EntryTime.toStdString());
        Entry["VClass"] =					Json::Value(pkItems.EntryVehicleClass) ;
        Entry["VColor"] =					Json::Value(pkItems.EntryVLColor );
        Entry["VType"] =					Json::Value(pkItems.EntryVehicleType) ;

        Operation["CardID"] =				Json::Value(pkItems.CardID.toStdString()) ;
        Operation["PassCertificateType"] =  Json::Value(pkItems.PassCertificateType) ;
        Operation["ShiftID"] =				Json::Value(pkItems.ShiftID );
        Operation["TollDate"] =				Json::Value(pkItems.TollDate );
        Operation["OperatorID"] =				Json::Value(pkItems.OperatorID.toStdString() );
        Operation["OperatorName"] =				Json::Value(pkItems.OperatorName.toStdString() );
        Operation["TicketNo"] =                                 Json::Value(pkItems.TicketNo.toStdString());

        Vehicle["AxisCount"] =				Json::Value(pkItems.AxisCount) ;
        Vehicle["Class"] =					Json::Value(pkItems.VehicleClass) ;
        Vehicle["License"] =				Json::Value(pkItems.VehicleLicense.toStdString() );
        Vehicle["Type"] =					Json::Value(pkItems.VehicleType );
        Vehicle["VLColor"] =				Json::Value(pkItems.VLColor );
        Vehicle["Weight"] =                              Json::Value(pkItems.TotalWeight) ;
        Vehicle["LimitWeight"] =                     Json::Value(pkItems.LimitWeight);

        Data["Debit"] = Debit;
        Data["Entry"] = Entry;
        Data["Operation"] = Operation;
        Data["OperationMode"] =				Json::Value(pkItems.OperationMode);
        Data["OverTime"] =					Json::Value(pkItems.OverTime);
        Data["Vehicle"] = Vehicle;

        DebitParamContext["Version"] =		Json::Value("1.0");
        DebitParamContext["Data"] = Data;
        root["DebitParamContext"] = DebitParamContext;

        //转化为字符串
        Json::FastWriter fast_writer;
        strReturn = QString(fast_writer.write(root).c_str());
        strReturn.replace("\n", "");
    }

    return strReturn;

}


/**************************************************************
*函数名称：ConstructMMIInfo(int nFormateType, const QString& strOrgString)
*函数描述：重构费显josn/xml
*函数参数：参数1：[in] nFormateType //数据类型 json  /  xml
*                    参数2：[in] strOrgString  //数据内容
*                    参数3：[out] nMMItype  //费显or清屏
*返回值：QString 重构后的格式化数据
****************************************************************/
QString TWSDNetPay::ConstructMMIInfo(int nFormateType, const QString& strOrgString , int& nMMItype)
{

    QString strDataString = "";
    //获取金额、车型车种、重量、限重
    int iMoney  = 0x00;
    int iWeight = 0x00;
    int iVType  = 0x00;
    int iVClass = 0x00;
    int iLimit  = 0x00;
    int iAxis   = 0x00;
    int iBalance= 0x00;
    QString strEntrySTName = "";

    if(0x01 == nFormateType)
    {
        //XML格式
        CDVXMLMMI xml;
        if(!xml.Init(strOrgString))
        {
            GetGlobalApp()->WriteLogFile(QString("[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(XML格式)有误，详细描述:%1").arg(xml.m_strLastErrorDesc));
            return strDataString;
        }
        nMMItype = xml.ClearMMI;
        if(xml.ClearMMI > 0)
        {
            strDataString = "{\"Type\":\"204\",\"MMIParamContext\":{\"Data\":{\"Value\":\"\"},\"Version\":\"1.0\"}}";
        }
        else
        {
            //重构XML数据
            //   QDomDocument doc;
            for(int i = 0;i<xml.Text.size();++i)
            {
                int iTemp = 0x00;
                switch(xml.Text.at(i).Sub)
                {
                case 1://车型
                {
                    iTemp = QString(xml.Text.at(i).Context).toInt();
                    iVType = iTemp/10;
                    iVClass = iTemp%10;
                }
                    break;
                case 2://金额
                {
                    iTemp = QString(xml.Text.at(i).Context).toInt();
                    iMoney = iTemp;
                }
                    break;
                case 3://载重
                {
                    iTemp = QString(xml.Text.at(i).Context).toInt();
                    iWeight = iTemp;
                }
                    break;
                case 4://轴数
                {
                    iTemp = QString(xml.Text.at(i).Context).toInt();
                    iAxis = iTemp;
                }
                    break;
                case 5://限重
                {
                    iTemp = QString(xml.Text.at(i).Context).toInt();
                    iLimit = iTemp;
                }
                    break;
                case 6://电子支付卡余额
                {
                    iTemp = QString(xml.Text.at(i).Context).toInt();
                    iBalance = iTemp;
                }
                    break;
                case 7://入口收费站点名称
                {
                    strEntrySTName   = QString(xml.Text.at(i).Context);
                }
                    break;
                }
            }


            //内部使用json格式传递参数
            Json::Value root;
            Json::Value MMIParamContext;
            Json::Value Data;

            Data["AxisCount"] = Json::Value(iAxis);
            Data["Balance"] = Json::Value(iBalance);
            Data["LimitWeight"] = Json::Value(iLimit);
            Data["Money"] = Json::Value(iMoney);
            Data["StationName"] = Json::Value((strEntrySTName.toStdString().c_str()));
            Data["VClass"] = Json::Value(iVClass);
            Data["VType"] = Json::Value(iVType);
            Data["Weight"] = Json::Value(iWeight);

            MMIParamContext["Data"] = Data;
            MMIParamContext["version"] = "1.0";

            root["MMIParamContext"] = MMIParamContext;
            root["Type"] = "203";

            Json::FastWriter fast_writer;
            strDataString = QString::fromStdString(fast_writer.write(root));


            //            QString xmlHeader = "version=\'1.0\' encoding=\'GB2312\'";
            //            doc.appendChild( doc.createProcessingInstruction( "xml",xmlHeader ));
            //            QDomElement root = doc.createElement( "MMIParamContext" );
            //            doc.appendChild(root);

            //            QString strData = "Data";
            //            xml.addTextNode(root, strData , "AxisCount" , QString::number(iAxis));
            //            xml.addTextNode(root, strData , "Balance" , QString::number(iBalance));
            //            xml.addTextNode(root, strData , "LimitWeight" , QString::number(iLimit));
            //            xml.addTextNode(root, strData , "Money" , QString::number(iMoney));
            //            xml.addTextNode(root, strData , "StationName" , strEntrySTName);
            //            xml.addTextNode(root, strData , "VClass" , QString::number(iVClass));
            //            xml.addTextNode(root, strData , "VType" , QString::number(iVType));
            //            xml.addTextNode(root, strData , "Weight" , QString::number(iWeight));

            //            xml.addTextNode(root,  "Version" , QString("1.0"));
            //            xml.addTextNode(root, "Type" , QString("203"));

            //            strDataString = doc.toString();
        }
    }//xml
    else
    {
        //JSON格式
        CDVJsonParamMMI* pJsonMMI = NULL;
        CJsonParamOperate jsonOperation;

        pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strOrgString,JSON_DT_PARAM_MMI);
        if(NULL == pJsonMMI)
        {
            GetGlobalApp()->WriteLogFile(QString("[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(JSON格式)有误，详细描述:%1").arg( jsonOperation.GetLastErrorDesc()));
            return strDataString;
        }
        nMMItype = pJsonMMI->ClearMMI;
        if(pJsonMMI->ClearMMI > 0)
        {
            strDataString = "{\"Type\":\"204\",\"MMIParamContext\":{\"Data\":{\"Value\":\"\"},\"Version\":\"1.0\"}}";
        }
        else
        {

            for(int i = 0;i<pJsonMMI->Text.size();++i)
            {
                int iTemp = 0x00;
                switch(pJsonMMI->Text.at(i).Sub)
                {
                case 1://车型
                {
                    iTemp = QString(pJsonMMI->Text.at(i).Context).toInt();
                    iVType = iTemp/10;
                    iVClass = iTemp%10;
                }
                    break;
                case 2://金额
                {
                    iTemp = QString(pJsonMMI->Text.at(i).Context).toInt();
                    iMoney = iTemp;
                }
                    break;
                case 3://载重
                {
                    iTemp = QString(pJsonMMI->Text.at(i).Context).toInt();
                    iWeight = iTemp;
                }
                    break;
                case 4://轴数
                {
                    iTemp = QString(pJsonMMI->Text.at(i).Context).toInt();
                    iAxis = iTemp;
                }
                    break;
                case 5://限重
                {
                    iTemp = QString(pJsonMMI->Text.at(i).Context).toInt();
                    iLimit = iTemp;
                }
                    break;
                case 6://电子支付卡余额
                {
                    iTemp = QString(pJsonMMI->Text.at(i).Context).toInt();
                    iBalance = iTemp;
                }
                    break;
                case 7://入口收费站点名称
                {
                    strEntrySTName = QString(pJsonMMI->Text.at(i).Context);
                }
                    break;
                }
            }

            Json::Value root;
            Json::Value MMIParamContext;
            Json::Value Data;

            Data["AxisCount"] = Json::Value(iAxis);
            Data["Balance"] = Json::Value(iBalance);
            Data["LimitWeight"] = Json::Value(iLimit);
            Data["Money"] = Json::Value(iMoney);
            Data["StationName"] = Json::Value(strEntrySTName.toStdString());
            Data["VClass"] = Json::Value(iVClass);
            Data["VType"] = Json::Value(iVType);
            Data["Weight"] = Json::Value(iWeight);

            MMIParamContext["Data"] = Data;
            MMIParamContext["version"] = "1.0";

            root["MMIParamContext"] = MMIParamContext;
            root["Type"] = "203";

            Json::FastWriter fast_writer;
            strDataString = QString::fromStdString(fast_writer.write(root));
        }

    } //json


    return strDataString;


}

//重构撤单JSON/xml
QString TWSDNetPay::ConstructCancleInfo(int nFormatType,QString strPayID)
{
    /*2017-05-31 zhuyabing 恢复了KeyItem节点*/
    QString strReturn;

    Json::Value root;
    Json::Value DebitCancelParamContext;
    Json::Value Data;
    Json::Value keyItem;


    keyItem["PayIdentifier"]=Json::Value(strPayID.toStdString().c_str());
    keyItem["Type"] = Json::Value(1);
    Data["TradeKeyItem"] = keyItem;

    DebitCancelParamContext["Data"]		= Data;
    DebitCancelParamContext["Version"]	= Json::Value(("1.0"));

    root["DebitCancelParamContext"]		= DebitCancelParamContext;

    //转化为字符串
    Json::FastWriter fast_writer;
    strReturn = (QString::fromStdString(fast_writer.write(root).c_str()));

    return strReturn;

}

QString TWSDNetPay::GetClearScreenJson()
{
    return "{\"Type\":\"204\",\"MMIParamContext\":{\"Data\":{\"Value\":\"\"},\"Version\":\"1.0\"}}";
}


bool TWSDNetPay::MakeDirectory(const char* p_dir)
{
    int ret;
    char* p_dup_dir = STRDUP(p_dir);
    int len = strlen(p_dup_dir);

    for (int i = 0; i < len; i++) {
        if (p_dup_dir[i] == '\\' || p_dup_dir[i] == '/') {
            p_dup_dir[i] = '\0';
            ret = ACCESS(p_dup_dir, 0);
            if (ret != 0) {
                ret = MKDIR(p_dup_dir);
                if (ret != 0) {
                    return -1;
                }
            }
            p_dup_dir[i] = '/';
        }
    }

    ret = MKDIR(p_dup_dir);
    free(p_dup_dir);
    return ret;
}


//获取指定文件的版本号
QString TWSDNetPay::GetVersionOfFile(const char* lpsPath)
{ 
    QString strVer = ("1.0.0.0");

    return strVer;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：NotifyCaller
//
// 功能描述：通知外部调用者，业务被异步处理得到的结果
//
// 输入参数：IN const int& iOption				-- 通知类型（1，扣款；2，撤单）
//         ：IN const int& iResult				-- 处理结果，见开发文档
// 输出参数：无
// 编写人员：FFFF
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1
//
////////////////////////////////////////////////////////////////////////////////////////////////
void TWSDNetPay::NotifyCaller( const int& iOption,  const int& iResult)
{
    switch (m_iNotifyMode)
    {
    case 1:
    {
    }
        break;
    case 2:
    {
        //使用回调函数通知外部调用者
        if (NULL != m_fnCallbackNotifyFun)
        {
            m_fnCallbackNotifyFun(iOption, iResult);
        }
        else
        {
            GetGlobalApp()->WriteLogFile(("[主进程响应],组件通知外部调用者的回调函数(NotifyCaller)指针为空，无法通知调用者"));
        }
    }
        break;
    case 3:
    {
        //使用回调函数通知外部调用者(广西特供)
        if (NULL != m_fnCallbackNotifyFun45)
        {
            m_fnCallbackNotifyFun45(iOption, iResult, m_pIndPtrFor45);
        }
        else
        {
            GetGlobalApp()->WriteLogFile(("[主进程响应],组件通知外部调用者的回调函数(NotifyCaller45)指针为空，无法通知调用者"));
        }
    }
        break;
    default:
        break;
    }
}

