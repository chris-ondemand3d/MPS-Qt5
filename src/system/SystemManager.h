#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <utils/Singleton.h>
#include <system/MPSSystem.h>

class MPSSystem;

class SystemManager : public Singleton<SystemManager>
{
    friend class Singleton;
private:
    SystemManager();
    
public:
    MPSSystem* m_system;
};

#endif // SYSTEMMANAGER_H
