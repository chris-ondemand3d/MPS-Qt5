/*
 *
 */

#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <utils/Singleton.h>
#include <dicom/net/DcmQuery.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <mongo/client/dbclient.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <QString>

#define DICOM_FILES_TABLE                       string("dicom_files")
#define MONGO_FUNC_insertInstance               string("insertInstance")
#define MONGO_TAG_VALUE                         "value"
#define MONGO_TAG_VR                            "VR"
#define MONGO_TAG_VM                            "VM"

using namespace std;
typedef mongo::BSONObjBuilder BSONObjBuilder;
typedef mongo::BSONObj BSONObj;
typedef mongo::Query Query;
typedef mongo::DBClientConnection DBClientConnection;
typedef mongo::DBClientCursor DBClientCursor;
typedef mongo::BSONArray BSONArray;

class DBManager : public Singleton<DBManager>
{
private:
    mongo::DBClientConnection* m_mongoConn;
    mongo::HostAndPort* m_mongoServer;
    bool m_isConnected;
    string m_dbName;

    void loadSettings();
    
public:
    DBManager();
    bool findSOPInstanceUID(OFString& studyInstanceUID, OFString& seriesInstanceUID, OFString& sopInstanceUID);
    bool findSeries(OFString& studyInstanceUID, OFString& seriesInstanceUID);
    bool findStudy(OFString& studyInstanceUID);
    bool openConnection();
    bool closeConnection();
    void runMongoFunction(string mongoFunction, mongo::BSONArray& funcParams);
    void store(DcmDataset* ds, char* fileName);
};

#endif // DBMANAGER_H
