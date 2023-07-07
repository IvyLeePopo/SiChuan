/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: CoreBusiness_Future.h
* 摘   要:  主业务单元，用于处理外部接口调用
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-10
* 修改记录:
*/
#ifndef CORE_BUSINESS_Future_H
#define CORE_BUSINESS_Future_H


#include "TWProtocolData.h"
#include "TableDebitCancel.h"
#include "CommondDak.h"
#include <list>
#include <set>
#include <map>

typedef void(*NotifyFun)(int type, const std::string& sendStr);

class CCoreBusiness_Future : public CWinThread
{
    DECLARE_DYNCREATE(CCoreBusiness_Future)
public:
    CCoreBusiness_Future();
    virtual ~CCoreBusiness_Future();
public:/*TR300 接口内容*/
    bool IF_InitDevice_L(IN NotifyFun fun);
    bool IF_Terminate_L();
    bool IF_InitPara_L( const std::string& paraStr, int paraType );
    bool IF_SetDevice_L( const std::string& paraStr, int paraType );
    bool IF_QueryDevice_L( int queryType, int resultType, std::string& resultStr );
    bool IF_StartDebit_L( const std::string& paraStr, int paraType, std::string& resultStr );
    bool IF_StartDebitCancle_L( const std::string& paraStr );
    bool IF_GetLastError_L(std::string& errorInfo);
    bool IF_GetLastResult_L(int& type, std::string& resultStr);
    bool IF_BreakDebit_L(const std::string& payID);
	bool IF_CheckDeviceStatus_L(DWORD& dwState);

    bool IF_PlaySound_L( const std::string& paraStr, int paraType );
    bool IF_ShowText_L( const std::string& paraStr, int paraType );

    bool IF_AgencyDebitQuery_L(IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult);


	bool IF_TransferImage_L(IN const std::string& image_para);
    bool IF_TransTickNo_L(const std::string& para);

	bool GreenChannelTest_L(int testType,const std::string& para,std::string& result);
	bool IF_ExcuteComCommond_L(const std::string& hexStr);
	bool IF_TakeComFeedbackCommond_L(CString& jsonStr);


	bool IF_TerminalSystemVersionStatus_L(int& status);
	bool IF_ExchangeTerminalSystem_L(const int& targetSystemVersion);
private:
    int WaitRespond(TWCmdKey crk, int overTime = 6);
	bool QuerySendResult(TWCmdKey crk);
    void ClearRespondRecord();
    bool SendUtilSuccess();/*直到发送成功*/
    bool SendAndWaitReply();  /*发送并且等待回馈*/

    bool GenerateDevieSetFrames(std::map<std::string, std::string>& srcPara, int type, std::list<TWProtocol_BaseFrame>&cmds);

	//add 2018-04
	bool TransferBigFile(const std::vector<unsigned char>& byteArray,const std::string& FileProp );
	void NewFeedbackCommand(TWProtocol_BaseFrame& frame);
	bool TakeOneFeedbackCommondJsonStr(std::string& jsonStr);

	//add 2018-05
	void Handle_11_Frame(const TWProtocol_BaseFrame& info);
	void Handle_12_Frame(const TWProtocol_BaseFrame& info);
	bool NewSystemSwitchRespond(const int &val);
	bool TakeOneSystemSwitchRespond(int& val);
	bool ClearSystemSwitchRespond();
	bool NewSystemStatusRespond(const int& val);
	bool TakeOneSystemStatusRespond(int& val);
	bool ClearSystemStatusRespond();

public:/*线程控制相关*/
    virtual afx_msg BOOL InitInstance();
    virtual afx_msg int ExitInstance();
    virtual afx_msg BOOL OnIdle(LONG lCount);

private:
    void Stop();
    bool SendRemainDebitCancle(const int& callCount);
    void HandleFrame_B_Beat(const TWProtocol_BaseFrame& info);
    void HandleFrame_B_Interactive(const TWProtocol_BaseFrame& info);
    void HandleFrame_B_StartPay(const TWProtocol_BaseFrame& info);
    void HandleFrame_B_CANCLE(const TWProtocol_BaseFrame& info);
    void HandleFrame_B_SendData(const TWProtocol_BaseFrame& info);
    void HandleFrame_B_HardwareConfig(const TWProtocol_BaseFrame& info);
    void HandleFrame_B_Auth(const TWProtocol_BaseFrame& info);
    void HandleFrame_B_AgencyDebit(const TWProtocol_BaseFrame& info);

