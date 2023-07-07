#ifndef LPR_INTERACTIVE_H
#define LPR_INTERACTIVE_H

#include <string>
#include <windows.h>


class ShareMemoryController//,共享内存控制器，可以单独放到一个文件中，此时不需要
{
public:
    ShareMemoryController(const std::string& shareName,size_t memSize);//共享内存大小
    ~ShareMemoryController();
public:
    int WriteString(const std::string& info);//写入一个字符串，默认返回0，其他值表示失败
    int ReadString(std::string& info);//读取一个字符串，默认返回0，其他值表示失败
private:
    size_t m_MemSize;
    HANDLE m_MemHandle;
    char* m_MemAddress;//车牌特征识别系统所需要的初始化信息
    bool m_IsValid;//是否有效
private:
    ShareMemoryController(const ShareMemoryController&);
    ShareMemoryController& operator = (const ShareMemoryController&);

};
enum LPR_PAY_RESULT
{
    LPR_PAY_PLATE=1,
    LPR_PAY_CODE
};
struct LPRInitData
{
    int ProvinceID;
    std::string AreaID;
    std::string RoadID;
    std::string StationID;
    std::string StationName;
    std::string LaneID;
    std::string WebServerIP;
    std::string WebServerPort;
};


//车牌特征识别组件交互类
class LPRInteractive
{
public:
    LPRInteractive();
    ~LPRInteractive();
public:
    void WriteInitPara(const LPRInitData& info);                                 //车道的初始化信息
    bool QueryImageIDAndTaskID(std::string& imageId,std::string& taskId );          //此处虽然名为查询，但是不会等待，而是立马获取
    void WritePayAbstract(const std::string&imageId,const std::string& taskId,const std::string& payId,int ChannelType );//交易的摘要信息
private:
    ShareMemoryController m_ShareInit;
    ShareMemoryController m_ShareCorrelationImage;
    ShareMemoryController m_SharePayAbstract;

private:
    LPRInteractive(const LPRInteractive&);
    LPRInteractive& operator = (const LPRInteractive&);

};

#endif // LPR_INTERACTIVE_H