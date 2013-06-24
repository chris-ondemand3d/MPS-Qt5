#ifndef TASKMANAGER_H
#define TASKMANAGER_H


#include <QHash>
#include <utils/Status.h>
#include <iostream>
#include <tasks/Task.h>

using namespace std;
class Task;
typedef Qt::HANDLE TaskID;

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
