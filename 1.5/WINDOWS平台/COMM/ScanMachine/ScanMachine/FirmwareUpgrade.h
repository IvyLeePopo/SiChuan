// 固件升级类，提供升级文件的数据信息访问
// 

#ifndef FIRMWARE_UPGRADE_H
#define FIRMWARE_UPGRADE_H
#include <string>
#include <list>
#include "ProtocolHelper.h"
#include "ThreadHelper.h"
class FirmwareUpgrade
{
public:
    FirmwareUpgrade();
    ~FirmwareUpgrade();
public:
    void SetUpgradeInfo(const std::string& paraJson);
	void KeepIdle();
    void RemoveUpgradeFile();//删除升级文件 
    void Reupgrade();//重新升级
public://
    FirmwareUpgradeControl UpgradeControlCMD(const FIRMWARE_UPGRADE_CONTROL_OPTION& ctl_op = FIRMWARE_UPGRADE_CONTROL_UNDIFINED );//生成控制指令

public:
    FIRMWARE_UPGRADE_STATE UpgradeState();//升级状态
	void SetUpgradeState(FIRMWARE_UPGRADE_STATE FUS);
    bool QueryUpgradeData(FirmwareUpgradeTransfer& upgradeData);//获取下一个升级数据片
	std::list<FreeDisplayData> FreeDisplayDataList();
	std::list<FreeDisplayStyle> FreeDisplayStyleList();

private:
	void SetUpgradeInfo(int firmwareType,const std::string& firmwarePath);//设置升级文件信息，如果正在升级中，则设置无效
    void GenerateUpgradeData();//生成更新数据包
	void AddFreeDisplayData(const FreeDisplayData& fdd);
	void AddFreeDisplayStyle(const FreeDisplayStyle& fds);
private:
	CRITICAL_SECTION m_accessMutex;
    FIRMWARE_UPGRADE_STATE m_upgradeState;
	int m_firmwareType;
    std::string m_firmwarePath;
    std::list<FirmwareUpgradeTransfer> m_upgradeDataQueue;

	std::list<FreeDisplayData> m_freeDataQueue;
	std::list<FreeDisplayStyle> m_freeStyleQueue;

    
private:
    FirmwareUpgrade(const FirmwareUpgrade&);
    FirmwareUpgrade& operator = (const FirmwareUpgrade&);

};
//本部分，暂没有对多线程访问做完全支持
#endif // FIRMWARE_UPGRADE_H
