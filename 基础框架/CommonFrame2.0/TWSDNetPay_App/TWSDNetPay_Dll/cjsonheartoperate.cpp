#include "cjsonheartoperate.h"

CJsonHeartOperate::CJsonHeartOperate(void)
{

}

CJsonHeartOperate::~CJsonHeartOperate(void)
{
}



QString CJsonHeartOperate::FormatSendData2Json( const CDVJsonHeart* pDVJsonHeart)
{
    QString strReturn;
    return strReturn;
}


CDVJsonBase* CJsonHeartOperate::FormatResponseResult2Class( const QString& strResponseResult, const int& iDataType)
{
    CDVJsonHeart* pDVJsonHeart = new CDVJsonHeart();
    try
    {
        if(NULL == pDVJsonHeart)
            throw -1;

    //  调试填充    陈老大
    //  pDVJsonHeart->ServerResponseCode		= HTTP_RC_DATA_VALID;
    //	pDVJsonHeart->ServerReceiveDataTime		= GetGlobalApp()->GetCurrentTimeString(1);
    //	pDVJsonHeart->ServerResponseDataTime	= GetGlobalApp()->GetCurrentTimeString(1);
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1://申请资源失败
            break;
        case -2://数据解析失败
            break;
        }
    }
    //返回解析结果
    return pDVJsonHeart;
}


