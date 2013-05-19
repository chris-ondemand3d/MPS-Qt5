
#include "DBManager.h"
#include "DBDcmTagTable.h"
#include <settings/MPSSystemSettings.h>
#include <dcmtk/dcmdata/dcelem.h>


DBManager::DBManager()
{
    // const string& dbName, const string& host , int port = 27017
    this->m_isConnected = false;
    this->loadSettings();
}


bool DBManager::closeConnection()
{
    if (this->m_mongoConn != NULL &&
        this->m_isConnected)
    {
        delete this->m_mongoConn; // Disconnecting
        this->m_mongoConn = NULL;
    }
}


bool DBManager::openConnection()
{
    if (this->m_mongoConn == NULL) // Is not connected
    {
        this->m_mongoConn = new mongo::DBClientConnection(true);
        string error;
        if (this->m_mongoConn->connect(*(this->m_mongoServer), error))
            return true;
        else
        {
            delete this->m_mongoConn;
            this->m_mongoConn = NULL;
        }
    }
    
    // TODO: Throw exception to notify that the connection is open
    return false;
    
}


void DBManager::store(DcmDataset* ds, char* fileName)
{
    mongo::BSONObjBuilder patientBSONBuilder, studyBSONBuilder, 
                          seriesBSONBuilder, instanceBSONBuilder,
                          *builderBSO;
    mongo::BSONObjBuilder();
    int card = ds->card();
    DBDcmTagTable* tagTableInstance = Singleton<DBDcmTagTable>::instance();
    
    for (int i = 0; i < card; i++)
    {
        DcmElement* elem = ds->getElement(i);
        int multiplicity = elem->getVM();
        
        // detecting the tag level and getting the corresponding BSONObjBuilder
        switch(tagTableInstance->getTagLevel(QString(elem->getTag().toString().c_str())))
        {
            case DcmQuery::PATIENT_LEVEL:
            {
                builderBSO = &patientBSONBuilder;
                break;
            }
            case DcmQuery::STUDY_LEVEL:
            {
                builderBSO = &studyBSONBuilder;
                break;
            }
            case DcmQuery::SERIES_LEVEL:
            {
                builderBSO = &seriesBSONBuilder;
                break;
            }
            case DcmQuery::IMAGE_LEVEL:
            {
                builderBSO = &instanceBSONBuilder;
                break;
            }
        }
        
        switch(elem->getVR())
        {
            case EVR_AT:
            {
                // checking multiplicity
                if (multiplicity == 1)
                    *builderBSO << elem->getTag().toString().c_str() << elem->getTag().toString().c_str();
                else
                {
                    OFString values;
                    string singleValue;
                    set<string> bsonValues;
                    for (int i = 0; i < multiplicity; i++)
                    {
                        if (values[i] == '\\')
                        {
                            bsonValues.insert(singleValue);
                            singleValue.clear();
                        }
                        else
                            singleValue += values[i];
                    }
                    *builderBSO << elem->getTag().toString().c_str() << bsonValues;
                }
                break;
            }
            
            case EVR_AS:
            case EVR_AE:
            case EVR_LO:
            case EVR_LT:
            case EVR_PN:
            case EVR_SH:
            case EVR_ST:
            case EVR_UT:
            case EVR_CS:
            case EVR_DA:
            case EVR_DT:
            case EVR_DS:
            case EVR_IS:
            case EVR_TM:
            case EVR_UI:
            {
                char* value;
                if (elem->getString(value).bad())
                    break;
                
                if (multiplicity == 1)
                    *builderBSO << elem->getTag().toString().c_str() << value;           
                                    
                else if (multiplicity > 1)
                {
                    set<string> bsonValues;
                    string singleValue;
                    int length = strlen(value);
                    for (int i = 0; i < length; i++)
                    {
                        if (value[i] == '\\')
                        {
                            bsonValues.insert(singleValue);
                            singleValue.clear();;
                        }
                        else
                        {
                            singleValue += value[i];
                        }
                    }
                    
                    *builderBSO << elem->getTag().toString().c_str() << bsonValues;
                }
                break;
            }
            
            case EVR_FD:
            {
                if (multiplicity == 1)
                {
                    double value;
                    if (elem->getFloat64(value).bad())
                        break;
                    
                    *builderBSO << elem->getTag().toString().c_str() << value;
                }
                
                else if (multiplicity > 1)
                {
                    set<double> bsonValues;
                    double* values;
                    
                    if (elem->getFloat64Array(values).bad())
                        break;
                    
                    for (int i = 0; i < multiplicity; i++)
                        bsonValues.insert(values[i]);
                    
                    *builderBSO << elem->getTag().toString().c_str();
                }
                break;
            }
            
            case EVR_FL:
            {
                if (multiplicity == 1)
                {
                    float value;
                    if (elem->getFloat32(value).good())
                        *builderBSO << elem->getTag().toString().c_str() << value;
                }
                
                else if (multiplicity > 1)
                {
                    set<float> bsonValues;
                    float* values;
                    
                    if (elem->getFloat32Array(values).bad())
                        break;
                    
                    for (int i = 0; i < multiplicity; i++)
                        bsonValues.insert(values[i]);
                    
                    *builderBSO << elem->getTag().toString().c_str();
                }
                break;
            }
            
            case EVR_SL:
            {
                if (multiplicity == 1)
                {
                    
                    int value;
                    if (elem->getSint32(value).good())
                        *builderBSO << elem->getTag().toString().c_str() << value;
                }
                else if (multiplicity > 1)
                {
                    set<int> bsonValues;
                    int* values;
                    if (elem->getSint32Array(values).good())
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        
                        *builderBSO << elem->getTag().toString().c_str() << bsonValues;
                    }                            
                }
                break;
            }
            
            case EVR_SS:
            {
                if (multiplicity == 1)
                {
                    float value;
                    if (elem->getFloat32(value).good())
                        *builderBSO << elem->getTag().toString().c_str() << value;
                }
                
                else if (multiplicity > 1)
                {
                    set<float> bsonValues;
                    float* values;
                    
                    if (elem->getFloat32Array(values).bad())
                        break;
                    
                    for (int i = 0; i < multiplicity; i++)
                        bsonValues.insert(values[i]);
                    
                    *builderBSO << elem->getTag().toString().c_str();
                }
                break;
            }
            
            case EVR_UL:
            {
                
                if (multiplicity == 1)
                {
                    unsigned int value;
                    if (elem->getUint32(value).good())
                        *builderBSO << elem->getTag().toString().c_str() << value;
                }
                else if (multiplicity > 1)
                {
                    set<unsigned int> bsonValues;
                    unsigned int* values;
                    if (elem->getUint32Array(values).good())
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        
                        *builderBSO << elem->getTag().toString().c_str() << bsonValues;
                    }                            
                }
                break;
            }
            
            case EVR_US:
            {
                if (multiplicity == 1)
                {
                    unsigned short value;
                    if (elem->getUint16(value).good())
                        *builderBSO << elem->getTag().toString().c_str() << value;
                }
                else if (multiplicity > 1)
                {
                    set<unsigned short> bsonValues;
                    unsigned short* values;
                    if (elem->getUint16Array(values).good())
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        
                        *builderBSO << elem->getTag().toString().c_str() << bsonValues;
                    }                            
                }
                break;
            }
        }
    }
    
    // Inseting data in DB
    // getting Study Instance UID, Series Instance UID and SOP Instance UID
    // to search in data base.
    OFString studyInstanceUID, seriesInstanceUID, sopInstanceUID;
    ds->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
    ds->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
    ds->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
    instanceBSONBuilder << "filename" << fileName;
    
    list<BSONObj> studies, series, instances;
    
    instances.push_front(instanceBSONBuilder.obj());
    seriesBSONBuilder.append("instances", instances);
    series.push_front(seriesBSONBuilder.obj());
    studyBSONBuilder.append("series", series);    
    studies.push_front(studyBSONBuilder.obj());
    patientBSONBuilder.append("studies", studies);
    
    mongo::BSONObj obj = patientBSONBuilder.done();
    mongo::BSONArray params = BSON_ARRAY(obj);
    this->runMongoFunction(MONGO_FUNC_insertInstance, params);
