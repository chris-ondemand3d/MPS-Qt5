#include "Callback.h"

template <class ClassType>
Callback<ClassType>::Callback(ClassType* instance, MethodPointer method)
{
    this->m_instance = instance;
    this->m_method = method;
}

template <class ClassType>
Status Callback<ClassType>::exec(void** params)
{
    
}


template <class ClassType>
Callback<ClassType>& Callback<ClassType>::operator=(const Callback<ClassType>& callback)
{
    this->m_instance = callback.m_instance;
    this->m_method = callback.m_method;
}

template <class ClassType>
Callback<ClassType>::~Callback()
{
    
}
