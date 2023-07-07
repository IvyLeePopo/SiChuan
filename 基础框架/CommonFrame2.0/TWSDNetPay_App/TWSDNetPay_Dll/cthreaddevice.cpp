#include "cthreaddevice.h"
#include "ctwsdnetpay_dllapp.h"
#include "httwsddevice.h"
#include <string>
#include <QDir>

CThreadDevice::CThreadDevice(void)
    :m_iDeviceVersion(2)
{
    m_pHWPerformance = HWPerformance::instance();

    connect(m_pHWPerformance ,SIGNAL(ON_MSSAGE_UNDER_TO_DEVICER(int)),this,SLOT(OnMsgGetData(int)));
}


CThreadDevice::~CThreadDevice()
{
}


bool CThreadDevice::Init(void)
{
    bool breturn = false;

    QString strLog;

    breturn = m_pHWPerformance->InitDev(22);

    breturn =  true;

    qDebug() << strLog;

    return breturn;
}

/***************************************************
 * 函数名称：GetDeviceState
 * 函数描述：获取硬件设备状态
 * 函数参数：void
 * 函数返回: quint32，按位进行标记，定义如下：0位,驱动加载失败;1,串口打开失败;2,无心跳(通讯故障)
 * ************************************************/
quint32 CThreadDevice::GetDeviceState(void)
{
    unsigned long dwState = 0x00;
    quint32 dwReturnState = 0x00;

    return dwReturnState;
}



/***************************************************
 * 函数名称：OnMsgStartGetData
 * 函数描述：执行启动获取数据的操作
 * 函数参数：INT
 * 函数返回: VOID
 * ************************************************/
void CThreadDevice::OnMsgStartGetData(int wParam)
{

}



/***************************************************
 * 函数名称：OnMsgStopGetData
 * 函数描述：执行停止获取数据的操作
 * 函数参数：VOID
 * 函数返回: VOID
 * ************************************************/
void CThreadDevice::OnMsgStopGetData(void)
{
    ////函数参数:	int iSingleType: 1，欢迎界面 2，支付成功 3，支付失败 4，清屏 5 , 扫描成功清稍等
    //     IF_SetSingle(4);
}


/***************************************************
 * 函数名称：OnMsgGetData
 * 函数描述：获取智能设备回馈的数据
 * 函数参数：VOID
 * 函数返回: VOID
 * ************************************************/
void CThreadDevice::OnMsgGetData(int lParam)
{
    int bGetData = lParam;

    tagAccountInfo * pAccountInfo = NULL ;

    pAccountInfo = new tagAccountInfo;

    //解析数据
    try
    {
        if(NULL == m_pHWPerformance || pAccountInfo == NULL)
            throw -1;

        //判断是否获取到成功的数据
        if(bGetData  == 0)
            throw -3;
        //获取数据
        char strData[1024];
        int iDataType = 0x00;
        int iSrcType  = 0x00;
        memset(strData,0,1024);

        if(!m_pHWPerformance->GetData(strData,iDataType,iSrcType))
            throw -4;

        //复制数据
        pAccountInfo->DataType		= iDataType;
        pAccountInfo->DataSize		= sizeof(strData);
        pAccountInfo->SrcType		= iSrcType;
        memcpy(pAccountInfo->DataContext,strData,sizeof(strData));

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
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
        bGetData = 0;
    }

    //通知业务管理线程已经获取用户信息
    emit WM_THREAD_DEVICE_MS_GETDATA_NOTIFY(bGetData,pAccountInfo);
}



/***************************************************
 * 函数名称：OnMsgShowDealInfo
 * 函数描述：接收到信息，要求执行费显功能
 * 函数参数：VOID
 * 函数返回: VOID
 * ************************************************/
void CThreadDevice::OnMsgShowDealInfo(QString wParam)
{
    try
    {
        if(wParam.isEmpty())
            throw -1;
        if(NULL == m_pHWPerformance)
            throw -2;

        //设置显示交互信息
        int iID = 0x01;
        m_pHWPerformance->SetDisplayContext(iID,1,wParam);

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            break;
        case -2:
            break;
        case -3:
            break;
        }
    }
}




/***************************************************
 * 函数名称：OnMsgStartDebitFlow
 * 函数描述：功能描述：接收到消息，启动扣款流程（整合操作模式）
 * 函数参数：VOID
 * 函数返回: VOID
 * ************************************************/
void CThreadDevice::OnMsgStartDebitFlow(const QString &wParam)
{
    QString strLog;
    try
    {
        if(wParam == NULL)
            throw -1;
        if(NULL == m_pHWPerformance)
            throw -2;

        QString strData = wParam;

        //设置启动扣款流程
        int iID = 0x01;
        m_pHWPerformance->SetDisplayContext(iID,1,strData);

        strLog  = "[设备控制线程],接收到指令，开始扣款扫码流程操作(0x57指令)";

        //保存日志
        qDebug() << strLog;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            break;
        case -2:
            break;
        case -3:
            break;
        }
    }

//    if(wParam != NULL)
//    {
//        delete wParam;
//        wParam = NULL;
//    }
}


/***************************************************
 * 函数名称：OnMsgControlResultMMI
 * 函数描述：接收到消息，要求展示缴费结果交互界面
 * 函数参数：int wParam     显示类型
 *         QString lParam 错误数据
 * 函数返回: VOID
 * ************************************************/
void CThreadDevice::OnMsgControlResultMMI(int wParam,QString lParam)
{
    QString strLog;
    int iResultMode = wParam;

    try
    {
        if(NULL == m_pHWPerformance)
            throw -1;

        m_pHWPerformance->SetDisplayPayResults(wParam,lParam);

        strLog  = "[设备控制线程],接收到消息，要求展示缴费结果交互界面";

        //保存日志
        qDebug() << strLog;

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog  = "[设备控制线程],接收到消息，异常硬件设备没有初始化";
            break;
        case -2:
            strLog  = "[设备控制线程],接收到消息，异常硬件设备状态异常";
            break;
        }

        qDebug()<<strLog;
    }
}



/***************************************************
 * 函数名称：ResourceDestroy
 * 函数描述：资源销毁
 * 函数参数：void
 * 函数返回: void
 * ************************************************/
void CThreadDevice::ResourceDestroy(void)
{

    QString strLog;

    try
    {
        if(NULL == m_pHWPerformance)
            throw -1;
        if(!m_pHWPerformance->Destroy())
            throw -2;

        strLog  = "[设备控制线程],成功主动资源注销";

        //保存日志
        qDebug() << strLog;

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog  = "[设备控制线程],异常硬件设备没有初始化";
            break;
        case -2:
            strLog  = "[设备控制线程],主动资源注销异常";
            break;
        }

        qDebug() << strLog;
    }

}
