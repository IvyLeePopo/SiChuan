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
#include <string>
#include "jsoncpp/json.h"
using namespace std;

class CHTSDDeviceDll : public QObject
{
    Q_OBJECT
public:
    explicit CHTSDDeviceDll(QObject *parent = 0);
    ~CHTSDDeviceDll(void);
    QByteArray DebitJsonToBlock(const QString& strData);
    QByteArray StrCopyToHex(const QString &strData);
    QByteArray Int4Array(int m);

signals:

public slots:


    //函数定义
public:
    /*******************************加载接口声明************************************/
    //初始化硬件 ,串口号，波特率，回调函数
    typedef bool	(  *defIF_InitDev)(const std::string& strPortName, const int& baudRate ,void (*NotifyFun)(int type,const QByteArray& sendStr));
    //清理资源
    typedef bool	(  *defIF_Destroy)(void);
    //获取上一次错误信息
    typedef bool	(  *defIF_GetLastErrorDesc)(std::string& errDesc);
    //执行查询操作
    typedef bool	(  *defIF_QueryDevice)(const std::string& paraStr,int paraType);
    //执行设置操作
    typedef bool	(  *defIF_SetDevice)(const std::string& paraStr,int paraType);
    //执行扣款操作
    typedef bool    (  *defIF_StartDebit)(const QByteArray& paraStr,int paraType,std::string& strResult);
    //执行撤单操作
    typedef bool    ( *defIF_StartDebitCancle)( const std::string& paraStr,int paraType);
    //执行设备初始配置操作
    typedef bool    ( *defIF_InitPara)( const std::string& paraStr, int paraType);
    //中断扣款操作
    typedef bool    (  *defIF_BreakDebit)( const std::string& payID);
    //执行检测硬件状态操作
    typedef bool  (*defIF_CheckDeviceStatus) (unsigned int& dwState);

public:
    //初始化动态库
    bool Init( const QString& strDllFilePath);
    //获取动态库加载状态
    inline  bool GetDllInitState(void){return m_bInitDll;}
    //获取组件初始化状态
    inline  bool GetDevInitState(void){return m_bInitDev;}

public:
    /*******************************公共操作************************************/
   //组件初始化
    bool InitDev(const std::string& strPortName, const int& baudRate ,void (*NotifyFun)(int type,const QByteArray& sendStr));
    //清理资源
    bool Destroy(void);
    //获取上一次错误信息
    bool GetLastErrorDesc(  std::string& strLastDesc);
    //执行检测硬件状态操作
    bool CheckDeviceStatus(  unsigned int& dwState);
    /*******************************TR300操作************************************/
    //扣款操作
    bool DebitMoney( const QString& strDebitInfo,  QString& strResult, const int& iTyp=0);
    //撤单操作
    bool DebitCancel( const QString& strCancelInfo, const int& iType=0);
    //设备初始化配置操作
    bool InitConfigDevice( const QString& strInitInfo, const int& iType =0);
    //==============================================
    bool SetDevice( const QString& para, int nType =0);
//Attributes
public:/*add zhuyabing 201706*/
    bool BreakDebit(const QString& payID);
private:
    //动态库加载句柄
    QLibrary*	m_hModule;
    //动态库初始化标记
    bool	m_bInitDll;
    //组件初始化标记
    bool	m_bInitDev;
    //错误说明
    QString  m_strLastError;

    //函数定义变量
    defIF_InitDev                       fnIF_InitDev;
    defIF_Destroy                      fnIF_Destroy;
    defIF_GetLastErrorDesc	  fnIF_GetLastErrorDesc;
    defIF_QueryDevice			  fnIF_QueryDevice;
    defIF_SetDevice				      fnIF_SetDevice;
    defIF_StartDebit                   fnIF_DebitMoney;
    defIF_StartDebitCancle       fnIF_DebitCancel;
    defIF_InitPara                      fnIF_InitDeviecCMD;
    defIF_BreakDebit                 fnIF_BreakDebit;
    defIF_CheckDeviceStatus     fnIF_CheckDeviceState;

};

#endif // CHTSDDEVICEDLL_H
