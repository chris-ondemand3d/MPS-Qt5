#include "TaskStoreSCP.h"


TaskStoreSCP::TaskStoreSCP(DcmNetSCP* scp,                            
                           T_ASC_Association* assoc, 
                           T_ASC_PresentationContextID idPC, 
                           QObject* parent): 
    DcmTask(assoc, idPC, Task::C_STORE_SCP, parent)
{
    this->m_scp = scp;
}


void TaskStoreSCP::run()
{
    this->m_scp->cstoreSCP(this->m_association, this->m_idPC);
}

TaskStoreSCP::~TaskStoreSCP()
{

}
