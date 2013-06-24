#include "TaskEchoSCU.h"
TaskEchoSCU::TaskEchoSCU(DcmNetSCU* scu, 
                              DcmAET* remoteAET, 
                              QObject* parent): 
                              Task(TaskType::C_ECHO_SCU, parent)
{
    this->m_remoteAET = remoteAET;
    this->m_scu = scu;
}

void TaskEchoSCU::run()
{
    this->m_scu->cecho_RQ(*(this->m_remoteAET));
}

TaskEchoSCU::~TaskEchoSCU()
{
    
}
