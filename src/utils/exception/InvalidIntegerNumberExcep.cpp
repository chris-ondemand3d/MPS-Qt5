#include "InvalidIntegerNumberExcep.h"

InvalidIntegerNumberExcep::InvalidIntegerNumberExcep(const string& message, 
                                                          ExcepType execpType, 
                                                          const string& strValue):
                                                          Exception(message, execpType), m_strValue(strValue)
{

}

