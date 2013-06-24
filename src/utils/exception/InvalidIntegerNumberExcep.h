#ifndef INVALIDINTEGERNUMBEREXCEP_H
#define INVALIDINTEGERNUMBEREXCEP_H

#include <utils/exception/Exception.h>


class InvalidIntegerNumberExcep : public Exception
{
private:
    string m_strValue;
    
public:
    InvalidIntegerNumberExcep(const string& message, 
                              ExcepType execpType, 
                              const string& strValue);
    inline string strValue() const {return this->m_strValue;}
};

#endif // INVALIDINTEGERNUMBEREXCEP_H
