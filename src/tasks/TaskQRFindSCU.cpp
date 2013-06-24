#include "TaskQRFindSCU.h"

TaskQRFindSCU::TaskQRFindSCU(DcmNetSCU* scu, 
                                  DcmAET* remoteAET, 
                                  DcmQuery* query, 
                                  QObject* parent): 
                                  Task(TaskType::C_FIND_SCU, parent)
{
    this->m_scu = scu;
    this->m_remoteAET = remoteAET;
    this->m_query = query;
}

void TaskQRFindSCU::run()
{
    this->m_scu->cfind_RQ(*(this->m_remoteAET), *(this->m_query));
}

TaskQRFindSCU::~TaskQRFindSCU()
{

}
