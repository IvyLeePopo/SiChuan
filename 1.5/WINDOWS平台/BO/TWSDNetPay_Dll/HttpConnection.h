#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

/**
 * Http����ͨѶ��
 * ����http��������������
 * ChangeRemotePeer���÷�������·��Ϣ
 * Post������Post����
 * LastHttpErrroDesc����ȡ��������ʧ������
 */
#include <afxinet.h>
#include <string>
class HttpConnection
{
public:
    HttpConnection();
    ~HttpConnection();

public:
    void ChangeRemotePeer(const std::string& ip, int port);/*Զ����Ϣ*/
    bool Post(const std::string& path,const std::string& data,std::string& FeedBack);/*��������*/
	bool Get(const std::string& path,const std::string& data,std::string& FeedBack);/*��������*/

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
