/*
* @Author: 朱亚冰
* @Date:   2017-12-06
* @Last Modified by:   windf
* @Last Modified time: 2017-12-26
*/
#include "stdafx.h"
#include <fstream>
#include <sstream>

#include "FirmwareUpgrade.h"
#include "IOAdapter.h"
#include "JsonHelper.h"

#include "ScanMachine.h"
extern CScanMachineApp theApp;

void WriteLog(const std::string&info)
{
	std::string dbgInfo = "[Firmware Core]" + info;
	theApp.RecordMsgTemp(dbgInfo.c_str(),1);
}

std::list<unsigned char> GetFileStream(const std::string& fileName,size_t maxFileSize=10*1024*1024/*最大文件大*/)
{
	if(FileGetSize(fileName) > maxFileSize)
	{
		return std::list<unsigned char>();
	}
	std::ifstream fileReader(fileName.c_str(), std::ios::binary);
	if(!fileReader.is_open())
	{
		
		return std::list<unsigned char>();
	}
	std::list<unsigned char> pieceList;
	while(!fileReader.eof())
	{
		 char buf[1000]={0};
		fileReader.read(buf,500);
		size_t realRead = fileReader.gcount();
		std::copy(buf, buf+realRead, std::back_inserter(pieceList));
	}
	/*
    std::istream_iterator<unsigned char> pieceBegin(fileReader);
    std::istream_iterator<unsigned char> pieceEnd;
    std::list<unsigned char> pieceList;
    std::copy(pieceBegin, pieceEnd, std::back_inserter(pieceList));
	*/

    fileReader.close();
	return pieceList;
}
std::string GetFileHexData(const std::string& fileName)
{
	std::list<unsigned char> fileBytes = GetFileStream(fileName);
	std::stringstream ss;
	for(std::list<unsigned char>::iterator it = fileBytes.begin();it!=fileBytes.end();++it)
	{
		char chs[10]={0};
		sprintf(chs,"%02X",*it);
		ss<<chs;
	}
	return ss.str();

	
}
FirmwareUpgrade::FirmwareUpgrade()
{

	::InitializeCriticalSection(&m_accessMutex);
	
    m_upgradeState = FIRMWARE_UPGRADE_STATE_IDLE;
    m_firmwareType = -1;

}
FirmwareUpgrade::~FirmwareUpgrade()
{
	::DeleteCriticalSection(&m_accessMutex);
}
//////////////////////////////////////////////////////////////
// 函数名称:FirmwareUpgrade::SetUpgradeInfo
//
// 功能描述:升级包参数
//
// 输入参数:int firmware_type, const std::string& firmware_path,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-12-06
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//
//////////////////////////////////////////////////////////////
void FirmwareUpgrade::SetUpgradeInfo(int firmware_type, const std::string& firmware_path)
{
	WriteLog("upgrade file path:"+firmware_path);
	CScopeLock apiLock(&m_accessMutex);
    if (m_upgradeState == FIRMWARE_UPGRADE_STATE_AVAILABLE
            || m_upgradeState == FIRMWARE_UPGRADE_STATE_UPGRADING )
    {
		WriteLog("update state dismatch:");
        return ;
    }
	
    if (firmware_type < 1 || firmware_type > 2)
    {
		WriteLog("firmware_type dismatch:");
        return ;//error type
    }
	
	
    if (access(firmware_path.c_str(), 0) != 0)
    {
		WriteLog("file Non accessible:"+firmware_path);
        return ;//file not exist
    }

    m_firmwarePath = firmware_path;
	m_firmwareType = firmware_type;
	
	WriteLog("set update info success");
    GenerateUpgradeData();
}


//////////////////////////////////////////////////////////////
// 函数名称:FirmwareUpgrade::UpgradeControlCMD
//
// 功能描述:生成控制命令
//
// 输入参数:const FIRMWARE_UPGRADE_CONTROL_OPTION& ctl_op,
// 输出参数:FirmwareUpgradeControl
// 编写人员:朱亚冰
// 编写时间:2017-12-07
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
FirmwareUpgradeControl FirmwareUpgrade::UpgradeControlCMD(const FIRMWARE_UPGRADE_CONTROL_OPTION& ctl_op  )
{
    FirmwareUpgradeControl FUC;
    FUC.firmware_type = m_firmwareType;
    FUC.reserved = 0;
    switch (ctl_op)
    {
    case FIRMWARE_UPGRADE_CONTROL_STARTUP:
        FUC.operation = 1;
        break;
    case FIRMWARE_UPGRADE_CONTROL_STOP:
        FUC.operation = 2;
        break;
    case FIRMWARE_UPGRADE_CONTROL_COMPLETE:
        FUC.operation = 3;
        break;
    case FIRMWARE_UPGRADE_CONTROL_UNDIFINED:
	default:
        FUC.operation = -1;
        break;
    }
    return FUC;

}

