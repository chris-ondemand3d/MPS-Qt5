#include "Exception.h"

Exception::Exception(const string& message, Exception::ExcepType execpType):
m_message(message), m_exceptype(execpType)
{
}

string Exception::message()
{
    return this->m_message;
}

Exception::ExcepType Exception::execptionType()
{
    return this->m_exceptype;
}