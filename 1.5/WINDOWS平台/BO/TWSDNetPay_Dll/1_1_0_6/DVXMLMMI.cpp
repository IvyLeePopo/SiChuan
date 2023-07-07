/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLMMI.cpp
 * ժ    Ҫ����������XML���ݽṹ��ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
 */
#include "StdAfx.h"
#include "DVXMLMMI.h"

CDVXMLMMI::CDVXMLMMI(void)
	:ClearMMI(FALSE)
{
	XMLFunctionDesc.Format(_T("MMIParamContext"));
}

CDVXMLMMI::~CDVXMLMMI(void)
{
	Text.RemoveAll();
	Voice.RemoveAll();
	Graph.RemoveAll();
	Media.RemoveAll();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVXMLMMI::Init
//     
// ��������������XML�ַ���
//     
// ���������IN const CString& strXMLString
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CDVXMLMMI::Init(IN const CString& strXMLString)
{
	BOOL bReturn = FALSE;
	CString strTemp;
	CMarkup xml;
	try
	{
		Text.RemoveAll();
		Voice.RemoveAll();
		Graph.RemoveAll();
		Media.RemoveAll();

		xml.SetDoc( strXMLString );

		xml.ResetMainPos();

		if(!xml.FindElem(XMLFunctionDesc))
			throw -1;
		xml.IntoElem();

		xml.ResetPos();
		if(!xml.FindChildElem(_T("Version")))
			throw -2;
		XMLVersion = xml.GetChildData();

		xml.ResetPos();
		if(!xml.FindChildElem(_T("Data")))
			throw -3;
		xml.IntoElem();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("ClearMMI")))
			throw -4;
		ClearMMI = _ttoi(xml.GetChildData());

		/*****************Item��ϸ��************************/
		if(0 == ClearMMI)
		{
			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("ItemData")))
				throw -1111;
			xml.IntoElem();

			xml.ResetChildPos();
			while(xml.FindChildElem(_T("Item")))
			{
				xml.IntoElem();

				xml.ResetChildPos();
				if(!xml.FindChildElem(_T("Type")))
					throw -6;
				int iType = _ttoi(xml.GetChildData());
				switch(iType)
				{
				case 1:
					{
						tagMMIItemText sItem = {0};
						sItem.Type = iType;

						xml.ResetChildPos();
						if(!xml.FindChildElem(_T("Sub")))
							throw -7;
						sItem.Sub = _ttoi(xml.GetChildData());

						xml.ResetChildPos();
						if(!xml.FindChildElem(_T("RowNo")))
							throw -7;
						sItem.Row = _ttoi(xml.GetChildData());

						xml.ResetChildPos();
						if(!xml.FindChildElem(_T("Context")))
							throw -8;
						strTemp = xml.GetChildData();
						_tcscpy_s(sItem.Context,_countof(sItem.Context),strTemp.GetBuffer(0));
						strTemp.ReleaseBuffer();

						xml.ResetChildPos();
						if(!xml.FindChildElem(_T("IsScroll")))
							throw -9;
						sItem.IsScroll = (BOOL)_ttoi(xml.GetChildData());

						Text.Add(sItem);
					}
					break;
					
				default:
					tagMMIMedia sItem = {0};
					sItem.Type = iType;

					xml.ResetChildPos();
					if(!xml.FindChildElem(_T("Context")))
						throw -13;
					strTemp = xml.GetChildData();
					_tcscpy_s(sItem.Content, _countof(sItem.Content), strTemp.GetBuffer(0));
					strTemp.ReleaseBuffer();

					Media.Add(sItem);
					break;

					/*
				case 2:
					{
						tagMMIItemVoice sItem = {0};
						sItem.Type = iType;

						xml.ResetChildPos();
						if(!xml.FindChildElem(_T("VoiceNo")))
							throw -10;
						sItem.No = _ttoi(xml.GetChildData());

						Voice.Add(sItem);
					}
					break;
				case 3:
					{
						tagMMIItemGraph sItem = {0};
						sItem.Type = iType;

						xml.ResetChildPos();
						if(!xml.FindChildElem(_T("GraphNo")))
							throw -11;
						sItem.No = _ttoi(xml.GetChildData());

						Graph.Add(sItem);
					}
					break;
					*/
				}
				xml.OutOfElem();
			}
		}
		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ������ͷ MMIParamContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ�����ݰ汾 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ�������� Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(�������) ClearMMI"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ�������� Item"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(���ý�������) Type"));
			break;
		case -7:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(�����������������) Sub"));
			break;
		case -8:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(���������к�) RowNo"));
			break;
		case -9:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(������������) Context"));
			break;
		case -10:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(�������ֲ��ŷ�ʽ) IsScroll"));
			break;
		case -11:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(������������) VoiceNo"));
			break;
		case -12:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(����ͼ�α���) GraphNo"));
			break;
		case -13:
			m_strLastErrorDesc.Format(_T("�����������ô����������ʧ��,�޷��ҵ�ָ��������(����ý������) Context"));
			break;
		}
	}

	return bReturn;
}