// �̼������࣬�ṩ�����ļ���������Ϣ����
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
    void RemoveUpgradeFile();//ɾ�������ļ� 
    void Reupgrade();//��������
public://
    FirmwareUpgradeControl UpgradeControlCMD(const FIRMWARE_UPGRADE_CONTROL_OPTION& ctl_op = FIRMWARE_UPGRADE_CONTROL_UNDIFINED );//���ɿ���ָ��

public:
    FIRMWARE_UPGRADE_STATE UpgradeState();//����״̬
	void SetUpgradeState(FIRMWARE_UPGRADE_STATE FUS);
    bool QueryUpgradeData(FirmwareUpgradeTransfer& upgradeData);//��ȡ��һ����������Ƭ
	std::list<FreeDisplayData> FreeDisplayDataList();
	std::list<FreeDisplayStyle> FreeDisplayStyleList();

private:
	void SetUpgradeInfo(int firmwareType,const std::string& firmwarePath);//���������ļ���Ϣ��������������У���������Ч
    void GenerateUpgradeData();//���ɸ������ݰ�
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
//�����֣���û�жԶ��̷߳�������ȫ֧��
#endif // FIRMWARE_UPGRADE_H
