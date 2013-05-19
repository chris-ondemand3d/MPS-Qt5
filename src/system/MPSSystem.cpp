#include "MPSSystem.h"

MPSSystem::MPSSystem(QObject* parent): QObject(parent)
{
    this->m_taskManager = new TaskManager(this);
    this->m_dbManager = new DBManager();
    this->m_settings = Singleton<MPSSystemSettings>::instance();
}

void MPSSystem::emitAssReceived()
{
    Q_EMIT this->associationReceived();
}

void MPSSystem::dcmObjectReceived(DcmDataset* ds, char* filename)
{
    this->m_dbManager->store(ds, filename);
}


MPSSystem::~MPSSystem()
{
    delete this->m_dbManager;
    delete this->m_settings;
    delete this->m_taskManager;
}
