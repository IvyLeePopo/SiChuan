#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

/**
 * Http连接通讯类
 * 进行http主动的连接请求
 * ChangeRemotePeer设置服务器链路信息
 * Post：发送Post请求
 * LastHttpErrroDesc：获取最后的连接失败请求
 */
#include <afxinet.h>
#include <string>
class HttpConnection
{
public:
    HttpConnection();
    ~HttpConnection();

public:
    void ChangeRemotePeer(const std::string& ip, int port);/*远端信息*/
    bool Post(const std::string& path,const std::string& data,std::string& FeedBack);/*发送数据*/
	bool Get(const std::string& path,const std::string& data,std::string& FeedBack);/*发送数据*/

    std::string LastHttpErrorDesc();
private:
    bool OpenConn(CInternetSession& session,CHttpConnection *&httpConn,CHttpFile *&httpFile,const std::string& path,bool postMethod=true);
    bool PostData(CHttpFile *&httpFile,const std::string&data);
    bool ReadResponse(CHttpFile *&httpFile,std::string&feedBack);
    bool CloseConn(CHttpConnection *&httpConn,CHttpFile *&httpFile);
private:
    std::string m_HttpErrorDesc;
    int m_Port;
    std::string m_IpStr;
	bool m_isPostMethod;

private:
    HttpConnection(const HttpConnection&);
    HttpConnection& operator = (const HttpConnection&);

};



#endif // HTTP_CONNECTION_H
