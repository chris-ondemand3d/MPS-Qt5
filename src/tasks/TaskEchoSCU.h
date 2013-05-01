#ifndef TASKECHOSCU_H
#define TASKECHOSCU_H

#include <tasks/Task.h>
#include <dicom/net/DcmNetSCU.h>
#include <tasks/TaskFactory.h>

class TaskEchoSCU : public Task
{
    Q_OBJECT

    friend class TaskFactory;
protected:
    DcmNetSCU* m_scu;
    DcmAET* m_remoteAET;
    explicit TaskEchoSCU(DcmNetSCU* scu, DcmAET* remoteAET, QObject* parent = 0);

public:    
    virtual void run();
    virtual ~TaskEchoSCU();
};

#endif // TASKECHOSCU_H
