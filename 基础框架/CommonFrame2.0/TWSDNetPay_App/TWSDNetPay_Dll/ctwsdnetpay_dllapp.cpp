#include "ctwsdnetpay_dllapp.h"
#include <qstring.h>
#include <QDebug>
#include "common/twsutils.h"

//typedef void (* Fun_ChargeResults)(int option,int result);

//唯一全局变量
CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp()  { return &theApp; }


/***********************变量初始化*********************************/
CThreadManage*                      CTWSDNetPay_DllApp::m_pThreadManage							= NULL;
bool                                CTWSDNetPay_DllApp::m_bIsInitEnvironment						= false;
bool                                CTWSDNetPay_DllApp:: m_bAuthorizedLegal					        = false;
bool                                CTWSDNetPay_DllApp::m_bNetErrorHttpServerInTollNetNotExist                          = false;
bool                                CTWSDNetPay_DllApp::m_bNetErrorHttpServerTrawePlatformNotExist                      = false;
bool                                CTWSDNetPay_DllApp::m_bSaveLog							= false;
quint32                             CTWSDNetPay_DllApp::m_dwThreadCheckSpanTime						= 300;
QString                             CTWSDNetPay_DllApp::m_strWorkDir							= "";
QString                             CTWSDNetPay_DllApp::m_strLastErrorDesc						= "";
QString                             CTWSDNetPay_DllApp::m_strVirtualCardPrefix						= "";
tagTranstParam                      CTWSDNetPay_DllApp::m_sTranstParam							= {0};
bool                                CTWSDNetPay_DllApp::m_bCurrentDebitMode						= false;
bool                                CTWSDNetPay_DllApp::m_bIsTrading							= false;
QString                             CTWSDNetPay_DllApp::m_payIdentifier                         = "";
tagPayKeyItems                      CTWSDNetPay_DllApp::m_sCurrentDebitInfo						= {0};
tagDebitResultItems                 CTWSDNetPay_DllApp::m_sCurrentDebitResult                                           = {0};
time_t                              CTWSDNetPay_DllApp::m_dwTradeBeginOvertime                                          = 0;
time_t                              CTWSDNetPay_DllApp::m_dwUpdateDevieTimeSpanTime                                     = 0;
bool                                CTWSDNetPay_DllApp::m_bIsTestData							= false;
QString                             CTWSDNetPay_DllApp::m_strNetPayDllVersion						= "";
QString                             CTWSDNetPay_DllApp::m_strDeviceDllVersion						= "";
int				    CTWSDNetPay_DllApp::m_iDeleteLogStartHour						= 0;
int				    CTWSDNetPay_DllApp::m_iDeleteLogEndHour						= 0;
int				    CTWSDNetPay_DllApp::m_iDeleteLogSpanTime						= 0;
int				    CTWSDNetPay_DllApp::m_iDeleteLogSaveDays						= 0;
int				    CTWSDNetPay_DllApp::m_iDebitTaskNo							= 0;
char                                CTWSDNetPay_DllApp::m_UpperMSName[30]                                               ={0};
QString                             CTWSDNetPay_DllApp::m_JsonVersion                                                   = "";
QString                             CTWSDNetPay_DllApp::m_XMLVersion                                                    = "";
char                                CTWSDNetPay_DllApp::m_PortName[20]                                                  ={0};
int                                 CTWSDNetPay_DllApp::m_BaudRate                                                      = 0;
Fun_ChargeResults                   CTWSDNetPay_DllApp::m_Fun_ChargeResults                                             = NULL;
QString                             CTWSDNetPay_DllApp::m_CurrentServerAddress                                          ="";
int                                 CTWSDNetPay_DllApp::m_CurrentServerPort                                             = 0;


C_logtxt CTWSDNetPay_DllApp::m_Logtxt;


CTWSDNetPay_DllApp::CTWSDNetPay_DllApp(void)
{  

    //主动资源销毁
    //  connect(this,SIGNAL(WM_THREAD_DESTROY_RESOURCE()),m_pThreadManage,SLOT(ResourceDestroy(void)));

}


void  CTWSDNetPay_DllApp::run()
{
    //  m_pThreadManage->StartWorkThreads(m_sTranstParam);
    //  m_pThreadManage->start();

    //  moveToThread(m_pThreadManage);
    exec();
}


/***************************************************
 * 函数名称：QString2time_t
 * 函数功能：将时间的字符串形式转化为time_t格式
 * 参   数：void
 * 返   回：void
 * *************************************************/
time_t CTWSDNetPay_DllApp::QString2time_t(QString lpstime)
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
const QString CTWSDNetPay_DllApp::GetCurrentTimeString(const int& iTimeType)
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
/***************************************************
 * 函数名称：RecordLog
 * 函数功能：记录日志
 * 参   数：void
 * 返   回：void
 * *************************************************/
//void  CTWSDNetPay_DllApp::RecordLog( const QString& strLog)
void  CTWSDNetPay_DllApp::RecordLog(const  QString& strLog)
{
    qDebug()<< strLog;
}

