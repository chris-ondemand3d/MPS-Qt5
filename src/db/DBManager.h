/*
 *
 */

#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <utils/Singleton.h>
#include <dicom/net/DcmQuery.h>
#include <db/DBQueryRsp.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <mongo/client/dbclient.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <QString>
#include <QMutexLocker>
#include <QBasicMutex>
#include <cstdlib>

#define DICOM_FILES_TABLE                       string("dicom_files")
#define MONGO_FUNC_insertInstance               string("insertInstance")
#define DB_STUDIES_COLLECTION                   string("mpsdb.studies")
#define MONGO_TAG_VR                            string("VR")
#define MONGO_TAG_VM                            string("VM")
#define MONGO_TAG_VALUE                         string("value")


using namespace std;
class DBQueryRsp;


class DBManager : public Singleton<DBManager>
{
private:
    mongo::DBClientConnection* m_mongoConn;
    mongo::HostAndPort* m_mongoServer;
    bool m_isConnected;
    string m_dbName;
    QMutex* m_writeMutex;
    
    void loadSettings();
    void registerInstance(mongo::BSONObj& studyBSON, mongo::BSONObj& seriesBSON, mongo::BSONObj& instanceBSONBuilder);
    
    void setValue(mongo::BSONType typeBSON,
                  void* value, 
                  mongo::BSONObjBuilder& query, 
                  mongo::BSONObjBuilder& projection,
                  mongo::BSONObj& queryBSON,
                  DcmElement* elem);
    void createElement2MongoQuery(mongo::BSONType elementType, 
                                  mongo::BSONObjBuilder& query, 
                                  mongo::BSONObjBuilder& projection, 
                                  DcmElement* elem,
                                  QueryLevel queryLevel);
    
    
    static bool strTag2DcmTag(mongo::BSONObj& mongoObj, int& group, int& element);
    
public:
    DBManager();
    DBQueryRsp* dcmFind(DcmDataset* ds);
    bool openConnection();
    bool closeConnection();
    void runMongoFunction(string mongoFunction, mongo::BSONArray& funcParams);
    void store(DcmDataset* ds, const string& fileName);
    static bool dcmDataset2BSON(DcmDataset* ds, mongo::BSONObjBuilder& mongoObj);
    static bool dcmElement2BSON(DcmElement* elem, mongo::BSONObjBuilder* mongoObj);
    static DcmElement* bson2DcmElement(mongo::BSONObj& mongoObj);
    static DcmDataset* bson2DcmDataset(mongo::BSONObj& mongoObj);
    static mongo::BSONType bsonTypeFromDcmVR(DcmEVR vr);
    
    ~DBManager();
};

#endif // DBMANAGER_H
