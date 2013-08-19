#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <utils/Singleton.h>
#include <system/MPSSystem.h>
#include <dicom/net/DcmAET.h>


class MPSSystem;

class SystemManager : public Singleton<SystemManager>
{
    friend class Singleton;
private:
    MPSSystem* m_system;
    SystemManager();
    
public:
    inline MPSSystem* mpsSystem() {return this->m_system;}
    bool registerRemoteDcmAET(const DcmAET& remoteAET);
    void initApp();
    ~SystemManager();
};

#endif // SYSTEMMANAGER_H