/******************************************
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
bool IF_InitEnvironment(
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

    theApp.m_bNeedPWD =true;
    strLog = QString(tty)+":"+QString::number(baudRate)+":"+QString(szAreaInfo)+":"+QString(szLoaclStation)+":"+QString(szServerInfo);
    qDebug() << "IF_InitEnvironment: " << strLog;

    QString t_StrLog;
    t_StrLog = "APP發布時間:"+QString( __DATE__ )+"-"+QString( __TIME__);
    theApp.RecordLog(t_StrLog);
    t_StrLog = "[主进程响应],组件初始化接口(IF_InitEnvironment)调用开始";
    theApp.RecordLog(t_StrLog);
    qDebug()<<"[主进程响应],组件初始化接口(IF_InitEnvironment)调用开始";
    theApp.RecordLog("[主进程响应],组件初始化接口(IF_InitEnvironment)调用开始");

    try
    {
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
        char   szTemp[256]={0};
        QString strTemp;
        theApp.m_JsonVersion             = "1.0";
        theApp.m_XMLVersion              = "1.0";

        //7.日志保存天数，整型，默认20
        theApp.m_bSaveLog                = logFlag;
        theApp.m_iDeleteLogSaveDays      = logSaveTime;
        //当前服务器地址
        theApp.m_CurrentServerAddress    = serverIP;
        //当前服务器端口
        theApp.m_CurrentServerPort       = serverPort;

        //省份等数据
        theApp.m_sTranstParam.ProvinceID		    = iProvinceID;
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

        QString stationStr = strTemp.left(strTemp.indexOf("#", 0, Qt::CaseInsensitive));
        qDebug()<<"stationStr:   "<<stationStr;

        memset(theApp.m_sTranstParam.LaneConfigInfo.StationID,0,32);
        memcpy(theApp.m_sTranstParam.LaneConfigInfo.StationID, stationStr.toLocal8Bit().data(), stationStr.toLocal8Bit().length());
        qDebug("StationID:  %s\n",theApp.m_sTranstParam.LaneConfigInfo.StationID);

        QString t_StrEnterStation = strTemp.right(strTemp.length()-strTemp.indexOf("#", 0, Qt::CaseInsensitive)-1);
        memset(theApp.m_sTranstParam.LaneConfigInfo.StationTab,0,32);
        memcpy(theApp.m_sTranstParam.LaneConfigInfo.StationTab,t_StrEnterStation.toLocal8Bit().data(),t_StrEnterStation.toLocal8Bit().length());

        QString stationId = theApp.m_sTranstParam.LaneConfigInfo.StationID;
        qDebug() << "------IF_InitEnvironment2------: StationID: " << stationId << "/" << t_StrEnterStation;

        memset(theApp.m_sTranstParam.PathCommDir,0,MAX_PATH);
        memcpy(theApp.m_sTranstParam.PathCommDir,theApp.m_strWorkDir.toLatin1().data(),sizeof(theApp.m_sTranstParam.PathCommDir));
        theApp.m_strWorkDir.length();

        theApp.RecordLog("[主进程响应],组件初始化接口(IF_InitEnvironment)，内部参数配置:"+t_StrEnterStation+"(区域编码:"+QString::number(theApp.m_sTranstParam.LaneConfigInfo.AreaID)+",路段编码:"+QString::number(theApp.m_sTranstParam.LaneConfigInfo.RoadID)+",站点编码:"+theApp.m_sTranstParam.LaneConfigInfo.StationID);

        //进行合法性校验
        theApp.m_bAuthorizedLegal = true;

        //獲取串口參數
        theApp.m_BaudRate = baudRate;
        memcpy(theApp.m_PortName,tty,sizeof(theApp.m_PortName));

        if (false == theApp.m_bIsInitEnvironment) {
            //启动业务管理线程
            QString strConfigFilePath;
            theApp.m_pThreadManage = new CThreadManage();

            if(NULL == theApp.m_pThreadManage)
                throw -12;


            theApp.m_Fun_ChargeResults = fun;                 //消息回调函数

            strConfigFilePath = theApp.m_strWorkDir+"\\"+FILE_NAME_APP_CONFIG;
            theApp.m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
            theApp.m_pThreadManage->SetParam(&theApp.m_sTranstParam);

            theApp.m_pThreadManage->StartWorkThreads(theApp.m_sTranstParam);

            //标记初始化状态
            theApp.m_bIsInitEnvironment = true;

            //返回结果
            bReturn = theApp.m_bIsInitEnvironment?true:false;

            theApp.start();

            qDebug()<<"theApp.start() out";
        }
        else {
            theApp.m_pThreadManage->SetParam(&theApp.m_sTranstParam);
            theApp.m_pThreadManage->updateTransParam(theApp.m_sTranstParam);

            bReturn = true;

            //标记初始化状态
            theApp.m_bIsInitEnvironment = true;
        }
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
        theApp.RecordLog(theApp.m_strLastErrorDesc);
        qDebug()<<strLog;
    }


    if(bReturn)
        theApp.RecordLog("[主进程响应],组件初始化接口(IF_InitEnvironment)调用结束--true");
    else
        theApp.RecordLog("[主进程响应],组件初始化接口(IF_InitEnvironment)调用结束--false");


    return bReturn;
}



/***************************************************
 * 函数名称：IF_Destroy
 * 函数功能：清理资源
 * 参   数：void
 * 返   回：void
 * *************************************************/
bool IF_Destroy(void)
{

    bool bReturn = false;
    QString strLog;
    try
    {
        quint64 dwSleepTime = 0;

        QString strLog;
        /**********************************************清理各种线程资源**********************************************/

        strLog = "[主进程响应],组件资源清理接口(IF_Destroy),正在开始退出后台线程...";
        //记录日志
        theApp.RecordLog(strLog);

        //线程退出标记
        quint32 dwThreadExitFlag = 0x00;
        //允许重试的次数
        int iAllowTryCount = 10;
        //已经重试次数
        int iHaveTryCount = 0;
        //每次重试的时候间隔时间（单位：毫秒）
        quint64	dwSpanTime = 100;

        //停止业务管理线程
        if(NULL != theApp.m_pThreadManage)
        {
            //设置该线程推出标记为真
            theApp.m_pThreadManage->m_bExit = true;


            strLog = "【业务管理线程】资源注销";
            //记录日志
            theApp.RecordLog(strLog);
        }

        strLog = "[主进程响应],组件即将退出...";
        theApp.RecordLog(strLog);
        //标记成功
        bReturn = true;
        theApp.quit();
    }
    catch(int& iErrorCode)
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
bool  IF_Authority( const char* szAuthKey, const int& iAuthKeySize)
{
    bool bReturn = false;

    return bReturn;

}

/***************************************************
 * 函数名称：IF_GetLastErrorDesc
 * 函数功能：获取上一次错误信息
 * 参   数：void
 * 返   回：char*
 * *************************************************/
const char* IF_GetLastErrorDesc(void)
{
    //AFX_MANAGE_STATE(AfxGetStaticModuleState());

    //临时处理
    return theApp.m_strLastErrorDesc.toLatin1();
}


/***************************************************
 * 函数名称：IF_StartGetAccountInfo
 * 函数功能：启动获取用户信息操作
 * 参   数：void
 * 返   回：char*
 * *************************************************/
bool IF_StartGetAccountInfo( const int& iOvertime)
{


    bool bReturn = false;
    QString strLog;
    theApp.RecordLog("[主进程响应],启动获取用户信息操作接口(IF_StartGetAccountInfo)调用开始");
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
        //给业务管理线程发送消息，要求控制硬件进行获取用户信息操作
        theApp.m_pThreadManage->SEND_WM_THREAD_DEVICE_START_GET_DATA(iOvertime,0x01);
        //标记成功
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        uchar byFunCode		= 0x03;
        uchar byExecuteState	= RCC_INNER_ERROR;
        switch(iErrorCode)
        {
        case -1:
            strLog = "[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件未成功初始化";
            byExecuteState	= RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog = "[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件未被授权";
            byExecuteState	= RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog = "[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件内部资源异常";
            break;
        }
        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            theApp.RecordLog(theApp.m_strLastErrorDesc);
        }
        //回馈调用者结果
        //        if(0 != m_sTranstParam.CallerThreadID)
        //        {
        //           // ::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
        //        }

        //        if(::IsWindow(m_sTranstParam.CallerWnd))
        //        {
        //            ::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
        //        }
        //保存日志
        theApp.RecordLog("[主进程响应],获取用户信息操作预处理失败，将结果通知调用者(功能代码:"+QString::number(byFunCode)+",执行状况:"+QString::number(byExecuteState));
    }
    theApp.RecordLog("[主进程响应],启动获取用户信息操作接口(IF_StartGetAccountInfo)调用结束");

    return bReturn;
}




