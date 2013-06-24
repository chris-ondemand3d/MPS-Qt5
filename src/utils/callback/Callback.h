#ifndef CALLBACK_H
#define CALLBACK_H

#include <utils/Status.h>


template <typename ClassType>
class Callback
{
private:
    typedef Status (Callback<ClassType>::*MethodPointer)(void** params);
    MethodPointer m_method;
    ClassType* m_instance;
    
public:
    Callback(ClassType* instance, MethodPointer method);
    Callback<ClassType>& operator=(const Callback<ClassType>& callback);
    Status exec(void** params);
    ~Callback();
};

#endif // CALLBACK_H
