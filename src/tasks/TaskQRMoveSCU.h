#ifndef TASKQRMOVESCU_H
#define TASKQRMOVESCU_H

#include <tasks/Task.h>
#include <dicom/net/DcmNetSCU.h>
#include <tasks/TaskFactory.h>


class TaskQRMoveSCU : public Task
{
    Q_OBJECT
    
    friend class TaskFactory;
    
protected:
    DcmNetSCU* m_scu;
    DcmAET* m_findAET;
    DcmAET* m_moveAET;
    DcmQuery* m_query;
    char* m_saveFolder;
    
    explicit TaskQRMoveSCU(DcmNetSCU* scu, DcmAET* findAET, DcmAET* moveAET, 
                           DcmQuery* query, char* saveFolder, QObject* parent = 0);
    
public:    
    virtual void run();
    virtual ~TaskQRMoveSCU();
};

#endif // TASKQRMOVESCU_H
