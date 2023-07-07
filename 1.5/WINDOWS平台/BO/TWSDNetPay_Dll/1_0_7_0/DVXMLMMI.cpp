/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLMMI.cpp
 * 摘    要：交互参数XML数据结构，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
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
// 函数名称：CDVXMLMMI::Init
//     
// 功能描述：解析XML字符串
//     
// 输入参数：IN const CString& strXMLString
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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

		/*****************Item详细项************************/
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
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据头 MMIParamContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据版本 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据区 Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(清屏标记) ClearMMI"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据区 Item"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(设置交互类型) Type"));
			break;
		case -7:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(设置文字数据项分类) Sub"));
			break;
		case -8:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(设置文字行号) RowNo"));
			break;
		case -9:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(设置文字内容) Context"));
			break;
		case -10:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(设置文字播放方式) IsScroll"));
			break;
		case -11:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(设置声音编码) VoiceNo"));
			break;
		case -12:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(设置图形编码) GraphNo"));
			break;
		case -13:
			m_strLastErrorDesc.Format(_T("交互界面设置传入参数解析失败,无法找到指定数据项(设置媒体数据) Context"));
			break;
		}
	}

	return bReturn;
}