FIRMWARE_UPGRADE_STATE FirmwareUpgrade::UpgradeState()
{
	CScopeLock apiLock(&m_accessMutex);
    return m_upgradeState;
}

//////////////////////////////////////////////////////////////
// 函数名称:FirmwareUpgrade::QueryUpgradeData
//
// 功能描述:查询是否有分片数据可以使用
//
// 输入参数:FirmwareUpgradeTransfer& upgrade_data,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-12-07
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//
//////////////////////////////////////////////////////////////
bool FirmwareUpgrade::QueryUpgradeData(FirmwareUpgradeTransfer& upgrade_data)
{
	CScopeLock apiLock(&m_accessMutex);
    if (m_upgradeState == FIRMWARE_UPGRADE_STATE_AVAILABLE) //可用更新(此时，尚未开始更新)
    {
        if (m_upgradeDataQueue.empty())
        {
            m_upgradeState = FIRMWARE_UPGRADE_STATE_ERROR;
            return false;//error
        }
        upgrade_data = m_upgradeDataQueue.front();
        m_upgradeDataQueue.pop_front();
        if (m_upgradeDataQueue.empty())
        {
            m_upgradeState = FIRMWARE_UPGRADE_STATE_ERROR;
            return false;//error, only one piece is impossible
        }
        m_upgradeState = FIRMWARE_UPGRADE_STATE_UPGRADING;
        return true;
    }
    else if (m_upgradeState == FIRMWARE_UPGRADE_STATE_UPGRADING) //更新中
    {
        if (m_upgradeDataQueue.empty())
        {
            m_upgradeState = FIRMWARE_UPGRADE_STATE_ERROR;
            return false;//error
        }
        upgrade_data = m_upgradeDataQueue.front();
        m_upgradeDataQueue.pop_front();
        if (m_upgradeDataQueue.empty())
        {
            m_upgradeState = FIRMWARE_UPGRADE_STATE_COMPLETE;//complete ,end of pieces
        }
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////
// 函数名称:FirmwareUpgrade::GenerateUpgradeData
//
// 功能描述:生成更新包数据
//
// 输入参数:
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-12-06
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void FirmwareUpgrade::GenerateUpgradeData()
{
    if ( m_upgradeState == FIRMWARE_UPGRADE_STATE_UPGRADING)
    {
		WriteLog("正在更新中，不处理");
        return ;
    }
	
    m_upgradeDataQueue.clear();


	

    FirmwareUpgradeTransfer dataBase;
	dataBase.firmware_type = m_firmwareType;
    dataBase.data_type = 1;
    dataBase.data_para1 = FileGetSize(m_firmwarePath);
    dataBase.data_para2 = 0;

	
    // 获取分片
	std::ifstream fileReader(m_firmwarePath.c_str(), std::ios::binary);
	if(!fileReader.is_open())
	{
		WriteLog("打开升级文件失败："+m_firmwarePath );
		return ;
	}
	
	FilePieceAdapter::SetFilePieceSize(512);
    //std::istream_iterator<FilePieceAdapter> pieceBegin(fileReader);
    //std::istream_iterator<FilePieceAdapter> pieceEnd;
    std::list<FilePieceAdapter> pieceList;

    //std::copy(pieceBegin, pieceEnd, std::back_inserter(pieceList));some error occure

	/*
	int i =0;
	while(!fileReader.eof())
	{
		++i;
		FilePieceAdapter debug_d;
		fileReader>>debug_d;
		char chs[100]={0};
		sprintf(chs,"debug_d:%d",debug_d.DataSize_Bytes);
		WriteLog(chs);
		FilePieceAdapter deddsd(debug_d);
		

		//pieceList.push_back(debug_d);
		
	}
	*/
	int pieceSeq = 1;
	while(!fileReader.eof())
	{
		char info[600]={0};
		fileReader.read(info,512);

		FirmwareUpgradeTransfer FUT;
		FUT.firmware_type = m_firmwareType;
        FUT.data_type = 2;
        FUT.data_para1 = pieceSeq++;
		FUT.data_para2 = fileReader.gcount();
        FUT.data = new char[FUT.data_para2];
        if (FUT.data == NULL)
        {
            m_upgradeDataQueue.clear();
            return ;//error
        }
        memcpy(FUT.data,info,FUT.data_para2);
        m_upgradeDataQueue.push_back(FUT);
		
	}
    fileReader.close();

	/*
	std::list<int> debug_int_list;
	for(std::list<int>::iterator it = debug_int_list.begin();it!=debug_int_list.end();++it)
	{

		WriteLog("emptyint ");
	}
	*/

	/*
    // 生成序列信息(may merge with up )
    int pieceSeq = 1;
    for (std::list<FilePieceAdapter>::iterator pieceIt = pieceList.begin(); pieceIt != pieceList.end(); ++pieceIt)
    {
		char chs[100]={0};
		sprintf(chs,"pieceIt:%d,%d",pieceIt->DataSize_Bytes,pieceList.size());
		WriteLog(chs);
        FirmwareUpgradeTransfer FUT;
        FUT.firmware_type = realFirmType;
        FUT.data_type = 2;
        FUT.data_para1 = pieceSeq++;
        FUT.data_para2 = pieceIt->DataSize_Bytes;
        FUT.data = new char[FUT.data_para2];
        if (FUT.data == NULL)
        {
            m_upgradeDataQueue.clear();
            return ;//error
        }
        memcpy(FUT.data,pieceIt->Data,FUT.data_para2);
        m_upgradeDataQueue.push_back(FUT);
		WriteLog("piece");
    }
	*/
	

    m_upgradeDataQueue.push_front(dataBase);//此为第一个分片
	m_upgradeState =FIRMWARE_UPGRADE_STATE_AVAILABLE;


}
void FirmwareUpgrade::SetUpgradeState(FIRMWARE_UPGRADE_STATE FUS)
{	
	CScopeLock apiLock(&m_accessMutex);
    m_upgradeState = FUS;
}

void FirmwareUpgrade::RemoveUpgradeFile()
{
	CScopeLock apiLock(&m_accessMutex);
    remove(m_firmwarePath.c_str());
}
void FirmwareUpgrade::Reupgrade() 
{
	CScopeLock apiLock(&m_accessMutex);
    SetUpgradeInfo(m_firmwareType,m_firmwarePath);
}

void FirmwareUpgrade::SetUpgradeInfo(const std::string& paraJson)
{
	WriteLog("收到远程控制指令，参数:"+paraJson);
	
    Json::Reader jsonReader;
    Json::Value rootNode;
    
    if (!jsonReader.parse(paraJson.c_str(), rootNode))
    {
        return ;//invalid json
    }
    Json::Value baseInfo = rootNode["FirmwareInfo"];
	std::string type;
    std::string path;
    bool flag = ReadJsonString(baseInfo,"FirmwareType",type);
	flag = flag ? ReadJsonString(baseInfo,"FirmwarePath",path):false;
    if(!flag)
    {
        return ;
    }
	
	int realType;
	if(type == "FW000")//master firmware
	{
		realType=1;
		SetUpgradeInfo(realType,path);
	}
	else if(type=="FW001")//slave firmware 
	{
		realType=2;
		SetUpgradeInfo(realType,path);
	}
	else if(type.find("C")==0)//content 
	{
		FreeDisplayData fdd;
		fdd.DataType = 1;
		fdd.HexData= GetFileHexData(path);
		if(remove(path.c_str())!=0)
		{
			char chs[100]={0};
			sprintf(chs,"delete file:%s,%d",path.c_str(),GetLastError());
			WriteLog(chs);
		}
		this->AddFreeDisplayData(fdd);
	}
	else if(type.find("A")==0)//audio
	{
		FreeDisplayData fdd;
		fdd.DataType = 2;
		fdd.HexData = GetFileHexData(path);
		if(remove(path.c_str())!=0)
		{
			char chs[100]={0};
			sprintf(chs,"delete file:%s,%d",path.c_str(),GetLastError());
			WriteLog(chs);
		}
		this->AddFreeDisplayData(fdd);
		
	}
	else if(type.find("S")==0)//style
	{
		FreeDisplayStyle fds;
		fds.HexData = GetFileHexData(path);
		WriteLog(fds.HexData);
		if(remove(path.c_str())!=0)
		{
			char chs[100]={0};
			sprintf(chs,"delete file:%s,%d",path.c_str(),GetLastError());
			WriteLog(chs);
		}
		this->AddFreeDisplayStyle(fds);
	}
	else
	{
		realType = 99;//???error **
	}

   

}

void FirmwareUpgrade::KeepIdle()
{
	CScopeLock apiLock(&m_accessMutex);
	m_upgradeState = FIRMWARE_UPGRADE_STATE_IDLE;
}




std::list<FreeDisplayData> FirmwareUpgrade::FreeDisplayDataList()
{
	CScopeLock apiLock(&m_accessMutex);
	std::list<FreeDisplayData> tmp;
	tmp.swap(m_freeDataQueue);
	return tmp;
}
std::list<FreeDisplayStyle> FirmwareUpgrade::FreeDisplayStyleList()
{
	CScopeLock apiLock(&m_accessMutex);
	std::list<FreeDisplayStyle> tmp;
	tmp.swap(m_freeStyleQueue);
	return tmp;
}


void FirmwareUpgrade::AddFreeDisplayData(const FreeDisplayData& fdd)
{
		
    CScopeLock apiLock(&m_accessMutex);
    m_freeDataQueue.push_back(fdd);

}
void FirmwareUpgrade::AddFreeDisplayStyle(const FreeDisplayStyle& fds)
{
     CScopeLock apiLock(&m_accessMutex);
     m_freeStyleQueue.push_back(fds);
}