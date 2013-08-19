#include "TaskQRMoveSCP.h"


TaskQRMoveSCP::TaskQRMoveSCP(DcmNetSCP* scp, 
                             T_ASC_Association* assoc, 
                             T_ASC_PresentationContextID idPC, 
                             string& moveDestination, 
                             QObject* parent)
    : DcmTask(assoc, idPC,TaskType::C_MOVE_SCP , parent), m_moveDestination(moveDestination)
{
    
}



void TaskQRMoveSCP::run()
{
    this->m_scp->cmoveSCP(this->m_association, this->m_idPC, this->m_moveDestination);
}

TaskQRMoveSCP::~TaskQRMoveSCP()
{

}
