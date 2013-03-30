#ifndef DCMAET_H
#define DCMAET_H

#include <iostream>
#include <string.h>

using namespace std;


class DcmAET
{

private:
    string m_aet;
    string m_hostname;
    int m_port;

public:
    DcmAET(const string& aet, const string& hostname, int port);
    DcmAET(const DcmAET& aet);
    inline void setAet(const string& aet){this->m_aet = aet;}
    inline void setHostname(const string& hostname) {this->m_hostname = hostname;}
    void setPort(int port) {this->m_port = port;}
    string aet() {return this->m_aet;}
    string hostname() {return this->m_hostname;}
    int port() {return this->m_port;}
    void print();
    virtual ~DcmAET(){};
};

#endif // DCMAET_H
