/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: TWProtocolData.h
* ժ   Ҫ:  ���崫��Э��Ľṹ
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-07
* �޸ļ�¼:
*/
/**
 * ˵�����ݣ�
 * 1������Э����δ��ȷ˵�������ݣ��������з��ŵġ����Э�����ᵽ�˸�ֵ��ȡֵ��Χ����ʹ����0x01��������ʽ������Ϊ����ֵ���޷��ŵġ�
 * 2������Э�������ݳ��Ⱥ�c++��Ӧ��Ĭ��2�ֽ�ʹ��short�����ֽ�ʹ��int��Ĭ��Ŀ�����֧�ָó���
 */
#ifndef TW_PROTOCOL_DATA_H
#define TW_PROTOCOL_DATA_H
// TWPD for TWProtocolData
#include "TWProtocolCMD.h"

#define TWPD_STARTFLAG  0xDF    //֡��ʼȡֵ
#define TWPD_ENDFLAG    0xFD    //֡����ȡֵ
#define TWPD_STARTFLAG_ESCAPE_FIRST 0xDE
#define TWPD_STARTFLAG_ESCAPE_SECOND 0x01
#define TWPD_ENDFLAG_ESCAPE_FIRST 0xFC
#define TWPD_ENDFLAG_ESCAPE_SECOND 0x01

#define TWPD_A_INTERACTIVE_WELCOME_PICVOICE 1   //ͼ��+����
#define TWPD_A_INTERACTIVE_WELCOME_PIC      2   //����ͼ��
#define TWPD_A_CANCLE_FEEDBACK_YES  1           //�����������
#define TWPD_A_CANCLE_FEEDBACK_NO   2         //���û����������
#define TWPD_A_SENDDATA_MODE_DEFAULT  0         //Ĭ����Ӫģʽ
#define TWPD_A_SENDDATA_MODE_WECHAT 1           //΢��ģʽ
#define TWPD_A_SENDDATA_MODE_WECHAT_PROMOTION 2           //΢�Ŵ���
#define TWPD_A_SENDDATA_MODE_ALIPAY 3               //֧����ģʽ
#define TWPD_A_SENDDATA_MODE_ALIPAY_PROMOTION 4     //֧��������
#define TWPD_A_HARDWARECONFIG_IPV4   0x00 //IPV4
#define TWPD_A_HARDWARECONFIG_IPV6   0x01 //IPV6


#define TWPD_B_BEAT_FAULT_CLOUD     0x00000001     //�ƶ��������
#define TWPD_B_BEAT_FAULT_4G        0x00000002      //4G ģ�����
#define TWPD_B_BEAT_FAULT_SWEEP    0x00000004       //ɨ��������
#define TWPD_B_BEAT_FAULT_VOICE    0x00000008   //����ģ�����
#define TWPD_B_BEAT_FAULT_NFC 0x00000010       //NFC ģ�����

#define TWPD_B_STARTPAY_PAYTYPE_WECHAT 1    //΢��֧��
#define TWPD_B_STARTPAY_PAYTYPE_ALIPAY 2    //֧����֧��
#define TWPD_B_STARTPAY_PAYTYPE_QUICKPASS 3    //����
#define TWPD_B_STARTPAY_PAYTYPE_APPLEPAY 4    //ApplePay
#define TWPD_B_STARTPAY_PAYTYPE_HUAWEIPAY 5    //HuaweiPay
#define TWPD_B_STARTPAY_PAYTYPE_SAMSUNGPAY 6    //SamsungPay