//     this->openConnection();
//     string systemJSColl = this->m_dbName + ".system.js";
//     BSONObj ret = BSON("value" << 1), result, info;
//     result = this->m_mongoConn->findOne(systemJSColl, QUERY("_id" << MONGO_FUNC_insertInstance),&ret);
// 
//     BSONObj query = BSON("eval" << result["value"].jsonString(mongo::JS,false) << "args" << BSON_ARRAY(obj));
//     this->m_mongoConn->runCommand(this->m_dbName, query,info);
    
//     cout << obj.extractFieldsUnDotted(BSON("studies.series.(0008,0060)" << "US")).toString().c_str() << endl;
//     conn.insert("mydb.test", obj);
//     conn.update("mydb.test", QUERY("_id" << 1.0), BSON("$set" << BSON("name.first" << "Nuevo Nombre")));
//     exit(0);
    
//     string sopInstUID(this->buildUID4BSON(sopInstanceUID, DcmQuery::IMAGE_LEVEL));
//     string seriesInstUID(this->buildUID4BSON(seriesInstanceUID, DcmQuery::SERIES_LEVEL));
//     string studyInstUID(this->buildUID4BSON(studyInstanceUID, DcmQuery::STUDY_LEVEL));
    
    
    
    
//     seriesBSONBuilder << "instances" << instances;
//     studyBSONBuilder << "series" << series;
//     cout << "studies: " << studyBSONBuilder.obj().toString().c_str() << endl;
    
