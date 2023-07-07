/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLMMI.h
 * 摘    要：交互参数XML数据结构，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLMMI :
	public CDVXMLBase
{
public:
	CDVXMLMMI(void);
	~CDVXMLMMI(void);
//Interface
public:
	//解析XML字符串
	const BOOL	  Init(IN const CString& strXMLString);
//Attributes
public:
	//交互界面特殊处理标记
	int ClearMMI;
	//文字显示数据
	CArray<tagMMIItemText,tagMMIItemText&>		Text;
	//声音数据
	CArray<tagMMIItemVoice,tagMMIItemVoice&>	Voice;
	//图形数据
	CArray<tagMMIItemGraph,tagMMIItemGraph&>	Graph;

	//媒体数据（用于广告播放）
	CArray<tagMMIMedia,tagMMIMedia&>			Media;
};
