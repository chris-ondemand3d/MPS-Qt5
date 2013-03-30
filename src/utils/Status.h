#ifndef STATUS_H
#define STATUS_H

#include <string>
using namespace std;

class Status
{
public:
    typedef enum {
        Succes,
        Error,
        ExistDicomFile,
        NotExistDicomFile,
        BadPresentationContext,
        NetworkIsNotReady,
        AssociationError,
        CStoreRQError,
        CStoreRSPError,
        CFindRQError,
        CFindRSPError,
        ThreadError,
    } OperationResult;
private:
    OperationResult m_status;
    string m_description;
public:
    Status(OperationResult status, const string& message);
    Status(const Status& other);
    bool good();
    bool bad();
    OperationResult status();
    string message();
    Status& operator=(const Status& status);
    virtual ~Status();
};

#endif // STATUS_H
