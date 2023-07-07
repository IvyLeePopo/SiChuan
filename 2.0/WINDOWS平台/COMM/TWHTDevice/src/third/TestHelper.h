/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: TestHelper.h
* ժ   Ҫ:  ��Ԫ�����������
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-13
* �޸ļ�¼: 
*/
#ifndef TEST_HELPER_H
#define TEST_HELPER_H
#define TW_SELF_TEST_TRACE

struct TestConfig
{
    unsigned int BeatInterval;
    int Money;
    int VehType;
    int VehClass;
    int VehPlateColor;
    char VehPlate[16];
    char EnStationName[24];
    unsigned char FlagStationNum;
    int AxleCount;
    int Weight;
    int WeightLimit;
    char TransID[32];
    int Balance;
    char* JsonInfo;
    int NeedFeedBack;
    int UnixTime;
    int ProvinceID;
    int AreaID;
    int RoadID;
    int StationID;
    char StationName[12];
    char ServerType;
    char ServerIP[4];
    char CANID;
    char EthIP[4];
    TestConfig()
    {
        memset(this,0,sizeof(TestConfig));
    }
};

#endif // TEST_HELPER_H
