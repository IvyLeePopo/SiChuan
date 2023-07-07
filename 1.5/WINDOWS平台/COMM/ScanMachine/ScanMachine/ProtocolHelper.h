#ifndef PROTOCOL_HELPER_H
#define PROTOCOL_HELPER_H

// �̼�����ѡ��
struct FirmwareUpgradeControl
{
    unsigned char firmware_type; // 1   2.0-M1�ն˹̼� 2   1.5�ӻ��̼�
    unsigned char operation; // 1   �����̼����� 2   ֹͣ�̼����� 3   �������
    unsigned char reserved; //0   δ���壬Ԥ��
    FirmwareUpgradeControl()
    {
        memset(this, 0, sizeof(FirmwareUpgradeControl));
    }
};

// �̼�����������������
struct FirmwareUpgradeTransfer
{
    unsigned char firmware_type;
    unsigned char data_type;
    unsigned int data_para1;
    unsigned int data_para2;//���庬����ĵ�
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
public://Ĭ�Ͻ�ֹ������ֵ����
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
    FIRMWARE_UPGRADE_CONTROL_COMPLETE,//�������
    FIRMWARE_UPGRADE_CONTROL_UNDIFINED
};
enum FIRMWARE_UPGRADE_STATE
{
    FIRMWARE_UPGRADE_STATE_IDLE,//����
    FIRMWARE_UPGRADE_STATE_AVAILABLE,//���¿���
    FIRMWARE_UPGRADE_STATE_UPGRADING,//������
    FIRMWARE_UPGRADE_STATE_COMPLETE,//���
    FIRMWARE_UPGRADE_STATE_ERROR,//����
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
