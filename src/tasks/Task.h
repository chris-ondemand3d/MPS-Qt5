#ifndef TASK_H
#define TASK_H

#include <utils/Status.h>
#include <QThread>
#include <QDateTime>


typedef Qt::HANDLE TaskID;

class Task : public QThread
{
    Q_OBJECT
public:
    typedef enum {
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
    } TaskType;
    
protected:
    TaskType m_type;
    explicit Task(TaskType taskType = UNKNOW, QObject* parent = 0);
public:
    virtual ~Task();
};

#endif // TASK_H
