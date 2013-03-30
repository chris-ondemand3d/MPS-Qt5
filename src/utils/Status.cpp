#include "Status.h"

Status::Status(Status::OperationResult status, const string& message)
{
    this->m_status = status;
    this->m_description = message;
}

Status::Status(const Status& other)
{
    this->m_description = other.m_description;
    this->m_status = other.m_status;
}

bool Status::bad()
{
    return !this->good();
}

bool Status::good()
{
    return this->m_status == Succes ||
        this->m_status == ExistDicomFile;
}

string Status::message()
{
    return this->m_description;
}

Status& Status::operator=(const Status& status)
{
    this->m_status = status.m_status;
    this->m_description = status.m_description;
}

Status::OperationResult Status::status()
{
    return this->m_status;
}


Status::~Status()
{
    
}

