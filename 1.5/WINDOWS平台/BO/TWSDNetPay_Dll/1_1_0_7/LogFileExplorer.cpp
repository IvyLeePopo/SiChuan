/*
* @Author: 朱亚冰
* @Date:   2017-11-03
* @Last Modified by:   windf
* @Last Modified time: 2017-11-15
*/
#include "LogFileExplorer.h"
#include "pystring.h"
#include <algorithm>
#include <windows.h>
#include "zip.h"

#include "TypeHelper.h"
bool isAllNumber(const std::string& str);

LogFileExplorer::LogFileExplorer()
{
    m_LogPath = pystring::os::path::join(fs_thisModulePath(), "TWSDNetPayLog");
    strncpy(m_logFileSearcher.root_catalog, m_LogPath.c_str(), 250);
    strcpy(m_logFileSearcher.pattern, "*.log");
    m_logFileSearcher.enable_wildcard = true;

}
LogFileExplorer::~LogFileExplorer()
{

}

void LogFileExplorer::RefreshFileView()
{
    m_CurrFileList = Search_files(m_logFileSearcher);
}
std::string LogFileExplorer::ZipTargetFiles(const std::set<std::string>&filePrefix, const std::string& startTime, const std::string& endTime,const std::string& fileName)
{
    std::list<std::string> targetList ;
    LogFileFilter LFF(filePrefix,  startTime,  endTime);
    /*寻找符合条件的文件*/

    std::remove_copy_if(m_CurrFileList.begin(), m_CurrFileList.end(), std::back_inserter(targetList), LFF);
    if (targetList.empty())
    {
        return "";
    }
    /*压缩文件*/
    std::string zipPath = pystring::os::path::join(m_LogPath, fileName+".zip");
    HZIP zipHandle = CreateZip(zipPath.c_str(), 0);


    for (std::list<std::string>::iterator fileIt = targetList.begin(); fileIt != targetList.end(); ++fileIt)
    {
        // MessageBox(0, fileIt->c_str(), "", 0);
        HANDLE hf = CreateFile(fileIt->c_str(),
                               GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if ( hf == INVALID_HANDLE_VALUE)
        {
            CloseZip(zipHandle);
            Delete_file(zipPath);
            zipPath = "";
            return zipPath;
        }

        ZRESULT addCode = ZipAddHandle(zipHandle, pystring::os::path::basename(*fileIt).c_str(), hf);
        CloseHandle(hf);
        
        if ( addCode != ZR_OK)
        {
            //MessageBox(0, ("zip fail" + Int2Str(addCode)).c_str(), "", 0);
            CloseZip(zipHandle);
            Delete_file(zipPath);
            zipPath = "";
            return zipPath;
        }
    }
    CloseZip(zipHandle);
    // MessageBox(0, "zip success", "", 0);
    return zipPath;



}
//////////////////////////////////////////////////////////////
// 函数名称:LogFileExplorer::GenerateZipFileName
//
// 功能描述:获取zip文件的临时名称，此文件不会长期保存
//
// 输入参数:
// 输出参数:std::string
// 编写人员:朱亚冰
// 编写时间:2017-11-07
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
std::string LogFileExplorer::GenerateZipFileName()
{
    static std::string nameChs = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWZYZ";
    std::string randName;
    srand(time(NULL));
    for (int i = 0; i < 10; ++i)
    {
        randName += nameChs[rand() % nameChs.size()];
    }
    randName += ".zip";
    return randName;

}

void SplitLogField(const std::string& fileName, std::string& prefix, std::string& date)
{
    std::string baseName = pystring::os::path::basename(fileName);
    prefix = "";
    date = "";

    size_t dotPos = baseName.find('.');
    if (dotPos == std::string::npos)
    {
        return ;
    }
    if (dotPos < 9 )
    {   /*for log format xxx20101010.log,最短也有8个*/
        return ;
    }
    prefix = baseName.substr(0, dotPos - 8);
    date = baseName.substr(dotPos - 8, 8);
    if (!isAllNumber(date))
    {
        prefix = "";
        date = "";
        return ;
    }

    return ;

}

bool isAllNumber(const std::string& str)
{
    char maxch = *std::max_element(str.begin(), str.end());
    char minch = *std::min_element(str.begin(), str.end());
    return (isdigit(maxch) && isdigit(minch));
}