	void HandleFrame_B_GreenChannel(const TWProtocol_BaseFrame& info);


    void HandleFrame_C_Reply(const TWProtocol_BaseFrame& info);
    void HandleFrame_D_Query(const TWProtocol_BaseFrame& info);
    void HandleFrame_D_SelfCheck(const TWProtocol_BaseFrame& info);
    void SetDefault();
    void CheckAlive();
    unsigned short GetNextSN();
    void HandleRecvFrameMessage();   //处理接收的数据内容
    void ReadFrame();//根据串口数据读取帧内容
private:
    CCommondDak* mpCmd;
    unsigned short mFrameSN;
    std::string mInitJsonStr;
    bool mBreakCurrentDebit;
    std::string mLastPayId;
    std::map<TWCmdKey, RespondResult> mRespondRecord;
    TWCmdKey mLastPayResponseCrk;
    int mWaitBOverTime;//在已经确认对方收到指令后（有C0正回馈），需要等待对方回应处理结果帧的时间，单位是秒。
    int mWaitBPayOverTime;/*支付信息的超时设置,可能需要等待扫码*/
    int mProvinceID;
    unsigned int mHardwareStatus;
    std::list<TWProtocol_BaseFrame> mFrameQueue;
	time_t mLastReadFrameTime;
	std::list<std::string> mLatestCanclePayQueue;
	std::list<std::string> m_FeedbackCommondStrQueue;

	std::list<int> m_SystemSwitchRespondQueue;
	std::list<int> m_SystemStatusRespondQueue;
private:
    bool mInitResult;
    bool mHasInit;
    bool m_bExit;
	bool mTerminate;
private:/*多线程控制变量*/
    CRITICAL_SECTION mRespondMutex; 
    CRITICAL_SECTION mFrameMutex;  /*协议帧处理访问锁*/
    CRITICAL_SECTION mDebitCancleMutex;   /*避免自动撤单手动撤单同时*/
    CRITICAL_SECTION mInterfaceMutex;   /*外部动态库接口访问锁*/
	CRITICAL_SECTION m_FeedbackCommondMutex;
    
private:
    CTableDebitCancel mDebitCancleEnsure;

    std::map<TWCmdKey, TWProtocolData_B_Beat> mBBeatDataQueue;
    std::map<TWCmdKey, TWProtocolData_B_Interactive> mBInterDataQueue;
    std::map<TWCmdKey, TWProtocolData_B_StartPay> mBStartPayDataQueue;
    std::map<TWCmdKey, TWProtocolData_B_CANCLE> mBCANCLEQueue;
    std::map<TWCmdKey, TWProtocolData_B_SendData> mBSendDataQueue;
    std::map<TWCmdKey, TWProtocolData_B_HardwareConfig> mBHardwareConfigDataQueue;
    std::map<TWCmdKey, TWProtocolData_B_Auth> mBAuthDataQueue;
    std::map<TWCmdKey, TWProtocolData_B_AgencyDebit> mBAgencyDebitDataQueue;

	std::map<TWCmdKey, TWProtocolData_B_GreenChannel> mBGreenChannelDataQueue;
//-------------------------------
private:
    CCoreBusiness_Future(const CCoreBusiness_Future&);
    CCoreBusiness_Future& operator = (const CCoreBusiness_Future&);

};

#endif // CORE_BUSINESS_Future_H