//     cout << patientBSONBuilder.obj().toString().c_str() << endl;
//     this->openConnection();
//     BSONObj info;
//     BSONElement ret;
//     BSONElement objFunc = conn.query("mydb.system.js")->next()["value"];
//     BSONObj args = BSON("SOPInstancesUID" << "1.2.276.0.7230010.3.1.4.504029059.8938.1358779961.300967");
//     BSONObj func = BSON("eval" << objFunc.jsonString(mongo::JS, false)<<"args" << BSON_ARRAY("1.2.276.0.7230010.3.1.4.504029059.8938.1358779961.300967"));
// //     conn.eval("mydb.test", "function (SOPInstanceUID) {var cursor = db.test.find({'studies.series.instances.(0008,0018)' : SOPInstanceUID});return cursor.hasNext();}");
//     BSONObj func3 = conn.findOne("db.system.js", QUERY("_id" << "findSOPInstanceUID"));
//     BSONObj func2 = BSON("eval" << func3["value"].str() << "args" << BSON_ARRAY("1.2.276.0.7230010.3.1.4.504029059.8938.1358779961.300967"));

}

void DBManager::runMongoFunction(string mongoFunction, mongo::BSONArray& funcParams)
{
    this->openConnection();
    string systemJSColl = this->m_dbName + ".system.js";
    BSONObj ret = BSON("value" << 1), result, info;
    result = this->m_mongoConn->findOne(systemJSColl, QUERY("_id" << mongoFunction),&ret);
    
    if (result.isEmpty())
        return;
   
    BSONObj query = BSON("eval" << result["value"].jsonString(mongo::JS,false) << "args" << funcParams);
    this->m_mongoConn->runCommand(this->m_dbName, query,info);
}





void DBManager::loadSettings()
{
    MPSSystemSettings* settings = Singleton<MPSSystemSettings>::instance();
//     const string& dbName, const string& host , int port = 27017
    string dbHostname;
    int port;
    
    if (settings->hasSetting(MPSSetting_MONGODB_DBNAME))
        this->m_dbName = settings->value(MPSSetting_MONGODB_DBNAME).toString().toStdString();
    else
    {
        this->m_dbName = "mpsdb";
        settings->settings()->setValue(MPSSetting_MONGODB_DBNAME, "mpsdb");
        settings->settings()->sync();
    }
    
    if (settings->hasSetting(MPSSetting_MONGODB_HOST))
        dbHostname = settings->value(MPSSetting_MONGODB_HOST).toString().toStdString();
    else
    {
        dbHostname = "localhost";
        settings->settings()->setValue(MPSSetting_MONGODB_HOST, "localhost");
        settings->settings()->sync();
    }
    
    if (settings->hasSetting(MPSSetting_MONGODB_PORT))
    {
        bool retValue;
        port = settings->value(MPSSetting_MONGODB_PORT).toInt(&retValue);
        if (port <= 0 || !retValue)
        {
            port = 27017;
            settings->settings()->setValue(MPSSetting_MONGODB_PORT, port);
            settings->settings()->sync();
        }
    }
    else
    {
        port = 27017;
        settings->settings()->setValue(MPSSetting_MONGODB_PORT, port);
        settings->settings()->sync();
    }
    
    this->m_mongoServer = new mongo::HostAndPort(dbHostname, port);
}


bool DBManager::findSeries(OFString& studyInstanceUID, OFString& seriesInstanceUID)
{
    if (this->m_isConnected)
    {
        Query query = QUERY("studies.(0020,000d)" << studyInstanceUID.c_str() <<
        "studies.series.(0020,000e)" << seriesInstanceUID.c_str());
        BSONObj projection = BSON("studies.series.(0020,000e)" << 1);
        auto_ptr<DBClientCursor> cursor = this->m_mongoConn->query(this->m_dbName, query, 0,0,&projection);
        
        return cursor->more();
    }
    
    return false;
}

bool DBManager::findSOPInstanceUID(OFString& studyInstanceUID, 
                                   OFString& seriesInstanceUID, 
                                   OFString& sopInstanceUID)
{
    if (this->m_isConnected)
    {
        Query query = QUERY("studies.(0020,000d)" << studyInstanceUID.c_str() <<
                            "studies.series.(0020,000e)" << seriesInstanceUID.c_str() <<
                            "studies.series.instances.(0008,0018)" << sopInstanceUID.c_str());
        BSONObj projection = BSON("studies.series.instances.(0008,0018)" << 1);
        auto_ptr<DBClientCursor> cursor = this->m_mongoConn->query(this->m_dbName, query, 0,0,&projection);
        
        return cursor->more();
    }
    
    return false;
}

bool DBManager::findStudy(OFString& studyInstanceUID)
{
    if (this->m_isConnected)
    {
        Query query = QUERY("studies.(0020,000d)" << studyInstanceUID.c_str());
        BSONObj projection = BSON("studies.(0020,000d)" << 1);        
        auto_ptr<DBClientCursor> cursor = this->m_mongoConn->query(this->m_dbName, query, 0, 0, &projection);
        
        return cursor->more();
    }
    
    return false;
}
