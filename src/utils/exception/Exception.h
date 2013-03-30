#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <utils/Status.h>


class Exception
{
public:
    typedef enum {
        InvalidInteger,
    } ExcepType;
protected:
    string m_message;
    ExcepType m_exceptype;
public:
    Exception(const string& message, ExcepType execpType);
    ExcepType execptionType();
    string message();
    virtual ~Exception(){}
};

#endif // EXCEPTION_H
