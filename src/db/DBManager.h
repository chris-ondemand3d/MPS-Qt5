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
    /*
     * This method fill the data for BSONObjBuilder corresponding to
     * Patient, Study, Series or Instance level.
     */
    void insertData(const mongo::BSONObjBuilder& builder);
    mongo::BSONObjBuilder* buildDICOMStructure(mongo::BSONObjBuilder& patient,
                                               mongo::BSONObjBuilder& study,
                                               mongo::BSONObjBuilder& series,
                                               mongo::BSONObjBuilder& instance,
                                               OFString& studyInstanceUID,
                                               OFString& seriesInstanceUID,
                                               OFString& sopInstanceUID);
    string buildUID4BSON(OFString& uid, DcmQuery::QueryLevel level); 
    
public:
    DBManager(const string& dbName, const string& host , int port = 27017);
    bool findSOPInstanceUID(OFString& studyInstanceUID, OFString& seriesInstanceUID, OFString& sopInstanceUID);
    bool findSeries(OFString& studyInstanceUID, OFString& seriesInstanceUID);
    bool findStudy(OFString& studyInstanceUID);
    bool openConnection();
    bool closeConnection();
    void runMongoFunction(string mongoFunction, mongo::BSONArray& funcParams);
    void store(DcmDataset* ds, char* fileName);
};

#endif // DBMANAGER_H
