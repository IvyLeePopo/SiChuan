/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: TWProtocolCMD.h
* ժ   Ҫ: ���� ����Ӳ���ڲ�ͨѶЭ��(TR300)��ɣ���Ҫ��������Э�����������λ��Ӧ
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-07
* �޸ļ�¼: 
*/
/**
 * �������ݣ�
 * 1��Э��5.2ָ�������У��ᵽ��ָ��������ȼ�������Ŀǰû�з��־����Ӧ�ã��ʴ˴�������
 */
#ifndef TW_PROTOCOL_CMD_H
#define TW_PROTOCOL_CMD_H
// TWPC for TWProtocolCMD
// ��������Ϊ��TWPC_ָ�����_ָ������_����ָ��

/**
 * Aָ��
 */
// A1
#define TWPC_A_BEAT             0xA1         //����,����
#define TWPC_A_BEAT_HANDSHAKE      0x01         //�������֣���λ������
#define TWPC_A_BEAT_SET         0x02         //������λ���Զ�����ʱ��

// A2
#define TWPC_A_INTERACTIVE      0xA2         //��Ϣ����
#define TWPC_A_INTERACTIVE_WELCOME      0x01    //���û�ӭ��Ϣģʽ
#define TWPC_A_INTERACTIVE_INTERFACEBLOCK      0x02    //��ʾ������Ϣ�����ݿ��ʽ��
#define TWPC_A_INTERACTIVE_BYE          0x03    //��ʾ������Ϣ���޲�����
#define TWPC_A_INTERACTIVE_04    0x04    //δ�����������ͣʹ��
#define TWPC_A_INTERACTIVE_SCREEN   0x05    //��Ļ��Ŀ����
#define TWPC_A_INTERACTIVE_INTERCACEJSON      0x06       //��ʾ������Ϣ��JSON ��ʽ��
#define TWPC_A_INTERACTIVE_TTS      0x07       //TTS��������
#define TWPC_A_INTERACTIVE_FREE_DISPLAY      0x08       //����������ʾ

// A3
#define TWPC_A_STARTPAY         0xA3         //����֧��
#define TWPC_A_STARTPAY_PAYBLOCK         0x01         //����֧�����̣����ݿ��ʽ��
#define TWPC_A_STARTPAY_PAYJSON        0x02        //����֧�����̣�JSON ��ʽ��


// A4
#define TWPC_A_CANCLE           0xA4         //�����˿�
#define TWPC_A_CANCLE_DATABLOCK 0x01        //�����������̣����ݿ��ʽ��
#define TWPC_A_CANCLE_JSON      0x02             //�����������̣�JSON ��ʽ��

// A5
#define TWPC_A_SENDDATA         0xA5         //���ݴ���
#define TWPC_A_SENDDATA_INITJSON    0x00        //�����ʼ����Ϣ��JSON ��ʽ��
#define TWPC_A_SENDDATA_LANETIME    0x01        //������λ���ĵ�ǰʱ��
#define TWPC_A_SENDDATA_LASTERROR      0x02        //Ҫ���ṩ�ϴδ�����Ϣ
#define TWPC_A_SENDDATA_INITBLOCK        0x03        //�����ʼ����Ϣ�����ݿ�ģʽ��
#define TWPC_A_SENDDATA_FILETRANS        0x04        //�ļ�����ָ��
#define TWPC_A_SENDDATA_TICKNOTRANS      0x05         //tickno