/***************************************************
 * 函数名称：IF_StopGetAccountInfo
 * 函数功能：停止获取用户信息操作
 * 参   数：void
 * 返   回：char*
 * *************************************************/
bool  IF_StopGetAccountInfo(void)
{


    bool bReturn = false;
    QString strLog;
    theApp.RecordLog("[主进程响应],停止获取用户信息操作接口(IF_StopGetAccountInfo)调用开始");
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
        //给业务管理线程发送消息，要求控制硬件进行获取用户信息操作
        theApp.m_pThreadManage->SEND_WM_THREAD_DEVICE_STOP_GET_DATA();
        //标记成功
        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[主进程响应],停止获取用户信息操作(IF_StopGetAccountInfo)出错,组件未成功初始化";
            break;
        case -2:
            strLog = "[主进程响应],停止获取用户信息操作(IF_StopGetAccountInfo)出错,组件未被授权";
            break;
        case -3:
            strLog = "[主进程响应],停止获取用户信息操作(IF_StopGetAccountInfo)出错,组件内部资源异常";
            break;
        case -4:
            strLog = "[主进程响应],停止获取用户信息操作(IF_StopGetAccountInfo)出错,硬件未处于工作状态";
            break;
        }
        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            theApp.RecordLog(theApp.m_strLastErrorDesc);
        }

    }
    theApp.RecordLog("[主进程响应],停止获取用户信息操作接口(IF_StopGetAccountInfo)调用结束");
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
bool IF_GetAccountInfo( char* szParamContext, int& iParamSize, const int& iFormatType)
{


    bool bReturn = false;
    QString strLog;
    QString strDataString;
    theApp.RecordLog("[主进程响应],获取用户信息接口(IF_GetAccountInfo)调用开始,参数1:"+QString(szParamContext)+",参数2:"+QString::number(iParamSize)+",参数3:"+QString::number(iFormatType));
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
            //            strDataString.TrimRight();
        }
        if(strDataString.isEmpty())
            throw -7;
        theApp.RecordLog("[主进程响应],获取用户信息接口(IF_GetAccountInfo)输出数据:"+strDataString);
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
            theApp.RecordLog(theApp.m_strLastErrorDesc);
        }
    }
    theApp.RecordLog("[主进程响应],获取用户信息接口(IF_GetAccountInfo)调用结束");
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
bool IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bReturn = false;
    QString strLog = "[主进程响应],执行扣款操作接口(IF_DebitMoney)调用开始,参数1"+QString(szParamContext)+",参数2:"+QString::number(iParamSize)+",参数3:"+QString::number(iFormatType);

    qDebug() << strLog;

    QString strOperationMode;
    QString strDataString;
    QString strAnalyError;
    QDateTime t_Tick;

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


        strDataString = QString::fromLocal8Bit(szParamContext);

        if(1 == iFormatType)
        {

            CDVXMLDebitParamContext xml;
            if(!xml.Init(strDataString))
            {
                strAnalyError = xml.m_strLastErrorDesc;
                throw -4;
            }

            //复制数据
            theApp.m_sCurrentDebitInfo.OverTime			    = xml.OverTime;        //超时阀值
            theApp.m_sCurrentDebitInfo.OperationMode		= xml.OperationMode;   //1、整合 2、单体
            theApp.m_sCurrentDebitInfo.PayCertificateCode	= xml.PayCertificateCode;
            theApp.m_sCurrentDebitInfo.PayCertificateType	= xml.PayCertificateType;
            theApp.m_sCurrentDebitInfo.PayIdentifier		= xml.PayIdentifier;
            theApp.m_sCurrentDebitInfo.Money				= xml.Money;
            theApp.m_sCurrentDebitInfo.SubTime				= xml.SubTime;
            theApp.m_sCurrentDebitInfo.DebitMainType		= xml.DebitMainType;
            theApp.m_sCurrentDebitInfo.VehicleType			= xml.VehicleType;
            theApp.m_sCurrentDebitInfo.VehicleClass		    = xml.VehicleClass;
            theApp.m_sCurrentDebitInfo.VLColor				= xml.VLColor;
            theApp.m_sCurrentDebitInfo.VehicleLicense		= xml.VehicleLicense;
            theApp.m_sCurrentDebitInfo.AxisCount			= xml.AxisCount;
            theApp.m_sCurrentDebitInfo.TotalWeight			= xml.Weight;
            theApp.m_sCurrentDebitInfo.LimitWeight			= xml.LimitWeight;
            theApp.m_sCurrentDebitInfo.PassCertificateType  = xml.PassCertificateType;
            theApp.m_sCurrentDebitInfo.CardID				= xml.CardID;

            theApp.m_sCurrentDebitInfo.OperatorID           = xml.OperatorID;
            theApp.m_sCurrentDebitInfo.OperatorName         = xml.OperatorName;

            theApp.m_sCurrentDebitInfo.TollDate			    = xml.TollDate;
            theApp.m_sCurrentDebitInfo.ShiftID				= xml.ShiftID;
            theApp.m_sCurrentDebitInfo.EntryStationID		= xml.EntryStationID;
            theApp.m_sCurrentDebitInfo.EntryStationName	    = xml.EntryStationName;
            theApp.m_sCurrentDebitInfo.EntryTime			= xml.EntryTime;
            theApp.m_sCurrentDebitInfo.EntryLane			= xml.EntryLane;
            theApp.m_sCurrentDebitInfo.EntryVehicleLicense  = xml.EntryVehicleLicense;
            theApp.m_sCurrentDebitInfo.EntryVLColor		    = xml.EntryVLColor;
            theApp.m_sCurrentDebitInfo.EntryVehicleType	    = xml.EntryVehicleType;
            theApp.m_sCurrentDebitInfo.EntryVehicleClass	= xml.EntryVehicleClass;
            theApp.m_sCurrentDebitInfo.EntryRoadID			= xml.EntryRoadID;
            theApp.m_sCurrentDebitInfo.EntryAreaID			= xml.EntryAreaID;
            theApp.m_sCurrentDebitInfo.TicketNo			    = xml.TicketNo;
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
            //复制数据
            theApp.m_sCurrentDebitInfo.OverTime			    = pDebitMoney->OverTime;
            theApp.m_sCurrentDebitInfo.OperationMode		= pDebitMoney->OperationMode;
            theApp.m_sCurrentDebitInfo.PayCertificateCode	= pDebitMoney->PayCertificateCode;
            theApp.m_sCurrentDebitInfo.PayCertificateType	= pDebitMoney->PayCertificateType;
            theApp.m_sCurrentDebitInfo.PayIdentifier		= pDebitMoney->PayIdentifier;
            theApp.m_sCurrentDebitInfo.Money				= pDebitMoney->Money;
            theApp.m_sCurrentDebitInfo.SubTime				= pDebitMoney->SubTime;
            theApp.m_sCurrentDebitInfo.DebitMainType		= pDebitMoney->DebitMainType;
            theApp.m_sCurrentDebitInfo.VehicleType			= pDebitMoney->VehicleType;
            theApp.m_sCurrentDebitInfo.VehicleClass		    = pDebitMoney->VehicleClass;
            theApp.m_sCurrentDebitInfo.VLColor				= pDebitMoney->VLColor;
            theApp.m_sCurrentDebitInfo.VehicleLicense		= pDebitMoney->VehicleLicense;
            theApp.m_sCurrentDebitInfo.AxisCount			= pDebitMoney->AxisCount;
            theApp.m_sCurrentDebitInfo.TotalWeight			= pDebitMoney->Weight;
            theApp.m_sCurrentDebitInfo.LimitWeight			= pDebitMoney->LimitWeight;
            theApp.m_sCurrentDebitInfo.PassCertificateType  = pDebitMoney->PassCertificateType;
            theApp.m_sCurrentDebitInfo.CardID				= pDebitMoney->CardID;

            theApp.m_sCurrentDebitInfo.OperatorID           = pDebitMoney->OperatorID;
            theApp.m_sCurrentDebitInfo.OperatorName         = pDebitMoney->OperatorName;

            theApp.m_sCurrentDebitInfo.TollDate			    = pDebitMoney->TollDate;
            theApp.m_sCurrentDebitInfo.ShiftID				= pDebitMoney->ShiftID;
            theApp.m_sCurrentDebitInfo.EntryStationID		= pDebitMoney->EntryStationID;
            theApp.m_sCurrentDebitInfo.EntryStationName	    = pDebitMoney->EntryStationName;
            theApp.m_sCurrentDebitInfo.EntryTime			= pDebitMoney->EntryTime;
            theApp.m_sCurrentDebitInfo.EntryLane			= pDebitMoney->EntryLane;
            theApp.m_sCurrentDebitInfo.EntryVehicleLicense  = pDebitMoney->EntryVehicleLicense;
            theApp.m_sCurrentDebitInfo.EntryVLColor		    = pDebitMoney->EntryVLColor;
            theApp.m_sCurrentDebitInfo.EntryVehicleType	    = pDebitMoney->EntryVehicleType;
            theApp.m_sCurrentDebitInfo.EntryVehicleClass	= pDebitMoney->EntryVehicleClass;
            theApp.m_sCurrentDebitInfo.EntryRoadID			= pDebitMoney->EntryRoadID;
            theApp.m_sCurrentDebitInfo.EntryAreaID			= pDebitMoney->EntryAreaID;
            theApp.m_sCurrentDebitInfo.TicketNo			    = pDebitMoney->TicketNo;
            theApp.m_sCurrentDebitInfo.Valid				= true;
        }
        //特殊处理代码区域
        switch(theApp.m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            QString strSTID;
            switch(theApp.m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //广东华快的入口站号特殊处理
            case 30:
            case 31:
            {
                //  strSTID.Format(_T("%04d%03d%02d"),m_sCurrentDebitInfo.EntryAreaID,m_sCurrentDebitInfo.EntryRoadID,m_sCurrentDebitInfo.EntryStationID);
                //  m_sCurrentDebitInfo.EntryStationID = _ttoi(strSTID);
            }
                break;
            }
        }
            break;
        }

        qDebug()<<"OverTime: "<<QString::number(theApp.m_sCurrentDebitInfo.OverTime);

        //检测业务管理线程是否正常
        if(NULL == theApp.m_pThreadManage)
            throw -6;

        //reset tradeTime
        int overTime = theApp.m_sCurrentDebitInfo.OverTime/1000;
        time_t tradeBeginTime = theApp.m_dwTradeBeginOvertime;
        int intval = time(NULL) - tradeBeginTime;
        if (intval > overTime + 3) {
            theApp.m_bIsTrading = false;
        }

        theApp.m_dwTradeBeginOvertime = time(NULL);

        uint timeNow = QDateTime::currentDateTime().toTime_t();
        uint debitStartTime = theApp.m_sCurrentDebitInfo.m_consDebitTime;
        int checkIntval = timeNow - debitStartTime;

        if (checkIntval > overTime + 3)
            theApp.m_bIsTrading = false;

        //判断上次交易是否完成
        if(theApp.m_bIsTrading)
            throw -7;

        theApp.m_sCurrentDebitInfo.m_consDebitTime = timeNow;

        //标记当前交易正在进行
        theApp.m_bIsTrading = true;

        //save current payId
        theApp.m_payIdentifier = theApp.m_sCurrentDebitInfo.PayIdentifier;

        /*****************************响应操作***********************************/

        switch(theApp.m_sCurrentDebitInfo.OperationMode)
        {
        case 1://整合操作
        {
            strOperationMode = "整合模式";
            theApp.m_bCurrentDebitMode = true;
            //重构扣款XML
            QDomDocument doc;
            QDomText text;
            QDomText childtext;
            QDomElement element;
            QDomElement childelement;
            QDomProcessingInstruction instruction;

            instruction = doc.createProcessingInstruction( "xml","version=\'1.0\' encoding=\'GB2312\'" );
            doc.appendChild( instruction );

            QDomElement root = doc.createElement("DisplayContext");
            doc.appendChild(root);

            element = doc.createElement( "Version" );
            text = doc.createTextNode( "1.0" );
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "ProvinceID" );
            text = doc.createTextNode(QString::number(theApp.m_sTranstParam.ProvinceID));
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "VerifyCode" );
            text = doc.createTextNode(QString::number(0));
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "Data" );
            root.appendChild(element);

            childelement = doc.createElement( "Type" );
            childtext = doc.createTextNode( QString::number(0x03) );
            childelement.appendChild(childtext);
            element.appendChild(childelement);

            childelement = doc.createElement( "Money" );
            childtext = doc.createTextNode( QString::number(theApp.m_sCurrentDebitInfo.Money) );
            childelement.appendChild(childtext);
            element.appendChild(childelement);

            childelement = doc.createElement( "Weight" );
            childtext = doc.createTextNode( QString::number(theApp.m_sCurrentDebitInfo.TotalWeight) );
            childelement.appendChild(childtext);
            element.appendChild(childelement);

            childelement = doc.createElement( "VehicleType" );
            childtext = doc.createTextNode( QString::number(theApp.m_sCurrentDebitInfo.VehicleType) );
            childelement.appendChild(childtext);
            element.appendChild(childelement);

            childelement = doc.createElement( "VehicleClass" );
            childtext = doc.createTextNode( QString::number(theApp.m_sCurrentDebitInfo.VehicleClass) );
            childelement.appendChild(childtext);
            element.appendChild(childelement);

            childelement = doc.createElement( "Balance" );
            childtext = doc.createTextNode( QString::number(theApp.m_sCurrentDebitInfo.Balance) );
            childelement.appendChild(childtext);
            element.appendChild(childelement);


            t_Tick = QDateTime::currentDateTime();
            childelement = doc.createElement( "CurrentTime" );
            childtext = doc.createTextNode(t_Tick.toString("yyyy-MM-dd hh:mm:ss"));
            childelement.appendChild(childtext);
            element.appendChild(childelement);

            childelement = doc.createElement( "EntrySTName" );
            childtext = doc.createTextNode(theApp.m_sCurrentDebitInfo.EntryStationName);
            childelement.appendChild(childtext);
            element.appendChild(childelement);

            //进行资源申请
            QString* pstrString =NULL;
            pstrString = new QString();
            if(pstrString == NULL)
                throw -1;
            *pstrString  = doc.toString();

            (*pstrString).replace(QString("\n"), QString(""));
            (*pstrString).replace(QString("\r"), QString(""));

            //20170918
            Json::FastWriter write;
            Json::Value jroot;
            Json::Value DisplayContext;
            Json::Value Data;

            Data["Axis"] = theApp.m_sCurrentDebitInfo.AxisCount;
            Data["Money"] = theApp.m_sCurrentDebitInfo.Money;
            Data["Weight"] = theApp.m_sCurrentDebitInfo.TotalWeight;
            Data["VehicleType"] = theApp.m_sCurrentDebitInfo.VehicleType;
            Data["VehicleClass"] = theApp.m_sCurrentDebitInfo.VehicleClass;
            Data["Balance"] = theApp.m_sCurrentDebitInfo.Balance;
            QString strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            Data["CurrentTime"] = strTime.toStdString();
            Data["EntrySTName"] = theApp.m_sCurrentDebitInfo.EntryStationName.toStdString();

            DisplayContext["ProvinceID"] = theApp.m_sTranstParam.ProvinceID;
            DisplayContext["VerifyCode"] = 0;
            DisplayContext["Version"] = 1.0;
            DisplayContext["Data"] = Data;

            jroot["DisplayContext"] = DisplayContext;
            QString strFlow;
            strFlow.clear();
            strFlow = QString::fromStdString(write.write(jroot));

            //关闭上次的定时器
            theApp.m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
            theApp.m_pThreadManage->StopTimer(TIMER_ID_DEBIT_CHECK);
            //启动扣款超时定时器
            theApp.m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY,theApp.m_sCurrentDebitInfo.OverTime);

            //开始计时
            //                QDateTime t_Tick = QDateTime::currentDateTime();
            //                theApp.m_dwTradeBeginOvertime = t_Tick.toTime_t();
            //启动扣款流程

            theApp.m_pThreadManage->SEND_WM_THREAD_MANAGE_START_DEBIT_FLOW(strFlow);


            //添加描述
            theApp.m_strLastErrorDesc = "[主进程响应],执行扣款操作(IF_DebitMoney),业务正在处理(整合操作):"+QString::number(theApp.m_sCurrentDebitInfo.OverTime);
        }
            break;
        case 2://单体操作
        {
            strOperationMode = "单体模式";
            theApp.m_bCurrentDebitMode = false;
            //                //开始计时
            //                QDateTime t_Tick = QDateTime::currentDateTime();
            //                theApp.m_dwTradeBeginOvertime = t_Tick.toTime_t();
            //关闭上次的定时器
            theApp.m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
            theApp.m_pThreadManage->StopTimer(TIMER_ID_DEBIT_CHECK);
            //启动扣款超时定时器
            theApp.m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY,theApp.m_sCurrentDebitInfo.OverTime);
            //执行扣款操作
            theApp.m_pThreadManage->SEND_WM_THREAD_MANAGE_ONLY_DEBIT_MONEY();
            //添加描述
            theApp.m_strLastErrorDesc = "[主进程响应],执行扣款操作(IF_DebitMoney),业务正在处理(单体操作)";
        }
            break;
        }

        //记录日志
        qDebug()<<theApp.m_strLastErrorDesc;
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
            break;
        case -7:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),内部处理异常:上次交易未完成，不允许频繁调用";
            byExecuteState	= RCC_LAST_TRADE_NOT_END;
            break;
        case -8:
            strLog = "[主进程响应],执行扣款操作(IF_DebitMoney)出错("+strOperationMode+"),内部处理异常:资源申请失败";
            break;
        }

        qDebug()<<strLog;
        //        //保存错误描述
        //        if(strLog != theApp.m_strLastErrorDesc)
        //        {
        //            theApp.m_strLastErrorDesc = strLog;
        //            theApp.RecordLog(theApp.m_strLastErrorDesc);
        //        }

        //保存日志
        theApp.RecordLog("[主进程响应],扣款操作预处理失败，将结果通知调用者(功能代码:"+QString::number(byFunCode)+",执行状况:"+QString::number(byExecuteState)+")");
    }
    theApp.RecordLog("[主进程响应],执行扣款操作接口(IF_DebitMoney)调用结束");

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

