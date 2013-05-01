#ifndef TASKECHOSCP_H
#define TASKECHOSCP_H

#include <tasks/DcmTask.h>
#include <tasks/TaskFactory.h>
#include <dicom/net/DcmNetSCP.h>

class DcmNetSCP;

class TaskEchoSCP : public DcmTask
{
    Q_OBJECT
    friend class TaskFactory;
protected:
    DcmNetSCP* m_scp;
    
    explicit TaskEchoSCP(DcmNetSCP* scp, 
                         T_ASC_Association* assoc, 
                         T_ASC_PresentationContextID idPC, 
                         QObject* parent = 0);

public:    
    virtual void run();
    virtual ~TaskEchoSCP();
};

#endif // TASKECHOSCP_H
