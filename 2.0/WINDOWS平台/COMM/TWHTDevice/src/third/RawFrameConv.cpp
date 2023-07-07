/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: RawFrameConv.cpp
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-11
* 修改记录:
*/
#include "stdafx.h"
#include "RawFrameConv.h"
#include "ErrorHelper.h"
#include "DataAuth.h"
#include <set>
#include <vector>
#include <algorithm>
#include "TypeHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



bool ForceStrCopy(char*&dst, const char* src, int src_len)
{
    if (src_len <= 0)
    {
        return false;
    }
    delete dst;
    dst = new char[src_len + 1];
    if (dst == NULL)
    {
        return false;
    }
    memset(dst, 0, src_len + 1);
    strncpy(dst, src, src_len);
    return true;
}


std::set<unsigned char> GetValidTopCMD()
{
    std::set<unsigned  char> result;
    result.insert(TWPC_B_BEAT);
    result.insert(TWPC_B_INTERACTIVE);
    result.insert(TWPC_B_STARTPAY);
    result.insert(TWPC_B_CANCLE);
    result.insert(TWPC_B_SENDDATA);
    result.insert(TWPC_B_HARDWARECONFIG);
    result.insert(TWPC_B_AUTHCONTROL);
	result.insert(TWPC_B_AGENCYDEBIT);
	result.insert(TWPC_B_GREENCHANNEL);
	result.insert(0xBE);

    result.insert(TWPC_C_REPLY);
    result.insert(TWPC_D_QUERY);
    result.insert(TWPC_D_SELFCHECK);
    return result;
}
//////////////////////////////////////////////////////////////
// 函数名称:RawDataToFrame
//
// 功能描述:将字节流数据，转换为协议数据
//
// 输入参数:const RawSerialData& src, TWProtocol_BaseFrame& dest, TWProtocol_BaseFrame&toReply,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool RawDataToFrame(const RawSerialData& src, TWProtocol_BaseFrame& dest, TWProtocol_BaseFrame&toReply)
{
    static std::set<unsigned char> topCMDs = GetValidTopCMD();

    toReply.TOPCMD = TWPC_C_REPLY;
    toReply.LEN = 4;
    delete toReply.DATA;
    toReply.DATA = (unsigned char*)new char[4];
    if (toReply.DATA == NULL)
    {
        TW_TRACE_DIARY("串口数据转换失败：内存分配失败");
        return false;
    }
    toReply.SUBCMD = TWPC_C_REPLY_SUCCESS;

    memset(toReply.DATA, 0, 4);

    RawSerialData afterEscape;
    if (!FrameUnEscape(src.data, afterEscape.data))
    {
        toReply.SUBCMD = TWPC_C_REPLY_INVALIDDATAUNDEF;
        TW_TRACE_DIARY("串口数据转换失败：转义失败");
        return false;//
    }


    if (afterEscape.data.size() < 10)
    {
        toReply.SUBCMD = TWPC_C_REPLY_INVALIDFORMAT;
        TW_TRACE_DIARY("串口数据转换失败：长度不对");
        return false;//
    }
	//add 2018-05 特殊逻辑处理
	{
		bool isSpecialCmd = (afterEscape.data[1]==0xFF)&&(afterEscape.data[2]==0xFF)&&(afterEscape.data[3]==0xFF)&&(afterEscape.data[4]==0xFF);
		if(isSpecialCmd)
		{
			
			dest.STX = TWPD_STARTFLAG;
			dest.LEN = 4;
			dest.SN = MAKEWORD (afterEscape.data[4], afterEscape.data[3]);
			dest.TOPCMD = afterEscape.data[7];
			dest.SUBCMD = 0x00;
			dest.ETX = TWPD_ENDFLAG;

			 if (dest.DATA != NULL)
			{
				delete dest.DATA;
				dest.DATA = NULL;
			}
			dest.DATA = new unsigned char[4];
			if(dest.DATA==NULL)
			{
				return false;
			}
			dest.DATA[0]=afterEscape.data[8];
			dest.DATA[1]=afterEscape.data[9];
			dest.DATA[2]=afterEscape.data[10];
			dest.DATA[3]=afterEscape.data[11];

			toReply.DATA[0] = 0x00;
			toReply.DATA[1] = 0x00;
			toReply.DATA[2] = 0x00;
			toReply.DATA[3] = 0x00;

			return true;
		}
	}
	

    if (topCMDs.count(afterEscape.data[5]) == 0)
    {
        toReply.SUBCMD = TWPC_C_REPLY_INVALIDCMD;
        TW_TRACE_DIARY("串口数据转换失败：主命令非法");
        return false;//
    }
    unsigned short lenData = (unsigned short)afterEscape.data.size() - 10;
    unsigned short lenExpect = MAKEWORD(afterEscape.data[2], afterEscape.data[1]);
    if (lenData != lenExpect)
    {
        toReply.SUBCMD = TWPC_C_REPLY_INVALIDFORMAT;
        TW_TRACE_DIARY("串口数据转换失败：长度比对失败");
        return false;//
    }

    unsigned short crcExpect = MAKEWORD(afterEscape.data[afterEscape.data.size() - 2], afterEscape.data[afterEscape.data.size() - 3]);
    std::vector<unsigned char>::const_iterator crcFirst = afterEscape.data.begin();
    std::vector<unsigned char>::const_iterator crcSecond = afterEscape.data.begin();
    std::advance(crcFirst, 1);
    std::advance(crcSecond, 1 + lenData + 6);
    unsigned short crcData = crc16(crcFirst, crcSecond); //afterEscape.data.begin(),afterEscape.data.begin()+lenData+6);//(afterEscape.data + 1, lenData + 6);
    if (crcData != crcExpect)
    {
        toReply.SUBCMD = TWPC_C_REPLY_INVALIDDATACHECK;
        TW_TRACE_DIARY("串口数据转换失败：CRC比对失败");
        return false;   //
    }

    dest.STX = TWPD_STARTFLAG;
    dest.LEN = lenData;
    dest.SN = MAKEWORD (afterEscape.data[4], afterEscape.data[3]);
    dest.TOPCMD = afterEscape.data[5];


    dest.SUBCMD = afterEscape.data[6];
    if (dest.DATA != NULL)
    {
        delete dest.DATA;
        dest.DATA = NULL;
    }
    dest.DATA = new unsigned char[lenData];
    if (dest.DATA == NULL)
    {
        TW_TRACE_DIARY("串口数据转换失败：没有内存可以存储结果数据");
        return false;
    }
    std::copy(afterEscape.data.begin() + 7, afterEscape.data.begin() + 7 + lenData, dest.DATA);
    //memcpy(dest.DATA, afterEscape.data.begin() + 7, lenData);
    dest.VERIFY = crcData;
    dest.ETX = TWPD_ENDFLAG;

    toReply.DATA[0] = dest.TOPCMD;
    toReply.DATA[1] = dest.SUBCMD;
    toReply.DATA[2] = HIBYTE(dest.SN);
    toReply.DATA[3] = LOBYTE(dest.SN);

    return true;
}
bool RawDataToFrame(const std::list<unsigned char>& src, TWProtocol_BaseFrame& dest, TWProtocol_BaseFrame&toReply)
{
    RawSerialData rsd;
    rsd.data.insert(rsd.data.end(), src.begin(), src.end());
    return RawDataToFrame(rsd, dest, toReply);
}
//////////////////////////////////////////////////////////////
// 函数名称:FrameEscape
//
// 功能描述:对字节数据进行转义，转义后的规则参见协议文档
//
// 输入参数:const unsigned char* const src, int src_len, unsigned char* &dst, int& dst_len,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool FrameEscape(const unsigned char* const src, int src_len, unsigned char* &dst, int& dst_len)
{
    if (src == NULL && src_len != 0)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    for (int i = 0; i < src_len; ++i)
    {
        if (src[i] == TWPD_STARTFLAG)
        {
            dataResult.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
            dataResult.push_back(TWPD_STARTFLAG_ESCAPE_SECOND);
        }
        else if (src[i] == TWPD_STARTFLAG_ESCAPE_FIRST)
        {
            dataResult.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
            dataResult.push_back(0x00);
        }
        else if (src[i] == TWPD_ENDFLAG)
        {
            dataResult.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
            dataResult.push_back(TWPD_ENDFLAG_ESCAPE_SECOND);
        }
        else if (src[i] == TWPD_ENDFLAG_ESCAPE_FIRST)
        {

            dataResult.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
            dataResult.push_back(0x00);
        }
        else
        {
            dataResult.push_back(src[i]);
        }
    }
    if (dst == NULL || dst_len != dataResult.size())
    {
        delete dst;
        dst = NULL;
        dst = new unsigned char[dataResult.size()];
        if (dst == NULL)
        {
            return false;
        }
        memset(dst, 0, dataResult.size());
    }
    std::copy(dataResult.begin(), dataResult.end(), dst);
    dst_len = (int)dataResult.size();


    return true;
}

