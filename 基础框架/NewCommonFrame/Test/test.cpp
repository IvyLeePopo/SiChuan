// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <Windows.h>

using namespace std;

//for test
typedef int (*OUT_myAdd)(int a, int b);


HMODULE	m_hModule = NULL;



string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0,STR, -1, NULL, 0, NULL, NULL);
	char* chRtn =new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	delete[] chRtn;
	return str;
}


int _tmain(int argc, _TCHAR* argv[])
{
	//获取当前工作目录
	TCHAR strDir[260] = {0};
	::GetCurrentDirectory(260,strDir);
	string tmpdir = TCHAR2STRING(strDir);
//	string strDllFilePath = tmpdir.substr(0,tmpdir.rfind('\\'));
//	printf("strDllFilePath = %s\n",strDllFilePath.c_str() );  


	tmpdir.append("\\CommonFrameMain.dll");
	printf("strDllFilePath = %s\n",tmpdir.c_str() );  



// "E:\\vs2005Project\\NewCommonFrame\\lib\\CommonFrameMain.dll";
	string fileName =tmpdir;
	int bufSize=MultiByteToWideChar(CP_ACP,0,fileName.c_str(),-1,NULL,0);
	wchar_t* wp=new wchar_t[bufSize];
	memset(wp,0,bufSize);
	MultiByteToWideChar(CP_ACP,0,fileName.c_str(),-1,wp,bufSize);
	
	
	HINSTANCE hDLL = LoadLibrary(wp);
	if(hDLL <(HMODULE)HINSTANCE_ERROR)
	{
		printf("can't load fileName=%s.\n",fileName.c_str());
		delete [] wp;
		wp = NULL;
	}
	else
	{
		printf("load success\n");
		OUT_myAdd fnmyAdd = (OUT_myAdd)GetProcAddress(hDLL,"myAdd");

		if (fnmyAdd)
		{
			int nSum =fnmyAdd(4,5);
			printf("fnmyAdd ( 4,5 )= %d\n", nSum);

		}
		
	}




	
	printf("hell test");
	getchar();

	return 0;
}

