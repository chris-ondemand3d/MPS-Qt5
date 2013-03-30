#ifndef AETTABLEMANAGER_H
#define AETTABLEMANAGER_H

#include <QList>
#include <utils/Singleton.h>

class AetTableManager : public Singleton<AetTableManager>
{
    friend class Singleton<AetTableManager>;
private:
    AetTableManager();
public:
    typedef enum {
        ALL_PERM,
        C_ECHO_PERM,
        C_STORAGE_PERM,
        C_FIND_PERM,
        C_MOVE_PERM,
        NO_PERMISSION
    } Permission;
    
    typedef QList<Permission> PermissionList;
    PermissionList getPermissions(char* aet);
    
    virtual ~AetTableManager();
};

#endif // AETTABLEMANAGER_H
