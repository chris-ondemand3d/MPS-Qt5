#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <utils/Singleton.h>
#include <system/MPSSystem.h>

class MPSSystem;

class SystemManager : public Singleton<SystemManager>
{
    friend class Singleton;
private:
    MPSSystem* m_system;
    SystemManager();
    
public:
    inline MPSSystem* mpsSystem() {return this->m_system;}
    ~SystemManager();
};

#endif // SYSTEMMANAGER_H
