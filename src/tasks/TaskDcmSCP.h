#ifndef TASKDCMSCP_H
#define TASKDCMSCP_H

#include <tasks/Task.h>
#include <dicom/net/DcmNetSCP.h>


class TaskDcmSCP : public Task
{
    Q_OBJECT
    
    friend class TaskFactory;
    
protected:
    DcmNetSCP* m_scp;
    explicit TaskDcmSCP(DcmNetSCP* scp, 
                        QObject* parent = 0);
    
public:    
    virtual void run();
    virtual ~TaskDcmSCP();
};

#endif // TASKDCMSCP_H
