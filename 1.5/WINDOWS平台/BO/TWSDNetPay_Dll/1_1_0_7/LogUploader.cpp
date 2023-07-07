/*
* @Author: ���Ǳ�
* @Date:   2017-11-02
* @Last Modified by:   windf
* @Last Modified time: 2017-11-14
*
* �˲����ڴ����б����˲��ֳ����ַ�����������ʹ�ñ�������ʽ���
* ������Ҫ�����Ը��Ĵ����⡣
*/
#include "stdafx.h"
#include "LogUploader.h"
#include <algorithm>
#include <fstream>
#include <iterator>
#include "pystring.h"
#include "base64.h"
#include "TWSDNetPay_Dll.h"


extern void WriteLog(const std::string& x);
#define TW_TRACE_DIARY(x)  WriteLog(x)

extern CString  GetVersionOfFile(LPCTSTR lpsPath);

std::map<DWORD, size_t> FilePieceAdapter::threadsFilePieceSize;

inline istream &operator>>(istream &input, FilePieceAdapter &dataBlock)
{
    dataBlock.PieceSize = dataBlock.FilePieceSize_Bytes();
    if (dataBlock.PieceSize == 0)
    {
        input.setstate(ios_base::eofbit);
        input.setstate(ios_base::failbit);
        return input;
    }

    //�������У����������ĩβ�������д����������һ�ο��ܶ����˲�������
    streampos currInputPos = input.tellg();
    input.seekg(0, ios_base::end);
    streampos endInputPos = input.tellg();
    input.seekg(currInputPos, ios_base::beg);
    if (currInputPos == endInputPos)
    {
        input.setstate(ios_base::eofbit);
        input.setstate(ios_base::failbit);
        return input;
    }
    delete dataBlock.Data;
    dataBlock.Data = NULL;
    dataBlock.DataSize_Bytes = 0;
    dataBlock.Data = new char[dataBlock.PieceSize];
    if (dataBlock.Data == NULL)
    {
        input.setstate(ios_base::eofbit);
        input.setstate(ios_base::failbit);
        return input;
    }
    if (dataBlock.PieceSize <= (endInputPos - currInputPos))
    {
        input.read(dataBlock.Data, dataBlock.PieceSize);
    }
    else
    {
        input.read(dataBlock.Data, endInputPos - currInputPos);
    }

    dataBlock.DataSize_Bytes = input.gcount();
    //dataBlock.DataSize = input.readsome(dataBlock.Data, dataBlock.PieceSize);

    return input;
}


LogUploader::LogUploader()
{
	
}
void LogUploader::SetUploadServer(const std::string& ip_str,int port)
{
    m_HttpLink.ChangeRemotePeer(ip_str,port);
}
LogUploader::~LogUploader()
{

}
int LogUploader::SetLaneCodeInfo(const int & provinceId, const std::string& roadId, const std::string& stationId, const std::string& laneId,const std::string& devType)
{
	m_LCP.provinceId = provinceId;
	m_LCP.roadId = roadId;
	m_LCP.stationId = stationId;
    m_LCP.laneId  = laneId;
	m_LCP.devType = devType;

	return 0;
	
}

//////////////////////////////////////////////////////////////
// ��������:LogUploader::UploadLog
//
// ��������:��ѯ�����������ϴ���־�ļ�
//
// �������:
// �������:void
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-11-03
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void LogUploader::UploadLog(const std::string& dev_no)
{
	m_devno = dev_no;
	m_LCP.devNo =dev_no;
    AskCmdId();
    ParseCmdIds();
    ExcuteCmds();
    return ;
}
//////////////////////////////////////////////////////////////
// ��������:LogUploader::AskCmdId
//
// ��������:�����˲�ѯ�µ��������������
//
// �������:
// �������:void
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-11-03
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void LogUploader::AskCmdId()
{
    std::string askPackage = PackMonitorBeat(m_LCP);
    std::string askResponsePackage;

    if (!m_HttpLink.Post("/trawe-itms/interface/t_heartbeat", askPackage, askResponsePackage))
    {
        return ;
    }
    std::vector<std::string> cmdList = UnPackBeatResponse(askResponsePackage);
    std::copy(cmdList.begin(), cmdList.end(), std::back_inserter(m_CmdIdQueue));

    return ;
}

//////////////////////////////////////////////////////////////
// ��������:LogUploader::ParseCmdIds
//
// ��������:����ÿ������ĺ���
//
// �������:
// �������:void
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-11-03
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void LogUploader::ParseCmdIds()
{
    std::for_each(m_CmdIdQueue.begin(), m_CmdIdQueue.end(), std::bind1st(std::mem_fun(&LogUploader::ParseTargetCmdId), this));
    m_CmdIdQueue.clear();
    return ;
}

//////////////////////////////////////////////////////////////
// ��������:LogUploader::ParseTargetCmdId
//
// ��������:���ض�����������������
//
// �������:const std::string& cmdId,
// �������:void
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-11-03
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void LogUploader::ParseTargetCmdId(const std::string cmdId)
{
	std::string targetIdPackage = PackMonitorCmdParse(cmdId,m_LCP);
    std::string ResponsePackage;
    if (m_HttpLink.Post("/trawe-itms/interface/t_cmd_query", targetIdPackage, ResponsePackage))
    {
        LogCmdPair lcp = UnPackCmdParseResponse(ResponsePackage, cmdId) ;
        m_CmdQueue.push_back(lcp);
    }
}

