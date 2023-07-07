/********************************************
 *
 * 类   名：CHTSDDeviceDll
 * 摘   要：底层设备管理类
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CHTSDDEVICEDLL_H
#define CHTSDDEVICEDLL_H

#include <QObject>
#include <QLibrary>
#include <QDebug>

class CHTSDDeviceDll : public QObject
{
    Q_OBJECT
public:

    //函数定义
    //初始化硬件
#ifdef _WIN32
    typedef bool  (*IF_InitFun_WIN)(int,int);
    typedef bool  (*IF_DestroyFun_WIN)(void);
#else
    typedef bool  (*IF_InitFun)(int,int,char *,char* ,int );
    typedef bool  (*IF_DestroyFun)(void);
#endif
        //获取上一次错误信息
        typedef char*   ( *IF_GetLastErrorDesc)(void);
        //执行启动获取数据操作
        typedef bool	( *IF_StartGetData)(const int& );
        //执行停止获取数据操作
        typedef bool	(  *IF_StopGetData)(void);
        //执行检测硬件状态操作
        typedef bool	( *IF_CheckDeviceStatus)(unsigned long& );
        //获取数据结果
        typedef bool	( *IF_GetData)(char* ,int&, int&);
        //获取固件版本信息
        typedef	bool	( *IF_GetDeviceFirewareVer)(char* );
        //获取显示内容信息
        typedef	bool	( *IF_SetDisplayContext)(int,int, char* );
        //执行控制IO操作
        typedef bool	( *IF_SetDisplayPayResults)(int, char*);


    explicit CHTSDDeviceDll(QObject *parent = 0);
    ~CHTSDDeviceDll(void);

    //初始化动态库
    const bool Init( const QString& strDllFilePath);
    //获取动态库加载状态
    inline const bool GetDllInitState(void){return m_bInitDll;}
    //获取组件初始化状态
    inline const bool GetDevInitState(void){return m_bInitDev;}


    //组件初始化
    bool InitDev(const quint32& nThreadID, const unsigned int& nNotifyMsgID, QString MSmane,char* t_PortName,int t_BaudRate);
    //清理资源
    bool Destroy(void);
    //获取上一次错误信息
    QString GetLastErrorDesc(void);
    //执行启动获取数据操作
    bool StartGetData( const int& iOvertime);
    //执行停止获取数据操作
    bool StopGetData(void);
    //执行检测硬件状态操作
    bool CheckDeviceStatus(unsigned long &dwState);
    //获取数据结果
    bool GetData(char *strData, int& iDataType, int& iSrcType);
    //获取固件版本信息
    bool GetDeviceFirewareVer(QString& strData);
    //获取显示内容信息
    bool SetDisplayContext(int iID, QString strData);
    //设置支付结果
    bool SetDisplayPayResults(int iID, QString strData);

signals:

public slots:

    //Attributes
private:
    //动态库加载句柄
    QLibrary	 *m_hModule;
    //动态库初始化标记
    bool	m_bInitDll;
    //组件初始化标记
    bool	m_bInitDev;
    //错误说明
    QString m_strLastError;
    //函数定义变量
#ifdef _WIN32
     IF_InitFun_WIN      fnIF_InitDev_WIN;
     IF_DestroyFun_WIN   fnDestroy_WIN;
#else
    IF_InitFun      fnIF_InitDev;
    IF_DestroyFun   fnIF_Destroy;

#endif
    IF_GetLastErrorDesc		    fnIF_GetLastErrorDesc;
    IF_StartGetData			    fnIF_StartGetData;
    IF_StopGetData			    fnIF_StopGetData;
    IF_CheckDeviceStatus		fnIF_CheckDeviceStatus;
    IF_GetData				    fnIF_GetData;
    IF_GetDeviceFirewareVer	    fnIF_GetDeviceFirewareVer;
    IF_SetDisplayContext		fnIF_SetDisplayContext;
    IF_SetDisplayPayResults		fnIF_SetDisplayPayResults;


};

#endif // CHTSDDEVICEDLL_H