// A6
#define TWPC_A_HARDWARECONFIG   0xA6         //Ӳ������
#define TWPC_A_HARDWARECONFIG_HARDVERSION   0x00    //��Ӳ���汾
#define TWPC_A_HARDWARECONFIG_CANID         0x01    //��CANID
#define TWPC_A_HARDWARECONFIG_MAX           0x02    //��MAX �루�������Σ���д���ڣ�
#define TWPC_A_HARDWARECONFIG_FIRMWARE      0x03    //��ȡӲ���̼��汾��
#define TWPC_A_HARDWARECONFIG_SCANTIME      0x04    //��ɨ��ʱ�������ֵ
#define TWPC_A_HARDWARECONFIG_ID            0x05    //���豸Ψһ���к�
#define TWPC_A_HARDWARECONFIG_BOOTSCREEN    0x06    //����������
#define TWPC_A_HARDWARECONFIG_ETH0IP        0x07    //������0��eth0�� ip ��ַ
#define TWPC_A_HARDWARECONFIG_ETH0MASK      0x08    //������0��eth0�� ��������
#define TWPC_A_HARDWARECONFIG_ETH0GATEWAY   0x09    //������0��eth0�� ���ص�ַ
#define TWPC_A_HARDWARECONFIG_ETH1IP        0x0A    //������1��eth1�� ip ��ַ
#define TWPC_A_HARDWARECONFIG_ETH1MASK      0x0B    //������1��eth1�� ��������
#define TWPC_A_HARDWARECONFIG_ETH1GATEWAY   0x0C    //������1��eth1�� ���ص�ַ
#define TWPC_A_HARDWARECONFIG_DEFAULTGATEWAY    0x0D    //��Ĭ�����أ�eth0��eth1��ppp0��
#define TWPC_A_HARDWARECONFIG_SOFTVERSION   0x0E        //��Ӳ��������汾
#define TWPC_A_HARDWARECONFIG_HARDWARETEMP  0x0F        //��Ӳ���¶�
#define TWPC_A_HARDWARECONFIG_HARDWARETIME  0x10        //��Ӳ����ǰʱ��
#define TWPC_A_HARDWARECONFIG_SETCANID    0x31        //����CANID
#define TWPC_A_HARDWARECONFIG_SETSCANTIME   0x32        //����ÿ��ɨ���ʱ�䣬��λ��
#define TWPC_A_HARDWARECONFIG_SETENV        0x33        //����Ӧ�û���������
#define TWPC_A_HARDWARECONFIG_SETETH0IP     0x40        //��������0��eth0�� ip ��ַ
#define TWPC_A_HARDWARECONFIG_SETETH0MASK   0x41        //��������0��eth0�� ��������
#define TWPC_A_HARDWARECONFIG_SETETH0GATEWAY    0x42    //��������0��eth0�� ���ص�ַ
#define TWPC_A_HARDWARECONFIG_SETETH1IP     0x43        //��������0��eth1�� ip ��ַ
#define TWPC_A_HARDWARECONFIG_SETETH1MASK   0x44        //��������0��eth1�� ��������
#define TWPC_A_HARDWARECONFIG_SETETH1GATEWAY    0x45    //��������0��eth1�� ���ص�ַ
#define TWPC_A_HARDWARECONFIG_SETLED        0x50        //����LED ����ʾ����8 ������
#define TWPC_A_HARDWARECONFIG_SETHARDVERSION 0x60       //����Ӳ���汾��Ϣ
#define TWPC_A_HARDWARECONFIG_SETMAX         0x61       //����MAX �루�������Σ���д���ڣ�
#define TWPC_A_HARDWARECONFIG_SETID         0x62        //�����豸Ψһ���к�
#define TWPC_A_HARDWARECONFIG_SETBOOTSCREENID  0x63     //���¿�������ID
#define TWPC_A_HARDWARECONFIG_TESTLED        0x70       //LED ��Ŀ���ԣ���ѯһ�飬���2s
#define TWPC_A_HARDWARECONFIG_DATATRANS      0x90       //����λ�������ݣ�͸����ָ������

// A7
#define TWPC_A_AUTHCONTROL      0xA7         //Ȩ�޿���
#define TWPC_A_AUTHCONTROL_CHECK 0x00       //�ն˺Ϸ���У��


// AA
#define TWPC_A_AGENCYDEBIT      0xAA         //�����������
#define TWPC_A_AGENCYDEBIT_QUERY 0x01        //���۲�ѯ���
#define TWPC_A_AGENCYDEBIT_TRANSFERIMAGE 0x02        //ͼ�����


//AF��add2018-04 ����Ҫ���ڲ�����
#define TWPC_A_GREENCHANNEL    0xAF
#define TWPC_A_GREENCHANNEL_NFC 0x01     


/**
 * Bָ��
 */
// B1
#define TWPC_B_BEAT             0xB1            //
#define TWPC_B_BEAT_AUTO        0x00            //�Զ��ϴ�������Ϣ
#define TWPC_B_BEAT_FEEDBACK    0x01            //�������֣���λ����������
#define TWPC_B_BEAT_SET         0x02            //���������Զ�����ʱ����

