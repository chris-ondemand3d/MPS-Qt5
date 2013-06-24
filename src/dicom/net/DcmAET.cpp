#include "DcmAET.h"
#include <QVariant>

DcmAET::DcmAET()
{
    this->m_port = 0;
}


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

string DcmAET::toString() const
{
    return this->m_aet + "@" + this->m_hostname + ":" + QVariant(this->m_port).toString().toStdString();
}


bool DcmAET::validateAetSettingValue(char* aetSettingVal, 
                                     string& aetVal, 
                                     string& hostVal, 
                                     int& portVal)
{
    int length = strlen(aetSettingVal);
    bool atSign = false, twoPoints = false, badResult = false;
    string port;
    
    for (int i = 0; i < length && !badResult; i++)
    {
        switch(aetSettingVal[i])
        {
            case ':':
            {
                if (!twoPoints && atSign)
                    twoPoints = true;
                else
                    badResult = true;
                break;
            }
            case '@':
            {
                if (!atSign)
                    atSign = true;
                else
                    badResult = false;
                
                break;
            }
            
            case ' ':
            {
                badResult = true;
                break;
            }
            default:
            {
                if (!atSign)
                    aetVal += aetSettingVal[i];
                else if (!twoPoints)
                    hostVal += aetSettingVal[i];
                else
                    port += aetSettingVal[i];
            }
        }
    }
    bool convResult;
    if (aetVal.empty() || hostVal.empty() || 
        ((portVal = QVariant(port.c_str()).toInt(&convResult)) == 0 && !convResult))
        badResult = true;
   
    if (badResult)
    {
        aetVal.clear();
        hostVal.clear();
        portVal = -1;
        return false;
    }
    
    return true;
}


void DcmAET::print() const
{
    
    cout << "AET: " << this->m_aet.c_str()<< endl
         << "Hostname: " << this->m_hostname.c_str()<< endl
         << "Port: " << this->m_port << endl;
}

