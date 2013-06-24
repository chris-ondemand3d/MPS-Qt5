#ifndef TASKHANDLERQDCMCONN_H
#define TASKHANDLERQDCMCONN_H
#include <tasks/Task.h>
#include <dicom/net/DcmNetSCP.h>
#include <tasks/TaskFactory.h>


class DcmNetSCP;

class TaskHandleRqDCMConn : public Task
{
    Q_OBJECT
    
    friend class TaskFactory;
protected:
    DcmNetSCP* m_scp;
    T_ASC_Network* m_network;
    T_ASC_Association* m_association;
    
    explicit TaskHandleRqDCMConn(DcmNetSCP* scp, T_ASC_Network* net, 
                                 T_ASC_Association* assoc, QObject* parent = 0);
    
public:
    virtual void run();
    virtual ~TaskHandleRqDCMConn();
};

#endif // TASKHANDLERQDCMCONN_H
