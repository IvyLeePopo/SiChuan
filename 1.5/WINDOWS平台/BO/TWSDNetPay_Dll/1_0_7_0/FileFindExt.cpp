//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// FileFindExt.cpp: implementation of the CFileFindExt class.
/********************************************************************/
/*      *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*              */
/*      |           中国科学院计算技术研究所         |              */
/*      *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*              */
/*      编程环境: Visual C++ 6.0  (Windows 2000)                    */
/*		作    者: 唐 胜 						                    */
/*		邮    箱: ts@ict.ac.cn      			                    */
/*      发布版本: V1.2.1                                              */
/*      发布日期: Nov. 01, 2003     		 	                    */
/*      功能说明：搜索同一目录下的所有同类型文件,并进行处理         */
/*      版本修改：                                                  */
/*                V1.0：Sep. 26, 2002, 递归实现                     */
/*                V1.1：Oct. 20, 2003, 为加快搜索速度，减少开销     */
/*                      采用先深遍历非递归化实现                    */
/*                V1.2：Oct. 21, 2003, 考虑目录的层次结构，采用     */
/*                      广度遍历非递归化实现，并直接做成类          */
/*                v1.2.1: Nov. 01, 2003 对V1.2进行了优化            */
/*      说    明：今后根据需要可增加其它多参数处理函数，或修改处    */
/*                理函数, 使用时请保留以上相关信息                  */
/********************************************************************/
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileFindExt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//构造函数和析构函数
/************************************************************************
功能说明: CFileFindExt的构造函数
参数说明: 无
输出说明: 无
函数调用: 类CStringArray的SetSize()和RemoveAll()
*************************************************************************/
CFileFindExt::CFileFindExt()
{
	m_FileNames.SetSize(MAX_PATH,MAX_PATH/2);
	m_FileNames.RemoveAll();
}

/************************************************************************
功能说明: CFileFindExt的析构函数
参数说明: 无
输出说明: 无
函数调用: Close()、类CStringArray的RemoveAll()
*************************************************************************/
CFileFindExt::~CFileFindExt()
{
	Close();
	m_FileNames.RemoveAll();	
}

//以下三个函数是成员变量操作函数
/************************************************************************
功能说明: 设置文件名数组成员变量的大小和内存分配增量
参数说明: nNewSizen：当数组的大小；GrowBy：内存分配增量
输出说明: 无
函数调用: 类CStringArray的SetSize()
*************************************************************************/
void CFileFindExt::SetSize(int nNewSize, int nGrowBy)
{
	m_FileNames.SetSize(nNewSize, nGrowBy);
}

/************************************************************************
功能说明: 获取文件名数组成员变量的大小
参数说明: 无
输出说明: 返回文件名数组成员变量的大小
函数调用: 类CStringArray的GetSize()
*************************************************************************/
int CFileFindExt::GetSize() const
{
	return m_FileNames.GetSize();
}

/************************************************************************
功能说明: 获取数组成员变量中的文件名
参数说明: nFileIndex: 要获取的文件名在数组中的位置
输出说明: 返回文件名
函数调用: 类CStringArray的GetAt()
*************************************************************************/
CString CFileFindExt::GetAt(int nFileIndex) const
{
	return m_FileNames.GetAt(nFileIndex);
}


//以下三个函数是对所有文件进行处理的成员函数
/************************************************************************
修改日期：2003-10-21
修改说明：为加快搜索速度，减少开销，用广度遍历实现非递归化
功能说明: 寻找指定目录下、指定扩展名的所有文件
调用格式: SearchFiles(DirName, FileExtName)
参数说明: DirName: 指定目录名
          FileExtName：指定扩展名，缺省为所有类型文件
输出说明: 1、所有文件的相对路径文件名（相对于起始搜索路径）
             保存在成员变量m_FileNames中；
		  2、成功返回true，分配内存不足时返回false
函数调用: FindFile()，FindNextFile()，GetFilePath()
*************************************************************************/
BOOL CFileFindExt::SearchFiles(CString DirName, CString FileExtName)
{
	const int MAXQUEUELEN=MAX_PATH<<4; //同一层所有目录下的最大子目录数
	char* Queue[MAXQUEUELEN] = {0};
	
	CString root;
	int rootlen=DirName.GetLength();
	//统一根目录最末尾不含'\'
	if(DirName.Right(1) != _T("\\"))
		root=DirName; 
	else
		root=DirName.Left(--rootlen);
	rootlen++;

	int head=0;
	int tail=0;
	Queue[tail++] = NULL; 
	char* str = NULL;

	CString strPath;		
	do{
		strPath=Queue[head];
		if (strPath!=_T(""))
			strPath=root+_T("\\")+strPath+_T("\\*.*");
		else
			strPath=root+strPath+_T("\\*.*");
		BOOL NotEndofCurDir = FindFile(strPath);
		while (NotEndofCurDir)
		{
			NotEndofCurDir = FindNextFile();
			
			if (IsDots())
				continue;
			
			// 若是子目录	
			if (IsDirectory())
			{
				//保存相对路径于队列中
				strPath = GetFilePath();
				int len=strPath.GetLength();
				strPath = strPath.Right(len-rootlen);
				len-=rootlen;
				if ((tail+1)%MAXQUEUELEN==head)
				{
					//队列已满
					//AfxMessageBox(_T("同一层子目录太多!"));
					//关闭和释放队列空间
					for (int i=head; head!=tail; )
					{
						str = Queue[head];
						if (str != _T(""))
							delete [] str;
						head=(head+1)%MAXQUEUELEN;
					}
					return false;
				}
				str=new TCHAR[len+1];
				_tcscpy(str,(LPCTSTR)strPath);
				Queue[tail]=str;
				tail=(tail+1)%MAXQUEUELEN;
				if (!NotEndofCurDir)
					break;
			}
			else
			{
				//处理当前访问的文件
				CString FileName=GetFilePath(); 
				FileName = FileName.Right(FileName.GetLength()-rootlen);
				if (FileExtName.Compare(_T("*.*")))
				{
					CString ext=FileName.Right(3);
					if (!ext.CompareNoCase(FileExtName.Right(3)))
						m_FileNames.Add(FileName);
				}
				else
					m_FileNames.Add(FileName);
			}
		} //End of while (NotEndofCurDir)
		str = Queue[head];
		if( str != NULL)
		{
			delete [] str;
			Queue[head] = NULL;
		}
		head=(head+1)%MAXQUEUELEN;	
	} while (head!=tail);
	Close();
	//m_iFiles=m_FileNames.GetSize();
	return true;
}

void CFileFindExt::Clear()
{
	m_FileNames.RemoveAll();	
}
