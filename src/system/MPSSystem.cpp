#include "MPSSystem.h"

MPSSystem::MPSSystem(QObject* parent): QObject(parent)
{
}

void MPSSystem::emitAssReceived()
{
    Q_EMIT this->associationReceived();
}


MPSSystem::~MPSSystem()
{
}
