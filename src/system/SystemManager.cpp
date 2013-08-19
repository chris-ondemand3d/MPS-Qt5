#include "SystemManager.h"
#include <system_error>

SystemManager::SystemManager()
{
    this->m_system = new MPSSystem();
}

void SystemManager::initApp()
{
    try
    {
        if (!this->m_system->dbManager()->openConnection())
            throw system_error(make_error_code(std::errc::host_unreachable), 
                               "MongoDB server is no running.");
            else
                this->m_system->startDicomServer();
    }
    catch (std::exception& except)
    {
        cout << "MPS1.0-> Error: " << except.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch(mongo::DBException& except)
    {
        cout << "MPS1.0-> Error MongoDB: " << except.toString() << endl;
        exit(EXIT_FAILURE);
    }
    catch(...)
    {
        cout << "MPS1.0-> Unknow error" << endl;
        exit(EXIT_FAILURE);
    }
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
