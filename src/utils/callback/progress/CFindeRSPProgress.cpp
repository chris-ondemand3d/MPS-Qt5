#include "CFindeRSPProgress.h"

CFindeRSPProgress::CFindeRSPProgress(const QObject* obj)
{
    this->m_obj = (QObject*)obj;
    this->m_queryRsp = nullptr;
}

Status CFindeRSPProgress::notifyProgress(void** params)
/*
 * Tha param structure is the following:
 * - params is an a list of arguments
 * - params[0] =  DcmDataset pointer with query result
 */
{
    if (params == nullptr)
        return Status(StatusResult::Error, "Parameters can not be nullptr.");
    ((DcmDataset*)params[0])->print(cout);
    
    return Status(StatusResult::Success, "");
}

CFindeRSPProgress::~CFindeRSPProgress()
{
    delete this->m_queryRsp;
}

