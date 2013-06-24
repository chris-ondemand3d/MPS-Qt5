#ifndef MPSSYSTEM_H
#define MPSSYSTEM_H

#include <QObject>
#include <tasks/TaskManager.h>
#include <db/DBManager.h>
#include <settings/MPSSystemSettings.h>
#include <dcmtk/dcmdata/dcdatset.h>


class MPSSystem : public QObject
{
    Q_OBJECT
private:
    TaskManager* m_taskManager;
    DBManager* m_dbManager;
    MPSSystemSettings* m_settings;
    
public:
    explicit MPSSystem(QObject* parent = 0);
    inline TaskManager* taskManager(){return this->m_taskManager;}
    inline DBManager* dbManager(){return this->m_dbManager;}
    inline MPSSystemSettings* settings(){return this->m_settings;}
    void emitAssReceived();
    
    virtual ~MPSSystem();
    
Q_SIGNALS:
    void associationReceived();
    
public Q_SLOTS:
    void dcmObjectReceived(DcmDataset* ds, char* filename);
};

#endif // SYSTEM_H
