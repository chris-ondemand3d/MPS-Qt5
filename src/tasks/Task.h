#ifndef TASK_H
#define TASK_H

#include <utils/Status.h>
#include <QThread>
#include <QDateTime>

#ifndef TASKMANAGER_H
#include <tasks/TaskManager.h>
#endif



typedef Qt::HANDLE TaskID;

enum class TaskType{
    DICOM_SCP,
    C_ECHO_SCP,
    C_ECHO_SCU,
    C_STORE_SCP,
    C_STORE_SCU,
    C_FIND_SCU,
    C_FIND_SCP,
    C_MOVE_SCU,
    C_MOVE_SCP,
    QUERY_RETRIEVE_SCP,
    QUERY_RETRIEVE_SCU,
    HANDLE_INCOMMING_DCM_CONN,
    UNKNOW,
};

class Task : public QThread
{
    Q_OBJECT
protected:
    TaskType m_type;
    explicit Task(TaskType taskType = TaskType::UNKNOW, QObject* parent = 0);
public:
    virtual ~Task();
};

#endif // TASK_H
