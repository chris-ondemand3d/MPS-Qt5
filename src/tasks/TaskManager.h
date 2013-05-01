#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <tasks/Task.h>
#include <QHash>
#include <utils/Status.h>
#include <tasks/Task.h>
#include <iostream>

using namespace std;

class TaskManager : public QObject
{
    Q_OBJECT
    
private:
    QHash< Qt::HANDLE, Task* > m_threads;
    
public:
    explicit TaskManager(QObject* parent = 0);
    Status runTask(TaskID taskID);
    virtual ~TaskManager();
    
public Q_SLOTS:
    void addTask();
    void removeTask();
};

#endif // TASKMANAGER_H
