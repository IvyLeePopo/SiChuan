/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: WorkingPara.h
* ժ   Ҫ:  ����ϵͳ����ʱ��ʹ�õĲ���
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-07
* �޸ļ�¼: 
*/
#ifndef WORKING_PARA_H
#define WORKING_PARA_H


size_t SystemParaComRetryCount();   //�����ط�����
size_t SystemParaComOvertime();      //���ڳ�ʱ�ȴ�����λ����
size_t SystemParaComPort();     //���ں�
size_t SystemParaComInBufferSize();
size_t SystemParaComOutBufferSize();

int SystemParaComBaud();

std::string SystemParaDBPath();

bool SystemParaEnableTransferImage();
bool SystemParaForceImageSize();

int SystemParaImageMaxSize();

#endif // WORKING_PARA_H