bool IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    //    if(GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist)
    //    {
    //       theApp.RecordLog("网络异常");
    //       qDebug()<<"网络异常";

    //       return false;
    //    }

    theApp.m_bNeedPWD = true;

    bool bReturn = false;
    QString strLog ;
    strLog = "[主进程响应],执行撤单操作接口(IF_DebitCancel)调用开始,参数1:"+QString(szParamContext)+",参数2:"+QString::number(iParamSize)+",参数3:"+QString::number(iFormatType);


    theApp.RecordLog(strLog);

    QString strDataString;
    QString strPayIdentifier;
    int cancelType = 1;

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

        strDataString = QString::fromLocal8Bit(szParamContext);

        // XML
        if(1 == iFormatType)
        {
            if(!xml.Init(strDataString))
                throw -4;
            strPayIdentifier = xml.PayIdentifier;
        }
        // JSON
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
            cancelType = pDebitCancel->m_type;
        }
        //检测业务管理线程是否正常
        if(NULL == theApp.m_pThreadManage)
            throw -6;
        //检测撤单交易标识是否合法
        //临时调试
        if(strPayIdentifier.isEmpty())
            throw -7;
        //进行资源申请
        QString pstrString ;
        pstrString = strPayIdentifier;

        // update type/payId to db
  //      TWSUtils::addDebitCancelId2DB(strPayIdentifier, cancelType);

        theApp.m_pThreadManage->SEND_WM_THREAD_MANAGE_DEBIT_CANCEL(pstrString);

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
        //        //保存错误描述
        //        if(strLog != theApp.m_strLastErrorDesc)
        //        {
        //            theApp.m_strLastErrorDesc = strLog;
        //            theApp.RecordLog(theApp.m_strLastErrorDesc);
        //        }
        //        //回馈调用者结果
        //        if(0 != m_sTranstParam.CallerThreadID)
        //        {
        //            ::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
        //        }

        //        if(::IsWindow(m_sTranstParam.CallerWnd))
        //        {
        //            ::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
        //        }
        //保存日志
        theApp.RecordLog("[主进程响应],撤单操作预处理失败，将结果通知调用者(功能代码:"+QString::number(byFunCode)+",执行状况:"+QString::number(byExecuteState)+")");
    }
    theApp.RecordLog("[主进程响应],执行撤单操作接口(IF_DebitCancel)调用结束");
    return bReturn;
}