//////////////////////////////////////////////////////////////
// ��������:LogUploader::ExcuteCmds
//
// ��������:ִ�н������������
//
// �������:
// �������:void
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-11-03
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void LogUploader::ExcuteCmds()
{
    std::for_each(m_CmdQueue.begin(), m_CmdQueue.end(), std::bind1st(std::mem_fun(&LogUploader::UploadFile), this));
    m_CmdQueue.clear();
    return ;
}

void LogUploader::UploadFile(const LogCmdPair lcp)
{
    if (lcp.second.cmdType != "1")//getLog
    {
		TW_TRACE_DIARY("unkown cmdType:"+lcp.second.cmdType);
        return ;
    }
	
    /*��ȡѹ���ļ�*/
    m_LogExplorer.RefreshFileView();
	std::string zipPath = m_LogExplorer.ZipTargetFiles(lcp.second.cmdParam.modules, lcp.second.cmdParam.startTime, lcp.second.cmdParam.endTime,lcp.first);
	if(zipPath.empty())
	{
		TW_TRACE_DIARY("nothing to upload,notify success");
		std::string ResponsePackage;
		m_HttpLink.Post("/trawe-itms/interface/t_cmd_confirm", PackMonitorConfirm(lcp.first,m_LCP), ResponsePackage);
		return;
	}
    std::ifstream sizeReader(zipPath.c_str(), ios::binary);
    if (!sizeReader.is_open())
    {
		TW_TRACE_DIARY("open file failed:"+zipPath);
        return;
    }
    sizeReader.seekg(0, ios_base::end);
    size_t zipFileSize = sizeReader.tellg();
    sizeReader.close();
    /*�ϴ�������Ϣ*/
	
    std::string logPackage = PackMonitorLogInfo(lcp.first, 1, zipPath, zipFileSize,m_LCP);
    std::string ResponsePackage;
    if (!m_HttpLink.Post("/trawe-itms/interface/create_upload_task", logPackage, ResponsePackage))
    {
		Delete_file(zipPath);
        return ;
    }
    int pieceSize =  UnPackLogInfoResponse(ResponsePackage) ;
    if (pieceSize <= 0 )
    {
		Delete_file(zipPath);
        return ;
    }
    FilePieceAdapter::SetFilePieceSize(pieceSize);

    /*��ʼ��Ƭ�ϴ�*/
    mb_FilePieceUploadContinue = true;
    m_FilePieceSequence = 1;
    m_FilePieceCmdId = lcp.first;
    std::ifstream zipReader(zipPath.c_str(), std::ios::binary);
	
    std::istream_iterator<FilePieceAdapter> pieceBegin(zipReader);
    std::istream_iterator<FilePieceAdapter> pieceEnd;

    std::for_each(pieceBegin, pieceEnd, std::bind1st(std::mem_fun(&LogUploader::UploadFilePiece), this));
	zipReader.close();
    std::string confirmPackStr;
    if (mb_FilePieceUploadContinue)
    {
        confirmPackStr = PackMonitorConfirm(lcp.first,m_LCP);
    }
    else
    {
        confirmPackStr = PackMonitorConfirm(lcp.first,m_LCP, "9001002", "fail");
    }
    m_HttpLink.Post("/trawe-itms/interface/t_cmd_confirm", confirmPackStr, ResponsePackage);

    mb_FilePieceUploadContinue = false;
    /*first_block next_block,file_+*/
    if (Delete_file(zipPath))
    {
        /*delete error*/
    }

}
void LogUploader::UploadFilePiece(const FilePieceAdapter pieceData)
{
    if (!mb_FilePieceUploadContinue)
    {
        return ;
    }
    if (pieceData.DataSize_Bytes == 0)
    {
        mb_FilePieceUploadContinue = false;
        return ;
    }
    /*����base64�ַ���*/
    int base64StrSize = pieceData.DataSize_Bytes * 2;
    char* base64Str = new char[base64StrSize];
    if (base64Str == NULL)
    {
        mb_FilePieceUploadContinue = false;
        return ;
    }
    memset(base64Str, 0, base64StrSize);
    Base64_Encode(base64Str, pieceData.Data,  pieceData.DataSize_Bytes);

    std::string filePiecePackage = PackMonitorFilePiece(m_FilePieceCmdId, m_FilePieceSequence, base64Str,m_LCP);
    ++m_FilePieceSequence;
    delete base64Str;
    base64Str = NULL;
    std::string responsePackage;
    if (!m_HttpLink.Post("/trawe-itms/interface/upload", filePiecePackage, responsePackage))
    {
        mb_FilePieceUploadContinue = false;
        return ;
    }
    if (!UnPackPieceInfoResponse(responsePackage))
    {

        mb_FilePieceUploadContinue = false;
        return ;
    }

}







