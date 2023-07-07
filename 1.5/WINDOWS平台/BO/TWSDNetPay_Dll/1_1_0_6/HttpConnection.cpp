/*
* @Author: 朱亚冰
* @Date:   2017-11-01
* @Last Modified by:   windf
* @Last Modified time: 2017-11-10
*/
#include "HttpConnection.h"
#include "TypeHelper.h"


HttpConnection::HttpConnection()
{

}
HttpConnection::~HttpConnection()
{

}

void HttpConnection::ChangeRemotePeer(const std::string& ip, int port)
{
    m_Port = port;
    m_IpStr = ip;
}
bool HttpConnection::Post(const std::string& path, const std::string& postData, std::string& FeedBack)
{

    CInternetSession netSession;
    CHttpConnection *httpConn = NULL;
    CHttpFile *httpFile = NULL;
    try
    {
        OpenConn(netSession, httpConn, httpFile, path);
        PostData(httpFile, postData);
        ReadResponse(httpFile, FeedBack);
        CloseConn(httpConn,httpFile);

        return true;
    }
    catch (CInternetException *px)
    {
        CloseConn(httpConn,httpFile);
        m_HttpErrorDesc = "Http exception:" + Int2Str(px->m_dwError);
        return false;
    }
    return false;

}

std::string HttpConnection::LastHttpErrorDesc()
{
    return m_HttpErrorDesc;
}

bool HttpConnection::OpenConn(CInternetSession& session, CHttpConnection* &httpConn, CHttpFile* &httpFile, const std::string& path)
{
    session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 2);
    session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 1000 * 30);
    session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 1000 * 6); //发送请求的超时时间

    httpConn = session.GetHttpConnection(m_IpStr.c_str(), (INTERNET_PORT)m_Port);
    httpFile = httpConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, path.c_str());

    return true;
}

bool HttpConnection::PostData(CHttpFile* &httpFile, const std::string&postData)
{
	if(httpFile==NULL)
	{
		return false;
	}
    httpFile->AddRequestHeaders("Content-Type: application/json");
    httpFile->SendRequest(NULL, 0, (LPVOID)postData.c_str(), postData.size());
    DWORD dwRet;
    httpFile->QueryInfoStatusCode(dwRet);

    if (dwRet != HTTP_STATUS_OK)
    {
        m_HttpErrorDesc =  "Http error status:" + Int2Str(dwRet);
        return false;
    }
    return true;
}
bool HttpConnection::ReadResponse(CHttpFile* &httpFile, std::string&feedBack)
{
	if(httpFile==NULL)
	{
		return false;
	}
    CString mystr;
    CString tmpRead;
    while (httpFile->ReadString(tmpRead))
    {
        mystr += tmpRead;
    }
    if (mystr.IsEmpty())
    {
        m_HttpErrorDesc = "no return";
        return false;
    }

    feedBack = mystr.GetBuffer(0);
    return true;
}

bool HttpConnection::CloseConn(CHttpConnection* &httpConn, CHttpFile* &httpFile)
{
    if (httpFile != NULL)
    {
        httpFile->Close();
        delete httpFile;
        httpFile = NULL;
    }
    if (httpConn != NULL)
    {
        httpConn->Close();
        delete httpConn;
        httpConn = NULL;
    }
    return true;
}