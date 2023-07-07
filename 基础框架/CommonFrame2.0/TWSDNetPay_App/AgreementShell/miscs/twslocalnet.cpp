#include "twslocalnet.h"
#include "httwsddevice.h"
#include <stdlib.h>
#include <QDebug>

TWSLocalnet::TWSLocalnet()
{
    m_localDefaultGw.clear();
    m_localGw0.clear();
    m_localGw1.clear();
    m_localIp0.clear();
    m_localIp1.clear();
    m_localNetmask0.clear();
    m_localNetmask1.clear();
}

void TWSLocalnet::setLocalIP0(string str)
{
    m_localIp0 = str;
}

void TWSLocalnet::setLocalIP1(string str)
{
    m_localIp1 = str;
}

void TWSLocalnet::setLocalNetmask0(string str)
{
    m_localNetmask0 = str;
}

void TWSLocalnet::setLocalNetmask1(string str)
{
    m_localNetmask1 = str;
}

void TWSLocalnet::setLocalGW0(string str)
{
    m_localGw0 = str;
}
void TWSLocalnet::setLocalGW1(string str)
{
    m_localGw1 = str;
}

void TWSLocalnet::setLocalDefaultGW(string str)
{
    m_localDefaultGw = str;
}

string TWSLocalnet::fill(string data)
{
    string str = data;
    if (!str.empty()) {
        str.insert(0, 1, 0x0);
        str.resize(16, 0x0);
    }

    return str;
}

void TWSLocalnet::execCommand(string cmd)
{
    qDebug() << "execCommand: " << QString::fromStdString(cmd);
    system(cmd.c_str());
}

void TWSLocalnet::writeDevices()
{
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

#define     WRITE_ETH0IPADDR                        207
#define     WRITE_ETH0NETMASK                       209
#define     WRITE_ETH0GATEWAY                       210

#define     WRITE_ETH1IPADDR                        208
#define     WRITE_ETH1NETMASK                       211
#define     WRITE_ETH1GATEWAY                       212

#define     WRITE_DEFAULTGATEWAY                    213

    // eth0
    string ip0 = fill(m_localIp0);
    if (!m_localIp0.empty())
        IF_WriteDevice(WRITE_ETH0IPADDR, ip0.length(), (char*)ip0.data(), iLen, pBuf);

    string netmask0 = fill(m_localNetmask0);
    if (!m_localNetmask0.empty())
        IF_WriteDevice(WRITE_ETH0NETMASK, netmask0.length(), (char*)netmask0.data(), iLen, pBuf);

    string gateway0 = fill(m_localGw0);
    if (!m_localGw0.empty())
        IF_WriteDevice(WRITE_ETH0GATEWAY, gateway0.length(), (char*)gateway0.data(), iLen, pBuf);

    // eth1
    string ip1 = fill(m_localIp1);
    if (!m_localIp1.empty())
        IF_WriteDevice(WRITE_ETH1IPADDR, ip1.length(), (char*)ip1.data(), iLen, pBuf);

    string netmask1 = fill(m_localNetmask1);
    if (!m_localNetmask1.empty())
        IF_WriteDevice(WRITE_ETH1NETMASK, netmask1.length(), (char*)netmask1.data(), iLen, pBuf);

    string gateway1 = fill(m_localGw1);
    if (!m_localGw1.empty())
        IF_WriteDevice(WRITE_ETH1GATEWAY, gateway1.length(), (char*)gateway1.data(), iLen, pBuf);

//    // default gs
//    if (!m_localDefaultGw.empty())
//        IF_WriteDevice(WRITE_DEFAULTGATEWAY, m_localDefaultGw.length(), (char*)m_localDefaultGw.data(), iLen, NULL);
}

bool TWSLocalnet::updateNetworksParam()
{
    qDebug() << "TWSLocalnet::updateNetworksParam begin";

    string ipcmd0 = "ifconfig eth0 ";
    string ipcmd1 = "ifconfig eth1 ";
    string routecmd = "route add default gw ";

    ipcmd0.append(m_localIp0).append(" netmask ").append(m_localNetmask0);
    ipcmd1.append(m_localIp1).append(" netmask ").append(m_localNetmask1);

    //
    string routecmd0;
    string routecmd1;
    string routecmdDefault;

    routecmd0.append(routecmd).append(m_localGw0).append(" dev eth0");
    routecmd1.append(routecmd).append(m_localGw1).append(" dev eth1");
    routecmdDefault.append(routecmd).append(m_localGw0);

    // set ip/gw for eth0
    if (!m_localIp0.empty() && !m_localNetmask0.empty())
        execCommand(ipcmd0);

    if (!m_localGw0.empty())
        execCommand(routecmd0);

    // set ip/gw for eth1
    if (!m_localIp1.empty() && !m_localNetmask1.empty())
        execCommand(ipcmd1);

//    if (!m_localGw1.empty())
//        execCommand(routecmd1);

//    // set default gw
//    if (!m_localGw1.empty())
//        execCommand(routecmdDefault);

    qDebug() << "TWSLocalnet::updateNetworksParam end";

    writeDevices();
    qDebug() << "TWSLocalnet::updateNetworksParam write end";

    return true;
}
