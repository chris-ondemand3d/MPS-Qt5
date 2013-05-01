#include "TaskEchoSCP.h"

TaskEchoSCP::TaskEchoSCP(DcmNetSCP* scp, 
                         T_ASC_Association* assoc, 
                         T_ASC_PresentationContextID idPC, 
                         QObject* parent): 
    DcmTask(assoc, idPC, C_ECHO_SCP, parent)
{
    this->m_scp = scp;
}


void TaskEchoSCP::run()
{
    this->m_scp->cechoSCP(this->m_association, this->m_idPC);
}


TaskEchoSCP::~TaskEchoSCP()
{

}
