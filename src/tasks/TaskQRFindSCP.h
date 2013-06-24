#ifndef TASKQRFINDSCP_H
#define TASKQRFINDSCP_H

#include <tasks/DcmTask.h>
#include <dicom/net/DcmNetSCP.h>
#include <tasks/TaskFactory.h>


class TaskQRFindSCP : public DcmTask
{
    Q_OBJECT
    
    friend class TaskFactory;
protected:
    DcmNetSCP* m_scp;
    
    explicit TaskQRFindSCP(DcmNetSCP* scp,                            
                           T_ASC_Association* assoc, 
                           T_ASC_PresentationContextID idPC,
                           QObject* parent = 0);
    
public:
    virtual void run();
    virtual ~TaskQRFindSCP();
};

#endif // TASKQRFINDSCP_H
