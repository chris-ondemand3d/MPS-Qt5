#include "WaitingMessageErrorExcep.h"

WaitingMessageErrorExcep::WaitingMessageErrorExcep(const string& message): 
                                                        Exception(message, ExcepType::WaitingMessageError)
{
    
}

WaitingMessageErrorExcep::~WaitingMessageErrorExcep()
{

}
