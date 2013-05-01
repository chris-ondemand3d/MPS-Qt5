#include "SystemManager.h"

SystemManager::SystemManager()
{
    this->m_system = new MPSSystem();
}

SystemManager::~SystemManager()
{
    delete this->m_system;
}