// ��������֡�Ϳ�����֡һ�£�ͨ��ͨ��������ָ���Ƿ�Ϊ���ж�
struct TWProtocol_BaseFrame
{
	unsigned char STX;
	unsigned short LEN;
	unsigned short SN;
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	unsigned char* DATA;            //��֡or����֡,�ֽ���
	unsigned short VERIFY;
	unsigned char ETX;
	TWProtocol_BaseFrame()
	{
		SetDefault();
	}
	TWProtocol_BaseFrame(unsigned char top, unsigned char sub, unsigned short sn)
	{
		SetDefault();
		TOPCMD = top;
		SUBCMD = sub;
		SN = sn;
	}
	~TWProtocol_BaseFrame()
	{
		delete this->DATA;
		this->DATA = NULL;
	}
	TWProtocol_BaseFrame& operator = (const TWProtocol_BaseFrame&other)
	{
		if(this==&other)
		{
			return *this;
		}
		memcpy(this,&other,sizeof(other));
		if(other.LEN>0)
		{
			this->DATA = new unsigned char[other.LEN];
			memcpy(this->DATA,other.DATA,other.LEN);
		}
		return *this;
	}
	TWProtocol_BaseFrame(const TWProtocol_BaseFrame&other)
	{
		if(this!=&other)
		{
			memcpy(this,&other,sizeof(other));
			if(other.LEN >0)
			{
				this->DATA = new unsigned char[other.LEN];
				memcpy(this->DATA,other.DATA,other.LEN);
			}
			
		}
		
	}
	void SetDefault()
	{
		memset(this, 0, sizeof(TWProtocol_BaseFrame));
		STX = TWPD_STARTFLAG;
		ETX = TWPD_ENDFLAG;
	}
	
};
struct TWProtocolData_Store_Key
{
	unsigned short SN;
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_Store_Key()
	{
		memset(this, 0, sizeof(TWProtocolData_Store_Key));
	}
	bool operator < (const TWProtocolData_Store_Key& sec)const
	{
		return SN < sec.SN;
	}
};


// A1
union TWProtocolData_A_Beat_U
{
	unsigned char Unkown;             //0x01,Э����û�жԸ��ֶ�ȡֵ����˵��
	unsigned short BeatInterval;    //0x02,�����������λ�����룬0x0001-0x0064��Ϊ100���룬����ȡֵ0x0065-0x00FF
	TWProtocolData_A_Beat_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_Beat_U));
	}
};

struct TWProtocolData_A_Beat
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_Beat_U data;
	TWProtocolData_A_Beat(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_Beat()
	{
	}
private:
	TWProtocolData_A_Beat(const TWProtocolData_A_Beat&);
	TWProtocolData_A_Beat& operator = (const TWProtocolData_A_Beat&);

};
// A2
union TWProtocolData_A_Interactive_U
{
	char WelcomeOption;         //0x01
	struct                      //0x02
	{
		int Money;          //��λ����
		signed char VehType;  //���ͣ�0-�ͳ���1-����
		signed char VehKind;  //����:01-09�����嶨��δָ��
		signed char VehPlateColor;    //������ɫ
		char Plate[16];                 //����
		int Balance;            //֧�������
		char EnStationName[24]; //���վ����
		signed char FlagStationCount; //��ʶվ����
		short AxleCount;        //����
		int Weight;             //��������λ��ǧ��
		int WeightLimit;        //���أ���λ��ǧ��
	} InteractionBlock;
	unsigned char ByeInfo;  /*0x03*/
	unsigned char ScreenInfo;/*0x05*/
	char* pInteractionJson;/*0x06*/

	struct 
	{
		int contentLen;
		char* content;
	}TTS;/*0x07 */

	struct
	{
		char Row1Data[8];
		unsigned char Row1Scroll;
		unsigned char Row1Resevered;

		char Row2Data[8];
		unsigned char Row2Scroll;
		unsigned char Row2Resevered;

		char Row3Data[8];
		unsigned char Row3Scroll;
		unsigned char Row3Resevered;

		char Row4Data[8];
		unsigned char Row4Scroll;
		unsigned char Row4Resevered;
	}FreeText;
	TWProtocolData_A_Interactive_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_Interactive_U));
	}
};


struct TWProtocolData_A_Interactive
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_Interactive_U data;
	TWProtocolData_A_Interactive(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_Interactive()
	{
		if ( TOPCMD != TWPC_A_INTERACTIVE)
		{
			;
		}
		if ( SUBCMD == TWPC_A_INTERACTIVE_INTERCACEJSON)
		{
			delete data.pInteractionJson;
			data.pInteractionJson = 0;
		}
		else if( SUBCMD == TWPC_A_INTERACTIVE_TTS)
		{
			delete data.TTS.content;
			data.TTS.content = NULL;
		}
		else if( SUBCMD == TWPC_A_INTERACTIVE_FREE_DISPLAY)
		{
		}
	}
private:
	TWProtocolData_A_Interactive(const TWProtocolData_A_Interactive&);
	TWProtocolData_A_Interactive& operator = (const TWProtocolData_A_Interactive&);

};

