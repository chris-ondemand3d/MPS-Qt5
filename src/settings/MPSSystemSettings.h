/*
 *
 */

#ifndef MPSSYSTEMSETTINGS_H
#define MPSSYSTEMSETTINGS_H

#include <QSettings>
#include <utils/Singleton.h>
#include <QDir>

#ifndef Settings_Defined
#define Settings_Defined                        true
    #define MPSSetting_LOCAL_AET_GROUP          QString("local-AET")
    #define MPSSetting_LOCAL_AET                QString(MPSSetting_LOCAL_AET_GROUP + "/AET")
    #define MPSSetting_LOCAL_PORT               QString(MPSSetting_LOCAL_AET_GROUP + "/Port")
    #define MPSSetting_LOCAL_HOSTNAME           QString(MPSSetting_LOCAL_AET_GROUP + "/Hostname")
    #define MPSSetting_LOCAL_SCP_FOLDER         QString(MPSSetting_LOCAL_AET_GROUP + "/DICOM-Files-Root-Dir")
    
    #define MPSSetting_REMOTE_AET_GROUP         QString("remote-AET-list")

    #define MPSSetting_MONGODB_GROUP            QString("mongodb-settings")
    #define MPSSetting_MONGODB_DBNAME           QString(MPSSetting_MONGODB_GROUP + "/database-name")
    #define MPSSetting_MONGODB_HOST             QString(MPSSetting_MONGODB_GROUP + "/database-host")
    #define MPSSetting_MONGODB_PORT             QString(MPSSetting_MONGODB_GROUP + "/database-port")
#endif


using namespace std;

class MPSSystemSettings : public Singleton<MPSSystemSettings>
{
    friend class Singleton;
private:
    QSettings* m_systemSettings;
    MPSSystemSettings();
public:
    QVariant value(const QString& keySetting);
    bool hasSetting(const QString& keySetting);
    bool insertRemoteAET(QString& aet, QString& hostname, int port);
    bool existRemoteAET(char* aet);
    inline QSettings* settings() {return this->m_systemSettings;}
    ~MPSSystemSettings();
};

#endif // MPSSYSTEMSETTINGS_H
