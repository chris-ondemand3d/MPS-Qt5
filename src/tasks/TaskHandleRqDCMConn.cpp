#include "TaskHandleRqDCMConn.h"


TaskHandleRqDCMConn::TaskHandleRqDCMConn(DcmNetSCP* scp, T_ASC_Network* net, 
                                         T_ASC_Association* assoc, QObject* parent): 
    Task(Task::HANDLE_INCOMMING_DCM_CONN, parent)
{
    this->m_association = assoc;
    this->m_network = net;
    this->m_scp = scp;
}


void TaskHandleRqDCMConn::run()
{
    this->m_scp->handleIncomingConnection(this->m_network, this->m_association);
}

TaskHandleRqDCMConn::~TaskHandleRqDCMConn()
{

}
