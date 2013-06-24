#include "Exception.h"

Exception::Exception(const string& message, ExcepType execpType):
m_message(message), m_exceptype(execpType)
{
}
