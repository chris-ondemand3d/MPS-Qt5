#include "CFindeRSPProgress.h"

CFindeRSPProgress::CFindeRSPProgress(const QObject* obj)
{
    this->m_obj = (QObject*)obj;
    this->m_queryRsp = NULL;
}

Status CFindeRSPProgress::notifyProgress(void** params)
/*
 * Tha param structure is the following:
 * - params is an a list of arguments
 * - params[0] =  DcmDataset pointer with query result
 */
{
    if (params == NULL)
        return Status(Status::Error, "Parameters can not be NULL.");
    ((DcmDataset*)params[0])->print(cout);
    
    return Status(Status::Succes, "");
}

CFindeRSPProgress::~CFindeRSPProgress()
{
    delete this->m_queryRsp;
}