//////////////////////////////////////////////////////////////
// 函数名称:FrameUnEscape
//
// 功能描述:对内容进行反转义
//
// 输入参数:const unsigned char*const  src, int src_len, unsigned char*&dst, int&dst_len,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool FrameUnEscape(const unsigned char*const  src, int src_len, unsigned char*&dst, int&dst_len)
{
    if (src == NULL && src_len != 0)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    int i = 0;
    while (i < src_len - 1) /**/
    {
        if (src[i] == TWPD_STARTFLAG_ESCAPE_FIRST )
        {
            if (src[i + 1] == TWPD_STARTFLAG_ESCAPE_SECOND)
            {
                dataResult.push_back(TWPD_STARTFLAG);
            }
            else if (src[i + 1] == 0x00)
            {
                dataResult.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
            }
            else
            {
                return false;
            }
            i += 2;
        }
        else if (src[i] == TWPD_ENDFLAG_ESCAPE_FIRST)
        {
            if (src[i + 1] == TWPD_ENDFLAG_ESCAPE_SECOND)
            {
                dataResult.push_back(TWPD_ENDFLAG);
            }
            else if (src[i + 1] == 0x00)
            {
                dataResult.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
            }
            else
            {
                return false;
            }
            i += 2;
        }
        else
        {
            dataResult.push_back(src[i]);
            i++;
        }
    }
    if (src[src_len - 1] == TWPD_STARTFLAG_ESCAPE_FIRST
            || src[src_len - 1] == TWPD_ENDFLAG_ESCAPE_FIRST)
    {
        return false;
    }
    else
    {
        dataResult.push_back(src[src_len - 1]);
    }

    if (dst == NULL || dst_len != dataResult.size())
    {
        delete dst;
        dst = NULL;
        dst = new unsigned char[dataResult.size()];
        if (dst == NULL)
        {
            return false;
        }
        memset(dst, 0, dataResult.size());
    }
    std::copy(dataResult.begin(), dataResult.end(), dst);
    dst_len = (int)dataResult.size();

    return true;
}

