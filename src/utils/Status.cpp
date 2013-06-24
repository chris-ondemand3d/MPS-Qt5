#include "Status.h"

Status::Status(StatusResult status, const string& message)
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
    if (this->m_status == StatusResult::Success ||
        this->m_status == StatusResult::ExistDicomFile)
        return true;
    
    return false;
}

Status& Status::operator=(const Status& status)
{
    this->m_status = status.m_status;
    this->m_description = status.m_description;
    return *this;
}
Status& Status::operator=(const Status&& status)
{
    this->m_description = std::move(status.m_description);
    this->m_status = std::move(status.m_status);
    return *this;
}



Status::~Status()
{
    
}

