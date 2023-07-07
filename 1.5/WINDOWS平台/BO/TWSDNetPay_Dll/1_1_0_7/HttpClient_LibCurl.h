#ifndef _INC_HTTPCLIENT_LIBCURL_H
#define _INC_HTTPCLIENT_LIBCURL_H
#include <string>
class HttpClientLibCurl
{
public:
	HttpClientLibCurl();
	~HttpClientLibCurl();
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int iTimeOut = 1000);
	void	FreeInstance() { delete this; };
};

#endif