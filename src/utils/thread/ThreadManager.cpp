#include "ThreadManager.h"

ThreadManager::ThreadManager(QObject* parent): QObject(parent)
{
    
}


Status ThreadManager::insertThread(Thread*& thread)
{
    this->m_threadList[thread->currentThreadId()] = thread;
    this->connect(thread, SIGNAL(removeFromThreadList(int)), this, SLOT(removeThread(int)));
}


Status ThreadManager::removeThread(Qt::HANDLE threadID)
{
    if (this->m_threadList.contains(threadID))
    {
        Q_EMIT this->threadRemoved(this->m_threadList[threadID]->currentThreadId());
        this->m_threadList.remove(threadID);
        
        return Status(Status::Succes, "Thread elimination success.");
    }
    
    return Status(Status::ThreadError, "This Thread ID dosen't exist.");
}


ThreadManager::~ThreadManager()
{    
    for (QHash<Qt::HANDLE, Thread*>::iterator itBegin = this->m_threadList.begin(), 
        itEnd = this->m_threadList.end(); itBegin != itEnd; itBegin++)
        (*itBegin)->quit();
    
    this->m_threadList.clear();
}
