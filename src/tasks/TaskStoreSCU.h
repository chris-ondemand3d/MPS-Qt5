#ifndef TASKSTORESCU_H
#define TASKSTORESCU_H

#include <tasks/Task.h>
#include <dicom/net/DcmNetSCU.h>

class TaskStoreSCU : public Task
{
    Q_OBJECT
    
protected:
    DcmNetSCU* m_scu;
    DcmAET* m_remoteAET;
    FileManager* m_files;
    
    explicit TaskStoreSCU(DcmNetSCU* scu, DcmAET* remoteAET, FileManager* files, QObject* parent = 0);
    
public:    
    virtual void run();
    virtual ~TaskStoreSCU();
};

#endif // TASKSTORESCU_H
