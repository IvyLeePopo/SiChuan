#include "stdafx.h"
#include "HttpClient_LibCurl.h"
#include "curl/curl.h"
//#ifdef _DEBUG
//#pragma comment(lib, "libcurld.lib")
//#else
//#pragma comment(lib, "libcurl.lib")
//#endif

HttpClientLibCurl::HttpClientLibCurl()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

HttpClientLibCurl::~HttpClientLibCurl()
{
	curl_global_cleanup();
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return nmemb;
}

int HttpClientLibCurl::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int iTimeOut)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, iTimeOut);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, iTimeOut);
	//curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}