bool FrameEscape(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst )
{
    dst.clear();
    for (size_t i = 0; i < src.size(); ++i)
    {
        if (src[i] == TWPD_STARTFLAG)
        {
            dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
            dst.push_back(TWPD_STARTFLAG_ESCAPE_SECOND);
        }
        else if (src[i] == TWPD_STARTFLAG_ESCAPE_FIRST)
        {
            dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
            dst.push_back(0x00);
        }
        else if (src[i] == TWPD_ENDFLAG)
        {
            dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
            dst.push_back(TWPD_ENDFLAG_ESCAPE_SECOND);
        }
        else if (src[i] == TWPD_ENDFLAG_ESCAPE_FIRST)
        {

            dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
            dst.push_back(0x00);
        }
        else
        {
            dst.push_back(src[i]);
        }
    }

    return true;
}
bool FrameUnEscape(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst )
{
    dst.clear();

    if (src.empty())
    {
        return true;
    }
    size_t i = 0;
    while (i < (src.size() - 1) ) /**/
    {
        if (src[i] == TWPD_STARTFLAG_ESCAPE_FIRST )
        {
            if (src[i + 1] == TWPD_STARTFLAG_ESCAPE_SECOND)
            {
                dst.push_back(TWPD_STARTFLAG);
            }
            else if (src[i + 1] == 0x00)
            {
                dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
            }
            else
            {
                return false;
            }
            i += 2;
        }
        else if (src[i] == TWPD_ENDFLAG_ESCAPE_FIRST)
        {
            if (src[i + 1] == TWPD_ENDFLAG_ESCAPE_SECOND)
            {
                dst.push_back(TWPD_ENDFLAG);
            }
            else if (src[i + 1] == 0x00)
            {
                dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
            }
            else
            {
                return false;
            }
            i += 2;
        }
        else
        {
            dst.push_back(src[i]);
            i++;
        }
    }
    /*last char unescape*/
    if (src.back() == TWPD_STARTFLAG_ESCAPE_FIRST
            || src.back() == TWPD_ENDFLAG_ESCAPE_FIRST)
    {
        return false;
    }
    else
    {
        dst.push_back(src.back());
    }


    return true;
}

bool MergeByteVector(unsigned char* &dst, const std::vector<unsigned char>& src)
{
    delete dst;
    dst = new unsigned char[src.size()];
    if (dst == NULL)
    {
        return false;
    }
    std::copy(src.begin(), src.end(), dst);
    return true;
}