// A3
union TWProtocolData_A_StartPay_U
{
	struct //0x01
	{
		char TransactionIDCode[40];     //����ʶ���루������0 ��ȫ��
		int Money;          //����λ����
		signed char VehTypeAndKind;  //���ͣ�0 �ͳ���1 ����,������Ҳʹ�ø��ֶ�
		// signed char VehKind;  //���֣�01-09�����鷶Χ��
		signed char VehPlateColor;    //������ɫ
		char Plate[16];                 //����
		char EnStationNO[20];
		char EnStationName[24]; //���վ����
		long EnTime;
		int StaticDate;
		signed char StaticShift;
		signed char CardLen;
		char CardID[10];
		signed char FlagStationCount; //��ʶվ����
		short AxleCount;        //����
		int Weight;             //��������λ��ǧ��
		char OperatorID[20];
		char OperatorName[10];
		int Balance;            //֧��������λ����
		signed char DebitMode;
		signed char DebitOverTime;
		long ExTime;
		// int WeightLimit;        //���أ���λ��ǧ��
		char EntryPlate[16];//add 2018-05
		unsigned char EntryPlateColor;
		unsigned char PassCertificateType;
		char AutoPlate[16];
		unsigned char AutoPlateColor;
		char TickNo[20];
		unsigned char PayModel;
	} PayBlock;
	char* pPayInfoJson;         /*0x02*/
	TWProtocolData_A_StartPay_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_StartPay_U));
	}

};


struct TWProtocolData_A_StartPay
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_StartPay_U data;
	TWProtocolData_A_StartPay(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_StartPay()
	{
		if ( TOPCMD != TWPC_A_STARTPAY)
		{
			;
		}
		if ( SUBCMD == TWPC_A_STARTPAY_PAYJSON)
		{
			delete data.pPayInfoJson;
			data.pPayInfoJson = 0;
		}
	}
private:
	TWProtocolData_A_StartPay(const TWProtocolData_A_StartPay&);
	TWProtocolData_A_StartPay& operator = (const TWProtocolData_A_StartPay&);

};

// A4
union TWProtocolData_A_Cancle_U
{
	struct/*0x01*/
	{
		signed char IsFeedBack;
		char PayCode[32];
	} DataBlock;
	char* pDataJson;/*0x02*/
	TWProtocolData_A_Cancle_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_Cancle_U));
	}
};

struct TWProtocolData_A_Cancle
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_Cancle_U data;
	TWProtocolData_A_Cancle(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_Cancle()
	{
		if ( TOPCMD != TWPC_A_CANCLE)
		{
			;
		}
		if ( SUBCMD == TWPC_A_CANCLE_JSON)
		{
			delete data.pDataJson;
			data.pDataJson = 0;
		}
	}
private:
	TWProtocolData_A_Cancle(const TWProtocolData_A_Cancle&);
	TWProtocolData_A_Cancle& operator = (const TWProtocolData_A_Cancle&);

};



// A5

union TWProtocolData_A_SendData_U
{
	char* pInitJson;/*0x00*/
	long LaneUnixTime;/*0x01*/
	unsigned int ErrorQuery;/*0x02*/
	struct
	{ 
		char     ProvinceID; //ʡ�ݱ���
		short    AreaID;     //�������
		short     RoadID;     //·�α���
		int     StationID;  //վ�����
		short     LaneID;     //��������
		char   StationName[12]; //վ������
		char     ServerType;  //����������
		unsigned char     ServerIP[4];   //����IP��ע�⣺�˴���Ҫ˵�����ĸ��ֽڣ�����ÿ���ֽڱ���һ��IP�Σ�����127.0.0.1���ĸ��λ�ŵ�һ��д��������
		short     DevicePayMode;//�豸��Ӫģʽ
	} InitBlock;
	struct 
	{
		unsigned char Version;
		unsigned char Type;
		unsigned int TotalLen;
		unsigned int Offset;
		unsigned short DataLen;
		unsigned char Data[1024*2];//ÿ�����1k
	}FileData;/*0x04*/
	char* pTickNo;
	TWProtocolData_A_SendData_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_SendData_U));
	}
};

struct TWProtocolData_A_SendData
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_SendData_U data;
	TWProtocolData_A_SendData(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_SendData()
	{
		if ( TOPCMD != TWPC_A_SENDDATA)
		{
			;
		}
		if ( SUBCMD == TWPC_A_SENDDATA_INITJSON)
		{
			delete data.pInitJson;
			data.pInitJson = 0;
		}
		else if(SUBCMD==TWPC_A_SENDDATA_TICKNOTRANS)
		{
			delete data.pTickNo;
			data.pTickNo=NULL;
		}
	}
private:
	TWProtocolData_A_SendData(const TWProtocolData_A_SendData&);
	TWProtocolData_A_SendData& operator = (const TWProtocolData_A_SendData&);

};

// A6
union TWProtocolData_A_HARDWARECONFIG_U
{
	unsigned char Unkown;   /*0x00-0x03 ,0x05-0x10*/
	unsigned short ScanTimeVal;  /*0x04*/
	struct/*0x31*/
	{
		unsigned char Value;/*CANID ������ֵ,0~255*/
		char VerifyCode[8];
	} CANID;
	struct /*0x32*/
	{
		unsigned short TimeVal;
		char VerifyCode[8];
	} ScanTime;
	struct /*0x33*/
	{
		char* Para;
	} Env;

	struct /*0x40-0x45*/
	{
		unsigned char IPVer;/*��ʶIP ����*/
		unsigned char IP[16];
	} IPInfo;

	struct /*0x50*/
	{
		struct
		{
			unsigned char Name;
			char Value[8];
		} Var;
		char VerifyCode[8];
	} LEDInfo;

	struct /*0x60*/
	{
		char HardVer[15];
	} HardInfo;

	struct /*0x61*/
	{
		char Batch[4];
		char BurnTime[14];
		char VerifyCode[8];
	} MaxInfo;

	struct /*0x62*/
	{
		char Val[20];/*ע�⣺�˴�û��˵��*/
		char VerifyCode[8];

	} DeviceID;

	struct /*0x63*/
	{
		char Val;
		char VerifyCode[8];
	} ScreenID;

	struct /*0x70*/
	{
		unsigned char Unuse;/*Ĭ��Ϊ��0x00*/
	} TestLED;

	struct /*0x90*/
	{
		unsigned char PortNO;/*1~9 ���ں� 10~11 CAN �ں�*/
		char VerifyCode[8];
		char* pContent;
	} DataTrans;

	TWProtocolData_A_HARDWARECONFIG_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_HARDWARECONFIG_U));
	}
};


struct TWProtocolData_A_HARDWARECONFIG
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_HARDWARECONFIG_U data;
	TWProtocolData_A_HARDWARECONFIG(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_HARDWARECONFIG()
	{
		if ( TOPCMD != TWPC_A_HARDWARECONFIG)
		{
			;
		}
		if ( SUBCMD == TWPC_A_HARDWARECONFIG_DATATRANS)
		{
			delete data.DataTrans.pContent;
			data.DataTrans.pContent = 0;
		}
	}
private:
	TWProtocolData_A_HARDWARECONFIG(const TWProtocolData_A_HARDWARECONFIG&);
	TWProtocolData_A_HARDWARECONFIG& operator = (const TWProtocolData_A_HARDWARECONFIG&);

};
// A7
union TWProtocolData_A_Auth_U
{
	unsigned char Unuse;/*0x00*/
	TWProtocolData_A_Auth_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_Auth_U));
	}
};


struct TWProtocolData_A_Auth
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_Auth_U data;
	TWProtocolData_A_Auth(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_Auth()
	{
	}
private:
	TWProtocolData_A_Auth(const TWProtocolData_A_Auth&);
	TWProtocolData_A_Auth& operator = (const TWProtocolData_A_Auth&);

};

// AA
union TWProtocolData_A_AgencyDebit_U
{
	struct 
	{
		unsigned char VehType;//����
		unsigned char VehKind;//���� 
		unsigned char VehColor;//
		char VehPlate[16];
		unsigned short AxleCount;
		unsigned int Weight;
		unsigned int WeightLimit;
	}QueryData;//0x01
	struct 
	{
		unsigned char ImageClass;
		char PayIdentifier[40];
		char ImageFormat[10];
		unsigned short ImageLen;
		unsigned char ImageContent[10*1024];	
	}ImageData;
	TWProtocolData_A_AgencyDebit_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_AgencyDebit_U));
	}
};


struct TWProtocolData_A_AgencyDebit
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_AgencyDebit_U data;
	TWProtocolData_A_AgencyDebit(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_AgencyDebit()
	{
	}
private:
	TWProtocolData_A_AgencyDebit(const TWProtocolData_A_AgencyDebit&);
	TWProtocolData_A_AgencyDebit& operator = (const TWProtocolData_A_AgencyDebit&);

};


union TWProtocolData_A_GreenChannel_U
{
	unsigned int NFCQuery;//0x01
	TWProtocolData_A_GreenChannel_U()
	{
		memset(this, 0, sizeof(TWProtocolData_A_GreenChannel_U));
	}
};



struct TWProtocolData_A_GreenChannel
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_A_GreenChannel_U data;
	TWProtocolData_A_GreenChannel(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_A_GreenChannel()
	{
	}
private:
	TWProtocolData_A_GreenChannel(const TWProtocolData_A_AgencyDebit&);
	TWProtocolData_A_GreenChannel& operator = (const TWProtocolData_A_AgencyDebit&);

};








// B1
union TWProtocolData_B_Beat_U
{
	struct /*0x00-0x01*/
	{
		unsigned int Result;
		unsigned int DeviceStatus;/*��λ����0�ǵ�λ,��32�Ǹ�λ��0��ʾȫ���޹���*/
	} BeatInfo;

	unsigned int SetResult;/*0x02*/
	TWProtocolData_B_Beat_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_Beat_U));
	}
};

struct TWProtocolData_B_Beat
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_Beat_U data;
	TWProtocolData_B_Beat(){};
	TWProtocolData_B_Beat(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_Beat()
	{
	}
	TWProtocolData_B_Beat& operator = (const TWProtocolData_B_Beat& other)
	{
		if (this != &other)
		{
			memcpy(this, &other, sizeof(TWProtocolData_B_Beat));
		}
		return *this;
	}
	TWProtocolData_B_Beat(const TWProtocolData_B_Beat&other)
	{
		*this = other;
	}
private:
	
};


// B2
union TWProtocolData_B_Interactive_U
{
	unsigned int Result;/*0x01-0x06*/
	TWProtocolData_B_Interactive_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_Interactive_U));
	}
};

struct TWProtocolData_B_Interactive
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_Interactive_U data;
	TWProtocolData_B_Interactive(){}
	TWProtocolData_B_Interactive(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_Interactive()
	{
	}
	TWProtocolData_B_Interactive& operator = (const TWProtocolData_B_Interactive&other)
	{
		if (this != &other)
		{
			memcpy(this, &other, sizeof(TWProtocolData_B_Interactive));
		}
		return *this;

	}
	TWProtocolData_B_Interactive(const TWProtocolData_B_Interactive&other)
	{
		*this = other;
	}
private:
	


};

// B3
union TWProtocolData_B_StartPay_U
{
	struct /*0x01*/
	{
		unsigned int Result;
		char TransSecurityCode[40];
		char TransactionIDCode[40];
		char OrderNO[32];
		int TransTime;  /*����˽���ʱ�䣬UNIX ��ʽ��4 �ֽ�����*/
		short PayType;
		int ConsumeTime;/*����ʱ��*/
	} DataBlock;

	char* pDataJson;/*0x02*/
	TWProtocolData_B_StartPay_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_StartPay_U));
	}
};

struct TWProtocolData_B_StartPay
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_StartPay_U data;
	TWProtocolData_B_StartPay(){}
	TWProtocolData_B_StartPay(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_StartPay()
	{
		if (TOPCMD != TWPC_B_STARTPAY)
		{
			;
		}
		if (SUBCMD == TWPC_B_STARTPAY_PAYJSON)
		{
			delete data.pDataJson;
			data.pDataJson = 0;
		}
	}
	TWProtocolData_B_StartPay& operator = (const TWProtocolData_B_StartPay&other)
	{
		if (this != &other)
		{
			if (SUBCMD == TWPC_B_STARTPAY_PAYJSON)
			{
				delete data.pDataJson;
				data.pDataJson = 0; 
			}
			memcpy(this, &other, sizeof(TWProtocolData_B_StartPay));
			if (SUBCMD == TWPC_B_STARTPAY_PAYJSON)
			{
				data.pDataJson = new char[strlen(other.data.pDataJson) + 1];
				memset(data.pDataJson, 0, strlen(other.data.pDataJson) + 1);
				strcpy(data.pDataJson, other.data.pDataJson);
			}
		}
		return *this;
	}

	TWProtocolData_B_StartPay(const TWProtocolData_B_StartPay&other)
	{
		*this = other;
	}
private:


};
// B4
union TWProtocolData_B_CANCLE_U
{
	char* pDataJson;/*0x02*/
	TWProtocolData_B_CANCLE_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_CANCLE_U));
	}
};



struct TWProtocolData_B_CANCLE
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_CANCLE_U data;
	TWProtocolData_B_CANCLE(){}
	TWProtocolData_B_CANCLE(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_CANCLE()
	{
		if (TOPCMD != TWPC_B_CANCLE)
		{
			;
		}
		if (SUBCMD == TWPC_B_CANCLE_JSON)
		{
			delete data.pDataJson;
			data.pDataJson = 0;
		}
	}
	TWProtocolData_B_CANCLE& operator = (const TWProtocolData_B_CANCLE&other)
	{
		if (this != &other)
		{
			if (SUBCMD == TWPC_B_CANCLE_JSON)
			{
				delete data.pDataJson;
				data.pDataJson = 0;
			}
			memcpy(this, &other, sizeof(TWProtocolData_B_CANCLE));
			if (SUBCMD == TWPC_B_CANCLE_JSON)
			{
				data.pDataJson = new char [strlen(other.data.pDataJson) + 1];
				memset(data.pDataJson, 0, strlen(other.data.pDataJson) + 1);
				strcpy(data.pDataJson, other.data.pDataJson);
			}
		}
		return *this;

	}
	
	TWProtocolData_B_CANCLE(const TWProtocolData_B_CANCLE&other)
	{
		*this = other;
	}
private:



};

// B5
union TWProtocolData_B_SendData_U
{
	unsigned int Result;/*0x00-0x04*/
	TWProtocolData_B_SendData_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_SendData_U));
	}
};

struct TWProtocolData_B_SendData
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_SendData_U data;
	TWProtocolData_B_SendData(){}
	TWProtocolData_B_SendData(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_SendData()
	{

	}
	TWProtocolData_B_SendData& operator = (const TWProtocolData_B_SendData&other)
	{
		if (this != &other)
		{
			memcpy(this, &other, sizeof(TWProtocolData_B_SendData));
		}
		return *this;
	}
	TWProtocolData_B_SendData(const TWProtocolData_B_SendData&other)
	{
		*this = other;
	}
private:
	


};
// B6
union TWProtocolData_B_HardwareConfig_U
{
	struct /*0x00*/
	{
		unsigned int Result;
		char Val[15];
	} VersionInfo;

	struct /*0x01*/
	{
		unsigned int Result;
		unsigned char Value;
	} CANID;

	struct /*0x02*/
	{
		unsigned int Result;
		struct
		{
			char Batch[8];
			char BurnTime[14];
		} ProductInfo;
	} MAX;

	struct /*0x03*/
	{
		unsigned int Result;
		char* pValue;
	} FireWare;

	struct /*0x04*/
	{
		unsigned int Result;
		unsigned short Value;
	} ScanTime;

	struct /*0x05*/
	{
		unsigned int Result;
		char Value[20];
	} DeviceID;

	struct /*0x06*/
	{
		unsigned int Result;
		unsigned char Value;
	} BootScreenID;

	struct /*0x07 - 0x0D*/
	{
		unsigned int Result;
		struct
		{
			unsigned char IPVer;
			char Value[16];
		} IP;
	} IPInfo;

	struct /*0x0E*/
	{
		unsigned int Result;
		char* pJson;
	} SoftwareInfo;

	struct /*0x0F*/
	{
		unsigned int Result;
		char* pValue;
	} HardTemp;

	struct /*0x10*/
	{
		unsigned int Result;
		char Value[28 + 1]; /*�˴�ʵ�����ַ�������˴�С+1*/
	} TerminalTime;

	struct /*0x31-0x90*/
	{
		unsigned int Result;
	} DefaultInfo;
	TWProtocolData_B_HardwareConfig_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_HardwareConfig_U));
	}
};



