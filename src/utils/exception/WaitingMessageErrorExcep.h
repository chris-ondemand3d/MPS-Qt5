#ifndef WAITINGMESSAGEERROREXCEP_H
#define WAITINGMESSAGEERROREXCEP_H

#include <utils/exception/Exception.h>

class WaitingMessageErrorExcep : public Exception
{
public:
    WaitingMessageErrorExcep(const string& message);
    virtual ~WaitingMessageErrorExcep();
};

#endif // WAITINGMESSAGEERROREXCEP_H
