#include <string>
#include <string.h>
#include <utils/exception/InvalidIntegerNumberExcep.h>

using namespace std;


inline string int2str(long int number)
{
    string strNumber;
    long int numberCpy = number;
    
    while(numberCpy != 0)
    {
        strNumber.insert(0, 1, (char)(numberCpy % 10 + 48));
        numberCpy /= 10;
    }
    
    if (number < 0)
        strNumber.insert(0, 1, '-');
    
    return strNumber;
}