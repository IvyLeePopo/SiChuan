#include "chtsddevicedll.h"

CHTSDDeviceDll::CHTSDDeviceDll(QObject *parent) :
      QObject(parent)
      ,m_bInitDll(false)
      ,m_bInitDev(false)
      ,m_strLastError("")
      ,m_hModule(NULL)
      ,fnIF_GetLastErrorDesc(NULL)
      ,fnIF_StartGetData(NULL)
      ,fnIF_StopGetData(NULL)
      ,fnIF_CheckDeviceStatus(NULL)
      ,fnIF_GetData(NULL)
      ,fnIF_GetDeviceFirewareVer(NULL)
      ,fnIF_SetDisplayContext(NULL)
      ,fnIF_SetDisplayPayResults(NULL)
{

}


CHTSDDeviceDll::~CHTSDDeviceDll(void)
{
     fnIF_Destroy();
}

/************************************************************
 * 函数名称：Init
 * 函数描述：初始化动态库
 * 参   数：strDllFilePath
 * 返   回：bool
 * **********************************************************/
const bool CHTSDDeviceDll::Init( const QString& strDllFilePath)
{

    qDebug()<<strDllFilePath;

  m_hModule = new QLibrary(strDllFilePath);

  if( m_hModule->load())
    {
      qDebug()<<"Dev lib open ok";


#ifdef _WIN32
       fnIF_InitDev_WIN =  (IF_InitFun_WIN)m_hModule->resolve("IF_InitDev_WIN");
       fnDestroy_WIN = (IF_DestroyFun_WIN)m_hModule->resolve("IF_Destroy_WIN");
#else

        fnIF_InitDev =  (IF_InitFun)m_hModule->resolve("IF_InitDev");
        fnIF_Destroy = (IF_DestroyFun)m_hModule->resolve("IF_Destroy");

#endif

        fnIF_GetLastErrorDesc		= (IF_GetLastErrorDesc)m_hModule->resolve("IF_GetLastErrorDesc");
        fnIF_StartGetData			= (IF_StartGetData)m_hModule->resolve("IF_StartGetData");
        fnIF_StopGetData			= (IF_StopGetData)m_hModule->resolve("IF_StopGetData");
        fnIF_CheckDeviceStatus		= (IF_CheckDeviceStatus)m_hModule->resolve("IF_CheckDeviceStatus");
        fnIF_GetData				= (IF_GetData)m_hModule->resolve("IF_GetData");
        fnIF_GetDeviceFirewareVer	= (IF_GetDeviceFirewareVer)m_hModule->resolve("IF_GetDeviceFirewareVer");
        fnIF_SetDisplayContext		= (IF_SetDisplayContext)m_hModule->resolve("IF_SetDisplayContext");
        fnIF_SetDisplayPayResults   = (IF_SetDisplayPayResults)m_hModule->resolve("IF_SetDisplayPayResults");


        if(
                (NULL != fnIF_GetLastErrorDesc)
             && (NULL != fnIF_StartGetData)
             && (NULL != fnIF_StopGetData)
             && (NULL != fnIF_CheckDeviceStatus)
             && (NULL != fnIF_GetData)
             && (NULL != fnIF_GetDeviceFirewareVer)
             && (NULL != fnIF_SetDisplayContext)
             && (NULL != fnIF_SetDisplayPayResults)

             )
        {
            m_bInitDll = true;
        }
    }
   else
      qDebug()<<"Dev lib open load error";

    //返回执行状态
    return m_bInitDll;
}





/************************************************************
 * 函数名称：InitDev
 * 函数描述：组件初始化
 * 参   数：const quint32& nThreadID,
 *         const unsigned int& nNotifyMsgID,
 *         QString MSmane
 * 返   回：bool
 * **********************************************************/
bool CHTSDDeviceDll::InitDev( const quint32& nThreadID, const unsigned int& nNotifyMsgID,QString MSmane,char* t_PortName,int t_BaudRate)
{
    if(m_bInitDll)
    {

      m_bInitDev = fnIF_InitDev(32,21,MSmane.toLocal8Bit().data(),t_PortName,t_BaudRate);

     return m_bInitDev;
    }
    return false;
}



/************************************************************
 * 函数名称：Destroy
 * 函数描述：清理资源
 * 参   数：void
 * 返   回：bool
 * **********************************************************/
