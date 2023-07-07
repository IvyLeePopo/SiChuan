#ifndef LOG_FILE_EXPLORER_H
#define LOG_FILE_EXPLORER_H
/**
 * ��־�ļ��Ĺ���
 * �ļ�ϵͳ���֣��ɿ���ʹ�ö������ļ���������
 */
#include<string>
#include<list>
#include <set>
#include "fs_funcs.h"
//�ݲ�֧�ֶ��̲߳���
class LogFileExplorer
{
public:
    LogFileExplorer();
    ~LogFileExplorer();

public:
    void RefreshFileView();/*ˢ���ļ���ͼ*/
	std::string ZipTargetFiles(const std::set<std::string>&filePrefix,const std::string& startTime,const std::string& endTime,const std::string& fileName);
private:/*over data*/
    std::string GenerateZipFileName();
private:
    Search_file_info m_logFileSearcher;
    std::string m_LogPath;
private:
    std::list<string> m_CurrFileList;

private:
    LogFileExplorer(const LogFileExplorer&);
    LogFileExplorer& operator = (const LogFileExplorer&);

};



void SplitLogField(const std::string& fileName,std::string& prefix,std::string& data);

struct LogFileFilter
{
    std::set<std::string> filePreName;
    std::string fileStartTime;
    std::string fileEndTime;
    LogFileFilter(const std::set<std::string>&filePrefix,const std::string& startTime,const std::string& endTime)
    {
        filePreName = filePrefix;
        fileStartTime = startTime;
        fileEndTime = endTime;
    }
    bool operator()(const std::string& filePath)
    {   
        std::string preName;
        std::string dateStr;
        SplitLogField(filePath,preName,dateStr);
        if(filePreName.count(preName)==0)
        {
            return true;
        }
        if( dateStr > fileEndTime 
            || dateStr < fileStartTime)
        {
            return true;
        }
        return false;
    }
};
#endif // LOG_FILE_EXPLORER_H
