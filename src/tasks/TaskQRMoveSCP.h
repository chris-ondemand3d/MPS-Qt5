#ifndef TASKQRMOVESCP_H
#define TASKQRMOVESCP_H

#include <tasks/DcmTask.h>
#include <dicom/net/DcmNetSCP.h>
#include <tasks/TaskFactory.h>


class TaskQRMoveSCP : public DcmTask
{
    Q_OBJECT
    
    friend class TaskFactory;
protected:
    DcmNetSCP* m_scp;
    T_ASC_Network* m_network;
    T_ASC_Association* m_association;
    
    explicit TaskQRMoveSCP(DcmNetSCP* scp,  
                           T_ASC_Association* assoc, 
                           T_ASC_PresentationContextID idPC,
                           QObject* parent = 0);
public:
    virtual void run();
    virtual ~TaskQRMoveSCP();
};

#endif // TASKQRMOVESCP_H
