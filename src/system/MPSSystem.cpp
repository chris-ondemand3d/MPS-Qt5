#include "MPSSystem.h"

MPSSystem::MPSSystem(QObject* parent): QObject(parent)
{
    this->m_taskManager = new TaskManager(this);
    this->m_dbManager = new DBManager();
    this->m_settings = MPSSystemSettings::instance();
    this->m_dicomServer = new DcmNetSCP();
}

void MPSSystem::emitAssReceived()
{
    Q_EMIT this->associationReceived();
}

void MPSSystem::dcmObjectReceived(DcmDataset* ds, char* filename)
{
    this->m_dbManager->store(ds, filename);
}

void MPSSystem::startDicomServer()
{
    Task* task = TaskFactory::newDcmSCPInstance(this->m_dicomServer);
    task->start();
}


MPSSystem::~MPSSystem()
{
    delete this->m_dbManager;
    delete this->m_settings;
    delete this->m_taskManager;
}
