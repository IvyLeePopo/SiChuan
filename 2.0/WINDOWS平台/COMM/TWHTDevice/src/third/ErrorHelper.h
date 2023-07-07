/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: ErrorHelper.h
* ժ   Ҫ:  ���ڼ�¼�Ͳ�ѯ������Ϣ
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-10
* �޸ļ�¼: 
*/
#ifndef ERROR_HELPER_H
#define ERROR_HELPER_H
#include <string>
#include "ErrorNo.h"


#define TW_SET_ERROR(x) (SetErrorNo(x))
#define TW_RESET_ERROR() (ResetErrorNO())


typedef unsigned int TW_ERROR_TYPE;
void SetErrorNo(const TW_ERROR_TYPE err);
TW_ERROR_TYPE GetErrorNo();
void ResetErrorNO();
std::string GetErrorDesc();



#endif // ERROR_HELPER_H