/***************************************************
 * 函数名称：IF_GetDebitResult
 * 函数功能：获取扣款/二次扣款结果
 * 参   数： IN OUT char* szParamContext
         ：IN OUT int& iParamSize
         ：IN const int& iFormatType
 * 返   回：char*
 * *************************************************/
bool IF_GetDebitResult( char* szParamContext, int& iParamSize, const int& iFormatType)
{
    qDebug() << "~~~~~~into IF_GetDebitResult~~~~~~";

    bool bReturn = false;
    QString strLog;
    QString strDataString;

    theApp.RecordLog("[主进程响应],获取扣款结果接口(IF_GetDebitResult)调用开始,参数1:"+QString(szParamContext)+",参数2:"+QString::number(iParamSize)+",参数3:"+QString::number(iFormatType));
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
            xml.XMLFunctionDesc = "COMMAND";
            xml.XMLVersion      = theApp.m_XMLVersion;
            xml.ResultType		= theApp.m_sCurrentDebitResult.ResultType;
            xml.ExecuteCode		= theApp.m_sCurrentDebitResult.ExecuteCode;
            xml.PayPlatformType = theApp.m_sCurrentDebitResult.PayPlatformType;
            xml.DebitTime		= QDateTime::fromString(theApp.m_sCurrentDebitResult.DebitTime, "yyyy-MM-dd hh:mm:ss").toTime_t();
            xml.ConsumeTime		= theApp.m_sCurrentDebitResult.ConsumeTime;
            xml.ExecuteDesc = theApp.m_sCurrentDebitResult.ExecuteDesc;
            xml.TradeSecurityCode = theApp.m_sCurrentDebitResult.TradeSecurityCode;
            xml.PayIdentifier = theApp.m_sCurrentDebitResult.PayIdentifier;
            xml.DebitOrder = theApp.m_sCurrentDebitResult.DebitOrder;

            strDataString = xml.GetXMLString();

            strDataString.replace(QString("\r"), QString(""));
            strDataString.replace(QString("\n"), QString(""));

        }
            break;
        case 2:
        {
            CDVJsonParamDebitResult jsonDebitResult;
            jsonDebitResult.ResultType		= theApp.m_sCurrentDebitResult.ResultType;
            jsonDebitResult.ExecuteCode		= theApp.m_sCurrentDebitResult.ExecuteCode;
            jsonDebitResult.PayPlatformType         = theApp.m_sCurrentDebitResult.PayPlatformType;

            // ServerDebitTime
            jsonDebitResult.DebitTime       = theApp.m_sCurrentDebitResult.DebitTime;
            jsonDebitResult.ConsumeTime		= theApp.m_sCurrentDebitResult.ConsumeTime;
            //                jsonDebitResult.ExecuteDesc             = theApp.m_sCurrentDebitResult.ExecuteDesc;
            jsonDebitResult.ExecuteDesc             = "";

            jsonDebitResult.TradeSecurityCode       = theApp.m_sCurrentDebitResult.TradeSecurityCode;
            jsonDebitResult.PayIdentifier           = theApp.m_sCurrentDebitResult.PayIdentifier;
            jsonDebitResult.DebitOrder              = theApp.m_sCurrentDebitResult.DebitOrder;

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


        theApp.RecordLog("[主进程响应],获取扣款结果接口(IF_GetDebitResult)输出结果如下: "+strDataString);

        iParamSize = strDataString.length();
        memcpy(szParamContext,strDataString.toLatin1().data(),strDataString.length()+1);
        strDataString.clear();
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
            strLog = "[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,容器太少";
            break;
        }
        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            theApp.RecordLog(theApp.m_strLastErrorDesc);
        }

        qDebug()<<strLog;
    }
    theApp.RecordLog("[主进程响应],获取扣款结果接口(IF_GetDebitResult)调用结束");
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
bool  IF_SetMMI( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    qDebug() << "~~~~~~into IF_SetMMI~~~~~~";

    bool bReturn = false;
    bool ClearMMISign = false;
    QString strLog = "[主进程响应],设置交互界面接口(IF_SetMMI)调用开始,参数1:"+QString(szParamContext)+",参数2:"+QString::number(iParamSize)+",参数3:"+QString::number(iFormatType);

    theApp.RecordLog(strLog);

    QString strParamString(szParamContext);
    QString strDataString;
    QString strSubError;
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

        //获取硬件状态
        int dwHDState = theApp.m_pThreadManage->m_pThreadDevice->GetDeviceState();


        if(0x01 == iFormatType)
        {
            //XML格式
            CDVXMLMMI xml;
            if(!xml.Init(strParamString))
            {
                strSubError = xml.m_strLastErrorDesc;
                throw -7;
            }

            //重构XML数据
            QDomDocument doc;
            QDomText text;
            QDomText childtext;
            QDomElement element;
            QDomElement childelement;
            QDomProcessingInstruction instruction;

            instruction = doc.createProcessingInstruction( "xml","version=\'1.0\' encoding=\'GB2312\'" );
            doc.appendChild( instruction );

            QDomElement root = doc.createElement( "DisplayContext" );
            doc.appendChild(root);

            element = doc.createElement( "Version" );
            text = doc.createTextNode( "1.0" );
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "ProvinceID" );
            text = doc.createTextNode( QString::number(theApp.m_sTranstParam.ProvinceID) );
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "VerifyCode" );
            text = doc.createTextNode( QString::number(0) );
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "Data" );
            root.appendChild(element);


            if(xml.ClearMMI > 0)
            {
                ClearMMISign = true;
                int iMMIType = 0x04;
                switch(xml.ClearMMI)
                {
                case 1:
                    iMMIType = 0x04;
                    break;
                case 2:
                    iMMIType = 0x05;
                    break;
                case 3:
                    iMMIType = 0x06;
                    break;
                }

                childelement = doc.createElement( "Type" );
                childtext = doc.createTextNode( QString::number(iMMIType) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

            }
            else
            {

                childelement = doc.createElement( "Type" );
                childtext = doc.createTextNode( QString::number(0x01) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                //获取金额、车型车种、重量、限重
                int iMoney  = 0x00;
                int iWeight = 0x00;
                int iVType  = 0x00;
                int iVClass = 0x00;

                int iLimit  = 0x00;
                int iAxis   = 0x00;
                int iBalance= 0x00;
                QString strEntrySTName;
                for(int i = 0;i<xml.Text.size();++i)
                {
                    int iTemp = 0x00;
                    switch(xml.Text.at(i).Sub)
                    {
                    case 1://车型
                    {
                        iTemp = QString(xml.Text.at(i).Context).toInt();
                        if(iTemp>10)
                        {
                            iVType = 0x02;
                        }
                        else
                        {
                            iVType = 0x01;
                        }
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
                        strEntrySTName = xml.Text.at(i).Context;
                    }
                        break;
                    }
                }

                childelement = doc.createElement( "Money" );
                childtext = doc.createTextNode( QString::number(iMoney) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "Weight" );
                childtext = doc.createTextNode( QString::number(iWeight) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "VehicleType" );
                childtext = doc.createTextNode( QString::number(iVType) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "VehicleClass" );
                childtext = doc.createTextNode( QString::number(iVClass) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "LimitWeight" );
                childtext = doc.createTextNode( QString::number(iLimit) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);


                childelement = doc.createElement( "AixsCount" );
                childtext = doc.createTextNode( QString::number(iAxis) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "Balance" );
                childtext = doc.createTextNode( QString::number(iBalance) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "EntrySTName" );
                childtext = doc.createTextNode( strEntrySTName );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "CurrentTime" );
                childtext = doc.createTextNode( QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") );
                childelement.appendChild(childtext);
                element.appendChild(childelement);
            }

            strDataString = doc.toString();

        }
        else
        {
            //JSON格式
            CDVJsonParamMMI* pJsonMMI = NULL;
            CJsonParamOperate jsonOperation;

            pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strParamString,JSON_DT_PARAM_MMI);
            if(NULL == pJsonMMI)
            {
                strSubError = jsonOperation.GetLastErrorDesc();
                throw -8;
            }

            //重构XML数据
            QDomDocument doc;
            QDomText text;
            QDomText childtext;
            QDomElement element;
            QDomElement childelement;
            QDomProcessingInstruction instruction;

            instruction = doc.createProcessingInstruction( "xml","version=\'1.0\' encoding=\'GB2312\'" );
            doc.appendChild( instruction );

            QDomElement root = doc.createElement( "DisplayContext" );
            doc.appendChild(root);

            element = doc.createElement( "Version" );
            text = doc.createTextNode( "1.0" );
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "ProvinceID" );
            text = doc.createTextNode( QString::number(theApp.m_sTranstParam.ProvinceID) );
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "VerifyCode" );
            text = doc.createTextNode( QString::number(0) );
            element.appendChild(text);
            root.appendChild(element);

            element = doc.createElement( "Data" );
            root.appendChild(element);


            if(pJsonMMI->ClearMMI > 0)
            {
                ClearMMISign = true;
                int iMMIType = 0x04;
                switch(pJsonMMI->ClearMMI)
                {
                case 1:
                    iMMIType = 0x04;
                    break;
                case 2:
                    iMMIType = 0x05;
                    break;
                case 3:
                    iMMIType = 0x06;
                    break;
                }

                childelement = doc.createElement( "Type" );
                childtext = doc.createTextNode( QString::number(iMMIType) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);
            }
            else
            {
                childelement = doc.createElement( "Type" );
                childtext = doc.createTextNode( QString::number(0x01) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);
                //获取金额、车型车种、重量、限重
                int iMoney  = 0x00;
                int iWeight = 0x00;
                int iVType  = 0x00;
                int iVClass = 0x00;
                int iLimit  = 0x00;
                int iAxis   = 0x00;
                int iBalance= 0x00;
                QString strEntrySTName;
                for(int i = 0;i<pJsonMMI->Text.size();++i)
                {
                    int iTemp = 0x00;
                    switch(pJsonMMI->Text.at(i).Sub)
                    {
                    case 1://车型
                    {
                        iTemp = QString(pJsonMMI->Text.at(i).Context).toInt();
                        if(iTemp>10)
                        {
                            iVType = 0x02;
                        }
                        else
                        {
                            iVType = 0x01;
                        }
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
                        strEntrySTName = pJsonMMI->Text.at(i).Context;
                    }
                        break;
                    }
                }

                childelement = doc.createElement( "Money" );
                childtext = doc.createTextNode( QString::number(iMoney) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "Weight" );
                childtext = doc.createTextNode( QString::number(iWeight) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "VehicleType" );
                childtext = doc.createTextNode( QString::number(iVType) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "VehicleClass" );
                childtext = doc.createTextNode( QString::number(iVClass) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "LimitWeight" );
                childtext = doc.createTextNode( QString::number(iLimit) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);


                childelement = doc.createElement( "AixsCount" );
                childtext = doc.createTextNode( QString::number(iAxis) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "Balance" );
                childtext = doc.createTextNode( QString::number(iBalance) );
                childelement.appendChild(childtext);
                element.appendChild(childelement);


                childelement = doc.createElement( "EntrySTName" );
                childtext = doc.createTextNode( strEntrySTName );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

                childelement = doc.createElement( "CurrentTime" );
                childtext = doc.createTextNode( QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") );
                childelement.appendChild(childtext);
                element.appendChild(childelement);

            }
            strDataString = doc.toString();
        }

        if(strDataString.isEmpty())
            throw -9;

        strDataString.replace(QString("\n"), QString(""));
        qDebug()<<"setMMi:"<<strDataString;

        //获取数据

        if(ClearMMISign)
            theApp.m_pThreadManage->SEND_WM_THREAD_DEVICE_CLEAR(3,"");
        else
            theApp.m_pThreadManage->SEND_WM_THREAD_DEVICE_SHOW_DEAL_INFO(strDataString);
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
        case -4:
        case -5:
        case -6:
        case -10:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,组件内部资源异常";
            break;
        case -7:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(XML格式)有误，详细描述:"+strSubError;
            break;
        case -8:
            strLog = "[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(JSON格式)有误，详细描述:"+strSubError;
            break;
        }

        qDebug()<<strLog;

        //保存错误描述
        if(strLog != theApp.m_strLastErrorDesc)
        {
            theApp.m_strLastErrorDesc = strLog;
            theApp.RecordLog(theApp.m_strLastErrorDesc);
        }
    }
    theApp.RecordLog("[主进程响应],设置交互界面接口(IF_SetMMI)调用结束");
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
bool IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize)
{
    //	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    QString strLog = "[主进程响应],获取硬件参数接口(IF_GetParam)暂不提供此项功能";

    if(strLog != theApp.m_strLastErrorDesc)
    {
        theApp.m_strLastErrorDesc = strLog;
        theApp.RecordLog(theApp.m_strLastErrorDesc);
    }
    return bReturn;
}



/***************************************************
 * 函数名称：IF_GetComponentStatus
 * 函数功能：检测组件状态
 * 参   数： IN OUT char* szParamContext
         ： IN OUT int& iParamSize
         ： IN const int& iFormatType
 * 返   回：char*
 * *************************************************/
bool IF_GetComponentStatus( unsigned int& uStatus)
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

        //支付子平台网络状态
        if(theApp.m_bNetErrorHttpServerInTollNetNotExist)
        {
            dwState |= DEVICE_STATE_PAY_WEB;
        }
        //支付子平台服务器状态
        if(theApp.m_bNetErrorHttpServerTrawePlatformNotExist)
        {
            dwState |= DEVICE_STATE_PAY_SERVER;
        }
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
bool IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    // AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    QString strLog = "[主进程响应],执行传输关键信息数据接口(IF_TranslateData)暂不提供此项功能";
    if(strLog != theApp.m_strLastErrorDesc)
    {
        theApp.m_strLastErrorDesc = strLog;
        theApp.RecordLog(theApp.m_strLastErrorDesc);
    }
    return bReturn;
}




