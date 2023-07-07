#include "cthreadmanage.h"
#include "twsdnetpay.h"
#include <QtSql>



void CThreadManage::run()
{
    //创建硬件控制线程
    if(NULL == m_pThreadDevice)
    {
        m_pThreadDevice = new CThreadDevice();
    }

    if(NULL != m_pThreadDevice)
    {
        tagTranstParam sTranstParam = {0};
        memcpy(&sTranstParam,&m_sTranstParam,sizeof(tagTranstParam));

        QString strConfigFilePath;
        strConfigFilePath = GetGlobalApp()->m_strWorkDir +"\\"+FILE_NAME_APP_CONFIG;
        m_pThreadDevice->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadDevice->SetParam(&sTranstParam);

        //启动硬件控制线程
        m_pThreadDevice->start();

        GetGlobalApp()->WriteLogFile("业务管理线程初始化完毕");

    }

    exec();
}

CThreadManage::CThreadManage(void):
    m_pThreadDevice(NULL),
    m_bAccountInfoIsReady(false),
    m_bDeviceLoadState(false)

{
    memset(&m_sAccountInfo,0x00,sizeof(tagAccountInfo));
    memset(&lastAccountInfo, 0x00, sizeof(tagAccountInfo)) ;

    connect(this, SIGNAL(WM_THREAD_MANAGE_SHOW_DEAL_INFO(QString , int)), this, SLOT(OnMsgShowInfo(QString , int)));
    connect(this, SIGNAL(WM_THREAD_MANAGE_START_DEBIT_FLOW(QString , int)), this, SLOT(OnMsgManageStartDebitFlow(QString, int)));
    connect(this, SIGNAL(WM_THREAD_MANAGE_DEBIT_CANCEL(QString , int)), this, SLOT(OnMsgDebitCancel(QString , int)));


    //定时器连接
    connect(&m_nTimerIDForDebitMoney,SIGNAL(timeout()),this,SLOT(TimerIDForDebitMoneyProc()));
    connect(&m_nTimerIDForDeviceState,SIGNAL(timeout()),  this, SLOT(TimerIDforDeviceState()));
    connect(&m_nTimerIDForThreadActive, SIGNAL(timeout()), this , SLOT(TimerIDForThreadActive()));
    connect(&m_nTimerIDForDeleteLogFile, SIGNAL(timeout()), this, SLOT(TimerIDForDeleteLogFile()));


    StartTimer(TIMER_ID_DEVICE_STATE, GetGlobalApp()->m_dwThreadCheckSpanTime * 1000);
    StartTimer(TIMER_ID_THREAD_MANAGE, GetGlobalApp()->m_dwThreadCheckSpanTime * 1000);
    StartTimer(TIMER_ID_DELETE_LOG_FILE, GetGlobalApp()->m_iDeleteLogSpanTime *1000);


}

CThreadManage::~CThreadManage()
{
    if(NULL != m_pThreadDevice)
    {
        delete m_pThreadDevice;
        m_pThreadDevice = NULL;
    }

}



/***************************************************
 * 函数名称：StartTimer
 * 函数功能：设置定时器
 * 参   数：nEventID  dwDelayTime
 * 返   回：void
 * *************************************************/
void CThreadManage::StartTimer( const quint32& nEventID, const quint64& dwDelayTime)
{
    switch(nEventID)
    {
    case TIMER_ID_DEBIT_MONEY://扣款流程定时器
        m_nTimerIDForDebitMoney.start(dwDelayTime);
        break;
    case TIMER_ID_DEVICE_STATE:
        m_nTimerIDForDeviceState.start(dwDelayTime);
        break;
    case TIMER_ID_THREAD_MANAGE:
        m_nTimerIDForThreadActive.start(dwDelayTime);
        break;
    case TIMER_ID_DELETE_LOG_FILE:
        m_nTimerIDForDeleteLogFile.start(dwDelayTime);
        break;

    }
}


/***************************************************
 * 函数名称：StopTimer
 * 函数功能：停止定时器
 * 参   数：nEventID
 * 返   回：void
 * *************************************************/
void CThreadManage::StopTimer( const quint32& nEventID)
{
    switch(nEventID)
    {
    case TIMER_ID_DEBIT_MONEY://扣款流程定时器
        if(m_nTimerIDForDebitMoney.isActive())
        {
            m_nTimerIDForDebitMoney.stop();
        }
        break;
    case TIMER_ID_DEVICE_STATE:
        if(m_nTimerIDForDeviceState.isActive())
        {
            m_nTimerIDForDeviceState.stop();
        }
        break;
    case TIMER_ID_THREAD_MANAGE:
        if(m_nTimerIDForThreadActive.isActive())
        {
            m_nTimerIDForThreadActive.stop();
        }
        break;
    case TIMER_ID_DELETE_LOG_FILE:
        if(m_nTimerIDForDeleteLogFile.isActive())
        {
            m_nTimerIDForDeleteLogFile.stop();
        }
        break;

    default:
        break;
    }
}



