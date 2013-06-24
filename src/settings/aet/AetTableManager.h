#ifndef AETTABLEMANAGER_H
#define AETTABLEMANAGER_H

#include <utils/Singleton.h>
#include <list>
#include <string>

using namespace std;


enum class AppPermission{
    ALL_PERM,
    C_ECHO_PERM,
    C_STORAGE_PERM,
    C_FIND_PERM,
    C_MOVE_PERM,
    NO_PERMISSION
};

typedef list<AppPermission> PermissionList;

class AetTableManager : public Singleton<AetTableManager>
{
    friend class Singleton<AetTableManager>;
private:
    AetTableManager();
public:
    PermissionList getPermissions(const string& aet);
    
    virtual ~AetTableManager();
};

#endif // AETTABLEMANAGER_H
