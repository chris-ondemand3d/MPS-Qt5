#include "TaskManager.h"

TaskManager::TaskManager(QObject* parent): QObject(parent)
{
}


void TaskManager::addTask()
{
    cout << "tarea adicionada" << endl;
    Task* task = (Task*)QThread::currentThread();
    this->m_threads[QThread::currentThreadId()] = task;
}

void TaskManager::removeTask()
{
    Qt::HANDLE taskID = QThread::currentThreadId();
    if (this->m_threads.contains(taskID))
    {
        Task* task = this->m_threads.take(taskID);
//         if (task->isRunning())
//             task->exit(1);
        delete task;
    }
    cout << "tarea eliminada" << endl; 
}

Status TaskManager::runTask(TaskID taskID)
{
    if (this->m_threads.contains(taskID) && 
        this->m_threads[taskID]->isRunning())
        return Status(Status::TaskError, "This task is running right now");
    
    return Status(Status::Succes, "Running task success.");
}


TaskManager::~TaskManager()
{
    this->m_threads.clear();
}