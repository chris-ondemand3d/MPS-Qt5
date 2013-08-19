#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <utils/Status.h>
#include <cerrno>
#include <errno.h>
#include <exception>
// for classes exception and bad_exception
#include <stdexcept>
// for most logic and runtime error classes
#include <system_error> // for system errors (since C++11)
#include <new>
// for out-of-memory exceptions
#include <ios>
// for I/O exceptions
#include <future>
// for errors with async() and futures (since C++11)
#include <typeinfo>
// for bad_cast and bad_typeid




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
    explicit Exception(const string& message, ExcepType execpType);
    inline ExcepType execptionType() const {return this->m_exceptype;}
    inline string message() const {return this->m_message;}
    virtual ~Exception(){}
};

#endif // EXCEPTION_H