bool CHTSDDeviceDll::Destroy(void)
{
    bool t_results = false;
    if(m_bInitDll)
   {
#ifdef _WIN32
      m_bInitDev = fnDestroy_WIN();
#else
      m_bInitDev = fnIF_Destroy();
#endif
    }

    return t_results;
}



/************************************************************
 * 函数名称：GetLastErrorDesc
 * 函数描述：获取上一次错误信息
 * 参   数：void
 * 返   回：QString
 * **********************************************************/
QString CHTSDDeviceDll::GetLastErrorDesc(void)
{
    if(m_bInitDll)
        return QString(fnIF_GetLastErrorDesc());
    return NULL;
}

/************************************************************
 * 函数名称：StartGetData
 * 函数描述：执行启动获取数据操作
 * 参   数：void
 * 返   回：QString
 * **********************************************************/
bool CHTSDDeviceDll::StartGetData( const int& iOvertime)
{
    qDebug()<<"into CHTSDDeviceDll::StartGetData--->>";

    if(m_bInitDll && m_bInitDev)
     {qDebug()<<"into Dll::StartGetData";     return fnIF_StartGetData(iOvertime);}
    return false;
}



/************************************************************
 * 函数名称：StopGetData
 * 函数描述：执行停止获取数据操作
 * 参   数：void
 * 返   回：bool
 * **********************************************************/
bool CHTSDDeviceDll::StopGetData(void)
{
    qDebug() << "CHTSDDeviceDll::StopGetData";

    if(m_bInitDll && m_bInitDev)
        return fnIF_StopGetData();
    return false;
}

/************************************************************
 * 函数名称：CheckDeviceStatus
 * 函数描述：执行检测硬件状态操作
 * 参   数：quint32
 * 返   回：bool
 * **********************************************************/
bool CHTSDDeviceDll::CheckDeviceStatus(unsigned long& dwState)
{
    qDebug() << "CHTSDDeviceDll::CheckDeviceStatus";

    if(m_bInitDll && m_bInitDev)
        return fnIF_CheckDeviceStatus(dwState);
    return false;
}

/************************************************************
 * 函数名称：GetData
 * 函数描述：获取数据结果
 * 参   数：quint32
 * 返   回：bool
 * **********************************************************/
bool CHTSDDeviceDll::GetData( char* strData,int& iDataType, int& iSrcType)
{
    qDebug() << "CHTSDDeviceDll::GetData";

    if(m_bInitDll && m_bInitDev)
        return fnIF_GetData(strData, iDataType, iSrcType);

    return false;
}



/************************************************************
 * 函数名称：GetDeviceFirewareVer
 * 函数描述：获取固件版本信息
 * 参   数：QString
 * 返   回：bool
 * **********************************************************/
bool CHTSDDeviceDll::GetDeviceFirewareVer(QString& strData)
{
    bool t_results = false;
    if(m_bInitDll && m_bInitDev)
     {

        char* t_GetDataTemp;
        t_results = fnIF_GetDeviceFirewareVer(t_GetDataTemp);
        strData = QString(t_GetDataTemp);

    }
    return t_results;
}


/************************************************************
 * 函数名称：GetDisplayContext
 * 函数描述：获取显示内容信息
 * 参   数：int QString
 * 返   回：bool
 * **********************************************************/
bool CHTSDDeviceDll::SetDisplayContext(int iID, QString strData)
{
    qDebug()<<"into CHTSDDeviceDll::SetDisplayContext";

    if(m_bInitDll && m_bInitDev)
     {
        int t_type = 1;
        return fnIF_SetDisplayContext(iID,t_type,strData.toLatin1().data());

    }
    return false;
}


/************************************************************
 * 函数名称：SetDisplayPayResults
 * 函数描述：设置支付结果
 * 参   数：int iID 设置类型 //1、支付成功 2、支付失败 3、清屏
 *         strJSON 附带信息
 * 返   回：bool
 * **********************************************************/
bool CHTSDDeviceDll::SetDisplayPayResults(int iID, QString strData)
{
    if(m_bInitDll && m_bInitDev)
        return fnIF_SetDisplayPayResults(iID,strData.toLocal8Bit().data());
    return false;
}
