//////////////////////////////////////////////////////////////
// 函数名称:FillFrameData
//
// 功能描述:用于将协议数据，转换成对应的指令
//
// 输入参数:TWProtocol_BaseFrame& frame, const TWProtocolData_A_Beat& dataStruct,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_Beat& dataStruct )
{
    if (frame.TOPCMD != TWPC_A_BEAT)
    {
        return false;
    }
    std::vector<unsigned char > dataResult;

    if (frame.SUBCMD == TWPC_A_BEAT_HANDSHAKE)
    {
        dataResult.push_back(0x00);
    }
    else if (frame.SUBCMD == TWPC_A_BEAT_SET)
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.BeatInterval));
    }
    else
    {
        return false;
    }
    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }

    return true;
}
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_Interactive& dataStruct )
{
    if (frame.TOPCMD != TWPC_A_INTERACTIVE)
    {
        return false;
    }
    std::vector<unsigned char > dataResult;
    if (frame.SUBCMD == TWPC_A_INTERACTIVE_WELCOME)
    {
        dataResult.push_back(dataStruct.data.WelcomeOption);
    }
    else if (frame.SUBCMD == TWPC_A_INTERACTIVE_INTERFACEBLOCK)
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InteractionBlock.Money));
        dataResult.push_back(dataStruct.data.InteractionBlock.VehType);
        dataResult.push_back(dataStruct.data.InteractionBlock.VehKind);
        dataResult.push_back(dataStruct.data.InteractionBlock.VehPlateColor);
        std::copy(dataStruct.data.InteractionBlock.Plate, dataStruct.data.InteractionBlock.Plate + sizeof(dataStruct.data.InteractionBlock.Plate), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InteractionBlock.Balance));
        std::copy(dataStruct.data.InteractionBlock.EnStationName, dataStruct.data.InteractionBlock.EnStationName + sizeof(dataStruct.data.InteractionBlock.EnStationName), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        dataResult.push_back(dataStruct.data.InteractionBlock.FlagStationCount);
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InteractionBlock.AxleCount));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InteractionBlock.Weight));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InteractionBlock.WeightLimit));

    }
    else if (frame.SUBCMD == TWPC_A_INTERACTIVE_BYE)
    {
        dataResult.push_back(dataStruct.data.ByeInfo);
    }
    else if (frame.SUBCMD == TWPC_A_INTERACTIVE_04)
    {
        /*nothing*/
    }
    else if (frame.SUBCMD == TWPC_A_INTERACTIVE_SCREEN)
    {
        dataResult.push_back(dataStruct.data.ScreenInfo);
    }
    else if (frame.SUBCMD == TWPC_A_INTERACTIVE_INTERCACEJSON)
    {
        std::copy(dataStruct.data.pInteractionJson, dataStruct.data.pInteractionJson + strlen(dataStruct.data.pInteractionJson), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if ( frame.SUBCMD == TWPC_A_INTERACTIVE_TTS)
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.TTS.contentLen));
        std::copy(dataStruct.data.TTS.content, dataStruct.data.TTS.content + dataStruct.data.TTS.contentLen, std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if ( frame.SUBCMD == TWPC_A_INTERACTIVE_FREE_DISPLAY)
    {
        std::copy(dataStruct.data.FreeText.Row1Data, dataStruct.data.FreeText.Row1Data + sizeof(dataStruct.data.FreeText.Row1Data) , std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        dataResult.push_back(dataStruct.data.FreeText.Row1Scroll);
        dataResult.push_back(dataStruct.data.FreeText.Row1Resevered);

        std::copy(dataStruct.data.FreeText.Row2Data, dataStruct.data.FreeText.Row2Data + sizeof(dataStruct.data.FreeText.Row2Data) , std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        dataResult.push_back(dataStruct.data.FreeText.Row2Scroll);
        dataResult.push_back(dataStruct.data.FreeText.Row2Resevered);

        std::copy(dataStruct.data.FreeText.Row3Data, dataStruct.data.FreeText.Row3Data + sizeof(dataStruct.data.FreeText.Row3Data) , std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        dataResult.push_back(dataStruct.data.FreeText.Row3Scroll);
        dataResult.push_back(dataStruct.data.FreeText.Row3Resevered);

        std::copy(dataStruct.data.FreeText.Row4Data, dataStruct.data.FreeText.Row4Data + sizeof(dataStruct.data.FreeText.Row4Data) , std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        dataResult.push_back(dataStruct.data.FreeText.Row4Scroll);
        dataResult.push_back(dataStruct.data.FreeText.Row4Resevered);
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }
    return true;
}
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_StartPay& dataStruct )
{
    if (frame.TOPCMD != TWPC_A_STARTPAY)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    if (frame.SUBCMD == TWPC_A_STARTPAY_PAYBLOCK)
    {
        std::copy(dataStruct.data.PayBlock.TransactionIDCode, dataStruct.data.PayBlock.TransactionIDCode + sizeof(dataStruct.data.PayBlock.TransactionIDCode), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.PayBlock.Money));
        dataResult.push_back(dataStruct.data.PayBlock.VehTypeAndKind);
        dataResult.push_back(dataStruct.data.PayBlock.VehPlateColor);
        std::copy(dataStruct.data.PayBlock.Plate, dataStruct.data.PayBlock.Plate + sizeof(dataStruct.data.PayBlock.Plate), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        std::copy(dataStruct.data.PayBlock.EnStationNO, dataStruct.data.PayBlock.EnStationNO + sizeof(dataStruct.data.PayBlock.EnStationNO), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        std::copy(dataStruct.data.PayBlock.EnStationName, dataStruct.data.PayBlock.EnStationName + sizeof(dataStruct.data.PayBlock.EnStationName), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.PayBlock.EnTime));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.PayBlock.StaticDate));

        dataResult.push_back(dataStruct.data.PayBlock.StaticShift);
        dataResult.push_back(dataStruct.data.PayBlock.CardLen);
        std::copy(dataStruct.data.PayBlock.CardID, dataStruct.data.PayBlock.CardID + sizeof(dataStruct.data.PayBlock.CardID), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        dataResult.push_back(dataStruct.data.PayBlock.FlagStationCount);
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.PayBlock.AxleCount));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.PayBlock.Weight));
        std::copy(dataStruct.data.PayBlock.OperatorID, dataStruct.data.PayBlock.OperatorID + sizeof(dataStruct.data.PayBlock.OperatorID), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        std::copy(dataStruct.data.PayBlock.OperatorName, dataStruct.data.PayBlock.OperatorName + sizeof(dataStruct.data.PayBlock.OperatorName), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.PayBlock.Balance));
        dataResult.push_back(dataStruct.data.PayBlock.DebitMode);
        dataResult.push_back(dataStruct.data.PayBlock.DebitOverTime);
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.PayBlock.ExTime));

		//add 2018-05
		std::copy(dataStruct.data.PayBlock.EntryPlate, dataStruct.data.PayBlock.EntryPlate + sizeof(dataStruct.data.PayBlock.EntryPlate), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
		dataResult.push_back(dataStruct.data.PayBlock.EntryPlateColor);
		dataResult.push_back(dataStruct.data.PayBlock.PassCertificateType);
		std::copy(dataStruct.data.PayBlock.AutoPlate, dataStruct.data.PayBlock.AutoPlate + sizeof(dataStruct.data.PayBlock.AutoPlate), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
		dataResult.push_back(dataStruct.data.PayBlock.AutoPlateColor);
		std::copy(dataStruct.data.PayBlock.TickNo, dataStruct.data.PayBlock.TickNo + sizeof(dataStruct.data.PayBlock.TickNo), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
		dataResult.push_back(dataStruct.data.PayBlock.PayModel);


    }
    else if (frame.SUBCMD == TWPC_A_STARTPAY_PAYJSON)
    {
        std::copy(dataStruct.data.pPayInfoJson, dataStruct.data.pPayInfoJson + strlen(dataStruct.data.pPayInfoJson), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }
    return true;

}
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_Cancle& dataStruct )
{
    if (frame.TOPCMD != TWPC_A_CANCLE)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    if (frame.SUBCMD == TWPC_A_CANCLE_DATABLOCK)
    {
        dataResult.push_back(dataStruct.data.DataBlock.IsFeedBack);
        std::copy(dataStruct.data.DataBlock.PayCode, dataStruct.data.DataBlock.PayCode + sizeof(dataStruct.data.DataBlock.PayCode) , std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_CANCLE_JSON)
    {
        std::copy(dataStruct.data.pDataJson, dataStruct.data.pDataJson + strlen(dataStruct.data.pDataJson), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }
    return true;

}
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_SendData& dataStruct )
{
    if (frame.TOPCMD != TWPC_A_SENDDATA)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    if (frame.SUBCMD == TWPC_A_SENDDATA_INITJSON)
    {
        std::copy(dataStruct.data.pInitJson, dataStruct.data.pInitJson + strlen(dataStruct.data.pInitJson), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_SENDDATA_LANETIME)
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.LaneUnixTime));
    }
    else if (frame.SUBCMD == TWPC_A_SENDDATA_LASTERROR)
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.ErrorQuery));
    }
    else if (frame.SUBCMD == TWPC_A_SENDDATA_INITBLOCK)
    {
        dataResult.push_back(dataStruct.data.InitBlock.ProvinceID);
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InitBlock.AreaID));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InitBlock.RoadID));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InitBlock.StationID));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InitBlock.LaneID));
        std::copy(dataStruct.data.InitBlock.StationName, dataStruct.data.InitBlock.StationName + sizeof(dataStruct.data.InitBlock.StationName), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        dataResult.push_back(dataStruct.data.InitBlock.ServerType);
        std::copy(dataStruct.data.InitBlock.ServerIP, dataStruct.data.InitBlock.ServerIP + sizeof(dataStruct.data.InitBlock.ServerIP), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.InitBlock.DevicePayMode));
    }

    else if (frame.SUBCMD == TWPC_A_SENDDATA_FILETRANS)
    {
        dataResult.push_back(dataStruct.data.FileData.Version);
        dataResult.push_back(dataStruct.data.FileData.Type);
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.FileData.TotalLen));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.FileData.Offset));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.FileData.DataLen));
        std::copy(dataStruct.data.FileData.Data,dataStruct.data.FileData.Data+dataStruct.data.FileData.DataLen,std::back_inserter(dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_SENDDATA_TICKNOTRANS)
    {
        std::copy(dataStruct.data.pTickNo, dataStruct.data.pTickNo + strlen(dataStruct.data.pTickNo), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }   
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }


    return true;
}
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_HARDWARECONFIG& dataStruct )
{
    if (frame.TOPCMD != TWPC_A_HARDWARECONFIG)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    if (
        (frame.SUBCMD == TWPC_A_HARDWARECONFIG_HARDVERSION)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_CANID)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_MAX)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_FIRMWARE)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SCANTIME)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_ID)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_BOOTSCREEN)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_ETH0IP)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_ETH0MASK)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_ETH0GATEWAY)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_ETH1IP)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_ETH1MASK)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_ETH1GATEWAY)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_DEFAULTGATEWAY)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SOFTVERSION)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_HARDWARETEMP)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_HARDWARETIME)
    )
    {
        dataResult.push_back(dataStruct.data.Unkown);
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETCANID)
    {
        dataResult.push_back(dataStruct.data.CANID.Value);
        std::copy(dataStruct.data.CANID.VerifyCode, dataStruct.data.CANID.VerifyCode + sizeof(dataStruct.data.CANID.VerifyCode), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETSCANTIME)
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.ScanTime.TimeVal));
        std::copy(dataStruct.data.ScanTime.VerifyCode, dataStruct.data.ScanTime.VerifyCode + sizeof(dataStruct.data.ScanTime.VerifyCode), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETENV)
    {
        std::copy(dataStruct.data.Env.Para, dataStruct.data.Env.Para + strlen(dataStruct.data.Env.Para), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (
        (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETETH0IP)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETETH0MASK)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETETH0GATEWAY)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETETH1IP)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETETH1MASK)
        || (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETETH1GATEWAY)

    )
    {
        // dataResult.push_back(dataStruct.data.IPInfo.IPVer);
        std::copy(dataStruct.data.IPInfo.IP, dataStruct.data.IPInfo.IP + sizeof(dataStruct.data.IPInfo.IP), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }


    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETLED)
    {
        dataResult.push_back(dataStruct.data.LEDInfo.Var.Name);
        std::copy(dataStruct.data.LEDInfo.Var.Value, dataStruct.data.LEDInfo.Var.Value + sizeof(dataStruct.data.LEDInfo.Var.Value), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        std::copy(dataStruct.data.LEDInfo.VerifyCode, dataStruct.data.LEDInfo.VerifyCode + sizeof(dataStruct.data.LEDInfo.VerifyCode), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETHARDVERSION)
    {
        std::copy(dataStruct.data.HardInfo.HardVer, dataStruct.data.HardInfo.HardVer + sizeof(dataStruct.data.HardInfo.HardVer), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETMAX)
    {
        std::copy(dataStruct.data.MaxInfo.Batch, dataStruct.data.MaxInfo.Batch + sizeof(dataStruct.data.MaxInfo.Batch), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        std::copy(dataStruct.data.MaxInfo.BurnTime, dataStruct.data.MaxInfo.BurnTime + sizeof(dataStruct.data.MaxInfo.BurnTime), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        std::copy(dataStruct.data.MaxInfo.VerifyCode, dataStruct.data.MaxInfo.VerifyCode + sizeof(dataStruct.data.MaxInfo.VerifyCode), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETID)
    {
        std::copy(dataStruct.data.DeviceID.Val, dataStruct.data.DeviceID.Val + sizeof(dataStruct.data.DeviceID.Val), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        std::copy(dataStruct.data.DeviceID.VerifyCode, dataStruct.data.DeviceID.VerifyCode + sizeof(dataStruct.data.DeviceID.VerifyCode), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_SETBOOTSCREENID)
    {
        dataResult.push_back(dataStruct.data.ScreenID.Val);
        std::copy(dataStruct.data.ScreenID.VerifyCode, dataStruct.data.ScreenID.VerifyCode + sizeof(dataStruct.data.ScreenID.VerifyCode), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_TESTLED)
    {
        dataResult.push_back(dataStruct.data.TestLED.Unuse);
    }
    else if (frame.SUBCMD == TWPC_A_HARDWARECONFIG_DATATRANS)
    {
        dataResult.push_back(dataStruct.data.DataTrans.PortNO);
        std::copy(dataStruct.data.DataTrans.VerifyCode, dataStruct.data.DataTrans.VerifyCode + sizeof( dataStruct.data.DataTrans.VerifyCode ), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
        std::copy(dataStruct.data.DataTrans.pContent, dataStruct.data.DataTrans.pContent + strlen(dataStruct.data.DataTrans.pContent), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }

    return true;
}
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_Auth& dataStruct )
{
    if (frame.TOPCMD != TWPC_A_AUTHCONTROL)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    if (frame.SUBCMD == TWPC_A_AUTHCONTROL_CHECK)
    {
        dataResult.push_back(dataStruct.data.Unuse);
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }

    return true;
}

bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_AgencyDebit& dataStruct )
{
    if (frame.TOPCMD != TWPC_A_AGENCYDEBIT)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    if (frame.SUBCMD == TWPC_A_AGENCYDEBIT_QUERY)
    {
        dataResult.push_back(dataStruct.data.QueryData.VehType);
        dataResult.push_back(dataStruct.data.QueryData.VehKind);
        dataResult.push_back(dataStruct.data.QueryData.VehColor);
		
        std::copy(dataStruct.data.QueryData.VehPlate, dataStruct.data.QueryData.VehPlate + sizeof( dataStruct.data.QueryData.VehPlate ), std::back_inserter( dataResult));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.QueryData.AxleCount));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.QueryData.Weight));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.QueryData.WeightLimit));
    }
    else if(frame.SUBCMD == TWPC_A_AGENCYDEBIT_TRANSFERIMAGE)
    {
        dataResult.push_back(dataStruct.data.ImageData.ImageClass);
        std::copy(dataStruct.data.ImageData.PayIdentifier,dataStruct.data.ImageData.PayIdentifier + sizeof(dataStruct.data.ImageData.PayIdentifier),std::back_inserter(dataResult));
        std::copy(dataStruct.data.ImageData.ImageFormat,dataStruct.data.ImageData.ImageFormat + sizeof(dataStruct.data.ImageData.ImageFormat),std::back_inserter(dataResult));
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.ImageData.ImageLen));
		std::copy(dataStruct.data.ImageData.ImageContent,dataStruct.data.ImageData.ImageContent + dataStruct.data.ImageData.ImageLen/*sizeof(dataStruct.data.ImageData.ImageContent)*/,std::back_inserter(dataResult));
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }

    return true;
}


bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_A_GreenChannel& dataStruct )
{
	 if (frame.TOPCMD != TWPC_A_GREENCHANNEL)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    if (frame.SUBCMD == TWPC_A_GREENCHANNEL_NFC)
    {
		JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.NFCQuery));
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }

    return true;
}

bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_E_Query& dataStruct )
{
    if (frame.TOPCMD != TWPC_E_QUERY)
    {
        return false;
    }
    std::vector<unsigned char> dataResult;
    if (frame.SUBCMD == TWPC_E_QUERY_STATUS)
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.Status.Result));
        std::copy(dataStruct.data.Status.Unkown, dataStruct.data.Status.Unkown + sizeof(dataStruct.data.Status.Unkown), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else if (frame.SUBCMD == TWPC_E_QUERY_VERSION)
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.Version.Result));
        std::copy(dataStruct.data.Version.Unkown, dataStruct.data.Version.Unkown + sizeof( dataStruct.data.Version.Unkown), std::back_insert_iterator<std::vector<unsigned char>>( dataResult));
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }

    return true;
}
bool FillFrameData(TWProtocol_BaseFrame& frame, const TWProtocolData_C_Reply& dataStruct )
{
    std::vector<unsigned char> dataResult;
    if (frame.TOPCMD != TWPC_C_REPLY)
    {
        return false;
    }

    if (
        (frame.SUBCMD == TWPC_C_REPLY_SUCCESS)
        || (frame.SUBCMD == TWPC_C_REPLY_INVALIDFORMAT)
        || (frame.SUBCMD == TWPC_C_REPLY_INVALIDDATACHECK)
        || (frame.SUBCMD == TWPC_C_REPLY_INVALIDDATAUNDEF)
        || (frame.SUBCMD == TWPC_C_REPLY_INVALIDCMD)
    )
    {
        JoinContainer(dataResult, ConvertNetBytes(dataStruct.data.Unuse));
    }
    else
    {
        return false;
    }

    frame.LEN = (unsigned short)dataResult.size();
    if (!MergeByteVector(frame.DATA, dataResult))
    {
        return false;
    }

    return true;
}


