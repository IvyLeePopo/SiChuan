//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// FileFindExt.cpp: implementation of the CFileFindExt class.
/********************************************************************/
/*      *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*              */
/*      |           �й���ѧԺ���㼼���о���         |              */
/*      *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*              */
/*      ��̻���: Visual C++ 6.0  (Windows 2000)                    */
/*		��    ��: �� ʤ 						                    */
/*		��    ��: ts@ict.ac.cn      			                    */
/*      �����汾: V1.2.1                                              */
/*      ��������: Nov. 01, 2003     		 	                    */
/*      ����˵��������ͬһĿ¼�µ�����ͬ�����ļ�,�����д���         */
/*      �汾�޸ģ�                                                  */
/*                V1.0��Sep. 26, 2002, �ݹ�ʵ��                     */
/*                V1.1��Oct. 20, 2003, Ϊ�ӿ������ٶȣ����ٿ���     */
/*                      ������������ǵݹ黯ʵ��                    */
/*                V1.2��Oct. 21, 2003, ����Ŀ¼�Ĳ�νṹ������     */
/*                      ��ȱ����ǵݹ黯ʵ�֣���ֱ��������          */
/*                v1.2.1: Nov. 01, 2003 ��V1.2�������Ż�            */
/*      ˵    ������������Ҫ��������������������������޸Ĵ�    */
/*                ����, ʹ��ʱ�뱣�����������Ϣ                  */
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


//���캯������������
/************************************************************************
����˵��: CFileFindExt�Ĺ��캯��
����˵��: ��
���˵��: ��
��������: ��CStringArray��SetSize()��RemoveAll()
*************************************************************************/
CFileFindExt::CFileFindExt()
{
	m_FileNames.SetSize(MAX_PATH,MAX_PATH/2);
	m_FileNames.RemoveAll();
}

/************************************************************************
����˵��: CFileFindExt����������
����˵��: ��
���˵��: ��
��������: Close()����CStringArray��RemoveAll()
*************************************************************************/
CFileFindExt::~CFileFindExt()
{
	Close();
	m_FileNames.RemoveAll();	
}

//�������������ǳ�Ա������������
/************************************************************************
����˵��: �����ļ��������Ա�����Ĵ�С���ڴ��������
����˵��: nNewSizen��������Ĵ�С��GrowBy���ڴ��������
���˵��: ��
��������: ��CStringArray��SetSize()
*************************************************************************/
void CFileFindExt::SetSize(int nNewSize, int nGrowBy)
{
	m_FileNames.SetSize(nNewSize, nGrowBy);
}

/************************************************************************
����˵��: ��ȡ�ļ��������Ա�����Ĵ�С
����˵��: ��
���˵��: �����ļ��������Ա�����Ĵ�С
��������: ��CStringArray��GetSize()
*************************************************************************/
int CFileFindExt::GetSize() const
{
	return m_FileNames.GetSize();
}

/************************************************************************
����˵��: ��ȡ�����Ա�����е��ļ���
����˵��: nFileIndex: Ҫ��ȡ���ļ����������е�λ��
���˵��: �����ļ���
��������: ��CStringArray��GetAt()
*************************************************************************/
CString CFileFindExt::GetAt(int nFileIndex) const
{
	return m_FileNames.GetAt(nFileIndex);
}


//�������������Ƕ������ļ����д���ĳ�Ա����
/************************************************************************
�޸����ڣ�2003-10-21
�޸�˵����Ϊ�ӿ������ٶȣ����ٿ������ù�ȱ���ʵ�ַǵݹ黯
����˵��: Ѱ��ָ��Ŀ¼�¡�ָ����չ���������ļ�
���ø�ʽ: SearchFiles(DirName, FileExtName)
����˵��: DirName: ָ��Ŀ¼��
          FileExtName��ָ����չ����ȱʡΪ���������ļ�
���˵��: 1�������ļ������·���ļ������������ʼ����·����
             �����ڳ�Ա����m_FileNames�У�
		  2���ɹ�����true�������ڴ治��ʱ����false
��������: FindFile()��FindNextFile()��GetFilePath()
*************************************************************************/
BOOL CFileFindExt::SearchFiles(CString DirName, CString FileExtName)
{
	const int MAXQUEUELEN=MAX_PATH<<4; //ͬһ������Ŀ¼�µ������Ŀ¼��
	char* Queue[MAXQUEUELEN] = {0};
	
	CString root;
	int rootlen=DirName.GetLength();
	//ͳһ��Ŀ¼��ĩβ����'\'
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
			
			// ������Ŀ¼	
			if (IsDirectory())
			{
				//�������·���ڶ�����
				strPath = GetFilePath();
				int len=strPath.GetLength();
				strPath = strPath.Right(len-rootlen);
				len-=rootlen;
				if ((tail+1)%MAXQUEUELEN==head)
				{
					//��������
					//AfxMessageBox(_T("ͬһ����Ŀ¼̫��!"));
					//�رպ��ͷŶ��пռ�
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
				//����ǰ���ʵ��ļ�
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
