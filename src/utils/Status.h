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
        CMoveRQError,
        CMoveRSPError,
        TaskError,
    } OperationResult;
private:
    OperationResult m_status;
    string m_description;
public:
    Status(OperationResult status = Succes, const string& message = "Succes");
    Status(const Status& other);
    bool good();
    bool bad();
    OperationResult status();
    string message();
    inline void setStatus (OperationResult status, const string& message)
    {
        this->m_status = status;
        this->m_description = message;
    }
    Status& operator=(const Status& status);
    virtual ~Status();
};

#endif // STATUS_H
