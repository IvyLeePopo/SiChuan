#ifndef PROTOCOL_HELPER_H
#define PROTOCOL_HELPER_H

// 固件升级选项
struct FirmwareUpgradeControl
{
    unsigned char firmware_type; // 1   2.0-M1终端固件 2   1.5从机固件
    unsigned char operation; // 1   启动固件升级 2   停止固件升级 3   升级完成
    unsigned char reserved; //0   未定义，预留
    FirmwareUpgradeControl()
    {
        memset(this, 0, sizeof(FirmwareUpgradeControl));
    }
};

// 固件升级传输数据内容
struct FirmwareUpgradeTransfer
{
    unsigned char firmware_type;
    unsigned char data_type;
    unsigned int data_para1;
    unsigned int data_para2;//具体含义见文档
    char *data;

    FirmwareUpgradeTransfer()
    {
        memset(this, 0, sizeof(FirmwareUpgradeTransfer));
    }
    ~FirmwareUpgradeTransfer()
    {
        delete this->data;
        this->data = NULL;
    }
public://默认禁止拷贝赋值操作
	void DeepClone(const FirmwareUpgradeTransfer&other)
	{
		if(this->data!=NULL)
		{
			delete this->data;
			this->data = NULL;
		}

        memcpy(this, &other, sizeof(other));
		data = NULL;
        if (other.data_type == 2)
        {
            this->data = new char[other.data_para2];
            memcpy(this->data, other.data, other.data_para2);
        }
	}
    FirmwareUpgradeTransfer& operator = (const FirmwareUpgradeTransfer&other)
    {
        if (this == &other)
        {
            return *this;
        }
		DeepClone(other);
        return *this;
    }
    FirmwareUpgradeTransfer(const FirmwareUpgradeTransfer&other)
    {
		data = NULL;
        if (this != &other)
        {
           DeepClone(other);

        }
    }
};


enum FIRMWARE_UPGRADE_CONTROL_OPTION
{
    FIRMWARE_UPGRADE_CONTROL_STARTUP,
    FIRMWARE_UPGRADE_CONTROL_STOP,
    FIRMWARE_UPGRADE_CONTROL_COMPLETE,//升级完成
    FIRMWARE_UPGRADE_CONTROL_UNDIFINED
};
enum FIRMWARE_UPGRADE_STATE
{
    FIRMWARE_UPGRADE_STATE_IDLE,//空闲
    FIRMWARE_UPGRADE_STATE_AVAILABLE,//更新可用
    FIRMWARE_UPGRADE_STATE_UPGRADING,//升级中
    FIRMWARE_UPGRADE_STATE_COMPLETE,//完成
    FIRMWARE_UPGRADE_STATE_ERROR,//错误
    FIRMWARE_UPGRADE_STATE_UNDEFINED
};

struct FreeDisplayStyle
{
    std::string HexData;
};
struct FreeDisplayData
{
    unsigned char DataType;
    std::string HexData;
};


#endif // PROTOCOL_HELPER_H
