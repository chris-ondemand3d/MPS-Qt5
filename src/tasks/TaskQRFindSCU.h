#ifndef TASKQRFINDSCU_H
#define TASKQRFINDSCU_H

#include <tasks/Task.h>
#include <dicom/net/DcmNetSCU.h>
#include <tasks/TaskFactory.h>



class TaskQRFindSCU : public Task
{
    Q_OBJECT
    
    friend class TaskFactory;
protected:
    DcmNetSCU* m_scu;
    DcmAET* m_remoteAET;
    DcmQuery* m_query;
    
    explicit TaskQRFindSCU(DcmNetSCU* scu, 
                           DcmAET* remoteAET, 
                           DcmQuery* query, 
                           QObject* parent = nullptr);
    
public:    
    virtual void run();
    virtual ~TaskQRFindSCU();;
};

#endif // TASKQRFINDSCU_H
