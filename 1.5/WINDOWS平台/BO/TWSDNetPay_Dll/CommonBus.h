#ifndef COMMONBUS_H
#define COMMONBUS_H


#define COMMONBUS_LIBRARY


#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
#if defined(COMMONBUS_LIBRARY)
#define COMMONBUSSHARED_EXPORT __declspec(dllexport)
#else
#define COMMONBUSSHARED_EXPORT __declspec(dllimport)
#endif
#else
#define COMMONBUSSHARED_EXPORT
#endif

enum COMMPENTS_KEY_TYPE
{
    KEY_COMMON_FRAME=1,

    KEY_COM_MANAGER=10,
    //ҵ����
    KEY_BUSINESS=20,
    KEY_BUSINESS_DEBIT=21,
    KEY_BUSINESS_DEBIT_CANCEL=22,
    KEY_BUSINESS_DEBIT_INIT=23,



    KEY_DB_MANAGER=40, //���ݿ�
    KEY_AUTO_UPDATE_MANAGER=60,//�Զ�����
    KEY_UPDATE_QUERY,
    KEY_UPDATE_DOWNLOAD,
    KEY_UPDATE_CONFIRM,
    KEY_UPDATE_HTTP_MANAGER,

    KEY_HTTP_MANAGER=70,//HTTP������
    //
    //����Ӳ����
    KEY_HEARTBEAT_INQUIRY_COM=101,//������ѯ
    KEY_HARDWARE_VERSION=102,//��Ӳ���汾
    KEY_READ_CANID=103,//��CANID
    KEY_READ_MAX=104,//��MAX��
    KEY_FIRMWARE_REVISION=105,//���̼��汾
    KEY_SCANNING_TIME=106, //��ɨ��ʱ����������
    KEY_SERIAL_NUMBER=107,//Ψһ���
    KEY_READ_BOOT_INTERFAC=108,//����������
    KEY_READ_IP_ADDERSS0=109,//������0(eth0) IP ��ַ
    KEY_READ_IP_ADDERSS1=110,//������1(eth1) IP ��ַ
    KEY_READ_TEMPERATURE=111,//��Ӳ���¶�
    KEY_READ_CURRENT_TIME=112,//��Ӳ����ǰʱ��
    KEY_READ_ETH0_NETMASK=113,
    KEY_READ_ETH0_GATEWAY=114,
    KEY_READ_ETH1_NETMASK=115,
    KEY_READ_ETH1_GATEWAY=116,
    KEY_READ_DEFAULT_GATEWAY=117,
    KEY_READ_SCANER_VERSION=118,

    KEY_SET_LCD_PARAMS=200,
    KEY_SET_HEARTBEAT_SEPTUM=201,//�����ն��������
    KEY_SET_BOOT_INTERFACE=202,//���ÿ�������
    KEY_DEBIT_FLOW_CHART_COM=203,//���ƽ�����ʾ��Ϣ(���Թ���
    KEY_DISPLAY_SEE_AGAIN_COM=204,//���ƽ������ټ���
    KEY_DISPALY_ADVERTISING_COM=205, //���ƽ�������棩
    KEY_ISSUED_TIME_COM=206,//�·�����ʱ��
    KEY_SET_IP_ADDERSS0_COM=207,//��������0(eth0) IP ��ַ
    KEY_SET_IP_ADDERSS1_COM=208, //��������1(eth1) IP ��ַ
    KEY_SET_ETH0_NETMASK=209,
    KEY_SET_ETH0_GATEWAY=210,
    KEY_SET_ETH1_NETMASK=211,
    KEY_SET_ETH1_GATEWAY=212,
    KEY_SET_DEFAULT_GATEWAY=213,
    KEY_SET_HW_VERSION214=220,

    KEY_SET_MAX_COM=301,//дCANID
    KEY_SET_CANID_COM=302, //дMAX��
    KEY_SET_SERIAL_NUMBER_COM=303,//Ψһ���
    KEY_SET_SCANNING_TIME_COM=304,//��ɨ��ʱ����������
    KEY_POLL_TEST_COM=305, //��Ŀ������ѯ����
    KEY_DATA_TO_PORT_COM=306,//͸�����ݵ�ָ������
    KEY_SYS_REBOOT_ST=307//reboot smartTerminal
};



class  COMMONBUSSHARED_EXPORT  CommonBus
{

public:
    CommonBus();
    virtual ~CommonBus();
    //ע��  key:���������ȫ�ֶ����Ψһ��ֵ
    virtual void register2Bus(int key);
    //ע��  key:���������ȫ�ֶ����Ψһ��ֵ
    virtual void unRegister2Bus(int key);
    /*
    ����
    destKey:����Ŀ�������ȫ�ֶ����Ψһ��ֵ
    msg:��Ҫ���͵���Ϣ����
    len:��Ҫ���͵���Ϣ���ݵĳ���
    srcKey���������ȫ�ֶ����Ψһ��ֵ��Ĭ��Ϊ0��
*/
    virtual int send2Bus(int destKey,char *msg,int len,int srcKey=0);

    /*
        ����
        destKey:���������ȫ�ֶ����Ψһ��ֵ
        msg:��Ҫ���͵���Ϣ����
        len:��Ҫ���͵���Ϣ���ݵĳ���
        srcKey������Դ�����ȫ�ֶ����Ψһ��ֵ
    */
    virtual int recvFromBus(int destKey,char *msg,int len, int srcKey=0);

public:

    int commpentsKey;
};




#endif // COMMONBUS_H