// B2
#define TWPC_B_INTERACTIVE      0xB2         //��Ϣ����
#define TWPC_B_INTERACTIVE_WELCOME      0x01    //�������û�ӭ��Ϣģʽ
#define TWPC_B_INTERACTIVE_INTERFACEBLOCK      0x02    //������ʾ������Ϣ�����ݿ��ʽ��
#define TWPC_B_INTERACTIVE_BYE          0x03    //�Ի���ʾ������Ϣ���޲�����
#define TWPC_B_INTERACTIVE_04    0x04    //δ�����������ͣʹ��
#define TWPC_B_INTERACTIVE_SCREEN   0x05    //������Ļ��Ŀ����
#define TWPC_B_INTERACTIVE_INTERCACEJSON      0x06       //������ʾ������Ϣ��JSON ��ʽ��


// B3
#define TWPC_B_STARTPAY         0xB3         //����֧��
#define TWPC_B_STARTPAY_PAYBLOCK         0x01         //��������֧�����̣����ݿ��ʽ��
#define TWPC_B_STARTPAY_PAYJSON         0x02        //��������֧�����̣�JSON ��ʽ��

// B4
#define TWPC_B_CANCLE           0xB4         //����
#define TWPC_B_CANCLE_DATABLOCK 0x01        //���������������̣����ݿ��ʽ��
#define TWPC_B_CANCLE_JSON      0x02             //���������������̣�JSON ��ʽ��

// B5
#define TWPC_B_SENDDATA         0xB5         //���ݴ���
#define TWPC_B_SENDDATA_INITJSON    0x00        //�����ʼ����Ϣ��JSON ��ʽ��
#define TWPC_B_SENDDATA_LANETIME    0x01        //������λ���ĵ�ǰʱ��
#define TWPC_B_SENDDATA_LASTERROR      0x02        //Ҫ���ṩ�ϴδ�����Ϣ
#define TWPC_B_SENDDATA_INITBLOCK        0x03        //�����ʼ����Ϣ�����ݿ�ģʽ��
#define TWPC_B_SENDDATA_FILETRANS        0x04        //�ļ�����
#define TWPC_B_SENDDATA_TICKNOTRANS        0x05        //tickno

