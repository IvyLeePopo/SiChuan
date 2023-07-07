/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: ErrorNoBase.h
* ժ   Ҫ:  ��������Ӳ���ڲ�ͨѶЭ��(TR300) ��¼8(�������)���½�����
* ����ϵͳָ����Ĵ������
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-06
* �޸ļ�¼:                         
*/
#define OP_SUCCESS                   0x00000000         //�����ɹ�
#define OP_INPUT_PASSWORD            0x00000001         //������������
#define OP_NO_MONEY                  0x00000002         //�˻�����
#define OP_QRCODE_ILLEGAL            0x00000003         //��ά��Ƿ�
#define OP_QRCODE_OT                 0x00000004         //��ά�볬����Ч��
#define OP_NO_ACCOUNT_INFO           0x00000005         //֧��ƾ֤�쳣
#define OP_DEBIT_OT                  0x00000006         //�ۿʱ
#define OP_PSW_ERR                   0x00000007         //�������
#define OP_PARAM_ERR                 0x00000008         //��������
#define OP_LAST_TRADE_NOT_END        0x00000009         //�ϴν���δ����
#define OP_COM_NOT_INIT              0x0000000A         //���δ��ʼ��
#define OP_COM_ILLEGAL               0x0000000B         //���У��Ƿ�
#define OP_PAY_PLATFORM_ERR          0x0000000C         //֧��ƽ̨�������
#define OP_SET_HEART_TIME_ERR        0x0000000D         //�����ն��Զ�����ʱ����ʧ��
#define OP_SET_INIT_ERR              0x0000000E         //���س�ʼ����Ϣʧ��
#define OP_GET_UPPER_TIME_ERR        0x00000010         //��ȡ��λ��ʱ���쳣
#define OP_READ_CANID_ERR            0x00000011         //��ȡ����CAN ��ID ����
#define OP_READ_PRI_ERR              0x00000012         //��ȡ�ն��������γ���
#define OP_READ_HW_FIRM_ERR          0x00000013         //��ȡ�̼��汾��Ϣ����
#define OP_READ_SCAN_TIME_ERR        0x00000014         //��ȡ�ն�ɨ���޶�ʱ�����
#define OP_READ_SERIAL_NO            0x00000015         //��ȡ�ն��豸Ψһ���кų���
#define OP_READ_WEL_ID_ERR           0x00000016         //��ȡ�ն��豸�����������
#define OP_READ_ETH0_IP_ERR          0x00000017         //��ȡ�ն�����0 ��IP ����
#define OP_READ_ETH0_SUB_ERR         0x00000018         //��ȡ�ն�����0 �������������
#define OP_READ_ETH0_GATE_ERR        0x00000019         //��ȡ�ն�����0 �����س���
#define OP_READ_ETH1_IP_ERR          0x0000001A         //��ȡ�ն�����1 ��IP ����
#define OP_READ_ETH1_SUB_ERR         0x0000001B         //��ȡ�ն�����1 �������������
#define OP_READ_ETH1_GATE_ERR        0x0000001C         //��ȡ�ն�����1 �����س���
#define OP_READ_DEF_GATE_ERR         0x0000001E         //��ȡ�ն�Ĭ�����س���
#define OP_READ_SOFT_VER_ERR         0x00000020         //��ȡӦ������汾��Ϣ����
#define OP_READ_HW_TMP_ERR           0x00000021         //��ȡ�ն��¶���ֵ����
#define OP_READ_HW_TIME_ERR          0x00000022         //��ȡ�ն˵�ǰʱ�����
#define OP_WRITE_CAN_ID_ERR          0x00000023         //�����ն�CAN �ڳ���
#define OP_WRITE_SCAN_TIME_ERR       0x00000024         //�����ն�ɨ���޶�ʱ�����
#define OP_WRITE_EVE_PARAM_ERR       0x00000025         //�����ն����л�����������
#define OP_WRITE_ETH0_IP_ERR         0x00000026         //�����ն�����0 ��IP ����
#define OP_WRITE_ETH0_SUB_ERR        0x00000027         //�����ն�����0 �������������
#define OP_WRITE_ETH0_GATE_ERR       0x00000028         //�����ն�����0 �����س���
#define OP_WRITE_ETH1_IP_ERR         0x00000029         //�����ն�����1 ��IP ����
#define OP_WRITE_ETH1_SUB_ERR        0x0000002A         //�����ն�����1 �������������
#define OP_WRITE_ETH1_GATE_ERR       0x0000002B         //�����ն�����1 �����س���
#define OP_WRITE_DEF_GATE_ERR        0x0000002C         //�����ն����ڵ�Ĭ�����س���
#define OP_WRITE_SCREEN_ERR          0x0000002D         //�����ն���ʾ��������Ϣ����
#define OP_WRITE_HW_VER_ERR          0x0000002E         //�����ն˹̼��汾��Ϣ����
#define OP_WRITE_MAX_ERR             0x00000030         //�����ն�������Ϣ����
#define OP_WRITE_SERIAL_NO_ERR       0x00000031         //�����ն�Ψһ��ų���
#define OP_WRITE_WEL_ID_ERR          0x00000032         //�����ն˻�ӭ����ID ����
#define OP_TEST_HW_ERR               0x00000033         //�����ն�Ӳ����ʾ����Ŀ����
#define OP_HW_ILLEGAL                0x00000034         //�ն˺Ϸ���У��ʧ��
#define OP_UPPER_STATE_ERR           0x00000035         //��ȡ��λ��״̬��Ϣ����
#define OP_UPPER_VER_ERR             0x00000036         //��ȡ��λ������汾����

#define OP_DEBIT_CANCEL_ERR 0x00000037		//����ʧ��
#define OP_WRITE_HARDWAREVER_ERR 0x00000038		//��Ӳ���汾ʧ��
#define OP_WRITE_FEIXIAN_ERR 0x00000039		//���ƽ�������ʧ��
#define OP_CONFIG_DISABLE_ERR 0x00000040   //������������
