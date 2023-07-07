#pragma once
#include "dvjsonbase.h"

class CDVJsonParamMMI :
	public CDVJsonBase
{
public:
	CDVJsonParamMMI(void);
	~CDVJsonParamMMI(void);
//���������
	CDVJsonParamMMI& operator=(IN const CDVJsonParamMMI& cSrc);
//Interface
public:
	//��������
	void  Reset(IN const int& iType=0);
	//��������У����
	DWORD CreateInnerVerifyCode(void){return 0;}
	//�ڲ�������У��
	BOOL  InnerVerify(void){return TRUE;}
//����
public:
	//�����������⴦����
	int ClearMMI;
	//������ʾ����
	CArray<tagMMIItemText,tagMMIItemText&>		Text;
	//��������
	CArray<tagMMIItemVoice,tagMMIItemVoice&>	Voice;
	//ͼ������
	CArray<tagMMIItemGraph,tagMMIItemGraph&>	Graph;

public:
		//��ڳ���
	CString	EntryVehicleLicense;
	//��ڳ�����ɫ
	int		EntryVLColor;
	//��ڳ���
	int		EntryVehicleType;
	//��ڳ���
	int		EntryVehicleClass;
	//���վ��
	int		EntryStationID;
	//���վ��
	CString EntryStationName;
		//����
	int		AxisCount;
	//����
	int		Weight;
	//����
	int		LimitWeight;
	//���׽��(��λ����)
	int		Money;
	//���(��λ����)
	int		Balance;
	//����վ�����
	int		CroseNum;
	//��Ŀ
	int		Program;
	//��ע��Ϣ
	CString Remark1;
	CString Remark2;


};
