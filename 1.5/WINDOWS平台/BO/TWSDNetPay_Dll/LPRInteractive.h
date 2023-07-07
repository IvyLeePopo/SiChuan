#ifndef LPR_INTERACTIVE_H
#define LPR_INTERACTIVE_H

#include <string>
#include <windows.h>


class ShareMemoryController//,�����ڴ�����������Ե����ŵ�һ���ļ��У���ʱ����Ҫ
{
public:
    ShareMemoryController(const std::string& shareName,size_t memSize);//�����ڴ��С
    ~ShareMemoryController();
public:
    int WriteString(const std::string& info);//д��һ���ַ�����Ĭ�Ϸ���0������ֵ��ʾʧ��
    int ReadString(std::string& info);//��ȡһ���ַ�����Ĭ�Ϸ���0������ֵ��ʾʧ��
private:
    size_t m_MemSize;
    HANDLE m_MemHandle;
    char* m_MemAddress;//��������ʶ��ϵͳ����Ҫ�ĳ�ʼ����Ϣ
    bool m_IsValid;//�Ƿ���Ч
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


//��������ʶ�����������
class LPRInteractive
{
public:
    LPRInteractive();
    ~LPRInteractive();
public:
    void WriteInitPara(const LPRInitData& info);                                 //�����ĳ�ʼ����Ϣ
    bool QueryImageIDAndTaskID(std::string& imageId,std::string& taskId );          //�˴���Ȼ��Ϊ��ѯ�����ǲ���ȴ������������ȡ
    void WritePayAbstract(const std::string&imageId,const std::string& taskId,const std::string& payId,int ChannelType );//���׵�ժҪ��Ϣ
private:
    ShareMemoryController m_ShareInit;
    ShareMemoryController m_ShareCorrelationImage;
    ShareMemoryController m_SharePayAbstract;

private:
    LPRInteractive(const LPRInteractive&);
    LPRInteractive& operator = (const LPRInteractive&);

};

#endif // LPR_INTERACTIVE_H