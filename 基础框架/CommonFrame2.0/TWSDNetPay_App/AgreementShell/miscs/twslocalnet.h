#ifndef TWSLOCALNET_H
#define TWSLOCALNET_H

#include <string>
#include <iostream>

using namespace std;

class TWSLocalnet
{
public:
    TWSLocalnet();

public:
    void setLocalIP0(string ip);
    void setLocalIP1(string ip);
    void setLocalNetmask0(string mask);
    void setLocalNetmask1(string mask);
    void setLocalGW0(string gw);
    void setLocalGW1(string gw);
    void setLocalDefaultGW(string gw);

    // update network params
    bool updateNetworksParam();

private:
    void execCommand(string cmd);
    void writeDevices();
    string fill(string str);

private:
    string m_localIp0;
    string m_localNetmask0;
    string m_localGw0;

    string m_localIp1;
    string m_localNetmask1;
    string m_localGw1;

    string m_localDefaultGw;
};

#endif // TWSLOCALNET_H