struct TWProtocolData_B_HardwareConfig
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_HardwareConfig_U data;
	TWProtocolData_B_HardwareConfig(){}
	TWProtocolData_B_HardwareConfig(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_HardwareConfig()
	{
		if (TOPCMD != TWPC_B_HARDWARECONFIG)
		{
			;
		}
		if (SUBCMD == TWPC_B_HARDWARECONFIG_FIRMWARE)
		{
			delete data.FireWare.pValue;
			data.FireWare.pValue = 0;
		}
		else if (SUBCMD == TWPC_B_HARDWARECONFIG_SOFTVERSION)
		{
			delete data.SoftwareInfo.pJson;
			data.SoftwareInfo.pJson = 0;
		}
		else if (SUBCMD == TWPC_B_HARDWARECONFIG_HARDWARETEMP)
		{
			delete data.HardTemp.pValue;
			data.HardTemp.pValue = 0;
		}
	}
	TWProtocolData_B_HardwareConfig& operator = (const TWProtocolData_B_HardwareConfig&other)
	{
		if (this != &other)
		{
			if (SUBCMD == TWPC_B_HARDWARECONFIG_FIRMWARE)
			{
				delete data.FireWare.pValue;
				data.FireWare.pValue = 0;
			}
			else if (SUBCMD == TWPC_B_HARDWARECONFIG_SOFTVERSION)
			{
				delete data.SoftwareInfo.pJson;
				data.SoftwareInfo.pJson = 0;
			}
			else if (SUBCMD == TWPC_B_HARDWARECONFIG_HARDWARETEMP)
			{
				delete data.HardTemp.pValue;
				data.HardTemp.pValue = 0;
			}
			memcpy(this,&other,sizeof(TWProtocolData_B_HardwareConfig));
			if (SUBCMD == TWPC_B_HARDWARECONFIG_FIRMWARE)
			{
				data.FireWare.pValue = new char [strlen(other.data.FireWare.pValue)+1];
				memset(data.FireWare.pValue,0,strlen(other.data.FireWare.pValue)+1);
				strcpy(data.FireWare.pValue,other.data.FireWare.pValue);
				
			}
			else if (SUBCMD == TWPC_B_HARDWARECONFIG_SOFTVERSION)
			{
				
				data.SoftwareInfo.pJson = new char [strlen(other.data.SoftwareInfo.pJson)+1];
				memset(data.SoftwareInfo.pJson,0,strlen(other.data.SoftwareInfo.pJson)+1);
				strcpy(data.SoftwareInfo.pJson,other.data.SoftwareInfo.pJson);
			}
			else if (SUBCMD == TWPC_B_HARDWARECONFIG_HARDWARETEMP)
			{
				data.HardTemp.pValue = new char [strlen(other.data.HardTemp.pValue)+1];
				memset(data.HardTemp.pValue,0,strlen(other.data.HardTemp.pValue)+1);
				strcpy(data.HardTemp.pValue,other.data.HardTemp.pValue);
			}
		}
		return *this;
	}
	TWProtocolData_B_HardwareConfig(const TWProtocolData_B_HardwareConfig&other)
	{
		*this = other;
	}
private:
	
};
// B7
union TWProtocolData_B_Auth_U
{
	unsigned int Result;
	TWProtocolData_B_Auth_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_Auth_U));
	}

};

struct TWProtocolData_B_Auth
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_Auth_U data;
	TWProtocolData_B_Auth(){}
	TWProtocolData_B_Auth(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_Auth()
	{

	}
	TWProtocolData_B_Auth& operator = (const TWProtocolData_B_Auth&other)
	{
		if (this != &other)
		{
			memcpy(this, &other, sizeof(TWProtocolData_B_Auth));
		}
		return *this;
	}
	TWProtocolData_B_Auth(const TWProtocolData_B_Auth&other)
	{
		*this = other;
	}
private:
};


union TWProtocolData_B_AgencyDebit_U
{
	unsigned int Result;//0x01
	TWProtocolData_B_AgencyDebit_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_AgencyDebit_U));
	}

};

struct TWProtocolData_B_AgencyDebit
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_AgencyDebit_U data;
	TWProtocolData_B_AgencyDebit(){}
	TWProtocolData_B_AgencyDebit(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_AgencyDebit()
	{

	}
	TWProtocolData_B_AgencyDebit& operator = (const TWProtocolData_B_AgencyDebit&other)
	{
		if (this != &other)
		{
			memcpy(this, &other, sizeof(TWProtocolData_B_AgencyDebit));
		}
		return *this;
	}
	TWProtocolData_B_AgencyDebit(const TWProtocolData_B_AgencyDebit&other)
	{
		*this = other;
	}
private:
};


union TWProtocolData_B_GreenChannel_U
{
	char NFCParaInfo[1000];//0x01
	TWProtocolData_B_GreenChannel_U()
	{
		memset(this, 0, sizeof(TWProtocolData_B_GreenChannel_U));
	}

};


