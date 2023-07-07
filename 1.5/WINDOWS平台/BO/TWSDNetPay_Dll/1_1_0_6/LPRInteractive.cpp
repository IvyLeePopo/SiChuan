/*
* @Author: ���Ǳ�
* @Date:   2018-05-03
* @Last Modified by:   windf
* @Last Modified time: 2018-05-03
*/
#include "LPRInteractive.h"
#include "string_func.h"

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
                          INVALID_HANDLE_VALUE,    // �����ļ����
                          NULL,                    // Ĭ�ϰ�ȫ����
                          PAGE_READWRITE,          // �ɶ���д
                          0,                       // ��λ�ļ���С
                          100,                // ��λ�ļ���С
                          shareName.c_str()                   // �����ڴ�����
                      );


        m_MemAddress = (char *)MapViewOfFile(
                           m_MemHandle,            // �����ڴ�ľ��
                           FILE_MAP_ALL_ACCESS, // �ɶ�д���
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
    if(m_ShareCorrelationImage.ReadString(imagePara)!=0)
    {
        return false;
    }
    std::list<std::string> strsLPR =  Split_str(imagePara,'#');
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
    m_SharePayAbstract.WriteString(paraStr);
}


