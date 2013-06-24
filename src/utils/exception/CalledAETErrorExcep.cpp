#include "CalledAETErrorExcep.h"

CalledAETErrorExcep::CalledAETErrorExcep(const string& message): 
                                              Exception(message, ExcepType::CalledAETError)
{
    this->m_message = message;
}

CalledAETErrorExcep::~CalledAETErrorExcep()
{

}