/***************************************************
 * 函数名称：StartWorkThreads
 * 函数功能：启动工作线程
 * 参   数：tagTranstParam
 * 返   回：void
 * *************************************************/

//const bool CThreadManage::StartWorkThreads( const tagTranstParam& sTranstParam)
//{
//    QString strLog;

//    strLog = "into StartWorkThreads";
//    bool bReturn = false;
//    try
//    {
//        /*******************启动相应线程*******************/


//        //沉默200毫秒
//        QThread::msleep(200);

//    }
//    catch(int& iErrorCode)
//    {
//        switch(iErrorCode)
//        {
//        case -1:
//            strLog = "业务管理线程初始化, 异常错误 :  内部资源申请失败";
//            break;
//        case -2:
//            strLog = "业务管理线程初始化，异常错误：硬件控制线程初始化错误";
//            break;
//        }
//        GetGlobalApp()->WriteLogFile(strLog);

//    }
//    return bReturn;
//}

/***************************************************
 * 函数名称：SetAccountDataReadyState
 * 函数功能：设置用户数据是否准备就绪
 * 参   数：nEventID
 * 返   回：void
 * *************************************************/
void CThreadManage::SetAccountDataReadyState( const bool& bState)
{
    m_bAccountInfoIsReady = bState;
    if(!bState)
    {
        memset(&m_sAccountInfo,0,sizeof(tagAccountInfo));
    }
}


/***************************************************
 * 函数名称：FinishTrade
 * 函数功能：交易结束的处理
 * 参   数：void
 * 返   回：void
 * *************************************************/
QString CThreadManage::FinishTrade( const bool& bNormal)
{
    QString strPayIdentifier = GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier;
    GetGlobalApp()->m_bIsTrading		= false;
    GetGlobalApp()->m_bCurrentDebitMode = false;

    //关闭扣款超时定时器
    StopTimer(TIMER_ID_DEBIT_MONEY);

    //超时截至导致的交易结束，需要自动发起撤单
    if(!bNormal)
    {

        //清理扣款相关资源
        GetGlobalApp()->m_sCurrentDebitInfo.Valid= false;
        GetGlobalApp()->m_sCurrentDebitResult.Valid = false;
        //构造撤单数据
        Json::Value root;
        Json::Value DebitCancelParamContext;
        Json::Value Data;
        // 标识关键数据项
        Json::Value KeyItem;

        KeyItem["PayIdentifier"]			= Json::Value(strPayIdentifier.toStdString().c_str());
        Data["TradeKeyItem"]				= KeyItem;

        DebitCancelParamContext["Data"]		= Data;
        DebitCancelParamContext["Version"]	= Json::Value(("1.0"));

        root["DebitCancelParamContext"]		= DebitCancelParamContext;

        //转化为字符串
        Json::FastWriter fast_writer;
        QString strData  = QString("%1").arg(fast_writer.write(root).c_str());


        if(NULL != m_pThreadDevice)
        {
            //发送撤单请求
            m_pThreadDevice->SEND_WM_THREAD_DEVICE_DEBIT_CANCEL(strData,0x00);
        }
    }
    //标识扣款流程结束
    if(GetGlobalApp()->m_bCurrentDebitMode)
    {
        GetGlobalApp()->m_iDebitTaskNo = 0x00;
    }

    return strPayIdentifier;
}



/***************************************************
 * 函数名称：OnMsgManageStartDebitFlow
 * 函数功能：接收到消息，启动扣款流程（整合操作模式）
 * 参   数：void
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgManageStartDebitFlow(QString wParam, int lParam)
{
    theApp.WriteLogFile("CThreadManage ：收到消息，进行扣款流程，OnMsgManageStartDebitFlow");

    QString strLog;
    try
    {
        //关闭上次的定时器
        StopTimer(TIMER_ID_DEBIT_MONEY);
        //启动扣款超时定时器
        StartTimer(TIMER_ID_DEBIT_MONEY,theApp.m_sCurrentDebitInfo.OverTime);

        //标记扣款流程开始
        GetGlobalApp()->m_iDebitTaskNo = 0x01;

        if(NULL == m_pThreadDevice)
            throw -2;

        //清除上一次的用户信息
        memset(&m_sAccountInfo,0,sizeof(tagAccountInfo));
        //通知硬件控制线程
        m_pThreadDevice->SEND_WM_THREAD_DEVICE_START_DEBIT_FLOW(wParam, lParam);

        //记录日志
        GetGlobalApp()->WriteLogFile(("[业务管理线程]:外部主动调用扣款操作(整合操作)，交易标识:"+GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier));

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[业务管理线程]: pstrXML is  Empty() ";
            break;
        case -2:
            strLog = "[业务管理线程]: m_pThreadDevice is  NULL ";
            break;
        }
        GetGlobalApp()->WriteLogFile(strLog);
    }
}



/***************************************************
 * 函数名称：OnMsgDebitCancel
 * 函数功能：执行撤单操作
 * 参   数：void
 * 返   回：void
 * *************************************************/


