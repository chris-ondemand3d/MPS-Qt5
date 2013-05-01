#include "MPSSystem.h"

MPSSystem::MPSSystem(QObject* parent): QObject(parent)
{
    this->m_taskManager = new TaskManager(this);
}

void MPSSystem::emitAssReceived()
{
    Q_EMIT this->associationReceived();
}


MPSSystem::~MPSSystem()
{
}
