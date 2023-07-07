#include "FrameConfigFileOperate.h"


#include <string>
//#include <QFile>
#include <stdio.h>


#define DOWNLOAD_STRING  "download"
#define LOADLIB_STRING  "loadLib"
#define LIB_STRING  "lib"
#define LOG_STRING  "log"
#define NET_PAY_STRING  "netPayPath"

#include <afxwin.h> //afx 
#include "pystring.h"




//#include "unexcept\CatchDump.h"

std::string TmpGetDumpDir()
{
    char modePath[300] = {0};
    memset(modePath, 0, 260);

    GetModuleFileName(AfxGetInstanceHandle(), modePath, 260);

    std::string pathStr = modePath;
    size_t pos = pathStr.find_last_of('\\');
    if (pos == std::string::npos)
    {
        return "";
    }
    pathStr = pathStr.substr(0, pos) + "\\TWSDNetPayLog";
    return pathStr;
}
int CreateDumpFileCore()
{
	/*
	std::string tmpStr = TmpGetDumpDir();
    CString strDumpFilePath;
    strDumpFilePath.Format(_T("%s\\DumpFile.dmp"), tmpStr.c_str());
    static CCatchDump* m_pCatchDump = new CCatchDump(1);

    if(NULL!=m_pCatchDump)
    {
        m_pCatchDump->InitCatchExecption(strDumpFilePath,FALSE,_T(""),2);
    }
	*/
	return 0;
}

int CreateDumpFile()
{
	static int  i = CreateDumpFileCore();
	return i;
}




using namespace std;


FrameConfigFileOperate::FrameConfigFileOperate()
{
	
	CreateDumpFile();
	
}

bool FrameConfigFileOperate::parseFilePathConfig(ST_FRAME_CONFIG_INFO *result,std::string& module_path )
{

	TCHAR szTempDir[260] = {0};
	::GetModuleFileName(AfxGetInstanceHandle(), szTempDir, sizeof(TCHAR) * 260);

	module_path = pystring::os::path::dirname(szTempDir);

	std::string lowStr = pystring::lower(szTempDir);

	strcpy(szTempDir,lowStr.c_str());


	//MessageBox(0,"",szTempDir,0);
	CString strTemp(szTempDir);
		
	CString strDir;
	strDir = strTemp.Left(strTemp.ReverseFind('\\'));
	
	


	//创建日志目录

	strTemp.Format(_T("%s\\%s"), strDir,FILE_PATH_CONFIG_PATH);
	//MessageBox(0,"",strTemp,0);
	

    FILE *fp = fopen(strTemp,"r");
    if( fp==NULL)
    {
        printf("[#FrameConfigFileOperate::parseFilePathConfig#]error:can not open file %s\n",FILE_PATH_CONFIG_PATH);
        return false;
    }
    /*char buf[MAX_BUF_LEN] ={0};
    fseek(fp,0L,SEEK_SET);
    fgets(buf, MAX_BUF_LEN, fp);
	*/
	std::string buf;
	while(!feof(fp))
    {
		char szTest[1000]={0};
        memset(szTest, 0, sizeof(szTest));  
        fgets(szTest, sizeof(szTest) - 1, fp); // 包含了\n  
        buf+=szTest;  
    }  
    fclose(fp);
	

	//MessageBox(0,"",buf.c_str(),0);

    Json::Value rootN;
    Json::Reader  reader;
    if (!reader.parse(buf.c_str() , rootN))
    {
        return false;
    }
	
    bool  getResultFlag = false;
    if(true==getJsonData(rootN,DOWNLOAD_STRING,result->downloadPath))
        getResultFlag = true;
    else
        getResultFlag =false;

    if(true==getJsonData(rootN,LOADLIB_STRING,result->loadLibPath))
        getResultFlag = true;
    else
        getResultFlag =false;
    if(true==getJsonData(rootN,LIB_STRING,result->libPath))
        getResultFlag = true;
    else
        getResultFlag =false;
    if(true==getJsonData(rootN,LOG_STRING,result->logPath))
        getResultFlag = true;
    else
        getResultFlag =false;
    if(true==getJsonData(rootN,NET_PAY_STRING,result->netPayPath))
        getResultFlag = true;
    else
        getResultFlag =false;
	
	//add 生成绝对路径
	strTemp.Format(_T("%s\\%s"), strDir,"twupdate");
	strcpy(result->downloadPath,strTemp);
	
	strTemp.Format(_T("%s\\%s"), strDir,result->loadLibPath);
	strcpy(result->loadLibPath,strTemp);
	strTemp.Format(_T("%s\\%s"), strDir,result->libPath);
	strcpy(result->libPath,strTemp);
	strTemp.Format(_T("%s\\%s"), strDir,"twsdnetpaycore_v1.dll");
	strcpy(result->netPayPath,strTemp);
	strTemp.Format(_T("%s\\%s"), strDir,"twsdnetpaycore_v1.dll");
	strcpy(result->netPayPath_V1,strTemp);

	strTemp.Format(_T("%s\\%s"), strDir,"twsdnetpaycore_v2.dll");
	strcpy(result->netPayPath_V2,strTemp);
	

	strTemp.Format(_T("%s\\%s"), strDir,"filepathconfig.txt");
	strcpy(result->configFilePath,strTemp);
	
	//add 2018-04
	strTemp.Format(_T("%s\\%s"), strDir,"itc.dll");
	strcpy(result->itc_path,strTemp);
	
	
	sprintf(result->updateCorePath,"%s\\%s",strDir,"twupdatecore.dll");
	return true;

    return getResultFlag;
}

bool FrameConfigFileOperate::getJsonData( Json::Value &rootN,char *msgType, char *result)
{
    Json::Value loadLibCode = rootN[msgType];
    if ( loadLibCode.type() != Json::stringValue )
    {
        printf("ERROR:[#ConfigFileOperate::parseFilePathConfig#]loadLibCode error %s",msgType);
        return false;
    }
    else
    {
        string msg = rootN[msgType].asCString();
        memcpy(result,msg.c_str(),msg.length());
        return true;
    }
}
