/*
* @Author: 朱亚冰
* @Date:   2018-05-03
* @Last Modified by:   windf
* @Last Modified time: 2018-05-03
*/
#include "stdafx.h"
#include "LPRInteractive.h"
#include "string_func.h"
#include "TWSDNetPay_Dll.h"

ShareMemoryController::ShareMemoryController(const std::string& shareName, size_t memSize)
{
    m_IsValid = false;
    m_MemHandle = INVALID_HANDLE_VALUE;
    m_MemAddress = NULL;
    m_MemSize = memSize;


    bool paraValid = (!shareName.empty()) && (m_MemSize > 10 && m_MemSize < 5000);
    if ( paraValid)
    {
        m_MemHandle = CreateFileMapping(
                          INVALID_HANDLE_VALUE,    // 物理文件句柄
                          NULL,                    // 默认安全级别
                          PAGE_READWRITE,          // 可读可写
                          0,                       // 高位文件大小
                          100,                // 地位文件大小
                          shareName.c_str()                   // 共享内存名称
                      );


        m_MemAddress = (char *)MapViewOfFile(
                           m_MemHandle,            // 共享内存的句柄
                           FILE_MAP_ALL_ACCESS, // 可读写许可
                           0,
                           0,
                           m_MemSize
                       );
        if (m_MemHandle != NULL && m_MemAddress != NULL)
        {
            m_IsValid = true;
            memset(m_MemAddress,0,m_MemSize);

        }
    }
}
ShareMemoryController::~ShareMemoryController()
{
    if (m_MemAddress)
    {
        ::UnmapViewOfFile(m_MemAddress);
        m_MemAddress = NULL;
    }

    if (m_MemHandle)
    {
        ::CloseHandle(m_MemHandle);
        m_MemHandle = NULL;
    }

}

int ShareMemoryController::WriteString(const std::string& info)
{
    if (!m_IsValid)
    {
        return -1;
    }
	//MessageBox(0,info.c_str(),"",0);
    strncpy(m_MemAddress, info.c_str(), m_MemSize - 1);
    m_MemAddress[m_MemSize - 1] = '\0';
    return 0;
}

int ShareMemoryController::ReadString(std::string& info)
{
    if (!m_IsValid)
    {
        return -1;
    }
    char chs[5000]={0};
    int readSize = (std::min)(4000,(int)m_MemSize);

    strncpy(chs,m_MemAddress,readSize);
    if(chs[0]=='\0')
    {
        return -1;
    }
    info = std::string(chs);
    return 0;
}



 LPRInteractive::LPRInteractive():
 m_ShareInit("TWMem_LPRInit",1000),
 m_ShareCorrelationImage("TWMem_LPRImageID",1000),
 m_SharePayAbstract("TWMem_LPRPayAbstract",1000)
 {

 }
LPRInteractive::~LPRInteractive()
{

}

void LPRInteractive::WriteInitPara(const LPRInitData& info)
{
	std::string paraStr = "#"+Int2str(info.ProvinceID);
    paraStr += "#"+info.AreaID;
    paraStr += "#"+info.RoadID;
    paraStr += "#"+info.StationID;
    paraStr += "#"+info.StationName;
    paraStr += "#"+info.LaneID;
    paraStr += "#"+info.WebServerIP;
    paraStr += "#"+info.WebServerPort;
    m_ShareInit.WriteString(paraStr);
}


bool LPRInteractive::QueryImageIDAndTaskID(std::string& imageId,std::string& taskId )
{
    std::string imagePara;
	GetGlobalApp()->RecordLog("Query ImageID and TaskID from LPR");
    if(m_ShareCorrelationImage.ReadString(imagePara)!=0)
    {
        return false;
    }
	GetGlobalApp()->RecordLog(("Query ImageID and TaskID from LPR success " + imagePara).c_str());
    std::list<std::string> strsLPR =  Split_str(imagePara,'#');
	for(std::list<std::string>::iterator it = strsLPR.begin();it!=strsLPR.end();++it)
	{
		GetGlobalApp()->RecordLog(("Para Parse " + *it).c_str());
	}
    if(strsLPR.size()!=2)
    {
        return false;
    }
    imageId = strsLPR.front();
    strsLPR.pop_front();
    taskId = strsLPR.front();

    return true;
}
void LPRInteractive::WritePayAbstract(const std::string&imageId,const std::string& taskId,const std::string& payId,int ChannelType )
{
	std::string paraStr = "#"+imageId;
    paraStr += "#"+taskId;
    paraStr += "#"+payId;
    paraStr += "#"+Int2str(ChannelType);
	GetGlobalApp()->RecordLog(("WritePayAbstract" +paraStr ).c_str());
    m_SharePayAbstract.WriteString(paraStr);
}


