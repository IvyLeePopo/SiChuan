#include "cdvjsoncommanddebit.h"

CDVJsonCommandDebit::CDVJsonCommandDebit(QObject *parent) : QObject(parent)
{

}



/*********************************************
 * 函数名称：GetVerifyCode
 * 描述   ：CRC校验
 * 参数   : t_lpData 数据  t_wLength 长度
 * 返回值 : void
 * *******************************************/
ushort CDVJsonCommandDebit::GetVerifyCode(QByteArray t_lpData, int t_wLength)
{
    uchar   CRC16Hi = 0xFF;
    uchar   CRC16Lo = 0xFF;
    uchar   CH = 0xA0;
    uchar   CL = 0x01;
    uchar   SaveHi = 0;
    uchar   SaveLo = 0;
    uchar   Flag;
    ushort  CRC_REC=0;

    for(int i = 1;i < t_wLength;i++)
    {
        CRC16Lo = CRC16Lo ^ t_lpData[i];
        for(Flag = 0; Flag < 8; Flag++)
        {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;
            CRC16Hi = CRC16Hi >> 1;
            CRC16Lo = CRC16Lo >> 1;
            if((SaveHi & 1) == 1)
                CRC16Lo |= 0x80;
            if((SaveLo & 1) == 1)
            {
                CRC16Hi = CRC16Hi ^ CH;
                CRC16Lo = CRC16Lo ^ CL;
            }
        }
    }
    CRC_REC = (CRC16Hi<<8);
    CRC_REC |= CRC16Lo;

    return CRC_REC;
}
