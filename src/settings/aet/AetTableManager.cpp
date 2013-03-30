#include "AetTableManager.h"

AetTableManager::AetTableManager()
{

}

AetTableManager::PermissionList AetTableManager::getPermissions(char* aet)
{
    // TODO: Check against the database the permissions of this AET.
    PermissionList perm;
    perm.append(AetTableManager::ALL_PERM);
    
    return perm;
}

AetTableManager::~AetTableManager()
{

}
