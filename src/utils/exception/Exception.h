#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <utils/Status.h>



enum class ExcepType
{
    InvalidInteger,
    CalledAETError,
    WaitingMessageError,
};

class Exception : public exception
{
protected:
    string m_message;
    ExcepType m_exceptype;
public:
    Exception(const string& message, ExcepType execpType);
    inline ExcepType execptionType() const {return this->m_exceptype;}
    inline string message() const {return this->m_message;}
    virtual ~Exception(){}
};

#endif // EXCEPTION_H