//////////////////////////////////////////////////////////////
// 函数名称:FillProtocolStruct
//
// 功能描述:系列函数，用于将指定的指令协议数据转换为帧明朗有
//
// 输入参数:TWProtocolData_B_Beat& dataStruct, const TWProtocol_BaseFrame& frame,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool FillProtocolStruct(TWProtocolData_B_Beat& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_B_BEAT)
    {
        return false;
    }
    if ( frame.SUBCMD == TWPC_B_BEAT_AUTO
            || frame.SUBCMD == TWPC_B_BEAT_FEEDBACK)
    {
        if (frame.LEN != 8)
        {
            return false;
        }
        dataStruct.data.BeatInfo.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        dataStruct.data.BeatInfo.DeviceStatus = MAKEINTBIG(frame.DATA[4], frame.DATA[5], frame.DATA[6], frame.DATA[7]);
    }
    else if (frame.SUBCMD == TWPC_B_BEAT_SET)
    {
        dataStruct.data.SetResult = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
    }
    else
    {
        return false;
    }
    return true;
}
bool FillProtocolStruct(TWProtocolData_B_Interactive& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_B_INTERACTIVE)
    {
        return false;
    }
    if (
        frame.SUBCMD == TWPC_B_INTERACTIVE_WELCOME
        || frame.SUBCMD == TWPC_B_INTERACTIVE_INTERFACEBLOCK
        || frame.SUBCMD == TWPC_B_INTERACTIVE_BYE
        || frame.SUBCMD == TWPC_B_INTERACTIVE_04
        || frame.SUBCMD == TWPC_B_INTERACTIVE_SCREEN
        || frame.SUBCMD == TWPC_B_INTERACTIVE_INTERCACEJSON
    )
    {
        dataStruct.data.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
    }
    else
    {
        return false;
    }
    return true;

}
bool FillProtocolStruct(TWProtocolData_B_StartPay& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_B_STARTPAY)
    {
        return false;
    }
    if (frame.SUBCMD == TWPC_B_STARTPAY_PAYBLOCK )
    {
        dataStruct.data.DataBlock.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        memcpy(dataStruct.data.DataBlock.TransSecurityCode, frame.DATA + 4, 40);
        memcpy(dataStruct.data.DataBlock.TransactionIDCode, frame.DATA + 44, 40);
        memcpy(dataStruct.data.DataBlock.OrderNO, frame.DATA + 84, 32);
        dataStruct.data.DataBlock.TransTime = MAKEINTBIG(frame.DATA[116], frame.DATA[117], frame.DATA[118], frame.DATA[119]);
        dataStruct.data.DataBlock.PayType = MAKEWORDBIG(frame.DATA[120], frame.DATA[121]);
        dataStruct.data.DataBlock.ConsumeTime = MAKEINTBIG(frame.DATA[122], frame.DATA[123], frame.DATA[124], frame.DATA[125]);
    }
    else if (frame.SUBCMD == TWPC_B_STARTPAY_PAYJSON)
    {
        if (!ForceStrCopy(dataStruct.data.pDataJson, (const char*)frame.DATA, frame.LEN))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}