void CThreadManage::OnMsgDebitCancel(QString wParam, int lParam)
{
    GetGlobalApp()->WriteLogFile("into CThreadManage::OnMsgDebitCancel");

    QString strLog;
    QString pstrPayIdentifier = wParam;
    //int nType = lParam;
    try
    {
        if( pstrPayIdentifier.isEmpty())
            throw -1;

        if(NULL == m_pThreadDevice)
            throw -2;

        //关闭检查扣款状态定时器
        StopTimer(TIMER_ID_DEBIT_MONEY);

        //重置扣款模式
        GetGlobalApp()->m_bCurrentDebitMode = false;
        //标识交易处理完成
        GetGlobalApp()->m_bIsTrading = false;

        //构造撤单数据并通知通讯线程开始工作
        QString strCancelInfo = (GetGlobalApp()->ConstructCancleInfo(0x02,pstrPayIdentifier));
        //递交撤单
        m_pThreadDevice->SEND_WM_THREAD_DEVICE_DEBIT_CANCEL(strCancelInfo, 0x02);

        //沉默800毫秒后清屏
        msleep(800);
        QString strClean = QString(GetGlobalApp()->GetClearScreenJson());
        m_pThreadDevice->SEND_WM_THREAD_DEVICE_SHOW_DEAL_INFO(strClean,204);

        //记录日志
        GetGlobalApp()->WriteLogFile(("[业务管理线程]:外部主动调用撤单操作，交易标识:"+pstrPayIdentifier));


    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[业务管理线程]:外部主动调用撤单接口,传入参数(交易识别码)指针为空";
            break;
        case -2:
            strLog = "[业务管理线程]:智能控制指针为空";
            break;
        }
        //缓存日志
        if(strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            GetGlobalApp()->WriteLogFile(m_strLastErrorDesc);
        }
    }

}


/***************************************************
 * 函数名称：OnMsgShowInf
 * 函数功能：执行显示操作
 * 参   数：QString
 * 返   回：void
 * *************************************************/


void CThreadManage::OnMsgShowInfo(QString wParam, int lParam)
{
    QString strLog;
    try
    {
        if(wParam.isEmpty())
            throw -1;
        if(NULL == m_pThreadDevice)
            throw -2;

        m_pThreadDevice->SEND_WM_THREAD_DEVICE_SHOW_DEAL_INFO(wParam, lParam);

        //记录日志
        GetGlobalApp()->WriteLogFile(("[业务管理线程]:外部主动调用显示操作"));

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[业务管理线程]:参数无效 ";
            break;
        case -2:
            strLog = "[业务管理线程]: 智能控制指针为空 ";
            break;
        }
        GetGlobalApp()->WriteLogFile(strLog);
    }

}


/***************************************************
 * 函数名称：ResourceDestroy
 * 函数描述：资源销毁
 * 函数参数：void
 * 函数返回：void
 * *************************************************/
void CThreadManage::ResourceDestroy(void)
{
    QString strLog;
    try
    {
        strLog = "[业务管理线程],正在开始退出工作线程...";
        //记录日志
        GetGlobalApp()->WriteLogFile(strLog);

        /**********************************/
        StopTimer(TIMER_ID_DEBIT_MONEY);
        StopTimer(TIMER_ID_DEVICE_STATE);
        StopTimer(TIMER_ID_THREAD_MANAGE);
        StopTimer(TIMER_ID_DELETE_LOG_FILE);

        /******************硬件线程注销********************/
        if(NULL != m_pThreadDevice)
        {
            m_pThreadDevice->ResourceDestroy();
            m_pThreadDevice->quit();
            m_pThreadDevice->wait();
        }

        strLog = "[业务管理线程],退出工作线程结束...";
        //记录日志
        GetGlobalApp()->WriteLogFile(strLog);
    }
    catch(int& )
    {

    }

}

/***************************************************
  * 函数名称：CheckAndDeleteLogFile
  * 函数描述：日志检查定时器处理--删除
  * 函数参数：void
  * 函数返回：void
  * *************************************************/
