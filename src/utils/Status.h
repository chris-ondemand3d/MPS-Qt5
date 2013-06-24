#ifndef STATUS_H
#define STATUS_H

#include <string>
using namespace std;


enum class StatusResult{
    Success,
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
    MongoDBError,
} ;

class Status
{
public:
    
private:
    StatusResult m_status;
    string m_description;
public:
    Status(StatusResult status = StatusResult::Success, const string& message = "Succes");
    Status(const Status& other);
    bool good();
    bool bad();
    inline StatusResult status(){return  this->m_status;}
    inline string message(){return this->m_description;}
    inline void setStatus (StatusResult status, const string& message)
    {
        this->m_status = status;
        this->m_description = message;
    }
    Status& operator=(const Status& status);
    Status& operator=(const Status&& status);
    virtual ~Status();
};

#endif // STATUS_H
