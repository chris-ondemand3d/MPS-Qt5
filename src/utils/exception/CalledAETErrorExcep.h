#ifndef CALLEDAETERROREXCEP_H
#define CALLEDAETERROREXCEP_H

#include <utils/exception/Exception.h>


class CalledAETErrorExcep : public Exception
{
public:
    CalledAETErrorExcep(const string& message);
    virtual ~CalledAETErrorExcep();
};

#endif // CALLEDAETERROREXCEP_H
