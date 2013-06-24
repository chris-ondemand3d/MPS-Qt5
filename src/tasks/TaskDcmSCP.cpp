#include "TaskDcmSCP.h"

TaskDcmSCP::TaskDcmSCP(DcmNetSCP* scp, QObject* parent): 
    Task(TaskType::DICOM_SCP, parent)
{
    this->m_scp = scp;
}

void TaskDcmSCP::run()
{
    this->m_scp->start();
}

TaskDcmSCP::~TaskDcmSCP()
{

}

