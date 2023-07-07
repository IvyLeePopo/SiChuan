#ifndef LOG_FILE_EXPLORER_H
#define LOG_FILE_EXPLORER_H
/**
 * 日志文件的管理
 * 文件系统部分，可考虑使用独立的文件操作函数
 */
#include<string>
#include<list>
#include <set>
#include "fs_funcs.h"
//暂不支持多线程操作
class LogFileExplorer
{
public:
    LogFileExplorer();
    ~LogFileExplorer();

public:
    void RefreshFileView();/*刷新文件视图*/
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
