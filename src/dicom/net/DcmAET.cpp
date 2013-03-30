#include "DcmAET.h"
#include <QVariant>

DcmAET::DcmAET(const string& aet, const string& hostname, int port)
{
    this->m_aet = aet;
    this->m_hostname = hostname;
    this->m_port = port;
}


DcmAET::DcmAET(const DcmAET& aet)
{
    this->m_aet = aet.m_aet;
    this->m_hostname = aet.m_hostname;
    this->m_port = aet.m_port;
}

void DcmAET::print()
{
    
    cout << "AET: " << this->m_aet.c_str()<< endl
         << "Hostname: " << this->m_hostname.c_str()<< endl
         << "Port: " << this->m_port << endl;
}

