#ifndef DCMTASK_H
#define DCMTASK_H

#include <tasks/Task.h>
#include <dcmtk/dcmnet/assoc.h>


class DcmTask : public Task
{
    Q_OBJECT
protected:
    T_ASC_Association* m_association;
    T_ASC_PresentationContextID m_idPC;
    
public:
    explicit DcmTask(T_ASC_Association* assoc, 
                     T_ASC_PresentationContextID idPC, 
                     TaskType taskType, 
                     QObject* parent = 0);
    virtual ~DcmTask();
};

#endif // DCMTASK_H
