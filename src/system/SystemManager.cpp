#include "SystemManager.h"

SystemManager::SystemManager()
{
    this->m_system = new MPSSystem();
}

bool SystemManager::registerRemoteDcmAET(const DcmAET& remoteAET)
{
    QString newAETSetting = MPSSetting_REMOTE_AET_GROUP + "/" + QString(remoteAET.aet().c_str());
    if (this->m_system->settings()->hasSetting(newAETSetting))
        return false; // this AET is defined in the config file
    
    // is new AET
    this->m_system->settings()->settings()->setValue(newAETSetting,
                                                     QString(remoteAET.toString().c_str()));
    this->m_system->settings()->settings()->sync();
    return true;
}


SystemManager::~SystemManager()
{
    delete this->m_system;
}
