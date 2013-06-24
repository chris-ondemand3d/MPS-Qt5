#ifndef TASKSTORESCP_H
#define TASKSTORESCP_H

#include <tasks/DcmTask.h>
#include <dicom/net/DcmNetSCP.h>
#include <tasks/TaskFactory.h>


class DcmNetSCP;

class TaskStoreSCP : public DcmTask
{
    Q_OBJECT
    
    friend class TaskFactory;
protected:
    DcmNetSCP* m_scp;
    
    explicit TaskStoreSCP(DcmNetSCP* scp,                           
                          T_ASC_Association* assoc, 
                          T_ASC_PresentationContextID idPC,
                          QObject* parent = 0);
    
public:    
    virtual void run();
    virtual ~TaskStoreSCP();
};
#endif // TASKSTORESCP_H