bool FillProtocolStruct(TWProtocolData_B_CANCLE& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_B_CANCLE)
    {
        return false;
    }
    if (frame.SUBCMD == TWPC_B_CANCLE_DATABLOCK)
    {
        return true;
    }
    else if (frame.SUBCMD == TWPC_B_CANCLE_JSON)
    {
        if (!ForceStrCopy(dataStruct.data.pDataJson, (const char*)frame.DATA, frame.LEN))
        {
            return false;
        }
    }
    else
    {
        return false;
    }



    return true;
}
bool FillProtocolStruct(TWProtocolData_B_SendData& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_B_SENDDATA)
    {
        return false;
    }
    if (
        frame.SUBCMD == TWPC_B_SENDDATA_INITJSON
        || frame.SUBCMD == TWPC_B_SENDDATA_LANETIME
        || frame.SUBCMD == TWPC_B_SENDDATA_LASTERROR
        || frame.SUBCMD == TWPC_B_SENDDATA_INITBLOCK
		|| frame.SUBCMD == TWPC_B_SENDDATA_FILETRANS
		|| frame.SUBCMD == TWPC_B_SENDDATA_TICKNOTRANS
    )
    {
        dataStruct.data.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
    }
    else
    {
        return false;
    }

    return true;
}
bool FillProtocolStruct(TWProtocolData_B_HardwareConfig& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_B_HARDWARECONFIG)
    {
        return false;
    }
    if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_HARDVERSION)
    {
        dataStruct.data.VersionInfo.Result =  MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        memcpy(dataStruct.data.VersionInfo.Val, frame.DATA + 4, 15);

    }
    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_CANID)
    {
        dataStruct.data.CANID.Result =  MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        dataStruct.data.CANID.Value = frame.DATA[4];
    }
    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_MAX)
    {
        dataStruct.data.MAX.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        memcpy(dataStruct.data.MAX.ProductInfo.Batch, frame.DATA + 4, 8);
        memcpy(dataStruct.data.MAX.ProductInfo.BurnTime, frame.DATA + 4 + 8, (std::min)(14, frame.LEN - 4 - 8));
    }
    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_FIRMWARE)
    {
        dataStruct.data.FireWare.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        dataStruct.data.FireWare.pValue = new char[frame.LEN - 4 + 1];

        if (dataStruct.data.FireWare.pValue == NULL)
        {
            return false;
        }
        memset(dataStruct.data.FireWare.pValue, 0, frame.LEN - 4 + 1);
        strncpy(dataStruct.data.FireWare.pValue, (const char*)frame.DATA + 4, frame.LEN - 4);
    }
    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SCANTIME)
    {
        dataStruct.data.ScanTime.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        dataStruct.data.ScanTime.Value =  MAKEWORDBIG(frame.DATA[4], frame.DATA[5]); //frame.DATA[4];//MAKEINTBIG(frame.DATA[4], frame.DATA[5], frame.DATA[6], frame.DATA[7]);
    }
    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_ID)
    {
        dataStruct.data.DeviceID.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        memcpy(dataStruct.data.DeviceID.Value, frame.DATA + 4, 20);
    }
    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_BOOTSCREEN)
    {
        dataStruct.data.BootScreenID.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        dataStruct.data.BootScreenID.Value = frame.DATA[4];
    }
    else if ( (frame.SUBCMD == TWPC_B_HARDWARECONFIG_ETH0IP)
              || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_ETH0MASK)
              || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_ETH0GATEWAY)
              || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_ETH1IP)
              || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_ETH1MASK)
              || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_ETH1GATEWAY)
              || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_DEFAULTGATEWAY)
            )
    {
        dataStruct.data.IPInfo.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        // dataStruct.data.IPInfo.IP.IPVer = frame.DATA[4];
        strncpy(dataStruct.data.IPInfo.IP.Value, (const char*)frame.DATA + 4, 15);

        //memcpy(dataStruct.data.IPInfo.IP.Value, frame.DATA + 4 + 1 - 1/*没有IP信息*/, (std::min)(16, frame.LEN - 4 ));
    }

    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SOFTVERSION)
    {
        dataStruct.data.SoftwareInfo.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        if (!ForceStrCopy(dataStruct.data.SoftwareInfo.pJson, (const char*)frame.DATA + 4, frame.LEN - 4))
        {
            return false;
        }
    }
    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_HARDWARETEMP)
    {
        dataStruct.data.HardTemp.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        if (!ForceStrCopy(dataStruct.data.HardTemp.pValue, (const char*)frame.DATA + 4, frame.LEN - 4))
        {
            return false;
        }
    }
    else if (frame.SUBCMD == TWPC_B_HARDWARECONFIG_HARDWARETIME)
    {
        dataStruct.data.TerminalTime.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
        strncpy(dataStruct.data.TerminalTime.Value, (const char*)frame.DATA + 4, (std::min)(28, frame.LEN - 4));
    }
    else if (
        (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETCANID)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETSCANTIME)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETENV)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETETH0IP)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETETH0MASK)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETETH0GATEWAY)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETETH1IP)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETETH1MASK)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETETH1GATEWAY)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETLED)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETHARDVERSION)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETMAX)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETID)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_SETBOOTSCREENID)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_TESTLED)
        || (frame.SUBCMD == TWPC_B_HARDWARECONFIG_DATATRANS)
    )
    {
        dataStruct.data.DefaultInfo.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
    }
    else
    {
        return false;
    }

    return true;
}
bool FillProtocolStruct(TWProtocolData_B_Auth& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_B_AUTHCONTROL)
    {
        return false;
    }
    if (frame.SUBCMD == TWPC_B_AUTHCONTROL_CHECK)
    {
        dataStruct.data.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
    }
    else
    {
        return false;
    }

    return true;
}

