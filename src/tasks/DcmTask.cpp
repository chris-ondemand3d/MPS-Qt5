#include "DcmTask.h"

DcmTask::DcmTask(T_ASC_Association* assoc, 
                 T_ASC_PresentationContextID idPC, 
                 Task::TaskType taskType, 
                 QObject* parent): Task(taskType, parent)
{
    this->m_association = assoc;
    this->m_idPC = idPC;
}

DcmTask::~DcmTask()
{

}
