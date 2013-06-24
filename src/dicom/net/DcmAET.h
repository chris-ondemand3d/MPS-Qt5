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
    DcmAET();
    inline void setAet(const string& aet){this->m_aet = aet;}
    inline void setHostname(const string& hostname) {this->m_hostname = hostname;}
    void setPort(int port) {this->m_port = port;}
    string aet() const {return this->m_aet;}
    string hostname() const {return this->m_hostname;}
    int port() const {return this->m_port;}
    void print() const;
    static bool validateAetSettingValue(char* aetSettingVal, 
                                        string& aetVal, 
                                        string& hostVal,
                                        int& portVal);
    string toString() const;
    virtual ~DcmAET(){};
};
#endif // DCMAET_H