bool FillProtocolStruct(TWProtocolData_B_AgencyDebit& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_B_AGENCYDEBIT)
    {
        return false;
    }
    if (frame.SUBCMD == TWPC_B_AGENCYDEBIT_QUERY
		||frame.SUBCMD == TWPC_B_AGENCYDEBIT_TRANSFERIMAGE)
    {
        dataStruct.data.Result = MAKEINTBIG(frame.DATA[0], frame.DATA[1], frame.DATA[2], frame.DATA[3]);
    }
    else
    {
        return false;
    }

    return true;
}

bool FillProtocolStruct(TWProtocolData_B_GreenChannel& dataStruct, const TWProtocol_BaseFrame& frame )
{
	if (frame.TOPCMD != TWPC_B_GREENCHANNEL)
    {
        return false;
    }
    if (frame.SUBCMD == TWPC_B_GREENCHANNEL_NFC)
    {
		int copyLen = (std::min)((size_t)frame.LEN,sizeof(dataStruct.data.NFCParaInfo)-1);
		strncpy(dataStruct.data.NFCParaInfo,(char*)frame.DATA,copyLen);
    }
    else
    {
        return false;
    }

    return true;
}

bool FillProtocolStruct(TWProtocolData_C_Reply& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_C_REPLY)
    {
        return false;
    }
    if ((frame.SUBCMD == TWPC_C_REPLY_SUCCESS)
            || (frame.SUBCMD == TWPC_C_REPLY_INVALIDFORMAT)
            || (frame.SUBCMD == TWPC_C_REPLY_INVALIDDATACHECK)
            || (frame.SUBCMD == TWPC_C_REPLY_INVALIDDATAUNDEF)
            || (frame.SUBCMD == TWPC_C_REPLY_INVALIDCMD)
       )
    {
        return true;
    }
    else
    {
        return false;
    }

    return true;
}
bool FillProtocolStruct(TWProtocolData_D_Query& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_D_QUERY)
    {
        return false;
    }
    if (frame.SUBCMD == TWPC_D_QUERY_STATUS
            || frame.SUBCMD == TWPC_D_QUERY_VERSION
       )
    {
        return true;
    }
    else
    {
        return false;
    }

    return true;
}
bool FillProtocolStruct(TWProtocolData_D_SelfCheck& dataStruct, const TWProtocol_BaseFrame& frame )
{
    if (frame.TOPCMD != TWPC_D_SELFCHECK)
    {
        return false;
    }
    if (frame.SUBCMD == TWPC_D_SELFCHECK_POWER)
    {
        if (!ForceStrCopy(dataStruct.data.pJson, (const char*)frame.DATA, frame.LEN))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}



bool HexStrToBaseFrame(const std::string& hexStr,TWProtocol_BaseFrame& frame)
{
    size_t hexSize = hexStr.size();
    if(hexSize > 1024*10)
    {
        return false;
    }
    size_t arrSize = hexSize/2;
    if(arrSize< 8)
    {
        return false;
    }
    char * byteArr = new char[arrSize];
    if(byteArr==NULL)
    {
        return false;
    }
    memset(byteArr,0,arrSize);
    if(!Hex2Bin(hexStr.c_str(),byteArr,hexSize))
    {
        return false;
    }
    frame.LEN = MAKEWORDBIG(byteArr[1],byteArr[2]);
    frame.SN = MAKEWORDBIG(byteArr[3],byteArr[4]);
    frame.TOPCMD = byteArr[5];
    frame.SUBCMD = byteArr[6];
    delete frame.DATA;
    frame.DATA = new unsigned char[frame.LEN];
    if(frame.DATA==NULL)
    {
        return false;
    }
    if( arrSize != (10+frame.LEN ) )
    {
        return false;
    }

    memcpy(frame.DATA,byteArr+7,frame.LEN);
    return true;
}