// B6
#define TWPC_B_HARDWARECONFIG   0xB6         //Ӳ������
#define TWPC_B_HARDWARECONFIG_HARDVERSION   0x00    //��Ӳ���汾
#define TWPC_B_HARDWARECONFIG_CANID         0x01    //��CANID
#define TWPC_B_HARDWARECONFIG_MAX           0x02    //��MAX �루�������Σ���д���ڣ�
#define TWPC_B_HARDWARECONFIG_FIRMWARE      0x03    //��ȡӲ���̼��汾��
#define TWPC_B_HARDWARECONFIG_SCANTIME      0x04    //��ɨ��ʱ�������ֵ
#define TWPC_B_HARDWARECONFIG_ID            0x05    //���豸Ψһ���к�
#define TWPC_B_HARDWARECONFIG_BOOTSCREEN    0x06    //����������
#define TWPC_B_HARDWARECONFIG_ETH0IP        0x07    //������0��eth0�� ip ��ַ
#define TWPC_B_HARDWARECONFIG_ETH0MASK      0x08    //������0��eth0�� ��������
#define TWPC_B_HARDWARECONFIG_ETH0GATEWAY   0x09    //������0��eth0�� ���ص�ַ
#define TWPC_B_HARDWARECONFIG_ETH1IP        0x0A    //������1��eth1�� ip ��ַ
#define TWPC_B_HARDWARECONFIG_ETH1MASK      0x0B    //������1��eth1�� ��������
#define TWPC_B_HARDWARECONFIG_ETH1GATEWAY   0x0C    //������1��eth1�� ���ص�ַ
#define TWPC_B_HARDWARECONFIG_DEFAULTGATEWAY    0x0D    //��Ĭ�����أ�eth0��eth1��ppp0��
#define TWPC_B_HARDWARECONFIG_SOFTVERSION   0x0E        //��Ӳ��������汾
#define TWPC_B_HARDWARECONFIG_HARDWARETEMP  0x0F        //��Ӳ���¶�
#define TWPC_B_HARDWARECONFIG_HARDWARETIME  0x10        //��Ӳ����ǰʱ��
#define TWPC_B_HARDWARECONFIG_SETCANID    0x31        //����CANID
#define TWPC_B_HARDWARECONFIG_SETSCANTIME   0x32        //����ÿ��ɨ���ʱ�䣬��λ��
#define TWPC_B_HARDWARECONFIG_SETENV        0x33        //����Ӧ�û���������
#define TWPC_B_HARDWARECONFIG_SETETH0IP     0x40        //��������0��eth0�� ip ��ַ
#define TWPC_B_HARDWARECONFIG_SETETH0MASK   0x41        //��������0��eth0�� ��������
#define TWPC_B_HARDWARECONFIG_SETETH0GATEWAY    0x42    //��������0��eth0�� ���ص�ַ
#define TWPC_B_HARDWARECONFIG_SETETH1IP     0x43        //��������0��eth1�� ip ��ַ
#define TWPC_B_HARDWARECONFIG_SETETH1MASK   0x44        //��������0��eth1�� ��������
#define TWPC_B_HARDWARECONFIG_SETETH1GATEWAY    0x45    //��������0��eth1�� ���ص�ַ
#define TWPC_B_HARDWARECONFIG_SETLED        0x50        //����LED ����ʾ����8 ������
#define TWPC_B_HARDWARECONFIG_SETHARDVERSION 0x60       //����Ӳ���汾��Ϣ
#define TWPC_B_HARDWARECONFIG_SETMAX         0x61       //����MAX �루�������Σ���д���ڣ�
#define TWPC_B_HARDWARECONFIG_SETID         0x62        //�����豸Ψһ���к�
#define TWPC_B_HARDWARECONFIG_SETBOOTSCREENID  0x63     //���¿�������ID
#define TWPC_B_HARDWARECONFIG_TESTLED        0x70       //LED ��Ŀ���ԣ���ѯһ�飬���2s
#define TWPC_B_HARDWARECONFIG_DATATRANS      0x90       //����λ�������ݣ�͸����ָ������

// B7

#define TWPC_B_AUTHCONTROL      0xB7         //Ȩ�޿���
#define TWPC_B_AUTHCONTROL_CHECK 0x00       //�ն˺Ϸ���У��


// BA

#define TWPC_B_AGENCYDEBIT     0xBA      //���ۻ���
#define TWPC_B_AGENCYDEBIT_QUERY 0x01   //���۲�ѯ����
#define TWPC_B_AGENCYDEBIT_TRANSFERIMAGE 0x02        //ͼ�����

//BF��add2018-04 ����Ҫ���ڲ�����
#define TWPC_B_GREENCHANNEL    0xBF
#define TWPC_B_GREENCHANNEL_NFC 0x01     

/**
 * Cָ��
 */
#define TWPC_C_REPLY             0xC0         //����,����
#define TWPC_C_REPLY_SUCCESS            0x00            //����У������
#define TWPC_C_REPLY_INVALIDFORMAT      0x01            //У��ʧ�ܣ���ʶ���ȴ���
#define TWPC_C_REPLY_INVALIDDATACHECK   0x02            //У��ʧ�ܣ�У�������)
#define TWPC_C_REPLY_INVALIDDATAUNDEF   0x03            //У��ʧ�ܣ�δ֪����
#define TWPC_C_REPLY_INVALIDCMD         0x04            //У��ʧ�ܣ�������Ƿ���



/**
 * Dָ��
 */
// D1
#define TWPC_D_QUERY 0xD1       //��λ����ѯ
#define TWPC_D_QUERY_STATUS 0x00 //��ѯ��λ��״̬
#define TWPC_D_QUERY_VERSION 0x01 //��ѯ��λ���汾

// D2
#define TWPC_D_SELFCHECK 0xD2   //��λ���Լ�
#define TWPC_D_SELFCHECK_POWER  0x00 //�ϵ��Լ�

/**
 * Eָ��
 */
// E1 
#define TWPC_E_QUERY 0xE1       //��λ����ѯ
#define TWPC_E_QUERY_STATUS 0x00 //��ѯ��λ��״̬
#define TWPC_E_QUERY_VERSION 0x01 //��ѯ��λ���汾




#endif // TW_PROTOCOL_CMD_H

