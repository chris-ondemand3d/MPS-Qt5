#include "TaskStoreSCU.h"

TaskStoreSCU::TaskStoreSCU(DcmNetSCU* scu, DcmAET* remoteAET, FileManager* files, QObject* parent): 
    Task(TaskType::C_STORE_SCU, parent)
{
    this->m_files = files;
    this->m_remoteAET = remoteAET;
    this->m_scu = scu;
}

void TaskStoreSCU::run()
{
    this->m_scu->cstore_RQ(*(this->m_remoteAET), *(this->m_files));
}


TaskStoreSCU::~TaskStoreSCU()
{

}