struct TWProtocolData_B_GreenChannel
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_B_GreenChannel_U data;
	TWProtocolData_B_GreenChannel(){}
	TWProtocolData_B_GreenChannel(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_B_GreenChannel()
	{

	}
	TWProtocolData_B_GreenChannel& operator = (const TWProtocolData_B_GreenChannel&other)
	{
		if (this != &other)
		{
			memcpy(this, &other, sizeof(TWProtocolData_B_GreenChannel));
		}
		return *this;
	}
	TWProtocolData_B_GreenChannel(const TWProtocolData_B_GreenChannel&other)
	{
		if(this != &other)
		{
			*this = other;
		}
	}
private:
};
// C0
union TWProtocolData_C_Reply_U
{
	unsigned int Unuse;/*0x00- 0x04,��һ���ֽ����������ڶ����ֽ����������������ֽ�Ϊ���к�*/
	TWProtocolData_C_Reply_U()
	{
		memset(this, 0, sizeof(TWProtocolData_C_Reply_U));
	}
};


struct TWProtocolData_C_Reply
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_C_Reply_U data;
	TWProtocolData_C_Reply(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_C_Reply()
	{

	}
private:
	TWProtocolData_C_Reply(const TWProtocolData_C_Reply&);
	TWProtocolData_C_Reply& operator = (const TWProtocolData_C_Reply&);
};

// D1
union TWProtocolData_D_Query_U
{
	unsigned char Unuse;/*0x00-0x01*/
	TWProtocolData_D_Query_U()
	{
		memset(this, 0, sizeof(TWProtocolData_D_Query_U));
	}
};



struct TWProtocolData_D_Query
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_D_Query_U data;
	TWProtocolData_D_Query(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_D_Query()
	{

	}
private:
	TWProtocolData_D_Query(const TWProtocolData_D_Query&);
	TWProtocolData_D_Query& operator = (const TWProtocolData_D_Query&);
};
// D2
union TWProtocolData_D_SelfCheck_U
{
	char* pJson;    /*0x00,ע�⣬Э��������Ϊ����4���д���*/
	TWProtocolData_D_SelfCheck_U()
	{
		memset(this, 0, sizeof(TWProtocolData_D_SelfCheck_U));
	}
};

struct TWProtocolData_D_SelfCheck
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_D_SelfCheck_U data;
	TWProtocolData_D_SelfCheck(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_D_SelfCheck()
	{
		if (TOPCMD != TWPC_D_SELFCHECK)
		{
			;
		}
		if (SUBCMD == TWPC_D_SELFCHECK_POWER)
		{
			delete data.pJson;
			data.pJson = 0;
		}
	}
private:
	TWProtocolData_D_SelfCheck(const TWProtocolData_D_SelfCheck&);
	TWProtocolData_D_SelfCheck& operator = (const TWProtocolData_D_SelfCheck&);
};
// E1
union TWProtocolData_E_Query_U
{
	struct /*0x00*/
	{
		unsigned int Result;
		char Unkown[4];/*ע�⣬�˴�Э����û��˵��*/
	} Status;

	struct /*0x01*/
	{
		unsigned int Result;
		char Unkown[4];/*ע�⣬Э����û��˵��*/
	} Version;
	TWProtocolData_E_Query_U()
	{
		memset(this, 0, sizeof(TWProtocolData_E_Query_U));
	}

};


struct TWProtocolData_E_Query
{
	unsigned char TOPCMD;
	unsigned char SUBCMD;
	TWProtocolData_E_Query_U data;
	TWProtocolData_E_Query(const TWProtocol_BaseFrame& owner)
	{
		TOPCMD = owner.TOPCMD;
		SUBCMD = owner.SUBCMD;
	}
	~TWProtocolData_E_Query()
	{
	}
private:
	TWProtocolData_E_Query(const TWProtocolData_E_Query&);
	TWProtocolData_E_Query& operator = (const TWProtocolData_E_Query&);
};


#define SWITCHTO_V2_0_SYN  10
#define SWITCHTO_V2_0_SYN_ACK 11
#define SWITCHTO_V2_0_ACK  12
#define SWITCHTO_V1_5_SYN  20
#define SWITCHTO_V1_5_SYN_ACK  21
#define SWITCHTO_V1_5_SYN  22

#endif // TW_PROTOCOL_DATA_H