void CThreadManage::CheckAndDeleteLogFile()
{
    GetGlobalApp()->WriteLogFile(("日志检查定时器处理"));


    //   QString path = QDir::currentPath() + "/log";
    //江苏版，指定了日志的路径 LOGOUTPATH
    QDir dir(LOGOUTPATH);
    //若目录不存在则返回退出
    if (!dir.exists())
        return;
    //设置过滤器(目录，文件或非上级目录)
    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    //当前时间
    time_t  t_CurrentTime = QDateTime::currentDateTime().toTime_t();
    //取得目录中文件列表(包含目录)
    QFileInfoList list = dir.entryInfoList();
    int i=0;
    do{
        QFileInfo fileInfo = list.at(i);
        //判断是否为目录，如果是目录则遍历，否则当前处理文件
        bool bisDir=fileInfo.isDir();

        if(!bisDir)
        {
            //取得文件类型后缀
            QString t_StrSuffix = fileInfo.suffix().toLower();
            bool t_BoolSuffix = (t_StrSuffix =="log");

            //取得文件名称
            QString t_FileName=fileInfo.fileName();
            QString t_strFileName=t_FileName.mid(0,13);
            bool    t_BoolFileName = (t_strFileName=="LogTWSDNetPay");

            time_t  t_FileTime = fileInfo.created().toTime_t();

            if(t_BoolSuffix && t_BoolFileName)
            {
                if((t_CurrentTime-t_FileTime)>LOGOUTTIME)
                    // if((t_CurrentTime-t_FileTime)>(60*60))
                {
                    //记录日志
                    GetGlobalApp()->WriteLogFile(("删除日志路径："+fileInfo.filePath()));

                    //刪除文件
                    QFile::remove(fileInfo.filePath());
                }
            }
        }
        i++;
    }while(i<list.size());

}



/***************************************************
 * 函数名称：TimerIDForDebitMoneyProc
 * 函数描述：定时器超时处理--扣费
 * 函数参数：void
 * 函数返回：void
 * *************************************************/
void CThreadManage::TimerIDForDebitMoneyProc()
{
    StopTimer(TIMER_ID_DEBIT_MONEY);
    //交易因为超时截至而完毕
    QString strTradeID = FinishTrade(false);
    //记录日志
    QString strLog;
    strLog = ("[业务管理线程]:扣款操作超时截至（预定超时,"+
              QString::number(::GetGlobalApp()->m_sCurrentDebitInfo.OverTime/1000)+
              " 秒），自动调用撤单操作，交易标识:"+strTradeID);
    GetGlobalApp()->WriteLogFile(strLog);

    QString strDebitInfo = ( GetGlobalApp()->ConstructCancleInfo(0x00, strTradeID));

    if(NULL != m_pThreadDevice)
    {
        //递交撤单
        m_pThreadDevice->SEND_WM_THREAD_DEVICE_DEBIT_CANCEL(strDebitInfo,0x00);
    }


    //通知调用者返回结果，当前操作失败
    GetGlobalApp()->NotifyCaller(0x01, 0x06);

}

void CThreadManage::TimerIDforDeviceState()
{
    if(NULL != m_pThreadDevice)
    {
        m_bDeviceLoadState = m_pThreadDevice->GetDeviceInitState();
    }

}

void CThreadManage::TimerIDForThreadActive()
{
    static unsigned int nPrintLog = 0;
    if(1 == nPrintLog%10)
    {
        ++nPrintLog;
        GetGlobalApp()->WriteLogFile( "[业务管理线程],线程存活日志检测");
    }

}

void CThreadManage::TimerIDForDeleteLogFile()
{
    CheckAndDeleteLogFile();
}



bool CThreadManage::SendBreakDebit(const QString& payID)
{
    if(NULL != m_pThreadDevice)
        return m_pThreadDevice->BreakDebit(payID);

    return false;
}

bool CThreadManage::Init_Dev()
{
    if(NULL != m_pThreadDevice)
        return m_pThreadDevice->Init_Dev();

    return false;
}
bool CThreadManage::Destroy_Dev()
{
    if(NULL != m_pThreadDevice)
        return m_pThreadDevice->Destroy_Dev();

    return false;
}


//display info
void  CThreadManage::SEND_WM_THREAD_MANAGE_SHOW_DEAL_INFO(QString wParam , int  lParam)
{
    emit WM_THREAD_MANAGE_SHOW_DEAL_INFO(wParam, lParam);
}

//debit money
void  CThreadManage::SEND_WM_THREAD_MANAGE_START_DEBIT_FLOW(QString   wParam, int lParam)
{

    emit WM_THREAD_MANAGE_START_DEBIT_FLOW(wParam, lParam);

}

//debit cancel
void  CThreadManage::SEND_WM_THREAD_MANAGE_DEBIT_CANCEL(QString wParam, int lParam)
{

    emit WM_THREAD_MANAGE_DEBIT_CANCEL(wParam, lParam);

}
