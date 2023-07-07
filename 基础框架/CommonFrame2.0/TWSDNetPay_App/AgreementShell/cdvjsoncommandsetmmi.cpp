#include "cdvjsoncommandsetmmi.h"

CDVJsonCommandSetMMI::CDVJsonCommandSetMMI()
{

}



bool CDVJsonCommandSetMMI::fnAnalysisStringForData(const QString &strResponseResult)
{

    bool bReturn;


    return bReturn;

}



QString	CDVJsonCommandSetMMI::FormatParamData2Json()
{

     QString strReturn;


     return strReturn;

}









/****************************************
 *函数名称：strSetClearScreen
 *函数描述：清屏指令
 *函数参数：char
 *函数返回：QString
*****************************************/
 QString  CDVJsonCommandSetMMI::strSetClearScreen()
 {
     QString strReturn;
     try
     {
         // 表示整个 json 对象
         Json::Value root;
         // 标识交互参数
         Json::Value MMIParamContext;
         // 标识详细数据项
         Json::Value Data;
         // 标识设置数据项区域
         Json::Value ItemData;
         // 标识显示文字区域
         Json::Value ViewText;
         // 标识显示声音区域
         Json::Value ViewVoice;
         // 标识显示图形区域
         Json::Value ViewGraph;

         Json::Value item;

         Json::FastWriter t_RootDocument;

         //标识版本
         MMIParamContext["Version"]=std::string(m_Version.toLocal8Bit());

         m_ClearMMI = 1;
         Data["ClearMMI"]=m_ClearMMI;

         Data["ItemCount"]=std::string(QString::number(int(m_Text.size() + m_Voice.size() + m_Graph.size())).toLocal8Bit());

         ItemData["Text"]=ViewText;
         ItemData["Voice"]=ViewVoice;
         ItemData["Graph"]=ViewGraph;

         Data["ItemData"]=ItemData;
         MMIParamContext["Data"]=Data;
         root["MMIParamContext"]= MMIParamContext;

         //转化为字符串
         strReturn = QString::fromStdString(t_RootDocument.write(root));
     }
     catch(int& iErrorCode)
     {
         switch(iErrorCode)
         {
         case -1:
             break;
         }
     }
     return strReturn;

 }



 /****************************************
  *函数名称：byteSetClearScreen
  *函数描述：写命令
  *函数参数：char
  *函数返回：QString
 *****************************************/
 QByteArray CDVJsonCommandSetMMI::byteSetClearScreen()
 {



 }
