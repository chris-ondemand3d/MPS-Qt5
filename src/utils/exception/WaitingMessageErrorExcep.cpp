#include "WaitingMessageErrorExcep.h"

WaitingMessageErrorExcep::WaitingMessageErrorExcep(const string& message): 
    Exception(message, Exception::WaitingMessageError)
{
    
}

WaitingMessageErrorExcep::~WaitingMessageErrorExcep()
{

}
