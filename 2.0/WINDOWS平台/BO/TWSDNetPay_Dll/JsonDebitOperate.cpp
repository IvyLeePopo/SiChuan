/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�JsonDebitOperate.cpp
 * ժ    Ҫ�����ƻ������ۿ����ݣ��������ݴ�ȡ�Լ���ʽ�����̣�ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��2��27��
 */
#include "StdAfx.h"
#include "JsonDebitOperate.h"

CJsonDebitOperate::CJsonDebitOperate(void)
{
}

CJsonDebitOperate::~CJsonDebitOperate(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonDebitOperate::FormatSendData2Json
//     
// ���������������յ������ݽṹת��Ϊ��׼��JSON��ʽ
//     
// ���������IN const CDVJsonDebit* pDVJsonDebit	--	��Ҫת�������ݽṹ��
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����PC����HTTP��
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString	CJsonDebitOperate::FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit)
{
	CString strReturn;
	
	if(NULL != pDVJsonDebit)
	{
		// ��ʾ���� json ����
		Json::Value root;
		// ��ʱJSON����
		Json::Value DebitCondition;

		Json::Value DebitConditionBase;

		Json::Value DebitConditionAddtional;

		Json::Value AddtionalEntryStation;

		Json::Value AddtionalPayStation;

		Json::Value AddtionalVehicleInfo;
		/***********************************************************����JSAON���ݽṹ***********************************************************/
		
		//��ʶ�汾
		root["Version"]				= Json::Value(pDVJsonDebit->Version);
		//��ʶ����ģʽ(��ʽ���ݻ��������)
		root["IsTestData"]			= Json::Value(pDVJsonDebit->TestingData);
		//��ʶ��������
		root["PackageType"]			= Json::Value(pDVJsonDebit->PackageType);
		//��ȫ��֤
		root["Token"]				= Json::Value(pDVJsonDebit->Token);
		//��ʶʡ��
		root["ProvinceID"]			= Json::Value(pDVJsonDebit->ProvinceID);
		//�ݽ�����
		root["SubmitCount"]			= Json::Value(pDVJsonDebit->SubmitCount);
		//�ݽ�ʱ��
		root["SubmitTime"]			= Json::Value(pDVJsonDebit->SubmitTime);
		//���ױ��
		root["RecordMatchCode"]		= Json::Value(pDVJsonDebit->RecordMatchCode);
		//�ۿ�ƾ֤
		root["PayIdentifier"]		= Json::Value(pDVJsonDebit->PayIdentifier);
		//���װ�ȫ��֤��
		root["TradeSecurityCode"]	= Json::Value(pDVJsonDebit->TradeSecurityCode);
		
		//�Ƿ��������
		root["TestingData"]			= Json::Value((int)pDVJsonDebit->TestingData);
		//ҵ��̬��汾��
		root["NetPayDllVersion"]	= Json::Value(pDVJsonDebit->NetPayDllVersion);
		//�豸��̬��汾��
		root["DeviceDllVersion"]	= Json::Value(pDVJsonDebit->DeviceDllVersion);
		//�û�����������Դ
		root["AccountInfoSrcType"]	= Json::Value(pDVJsonDebit->AccountInfoSrcType);
		//�û����ݾ����豸��
		root["AccountInfoDeviceID"]	= Json::Value(pDVJsonDebit->AccountInfoDeviceID);
		//�û����ݾ�������
		root["AccountInfoDataType"]	= Json::Value(pDVJsonDebit->AccountInfoDataType);

		//��������Ϣ
		DebitConditionBase["ChannelUserId"]			= Json::Value(pDVJsonDebit->IDBindInPayPlatform);
		DebitConditionBase["ChannelType"]			= Json::Value(pDVJsonDebit->IDBindInPayPlatformType);
		DebitConditionBase["PayPlatformQRCode"]		= Json::Value(pDVJsonDebit->QRCodeData);
		DebitConditionBase["VirtualCardID"]			= Json::Value(pDVJsonDebit->SMVirtualCardID);
		DebitConditionBase["EntryInfoUniqueFlag"]	= Json::Value(pDVJsonDebit->EntryInfoUniqueFlag);
		DebitConditionBase["DebitMoney"]			= Json::Value(pDVJsonDebit->DebitMoney);
		DebitConditionBase["DebitType"]				= Json::Value(pDVJsonDebit->DebitType);
		DebitConditionBase["DebitTime"]				= Json::Value(pDVJsonDebit->DebitTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		DebitConditionBase["TollDate"]				= Json::Value(pDVJsonDebit->DebitTollDate);
		DebitConditionBase["TollShift"]				= Json::Value(pDVJsonDebit->DebitTollShift);

		//��丽����Ϣ -- ��վ��Ϣ
		AddtionalEntryStation["EntryProvinceId"]	= Json::Value(pDVJsonDebit->EntryProvinceID);
		AddtionalEntryStation["EntryStationId"]		= Json::Value(pDVJsonDebit->EntryStationID);
		AddtionalEntryStation["EntryStationName"]	= Json::Value(pDVJsonDebit->EntryStationName);
		AddtionalEntryStation["EntryLane"]			= Json::Value(pDVJsonDebit->EntryLane);
		AddtionalEntryStation["EntryTime"]			= Json::Value(pDVJsonDebit->EntryTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		AddtionalEntryStation["EntryRoadNo"]		= Json::Value(pDVJsonDebit->EntryRoadID);
		AddtionalEntryStation["EntryAreaNo"]		= Json::Value(pDVJsonDebit->EntryAreaID);

		//��丽����Ϣ -- �۷�վ��Ϣ
		AddtionalPayStation["PayProvinceId"]		= Json::Value(pDVJsonDebit->PayProvinceID);
		AddtionalPayStation["PayStationId"]			= Json::Value(pDVJsonDebit->PayStationID);
		AddtionalPayStation["PayStationName"]		= Json::Value(pDVJsonDebit->PayStationName);
		AddtionalPayStation["PayLane"]				= Json::Value(pDVJsonDebit->PayLane);
		AddtionalPayStation["PayRoadNo"]			= Json::Value(pDVJsonDebit->PayRoadID);

		//��丽����Ϣ -- �۷ѳ�����Ϣ
		AddtionalVehicleInfo["VehicleClass"]		= Json::Value(pDVJsonDebit->VehicleClass);
		AddtionalVehicleInfo["VehicleType"]			= Json::Value(pDVJsonDebit->VehicleType);
		AddtionalVehicleInfo["VehicleLicense"]		= Json::Value(pDVJsonDebit->VehicleLicense);
		AddtionalVehicleInfo["VehicleColor"]		= Json::Value(pDVJsonDebit->VehicleColor);
		AddtionalVehicleInfo["AxisCount"]			= Json::Value(pDVJsonDebit->AxisCount);
		AddtionalVehicleInfo["TotalWeight"]			= Json::Value(pDVJsonDebit->TotalWeight);
		AddtionalVehicleInfo["LimitWeight"]			= Json::Value(pDVJsonDebit->LimitWeight);

		//��ʽ��
		DebitConditionAddtional["EntryStation"] = AddtionalEntryStation;
		DebitConditionAddtional["PayStation"]	= AddtionalPayStation;
		DebitConditionAddtional["VehicleInfo"]	= AddtionalVehicleInfo;

		DebitCondition["Base"]			= DebitConditionBase;
		DebitCondition["Addtional"]		= DebitConditionAddtional;

		root["Data"] = DebitCondition;

		//ת��Ϊ�ַ���
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
		//����ʹ�ã���/дȡ����
		#ifdef _JSON_DEBUG_LOG_STRING
			try
			{
				CFile cFile;
				size_t ttLen = 0;
				TCHAR* szBuff = NULL;
				CString strResult(strReturn);
				CString strFilePath;
				strFilePath.Format(_T("%s\\DebitMoney.txt"),::GetGlobalApp()->m_strPathTempDir);
				if(cFile.Open(strFilePath,CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate))
				{
					////������
					//szBuff = new TCHAR[cFile.GetLength()+1];
					//memset(szBuff,0,cFile.GetLength()+1);
					//cFile.Read(szBuff,cFile.GetLength());
					//cFile.Close();
					//strResult.Format(_T("%s"),(LPCTSTR)szBuff);
					//ttLen = strResult.GetLength();
					//delete szBuff;
					//szBuff = NULL;

					//д����
					cFile.SeekToBegin();
					cFile.Write(strResult.GetBuffer(0),strResult.GetLength());
					strResult.ReleaseBuffer();
					cFile.Flush();
					cFile.SetLength(strResult.GetLength());
					cFile.Close();
				}
			}
			catch(...)
			{
			}
		#endif
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CString	CJsonDebitOperate::FormatSendData2Json
//     
// ���������������յ������ݽṹת��Ϊ��׼��JSON��ʽ -- �۷�״̬��ѯ
//     
// ���������IN const CDVJsonDebitCheck* pDVJsonDebitCheck
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����PC->Http
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString	CJsonDebitOperate::FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck)
{
	CString strReturn;
	if(NULL != pDVJsonDebitCheck)
	{
		// ��ʾ���� json ����
		Json::Value root;
		// ��ʱJSON����
		Json::Value DebitCheckCondition;

		Json::Value DebitCheckBase;
		/***********************************************************����JSAO�����ݽṹ***********************************************************/
		
		//��ʶ�汾
		root["Version"]				= Json::Value(pDVJsonDebitCheck->Version);
		//��ʶ��������
		root["PackageType"]			= Json::Value(pDVJsonDebitCheck->PackageType);
		//��ȫ��֤
		root["Token"]				= Json::Value(pDVJsonDebitCheck->Token);
		//��ʶʡ��
		root["ProvinceID"]			= Json::Value(pDVJsonDebitCheck->ProvinceID);
		//�ݽ�����
		root["SubmitCount"]			= Json::Value(pDVJsonDebitCheck->SubmitCount);
		//�ݽ�ʱ��
		root["SubmitTime"]			= Json::Value(pDVJsonDebitCheck->SubmitTime);
		//�ۿ�ƾ֤
		root["PayIdentifier"]		= Json::Value(pDVJsonDebitCheck->PayIdentifier);

		////���������Ϣ
		//DebitCheckBase["ChannelUserId"]		= Json::Value(pDVJsonDebitCheck->IDBindInPayPlatform);
		//DebitCheckBase["ChannelType"]		= Json::Value(pDVJsonDebitCheck->IDBindInPayPlatformType);
		//DebitCheckBase["PayPlatformQRCode"]	= Json::Value(pDVJsonDebitCheck->QRCodeData);
		//DebitCheckBase["PayIdentifier"]	= Json::Value(pDVJsonDebitCheck->PayIdentifier);
		//DebitCheckBase["DebitMoney"]		= Json::Value(pDVJsonDebitCheck->DebitMoney);
		//DebitCheckBase["DebitType"]			= Json::Value(pDVJsonDebitCheck->DebitType);
		//DebitCheckBase["DebitTime"]			= Json::Value(pDVJsonDebitCheck->DebitTime.Format(_T("%Y-%m-%d %H:%M:%S")));

		////��ʽ��
		//DebitCheckCondition["Base"]	= DebitCheckBase;
		//root["Data"] = DebitCheckCondition;

		//ת��Ϊ�ַ���
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CString	CJsonDebitOperate::FormatSendData2Json
//     
// ���������������յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������(����)
//     
// ���������IN const CDVJsonDebitCancel* pDVJsonDebitCancel
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����PC->Http
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString	CJsonDebitOperate::FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel)
{
	CString strReturn;
	if(NULL != pDVJsonDebitCancel)
	{
		// ��ʾ���� json ����
		Json::Value root;
		// ��ʱJSON����
		Json::Value DebitCancelCondition;

		Json::Value DebitCancelBase;
		/***********************************************************����JSAO���ݽṹ***********************************************************/
		
		//��ʶ�汾
		root["Version"]				= Json::Value(pDVJsonDebitCancel->Version);
		//��ʶ��������
		root["PackageType"]			= Json::Value(pDVJsonDebitCancel->PackageType);
		//��ȫ��֤
		root["Token"]				= Json::Value(pDVJsonDebitCancel->Token);
		//��ʶʡ��
		root["ProvinceID"]			= Json::Value(pDVJsonDebitCancel->ProvinceID);
		//�ݽ�����
		root["SubmitCount"]			= Json::Value(pDVJsonDebitCancel->SubmitCount);
		//�ݽ�ʱ��
		root["SubmitTime"]			= Json::Value(pDVJsonDebitCancel->SubmitTime);
		//�ۿ�ƾ֤
		root["PayIdentifier"]		= Json::Value(pDVJsonDebitCancel->PayIdentifier);

		//ת��Ϊ�ַ���
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonDebitOperate::FormatSendData2Json
//     
// ���������������յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧�����󣨶��δ���
//     
// ���������IN const CDVJsonDebitSecond* pDVJsonDebitSecond
// ���������CString
// ��д��Ա��ROCY
// ��дʱ�䣺2016��5��30��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵��������-��Http�����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CJsonDebitOperate::FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond)
{
	CString strReturn;
	
	if(NULL != pDVJsonDebitSecond)
	{
		// ��ʾ���� json ����
		Json::Value root;
		// ��ʱJSON����
		Json::Value DebitCondition;

		Json::Value DebitConditionBase;

		Json::Value DebitConditionAddtional;

		Json::Value AddtionalEntryStation;

		Json::Value AddtionalPayStation;

		Json::Value AddtionalVehicleInfo;
		/***********************************************************����JSAO�����ݽṹ***********************************************************/
		
		//��ʶ�汾
		root["Version"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.Version);
		//��ʶ��������
		root["PackageType"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PackageType);
		//��ȫ��֤
		root["Token"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.Token);
		//��ʶʡ��
		root["ProvinceID"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.ProvinceID);
		//�ݽ�����
		root["SubmitCount"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.SubmitCount);
		//�ݽ�ʱ��
		root["SubmitTime"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.SubmitTime);
		//���ױ��
		root["RecordMatchCode"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.RecordMatchCode);
		//�ϴοۿ�ƾ֤
		root["ReversePayIdentifier"]= Json::Value(pDVJsonDebitSecond->m_strLastPayIdentifier);
		//�ۿ�ƾ֤
		root["PayIdentifier"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayIdentifier);
		//���װ�ȫ��֤��
		root["TradeSecurityCode"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.TradeSecurityCode);

		//��������Ϣ
		DebitConditionBase["ChannelUserId"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.IDBindInPayPlatform);
		DebitConditionBase["ChannelType"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.IDBindInPayPlatformType);
		DebitConditionBase["PayPlatformQRCode"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.QRCodeData);
		DebitConditionBase["VirtualCardID"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.SMVirtualCardID);
		DebitConditionBase["EntryInfoUniqueFlag"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryInfoUniqueFlag);
		DebitConditionBase["DebitMoney"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.DebitMoney);
		DebitConditionBase["DebitType"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.DebitType);
		DebitConditionBase["DebitTime"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.DebitTime.Format(_T("%Y-%m-%d %H:%M:%S")));

		//��丽����Ϣ -- ��վ��Ϣ
		AddtionalEntryStation["EntryProvinceId"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryProvinceID);
		AddtionalEntryStation["EntryStationId"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryStationID);
		AddtionalEntryStation["EntryStationName"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryStationName);
		AddtionalEntryStation["EntryLane"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryLane);
		AddtionalEntryStation["EntryTime"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryTime.Format(_T("%Y-%m-%d %H:%M:%S")));

		//��丽����Ϣ -- �۷�վ��Ϣ
		AddtionalPayStation["PayProvinceId"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayProvinceID);
		AddtionalPayStation["PayStationId"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayStationID);
		AddtionalPayStation["PayStationName"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayStationName);
		AddtionalPayStation["PayLane"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayLane);

		//��丽����Ϣ -- �۷ѳ�����Ϣ
		AddtionalVehicleInfo["VehicleClass"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleClass);
		AddtionalVehicleInfo["VehicleType"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleType);
		AddtionalVehicleInfo["VehicleLicense"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleLicense);
		AddtionalVehicleInfo["VehicleColor"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleColor);

		//��ʽ��
		DebitConditionAddtional["EntryStation"] = AddtionalEntryStation;
		DebitConditionAddtional["PayStation"]	= AddtionalPayStation;
		DebitConditionAddtional["VehicleInfo"]	= AddtionalVehicleInfo;

		DebitCondition["Base"]			= DebitConditionBase;
		DebitCondition["Addtional"]		= DebitConditionAddtional;

		root["Data"] = DebitCondition;

		//ת��Ϊ�ַ���
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
		//����ʹ�ã���/дȡ����
		#ifdef _JSON_DEBUG_LOG_STRING
			try
			{
				CFile cFile;
				size_t ttLen = 0;
				TCHAR* szBuff = NULL;
				CString strResult(strReturn);
				CString strFilePath;
				strFilePath.Format(_T("%s\\DebitMoney.txt"),::GetGlobalApp()->m_strPathTempDir);
				if(cFile.Open(strFilePath,CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate))
				{
					////������
					//szBuff = new TCHAR[cFile.GetLength()+1];
					//memset(szBuff,0,cFile.GetLength()+1);
					//cFile.Read(szBuff,cFile.GetLength());
					//cFile.Close();
					//strResult.Format(_T("%s"),(LPCTSTR)szBuff);
					//ttLen = strResult.GetLength();
					//delete szBuff;
					//szBuff = NULL;

					//д����
					cFile.SeekToBegin();
					cFile.Write(strResult.GetBuffer(0),strResult.GetLength());
					strResult.ReleaseBuffer();
					cFile.Flush();
					cFile.SetLength(strResult.GetLength());
					cFile.Close();
				}
			}
			catch(...)
			{
			}
		#endif
	}
	return strReturn;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�LPVOID	CJsonDebitOperate::FormatResponseResult2Class
//     
// ���������������������ص�JSON��ʽת��Ϊ��׼���ݽṹ��
//     
// ���������IN const CString& strResponseResult	--	��Ҫת����JSON����
//         ��IN const int& iDataType				--	��Ҫת������������
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonBase* CJsonDebitOperate::FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)
{
	CDVJsonBase*  pDVJsonBase = NULL;
	switch(iDataType)
	{
	case JSON_DT_SEND_DIBIT:
		pDVJsonBase = fnAnalysisStringForDebit(strResponseResult);
		break;
	case JSON_DT_SEND_DIBIT_CHECK:
		pDVJsonBase = fnAnalysisStringForDebitCheck(strResponseResult);
		break;
	case JSON_DT_SEND_DIBIT_CANCEL:
		pDVJsonBase = fnAnalysisStringForDebitCancel(strResponseResult);
		break;
	case JSON_DT_SEND_DIBIT_SECOND:
		pDVJsonBase = fnAnalysisStringForDebitSecond(strResponseResult);
		break;
	}
	return pDVJsonBase;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonDebitOperate::fnAnalysisStringForDebit
//     
// ���������������ַ�������ÿۿ������������
//     
// ���������IN const CString& strResponseResult
// ���������CDVJsonDebit*
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����Http->PC
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebit* CJsonDebitOperate::fnAnalysisStringForDebit(IN const CString& strResponseResult)
{
	CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();

	try
	{
		if(NULL == pDVJsonDebit)
			throw -1;

		//�������ݸ�ʽת��
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
			throw -2;
		strResult.ReleaseBuffer();
		//��ȡ�汾
		if(Json::stringValue == root["Version"].type())
		{
			pDVJsonDebit->Version					= root["Version"].asCString();
		}
		//��ȡ���ݰ�����
		if(Json::intValue == root["PackageType"].type())
		{
			pDVJsonDebit->PackageType				= root["PackageType"].asInt();
		}
		//��ȡ��Ӧ״̬����
		if(Json::intValue == root["Status"].type())
		{
			pDVJsonDebit->ServerResponseCode		= root["Status"].asInt();
		}
		//��ȡ��Ӧ״̬����
		if(Json::stringValue == root["Message"].type())
		{
			pDVJsonDebit->ServerResponseResultDesc	= root["Message"].asCString();
		}
		//��ȡ��������������ʱ��
		if(Json::stringValue == root["ReceveTime"].type())
		{
			pDVJsonDebit->ServerReceiveDataTime		= root["ReceveTime"].asCString();
		}
		//��ȡ��������Ӧ����ʱ��
		if(Json::stringValue == root["ResponseTime"].type())
		{
			pDVJsonDebit->ServerResponseDataTime	= root["ResponseTime"].asCString();
		}
		//��ȡ������ʵ�ʿۿ�ʱ��
		if(Json::stringValue == root["ServerDebitTime"].type())
		{
			pDVJsonDebit->ServerDebitTime	= CTime(GetGlobalApp()->CString2time_t(root["ServerDebitTime"].asCString()));
		}

		//��ȡ�ۿ���ϸ״̬����
		if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
		{
			pDVJsonDebit->DebitStatusCode			= root["ErrorCodeByPayPlatform"].asInt();
		}
		//��ȡ�ۿ���ϸ״̬����
		if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
		{
			pDVJsonDebit->DebitStatusDesc			= root["ErrorDescByPayPlatform"].asCString();
		}
		//��ȡ�ۿ����
		if(Json::stringValue == root["DebitOrderNumber"].type())
		{
			pDVJsonDebit->DebitOrderNumber		= root["DebitOrderNumber"].asCString();
		}
		//��ȡ���װ�ȫ��
		if(Json::stringValue == root["TradeSecurityCode"].type())
		{
			pDVJsonDebit->TradeSecurityCode		= root["TradeSecurityCode"].asCString();
		}
		//��ȡ����ʶ����
		if(Json::stringValue == root["PayIdentifier"].type())
		{
			pDVJsonDebit->PayIdentifier		= root["PayIdentifier"].asCString();
		}
		//��ȡ֧��ƽ̨����
		if(Json::intValue == root["PayChannelType"].type())
		{
			pDVJsonDebit->QRCodeType		= root["PayChannelType"].asInt();
		}
		//��ȡ���⿨��
		if(Json::stringValue == root["PayVirtualCardID"].type())
		{
			CString strTempID(root["PayVirtualCardID"].asCString());
			pDVJsonDebit->PayVirtualCardID		= strTempID.IsEmpty()?_T(""):GetGlobalApp()->FmtStr(_T("%s%s"),GetGlobalApp()->m_strVirtualCardPrefix,root["PayVirtualCardID"].asCString()); 
		}
		//������
	//	pDVJsonDebit->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://������Դʧ��
			break;
		case -2://���ݽ���ʧ��
			break;
		}
	}
	return pDVJsonDebit;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonDebitOperate::fnAnalysisStringForDebitCheck
//     
// ���������������ַ�������ÿۿ�״̬��ѯ��������
//     
// ���������IN const CString& strResponseResult
// ���������CDVJsonDebitCheck*
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����Http->PC
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCheck* CJsonDebitOperate::fnAnalysisStringForDebitCheck(IN const CString& strResponseResult)
{
	CDVJsonDebitCheck* pDVJsonDebitCheck = new CDVJsonDebitCheck();

	try
	{
		if(NULL == pDVJsonDebitCheck)
			throw -1;

		//�������ݸ�ʽת��
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
			throw -2;
		strResult.ReleaseBuffer();
		//��ȡ�汾
		if(Json::stringValue == root["Version"].type())
		{
			pDVJsonDebitCheck->Version					= root["Version"].asCString();
		}
		//��ȡ���ݰ�����
		if(Json::intValue == root["PackageType"].type())
		{
			pDVJsonDebitCheck->PackageType				= root["PackageType"].asInt();
		}
		//��ȡ��Ӧ״̬����
		if(Json::intValue == root["Status"].type())
		{
			pDVJsonDebitCheck->ServerResponseCode		= root["Status"].asInt();
		}
		//��ȡ��Ӧ״̬����
		if(Json::stringValue == root["Message"].type())
		{
			pDVJsonDebitCheck->ServerResponseResultDesc	= root["Message"].asCString();
		}
		//��ȡ��������������ʱ��
		if(Json::stringValue == root["ReceveTime"].type())
		{
			pDVJsonDebitCheck->ServerReceiveDataTime		= root["ReceveTime"].asCString();
		}
		//��ȡ��������Ӧ����ʱ��
		if(Json::stringValue == root["ResponseTime"].type())
		{
			pDVJsonDebitCheck->ServerResponseDataTime	= root["ResponseTime"].asCString();
		}
		//��ȡ������ʵ�ʿۿ�ʱ��
		if(Json::stringValue == root["ServerDebitTime"].type())
		{
			pDVJsonDebitCheck->ServerDebitTime	= CTime(GetGlobalApp()->CString2time_t(root["ServerDebitTime"].asCString()));
		}
		//��ȡ�ۿ���ϸ״̬����
		if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
		{
			pDVJsonDebitCheck->DebitStatusCode			= root["ErrorCodeByPayPlatform"].asInt();
		}
		//��ȡ�ۿ���ϸ״̬����
		if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
		{
			pDVJsonDebitCheck->DebitStatusDesc			= root["ErrorDescByPayPlatform"].asCString();
		}
		//��ȡ�ۿ����
		if(Json::stringValue == root["DebitOrderNumber"].type())
		{
			pDVJsonDebitCheck->DebitOrderNumber			= root["DebitOrderNumber"].asCString();
		}
		//��ȡ����ʶ����
		if(Json::stringValue == root["PayIdentifier"].type())
		{
			pDVJsonDebitCheck->PayIdentifier		= root["PayIdentifier"].asCString();
		}
		//��ȡ֧��ƾ֤����������֧��ƽ̨����
		if(Json::intValue == root["PayChannelType"].type())
		{
			pDVJsonDebitCheck->QRCodeType		= root["PayChannelType"].asInt();
		}

		//������
	//	pDVJsonDebitCheck->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://������Դʧ��
			break;
		case -2://���ݽ���ʧ��
			break;
		}
	}
	return pDVJsonDebitCheck;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonDebitOperate::fnAnalysisStringForDebitCancel
//     
// ���������������ַ��������֧�����󣨳���������˻�������
//     
// ���������IN const CString& strResponseResult
// ���������CDVJsonDebitCancel*
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����Http->PC
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCancel* CJsonDebitOperate::fnAnalysisStringForDebitCancel(IN const CString& strResponseResult)
{
	CDVJsonDebitCancel* pDVJsonDebitCancel = new CDVJsonDebitCancel();

	try
	{
		if(NULL == pDVJsonDebitCancel)
			throw -1;

		//�������ݸ�ʽת��
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
			throw -2;
		strResult.ReleaseBuffer();
		//��ȡ�汾
		if(Json::stringValue == root["Version"].type())
		{
			pDVJsonDebitCancel->Version					= root["Version"].asCString();
		}
		//��ȡ���ݰ�����
		if(Json::intValue == root["PackageType"].type())
		{
			pDVJsonDebitCancel->PackageType				= root["PackageType"].asInt();
		}
		//��ȡ��Ӧ״̬����
		if(Json::intValue == root["Status"].type())
		{
			pDVJsonDebitCancel->ServerResponseCode		= root["Status"].asInt();
		}
		//��ȡ��Ӧ״̬����
		if(Json::stringValue == root["Message"].type())
		{
			pDVJsonDebitCancel->ServerResponseResultDesc	= root["Message"].asCString();
		}
		//��ȡ��������������ʱ��
		if(Json::stringValue == root["ReceveTime"].type())
		{
			pDVJsonDebitCancel->ServerReceiveDataTime		= root["ReceveTime"].asCString();
		}
		//��ȡ��������Ӧ����ʱ��
		if(Json::stringValue == root["ResponseTime"].type())
		{
			pDVJsonDebitCancel->ServerResponseDataTime	= root["ResponseTime"].asCString();
		}

		//��ȡ�����Ķ�����
		if(Json::stringValue == root["DebitOrderNumber"].type())
		{
			pDVJsonDebitCancel->ServerReceiveDataTime		= root["DebitOrderNumber"].asCString();
		}

		//������
	//	pDVJsonDebitCancel->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://������Դʧ��
			break;
		case -2://���ݽ���ʧ��
			break;
		}
	}
	return pDVJsonDebitCancel;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonDebitOperate::fnAnalysisStringForDebitSecond
//     
// ���������������ַ��������֧�����󣨶��δ�������˻�������
//     
// ���������IN const CString& strResponseResult
// ���������CDVJsonDebit*
// ��д��Ա��ROCY
// ��дʱ�䣺2016��5��30��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����Http->PC
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebit* CJsonDebitOperate::fnAnalysisStringForDebitSecond(IN const CString& strResponseResult)
{
	CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();

	try
	{
		if(NULL == pDVJsonDebit)
			throw -1;

		//�������ݸ�ʽת��
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
			throw -2;
		strResult.ReleaseBuffer();
		//��ȡ�汾
		if(Json::stringValue == root["Version"].type())
		{
			pDVJsonDebit->Version					= root["Version"].asCString();
		}
		//��ȡ���ݰ�����
		if(Json::intValue == root["PackageType"].type())
		{
			pDVJsonDebit->PackageType				= root["PackageType"].asInt();
		}
		//��ȡ��Ӧ״̬����
		if(Json::intValue == root["Status"].type())
		{
			pDVJsonDebit->ServerResponseCode		= root["Status"].asInt();
		}
		//��ȡ��Ӧ״̬����
		if(Json::stringValue == root["Message"].type())
		{
			pDVJsonDebit->ServerResponseResultDesc	= root["Message"].asCString();
		}
		//��ȡ��������������ʱ��
		if(Json::stringValue == root["ReceveTime"].type())
		{
			pDVJsonDebit->ServerReceiveDataTime		= root["ReceveTime"].asCString();
		}
		//��ȡ��������Ӧ����ʱ��
		if(Json::stringValue == root["ResponseTime"].type())
		{
			pDVJsonDebit->ServerResponseDataTime	= root["ResponseTime"].asCString();
		}

		//��ȡ�ۿ���ϸ״̬����
		if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
		{
			pDVJsonDebit->DebitStatusCode			= root["ErrorCodeByPayPlatform"].asInt();
		}
		//��ȡ�ۿ���ϸ״̬����
		if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
		{
			pDVJsonDebit->DebitStatusDesc			= root["ErrorDescByPayPlatform"].asCString();
		}
		//��ȡ�ۿ����
		if(Json::stringValue == root["DebitOrderNumber"].type())
		{
			pDVJsonDebit->DebitOrderNumber		= root["DebitOrderNumber"].asCString();
		}
		//��ȡ���װ�ȫ��
		if(Json::stringValue == root["TradeSecurityCode"].type())
		{
			pDVJsonDebit->TradeSecurityCode		= root["TradeSecurityCode"].asCString();
		}
		//��ȡ֧��ƽ̨����
		if(Json::intValue == root["PayChannelType"].type())
		{
			pDVJsonDebit->ServerJudgeDebitIDType		= root["PayChannelType"].asInt();
		}
		//��ȡ֧��ƽ̨����
		if(Json::stringValue == root["PayVirtualCardID"].type())
		{
			CString strTempID(root["PayVirtualCardID"].asCString());
			pDVJsonDebit->PayVirtualCardID		= strTempID.IsEmpty()?_T(""):GetGlobalApp()->FmtStr(_T("%s%s"),GetGlobalApp()->m_strVirtualCardPrefix,root["PayVirtualCardID"].asCString()); 
		}
		//������
	//	pDVJsonDebit->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://������Դʧ��
			break;
		case -2://���ݽ���ʧ��
			break;
		}
	}
	return pDVJsonDebit;
}