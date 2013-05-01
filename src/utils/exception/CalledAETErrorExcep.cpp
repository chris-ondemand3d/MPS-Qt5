#include "CalledAETErrorExcep.h"

CalledAETErrorExcep::CalledAETErrorExcep(const string& message): 
    Exception(message, CalledAETError)
{
    this->m_message = message;
}

CalledAETErrorExcep::~CalledAETErrorExcep()
{

}
