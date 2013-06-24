#include "TaskQRFindSCP.h"


TaskQRFindSCP::TaskQRFindSCP(DcmNetSCP* scp,                               
                             T_ASC_Association* assoc, 
                             T_ASC_PresentationContextID idPC,
                             QObject* parent): 
                             DcmTask(assoc, idPC, TaskType::C_FIND_SCP, parent)
{
    this->m_scp = scp;
}



void TaskQRFindSCP::run()
{
    this->m_scp->cfindSCP(this->m_association, this->m_idPC);
}

TaskQRFindSCP::~TaskQRFindSCP()
{

}

