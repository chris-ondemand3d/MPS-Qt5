#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QHash>
#include <utils/thread/Thread.h>
#include <utils/Status.h>
#include <QObject>

class ThreadManager : QObject
{
    Q_OBJECT
    
private:
    QHash<Qt::HANDLE, Thread*> m_threadList;
    
public:
    explicit ThreadManager(QObject* parent = 0);
    Status insertThread(Thread*& thread);
    virtual ~ThreadManager();

Q_SIGNALS:
    void threadRemoved(Qt::HANDLE);
    
public Q_SLOTS:
    Status removeThread(Qt::HANDLE threadID);
};

#endif // THREADMANAGER_H
