#ifndef MPSSYSTEM_H
#define MPSSYSTEM_H

#include <QObject>
#include <tasks/TaskManager.h>

class MPSSystem : public QObject
{
    Q_OBJECT
private:
    TaskManager* m_taskManager;
    
public:
    explicit MPSSystem(QObject* parent = 0);
    inline TaskManager* taskManager(){return this->m_taskManager;}
    void emitAssReceived();
    
    virtual ~MPSSystem();
    
Q_SIGNALS:
    void associationReceived();
};

#endif // SYSTEM_H
