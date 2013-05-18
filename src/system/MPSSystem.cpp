#include "MPSSystem.h"

MPSSystem::MPSSystem(QObject* parent): QObject(parent)
{
    this->m_taskManager = new TaskManager(this);
    this->m_dbManager = new DBManager("mpsdb", "localhost");
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
}
