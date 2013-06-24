#include "AetTableManager.h"

AetTableManager::AetTableManager()
{

}

PermissionList AetTableManager::getPermissions(const string& aet)
{
    // TODO: Check against the database the permissions of this AET.
    PermissionList permissions;
    permissions.push_back(AppPermission::ALL_PERM);
    return permissions;
}

AetTableManager::~AetTableManager()
{
    
}
