/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: RawFrameConv.h
* 摘   要:  进行原始串口数据和帧数据之间的转换，以及帧数据内部的转换
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-11
* 修改记录:
*/
#ifndef RAW_FRAME_CONV_H
#define RAW_FRAME_CONV_H
#include "TWProtocolData.h"
#include "SerialPort.h"
#include <vector>

#define MAKEINTBIG(fir,sec,thir,four) ( MAKELONG(MAKEWORD((four),(thir)),MAKEWORD((sec),(fir) )) )
#define MAKEWORDBIG(fir,sec) ( MAKEWORD((sec),(fir)) )

bool HexStrToBaseFrame(const std::string& hexStr,TWProtocol_BaseFrame& frame);

bool RawDataToFrame(const RawSerialData& src, TWProtocol_BaseFrame& dest, TWProtocol_BaseFrame&toReply);

bool RawDataToFrame(const std::list<unsigned char>& src, TWProtocol_BaseFrame& dest, TWProtocol_BaseFrame&toReply);
bool FrameEscape(const unsigned char*const  src, int src_len, unsigned char* &dst, int& dst_len);
bool FrameUnEscape(const unsigned char*const  src, int src_len, unsigned char*&dst, int&dst_len);

bool FrameEscape(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst ); //非手动申请内存形式
bool FrameUnEscape(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst ); //非手动申请内存形式

bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_Beat& dataStruct );
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_Interactive& dataStruct );
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_StartPay& dataStruct );
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_Cancle& dataStruct );
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_SendData& dataStruct );
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_HARDWARECONFIG& dataStruct );
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_Auth& dataStruct );
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_AgencyDebit& dataStruct );

bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_GreenChannel& dataStruct );

bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_E_Query& dataStruct );
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_C_Reply& dataStruct );



bool FillProtocolStruct(TWProtocolData_B_Beat& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_B_Interactive& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_B_StartPay& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_B_CANCLE& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_B_SendData& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_B_HardwareConfig& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_B_Auth& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_B_AgencyDebit& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_B_GreenChannel& dataStruct, const TWProtocol_BaseFrame& frame );



bool FillProtocolStruct(TWProtocolData_C_Reply& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_D_Query& dataStruct, const TWProtocol_BaseFrame& frame );
bool FillProtocolStruct(TWProtocolData_D_SelfCheck& dataStruct, const TWProtocol_BaseFrame& frame );






#endif // RAW_FRAME_CONV_H





