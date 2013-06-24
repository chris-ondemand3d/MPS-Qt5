
#include "DBManager.h"
#include "DBDcmTagTable.h"
#include <settings/MPSSystemSettings.h>
#include <system/MPSSystem.h>
#include <system/SystemManager.h>
#include <dcmtk/dcmdata/dcelem.h>
#include <dcmtk/dcmdata/dcvrlt.h>
#include <dcmtk/dcmdata/dcvrlo.h>
#include <dcmtk/dcmdata/dcvrpn.h>
#include <dcmtk/dcmdata/dcvrsh.h>
#include <dcmtk/dcmdata/dcvrst.h>
#include <dcmtk/dcmdata/dcvrut.h>
#include <dcmtk/dcmdata/dcvrcs.h>
#include <dcmtk/dcmdata/dcvrda.h>
#include <dcmtk/dcmdata/dcvrdt.h>
#include <dcmtk/dcmdata/dcvrds.h>
#include <dcmtk/dcmdata/dcvris.h>
#include <dcmtk/dcmdata/dcvrtm.h>
#include <dcmtk/dcmdata/dcvrui.h>
#include <dcmtk/dcmdata/dcvrfd.h>
#include <dcmtk/dcmdata/dcvrfl.h>
#include <dcmtk/dcmdata/dcvrsl.h>
#include <dcmtk/dcmdata/dcvrss.h>
#include <dcmtk/dcmdata/dcvrat.h>
#include <dcmtk/dcmdata/dcvrus.h>
#include <dcmtk/dcmdata/dcvrul.h>
#include <QMutex>

DBManager::DBManager()
{
    // const string& dbName, const string& host , int port = 27017
    this->m_isConnected = false;
    this->loadSettings();
    this->m_writeMutex = new QMutex(QMutex::Recursive);
    this->m_mongoConn = nullptr;
}


bool DBManager::closeConnection()
{
    if (this->m_mongoConn != nullptr &&
        this->m_isConnected)
    {
        delete this->m_mongoConn; // Disconnecting
        this->m_mongoConn = nullptr;
    }
}


bool DBManager::openConnection()
{
    if (this->m_mongoConn == nullptr) // Is not connected
    {
        this->m_mongoConn = new mongo::DBClientConnection(true);
        string error;
        if (this->m_mongoConn->connect(*(this->m_mongoServer), error))
        {
            cout << "Connection with MongoDB opened...";
            return true;
        }
        else
        {
            delete this->m_mongoConn;
            this->m_mongoConn = nullptr;
        }
    }
    
    // TODO: Throw exception to notify that the connection is open
    return false;
    
}

void DBManager::registerInstance(mongo::BSONObj& studyBSON, 
                                 mongo::BSONObj& seriesBSON, 
                                 mongo::BSONObj& instanceBSON)
{    
    try
    {
        auto incNumLambda = [&studyBSON, &seriesBSON, this](long long int inc, string tagValue, QueryLevel level)
        {
            // update tag (0020,1200)
            // checking if exist this tag
            string query;
            mongo::BSONObj proj;
            switch(level)
            {
//                 case QueryLevel::PATIENT_LEVEL:
//                 {
//                     query = (string)"{'(0010,0020).value': '"+ studyBSON["(0010,0020)"][MONGO_TAG_VALUE].String() +"'," +
//                         "'(0010,0010).value': '"+ studyBSON["(0010,0010)"][MONGO_TAG_VALUE].String() +"'}";
//                         proj = BSON(tagValue << 1 << "(0020,000d)" << 1 <<  
//                         "(0010,0010)"<< 1 << "(0010,0020)" << 1);
//                         break;
//                 }
                case QueryLevel::STUDY_LEVEL:
                {
                    query = (string)"{'(0020,000d).value': '"+ studyBSON["(0020,000d)"][MONGO_TAG_VALUE].String() +"'}";
                    proj = BSON(tagValue << 1 << "(0020,000d)" << 1 << "(0010,0010)"<< 1 << "(0010,0020)" << 1);
                    break;
                }
                case QueryLevel::SERIES_LEVEL:
                {
                    query = (string)"{'(0020,000d).value':'" + studyBSON["(0020,000d)"][MONGO_TAG_VALUE].String() + "',"+
                        "'series.(0020,000e).value':'"+ seriesBSON["(0020,000e)"][MONGO_TAG_VALUE].String() +"'}";
                        proj = BSON((string)"series." + tagValue << 1 << "(0020,000d)" << 1 <<  
                        "(0010,0010)"<< 1 << "(0010,0020)" << 1 << "series.(0020,000e)" << 1);
                        break;
                }
            }
            unique_ptr<mongo::DBClientCursor> cursor = this->m_mongoConn->query(DB_STUDIES_COLLECTION, 
                                                                                query,
                                                                                0,
                                                                                0,
                                                                                &proj
                                                                               );

            
            
            while (cursor->more()) // the study exist in the database
            {
                mongo::BSONObj bElem = cursor->next();
                if (level == QueryLevel::SERIES_LEVEL)
                {
                    vector<mongo::BSONElement> seriesList = bElem["series"].Array();
                    int pos = 0;
                    cout << "*********************" <<endl;
                    for (mongo::BSONElement eachSeries: seriesList)
                    {
                        string seriesInstanceUID = eachSeries.Obj()["(0020,000e)"][MONGO_TAG_VALUE].String();
                        cout << "series " << pos << ": " << seriesInstanceUID << endl;
                        if (seriesInstanceUID == seriesBSON["(0020,000e)"][MONGO_TAG_VALUE].String())         
                        {
                            string countVal = eachSeries.Obj()[tagValue][MONGO_TAG_VALUE].String();
                            long long int countStudies = atoll(countVal.c_str()) + inc;
                            cout << "tag: " << tagValue << " countStudies: "<< countStudies << " pos: " << pos << endl;
                            stringstream convStr, strPos;
                            convStr << countStudies;
                            strPos << pos;
                            mongo::BSONObj update = BSON("$set" << BSON(string("series." + strPos.str() + ".") + tagValue  + "." + MONGO_TAG_VALUE<< convStr.str()));
                            this->m_mongoConn->update(DB_STUDIES_COLLECTION, 
                                                      query, 
                                                      update);
                        }
                        pos++;
                    }
                    cout << "*********************" <<endl;
                }
                else
                {
                    long long int countStudies = atoll(bElem[tagValue][MONGO_TAG_VALUE].String().c_str()) + inc;
                    //                     query = (string)"{'(0008,00d).value': '"+ bElem["(0020,000d)"][MONGO_TAG_VALUE].String() +"'}";
                    stringstream convStr;
                    convStr << countStudies;
                    this->m_mongoConn->update(DB_STUDIES_COLLECTION, 
                                            query, 
                                            BSON("$set" << BSON(tagValue + "." + MONGO_TAG_VALUE << convStr.str())),
                                            false,
                                            true
                    );
                }
            }
        };

        auto insertTagsQRLambda = [](QueryLevel level, mongo::BSONObjBuilder& builder)
        {
            switch(level)
            {
                case QueryLevel::STUDY_LEVEL:
                {
                    builder << 
                    "(0020,1208)" << BSON(
                        MONGO_TAG_VR << "IS" <<
                        MONGO_TAG_VM << 1 <<
                        MONGO_TAG_VALUE << "1"
                    ) << 
                    "(0020,1206)" << BSON(
                        MONGO_TAG_VR << "IS" <<
                        MONGO_TAG_VM << 1 <<
                        MONGO_TAG_VALUE << "1"
                    );
                    break;
                }
                case QueryLevel ::SERIES_LEVEL:
                {
                    builder <<
                    "(0020,1209)" << BSON(
                        MONGO_TAG_VR << "IS" <<
                        MONGO_TAG_VM << 1 <<
                        MONGO_TAG_VALUE << "1"
                    );
                    break;
                }
            }
        };
        
        auto updatingModalitiesInStudy = [&studyBSON, &seriesBSON, this] ()
        {
            string query = "{'(0020,000d).value':'" + studyBSON["(0020,000d)"][MONGO_TAG_VALUE].String() + "'," +
            " 'series.(00020,000e).value':'" + seriesBSON["(0020,000e)"][MONGO_TAG_VALUE].String() + "'}";
            mongo::BSONObj projection = BSON(
                "(0020,000d)" << 1 << 
                "series.(0020,000e)"<< 1 << 
                "series.(0008,0060)"
                "(0008,0061)" << 1
            );
            
            mongo::Query mongoQuery(query);
            unique_ptr<mongo::DBClientCursor> cursor = this->m_mongoConn->query(DB_STUDIES_COLLECTION,
                                                                                mongoQuery,
                                                                                0,
                                                                                0,
                                                                                &projection);
            if (!cursor->more())
            {
                this->m_mongoConn->update(DB_STUDIES_COLLECTION,
                                          mongoQuery,
                                          BSON("$push" << BSON("(0008,0061)." + MONGO_TAG_VALUE << seriesBSON[""][MONGO_TAG_VALUE]) <<
                                               "$inc" << BSON("(0008,0061)." + MONGO_TAG_VM << 1))
                );
            }
        };
        
        if (!this->m_isConnected)
            this->openConnection();
        // checking that this instance is not register
        string queryStr = (string)"{'series.instances.(0008,0018).value':'" + instanceBSON["(0008,0018)"][MONGO_TAG_VALUE].String() +"'}";
        mongo::BSONObjBuilder* projection = new mongo::BSONObjBuilder();
        *projection << "'series.instances.(0008,0018)'" << 1;
        mongo::BSONObj* proj = new mongo::BSONObj(std::move(projection->done()));
        unique_ptr<mongo::DBClientCursor> cursor = std::move(this->m_mongoConn->query(DB_STUDIES_COLLECTION, 
                                                                                      queryStr,
                                                                                      0,
                                                                                      0,
                                                                                      proj));
        delete proj;
        delete projection;
        if (cursor->more())
            return;
        
        
        // Instances dosen't exist
        // checking if this Series Instances UID exist
        queryStr = (string)"{'series.(0020,000e).value': "+ seriesBSON["(0020,000e)"][MONGO_TAG_VALUE].toString(false) +"}";
        cursor = std::move(this->m_mongoConn->query(DB_STUDIES_COLLECTION, queryStr));
        
        if (cursor->more()) // exist this series
        {
            mongo::BSONObj bElem = cursor->next();
            vector<mongo::BSONElement> seriesList = bElem["series"].Array();
            int pos = 0;
            for (mongo::BSONElement seriesElem : seriesList)
            {
                if (seriesElem["(0020,000e)"][MONGO_TAG_VALUE].String() == seriesBSON["(0020,000e)"][MONGO_TAG_VALUE].String())
                    break;
                pos++;
            }
            stringstream strPos;
            strPos << pos;
            mongo::BSONObj bsonUpdate = BSON("$push" << BSON((string)"series." + strPos.str() +".instances" << instanceBSON));
            queryStr = (string)"{'(0020,000d).value':'"+ studyBSON["(0020,000d)"][MONGO_TAG_VALUE].String() +"'," + 
                "'series.(0020,000e).value':'"+ seriesBSON["(0020,000e)"][MONGO_TAG_VALUE].String() +"',"+
                "'series.instances' : {$exists: 1}}";
            this->m_mongoConn->update(DB_STUDIES_COLLECTION, queryStr, bsonUpdate);
            incNumLambda(1, "(0020,1209)", QueryLevel::SERIES_LEVEL); // Number of Series Related Instances
            incNumLambda(1, "(0020,1208)", QueryLevel::STUDY_LEVEL); // Number of Study Related Instances
            seriesList.clear();
        }
        else // this series dosen't exist
        {
            // checking the study
            queryStr = (string)"{'(0020,000d).value':'" + studyBSON["(0020,000d)"][MONGO_TAG_VALUE].String() + "'}";
            cursor = std::move(this->m_mongoConn->query(DB_STUDIES_COLLECTION, queryStr));
            
            if (cursor->more()) // the study exist
            {
                mongo::BSONObjBuilder seriesData;
                seriesData.appendElements(seriesBSON);
                insertTagsQRLambda(QueryLevel::SERIES_LEVEL, seriesData);
                seriesData <<"instances" << BSON_ARRAY(instanceBSON);                
                this->m_mongoConn->update(DB_STUDIES_COLLECTION, queryStr, BSON("$push" << BSON("series" << seriesData.done())));
//                 incNumLambda(1, "(0020,1209)", QueryLevel::SERIES_LEVEL); // Number of Series Related Instances
                incNumLambda(1, "(0020,1208)", QueryLevel::STUDY_LEVEL); // Number of Study Related Instances
                incNumLambda(1, "(0020,1206)", QueryLevel::STUDY_LEVEL); // Number of Study Related Series
            }
            else
            {
                // TODO: Insert the QR Tags for the QR Service.
                mongo::BSONObjBuilder studyData, seriesData;
                studyData.appendElements(studyBSON);
                studyData << "(0008,0061)" << BSON(
                    MONGO_TAG_VR << "CS" <<
                    MONGO_TAG_VM << 1 <<
                    MONGO_TAG_VALUE << BSON_ARRAY(seriesBSON["(0008,0060)"][MONGO_TAG_VALUE].String())
                );
                seriesData.appendElements(seriesBSON);
                insertTagsQRLambda(QueryLevel::SERIES_LEVEL, seriesData);
                seriesData << "instances" << BSON_ARRAY(instanceBSON);
                insertTagsQRLambda(QueryLevel::STUDY_LEVEL, studyData);
                studyData << "series" << BSON_ARRAY(seriesData.done());
                
                this->m_writeMutex->lock();
                this->m_mongoConn->insert(DB_STUDIES_COLLECTION, studyData.done());
                this->m_writeMutex->unlock();
            }
        }
        
    }
    catch(mongo::DBException& e)
    {
        cout << "Mongo Exception: " << e.toString() << endl;
    }
}



void DBManager::store(DcmDataset* ds, const string& fileName)
{
    mongo::BSONObjBuilder studyBSONBuilder, 
                          seriesBSONBuilder, instanceBSONBuilder,
                          *builderBSO;
    int card = ds->card();
    DBDcmTagTable* tagTableInstance = DBDcmTagTable::instance();
    DcmVR elemVR;
    for (int i = 0; i < card; i++)
    {
        DcmElement* elem = ds->getElement(i);        
        // detecting the tag level and getting the corresponding BSONObjBuilder
        switch(tagTableInstance->getTagLevel((string)elem->getTag().toString().c_str()))
        {
            case QueryLevel::PATIENT_LEVEL:
            case QueryLevel::STUDY_LEVEL:
            {
                this->dcmElement2BSON(elem, &studyBSONBuilder);
                break;
            }
            case QueryLevel::SERIES_LEVEL:
            {
                this->dcmElement2BSON(elem, &seriesBSONBuilder);
                break;
            }
            case QueryLevel::IMAGE_LEVEL:
            {
                this->dcmElement2BSON(elem, &instanceBSONBuilder);
                break;
            }
        }
    }
    
    // Inseting data in DB
    // getting Study Instance UID, Series Instance UID and SOP Instance UID
    // to search in data base.
    
    mongo::BSONObj study = std::move(studyBSONBuilder.done());
    mongo::BSONObj series = std::move(seriesBSONBuilder.done());
    mongo::BSONObj instance = std::move(instanceBSONBuilder.done());
    
    this->m_writeMutex->lock(); // Locking this thread
    this->registerInstance(study, series, instance);
    this->m_writeMutex->unlock();
    
}

bool DBManager::dcmElement2BSON(DcmElement* elem, mongo::BSONObjBuilder* mongoObj)
{
    int multiplicity = elem->getVM();
    string tagValue (elem->getTag().toString().c_str());
    DcmVR* vr = new DcmVR(elem->getVR());
    mongo::BSONObjBuilder* data = new mongo::BSONObjBuilder();
    *data << MONGO_TAG_VR << vr->getVRName() << MONGO_TAG_VM << multiplicity;
    
    if (elem->isEmpty())
        *data << MONGO_TAG_VALUE << mongo::jstNULL;
    else
    {
        switch(elem->getVR())
        {
            case EVR_DT:
            case EVR_TM:
            case EVR_DA:
            {
                if (elem->isEmpty())
                {
                    *data << MONGO_TAG_VALUE << mongo::jstNULL;
                    break;
                }
                
                OFString value;
                elem->getOFString(value, 0);
                string valueStr(value.c_str());
                
                if (multiplicity == 1)
                {
                    long long int dtVal = atol(value.c_str());
                    if (dtVal > 0) // datetime value is fine
                    {
                        *data << MONGO_TAG_VALUE << dtVal;
                    }
                    else;
                    // TODO: Notify that the datetime value has an error
                }
                else if (multiplicity > 1)
                {
                    list<long long int> values;
                    string singleValue;
                    long val;
                    for (auto charVal : valueStr)
                    {
                        if (charVal == '\\')
                        {
                            if ((val = atol(singleValue.c_str())) > 0)
                                values.push_back(val);
                            singleValue.clear();
                        }
                        else
                        {
                            singleValue += charVal;
                        }
                    }
                    if ((val = atol(singleValue.c_str())) > 0)
                        values.push_back(val);
                    *data << MONGO_TAG_VALUE << std::move(values);
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
            case EVR_DS:
            case EVR_IS:        
            case EVR_UI:
            {
                OFString value;
                if (elem->getOFString(value, 0).good())
                {
                    string valueStr(value.c_str());
                    if (multiplicity == 1)
                        *data << MONGO_TAG_VALUE << valueStr;
                    else if (multiplicity > 1)
                    {   
                        list<string> bsonValues;
                        string singleValue;
                        for (auto charVal : valueStr)
                        {
                            if (charVal == '\\')
                            {
                                bsonValues.push_back(singleValue);
                                singleValue.clear();
                            }
                            else
                            {
                                singleValue += charVal;
                            }
                        }
                        bsonValues.push_back(singleValue);
                        *data << MONGO_TAG_VALUE << bsonValues;
                    }              
                }
                else
                    *data << MONGO_TAG_VALUE << mongo::jstNULL;
                
                break;
            }
            
            case EVR_FD:
            {
                if (multiplicity == 1)
                {
                    double value;
                    *data << MONGO_TAG_VALUE << (elem->getFloat64(value).good() ? value : mongo::jstNULL);
                }
                
                else if (multiplicity > 1)
                {
                    set<double> bsonValues;
                    double* values;
                    
                    if (elem->getFloat64Array(values).bad())
                        *data << MONGO_TAG_VALUE << mongo::jstNULL;
                    else
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        *data << MONGO_TAG_VALUE << bsonValues;
                        delete values;
                    }
                }
                break;
            }
            
            case EVR_FL:
            {
                if (multiplicity == 1)
                {
                    float value;
                    *data << MONGO_TAG_VALUE << (elem->getFloat32(value).good() ? value : mongo::jstNULL);
                }
                
                else if (multiplicity > 1)
                {
                    set<float> bsonValues;
                    float* values;
                    
                    if (elem->getFloat32Array(values).bad())
                        *data << MONGO_TAG_VALUE << mongo::jstNULL;
                    else
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        
                        *data << MONGO_TAG_VALUE << bsonValues;
                        delete values;
                    }
                }
                break;
            }
            
            case EVR_SL:
            {
                if (multiplicity == 1)
                {
                    
                    int value;
                    *data << MONGO_TAG_VALUE << (elem->getSint32(value).good() ? value : mongo::jstNULL);
                }
                else if (multiplicity > 1)
                {
                    set<int> bsonValues;
                    int* values;
                    if (elem->getSint32Array(values).good())
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        
                        *data << MONGO_TAG_VALUE << bsonValues;
                    }                            
                    else
                        *data << MONGO_TAG_VALUE << mongo::jstNULL;
                    
                    delete values;
                }
                break;
            }
            
            case EVR_SS:
            {
                if (multiplicity == 1)
                {
                    short value;
                    *data << MONGO_TAG_VALUE << (elem->getSint16(value).good() ? value : mongo::jstNULL);
                }
                
                else if (multiplicity > 1)
                {
                    set<short> bsonValues;
                    short* values;
                    
                    if (elem->getSint16Array(values).good())
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        
                        *data << MONGO_TAG_VALUE << bsonValues;
                    }
                    else
                        *data << MONGO_TAG_VALUE << mongo::jstNULL;
                    
                    delete values;
                }
                break;
            }
            
            case EVR_AT:
            case EVR_UL:
            {
                
                if (multiplicity == 1)
                {
                    unsigned int value;
                    *data << MONGO_TAG_VALUE << (elem->getUint32(value).good() ? value : mongo::jstNULL);
                }
                else if (multiplicity > 1)
                {
                    set<unsigned int> bsonValues;
                    unsigned int* values;
                    if (elem->getUint32Array(values).good())
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        
                        *data << MONGO_TAG_VALUE << bsonValues;
                    }       
                    else
                        *data << MONGO_TAG_VALUE << mongo::jstNULL;
                    
                    delete values;
                }
                break;
            }
            
            case EVR_US:
            {
                if (multiplicity == 1)
                {
                    unsigned short value;
                    *data << MONGO_TAG_VALUE << (elem->getUint16(value).good() ? value : mongo::jstNULL);
                }
                else if (multiplicity > 1)
                {
                    set<unsigned short> bsonValues;
                    unsigned short* values;
                    if (elem->getUint16Array(values).good())
                    {
                        for (int i = 0; i < multiplicity; i++)
                            bsonValues.insert(values[i]);
                        
                        *data << MONGO_TAG_VALUE << bsonValues;
                    }
                    else
                        *data << MONGO_TAG_VALUE << mongo::jstNULL;
                    
                    delete values;
                }
                break;
            }
        }
    }
    *mongoObj << tagValue << data->done();
    delete vr;
    delete data;
    return true;
}


void DBManager::runMongoFunction(string mongoFunction, mongo::BSONArray& funcParams)
{
    this->openConnection();
    string systemJSColl = this->m_dbName + ".system.js";
    mongo::BSONObj ret = std::move(BSON("value" << 1)), result, info;
    result = this->m_mongoConn->findOne(systemJSColl, QUERY("_id" << mongoFunction),&ret);
    
    if (result.isEmpty())
        return;
   
    mongo::BSONObj query = std::move(BSON("eval" << result["value"].jsonString(mongo::JS,false) << "args" << funcParams));
    this->m_mongoConn->runCommand(this->m_dbName, query, info);
}


void DBManager::loadSettings()
{
    MPSSystemSettings* settings = MPSSystemSettings::instance();
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
//             port = 27017;
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

bool DBManager::dcmDataset2BSON(DcmDataset* ds, mongo::BSONObjBuilder& mongoObj)
{
    int card = ds->card();
    for (int i = 0; i < card; i++)
    {
        if (!dcmElement2BSON(ds->getElement(i), &mongoObj))
            return false;
    }
    return true;
}

void DBManager::createElement2MongoQuery(mongo::BSONType elementType, 
                                         mongo::BSONObjBuilder& query, 
                                         mongo::BSONObjBuilder& projection, 
                                         DcmElement* elem, QueryLevel queryLevel)
{    
    // checking the tag level
    string strTag = elem->getTag().toString().c_str();
    QueryLevel tagLevel = DBDcmTagTable::instance()->getTagLevel(strTag);
    if (tagLevel > queryLevel)
        return;
    
    
    mongo::BSONObjBuilder* bsonElement = new mongo::BSONObjBuilder();
    DBManager::dcmElement2BSON(elem, bsonElement);
    string levelString;
    
    switch(tagLevel)
    {        
        case QueryLevel::IMAGE_LEVEL:
        {
            levelString = "series.instances.";
            break;
        }
        case QueryLevel::SERIES_LEVEL:
        {
            levelString = "series.";
            break;
        }
        case QueryLevel::STUDY_LEVEL:
        case QueryLevel::PATIENT_LEVEL:
        {
            levelString = "";
            break;
        }
    }
    
    string quote = "'";
    string subfixQueryValue = quote +  levelString + strTag + "." + MONGO_TAG_VALUE + quote;
    if (elem->isEmpty())
        query << subfixQueryValue << BSON("$exists" << 1);
        
    else 
    {
        mongo::BSONObj bElem = std::move(bsonElement->done());
        switch(elementType)
        {
            case mongo::BSONType::NumberDouble:
            {
                if (!bElem[strTag][MONGO_TAG_VALUE].isNull())
                    query << subfixQueryValue << bElem[strTag][MONGO_TAG_VALUE].Double();
                break;
            }
            case mongo::BSONType::NumberInt:
            {
                if (!bElem[strTag][MONGO_TAG_VALUE].isNull())
                    query << subfixQueryValue << bElem[strTag][MONGO_TAG_VALUE].Int();
                break;
            }
            case mongo::BSONType::NumberLong:
            {
                if (!bElem[strTag][MONGO_TAG_VALUE].isNull())
                {
                    // checking if it is a DA, DT or TM type of VR
                    if (elem->getVR() == EVR_DA || elem->getVR() == EVR_DT || elem->getVR() == EVR_TM)
                    {
                        stringstream data;
                        char* strValue;
                        elem->getString(strValue);
                        string dtStr = strValue;
                        // checking if there it is a range value
                        
                        auto index = dtStr.find_first_of('-');
                        if (index != dtStr.find_last_of('-')) // checking that the character '-' appear just one time
                            return;
                        // everything is fine
                        if (index == string::npos)// checking if the character '-' dosen't exist
                            query << subfixQueryValue<< atoll(dtStr.c_str());
                        
                        else if (index == 0) // checking if it is a "until" query
                            query << subfixQueryValue << BSON("$lte" << atoll(dtStr.c_str()));
                        
                        else if (index == dtStr.length() - 1) // checking if it is a "from" query
                            query << subfixQueryValue << BSON("$gte" << atoll(dtStr.c_str()));
                        
                        else // the query is a "until - from" query
                        {
                            string dt1 = dtStr.substr(0, index + 1);
                            string dt2 = dtStr.substr(index + 1, dtStr.length());
                            
                            if (dt1.length() != dt2.length()) // error
                                return;
                            
                            // everything is fine
                            // Now, checking that the datetime1 <= datetime2
                            long long int dtLong1 = atoll(dt1.c_str());
                            long long int dtLong2 = atoll(dt2.c_str());
                            if (dtLong1 > dtLong2) // Error
                                return;
                            
                            // datetime1 and datetime2 are fine
                            query << subfixQueryValue << BSON("$lte" << dtLong2 << "$gte" << dtLong1);
                        }
                    }
                    else
                        query << subfixQueryValue << bElem[strTag][MONGO_TAG_VALUE].Long();
                }
                break;
            }
            case mongo::BSONType::String:
            {
                if (!bElem[strTag][MONGO_TAG_VALUE].isNull())
                    query << subfixQueryValue << bElem[strTag][MONGO_TAG_VALUE].String();
                break;
            }
        }
        
    }
    
    projection << subfixQueryValue << 1;
    if (bsonElement != nullptr)
    {
        delete bsonElement;
        bsonElement = nullptr;
    }
}


mongo::BSONType DBManager::bsonTypeFromDcmVR(DcmEVR vr)
{
    switch(vr)
    {
        case EVR_AS:
        case EVR_AE:
        case EVR_LO:
        case EVR_LT:
        case EVR_PN:
        case EVR_SH:
        case EVR_ST:
        case EVR_UT:
        case EVR_CS:        
        case EVR_DS:
        case EVR_IS:        
        case EVR_UI:
            return mongo::String;
            
        case EVR_DT:
        case EVR_TM:
        case EVR_DA:
            return mongo::NumberLong;
            
        case EVR_FD:
        case EVR_FL:
            return mongo::NumberDouble;
            
        case EVR_SL:
        case EVR_SS:
        case EVR_AT:
        case EVR_US:
        case EVR_UL:
            return mongo::NumberInt;
    }
    
    return mongo::EOO;
}


DBQueryRsp* DBManager::dcmFind(DcmDataset* ds)
{
    mongo::BSONObjBuilder dcmQueryBSON;
    mongo::BSONObjBuilder query, projection;    
    unsigned long card = ds->card();
    
    OFString queryLevel;
    if (ds->findAndGetOFString(DCM_QueryRetrieveLevel, queryLevel).bad())
        return nullptr;
    
    for (unsigned long i = 0; i < card; i++)
    {
        DcmElement* dcmElement = ds->getElement(i);
        DBManager::createElement2MongoQuery(DBManager::bsonTypeFromDcmVR(dcmElement->getVR()),
                                            query,
                                            projection,
                                            dcmElement,
                                            DcmQuery::str2QueryLevel((string)queryLevel.c_str()));
    }
    mongo::BSONObj fieldsToReturn = std::move(projection.done());
    cout << "projection: " << projection.obj() << endl << "query: " << query.done() << endl;
    unique_ptr<mongo::DBClientCursor> cursor = this->m_mongoConn->query(DB_STUDIES_COLLECTION,
                                                                        mongo::Query(std::move(query.done())),
                                                                        0,
                                                                        0,
                                                                        &fieldsToReturn);
    if (cursor->more())
        return new DBQueryRsp(cursor);
    
    return nullptr;
}



/*
 * asdasd asdas dasda sdas d
 */
void DBManager::setValue(mongo::BSONType typeBSON, 
                         void* value, 
                         mongo::BSONObjBuilder& query, 
                         mongo::BSONObjBuilder& projection, 
                         mongo::BSONObj& queryBSON, 
                         DcmElement* elem)
{
    
    string queryLevel = queryBSON[string(DCM_QueryRetrieveLevel.toString().c_str())][MONGO_TAG_VALUE].String();
    string tagValue = (string)elem->getTag().toString().c_str();
    
    
    switch(typeBSON)
    {
        case mongo::String:
        {
            break;
        }
        
        case mongo::NumberDouble:
        {
            break;
        }
        
        case mongo::NumberInt:
        case mongo::NumberLong:
        {
            break;
        }
    }
//     if (boolStr)
//     {
//         if (!elem->isEmpty())
//             query << tagValueComplete << strVal;
//         projection << tagValueComplete << 1;
//     }
//     else if (boolDouble)
//     {
//         if (!elem->isEmpty())
//             query << tagValueComplete << doubleVal;
//         projection << tagValueComplete << 1;
//     }
//     else if (boolInt)
//     {
//         if (!elem->isEmpty())
//             query << tagValueComplete << intVal;
//         projection << tagValueComplete << 1;
//     }
//     else 
//         // TODO: notify type error
//         return;
}

DcmDataset* DBManager::bson2DcmDataset(mongo::BSONObj& mongoObj)
{
    int card = mongoObj.nFields();
    DcmDataset* ds = new DcmDataset();
    
    for (int i = 0; i < card; i++)
    {
        mongo::BSONObj mongoElem = mongoObj[i].Obj();
        DcmElement* elem = DBManager::bson2DcmElement(mongoElem);
        if (elem == nullptr)
            return nullptr;
        
        // everythong is fine
        ds->insert(elem);
    }
    
    return ds;
}

DcmElement* DBManager::bson2DcmElement(mongo::BSONObj& mongoObj)
{
    string vrValue = mongoObj[0][MONGO_TAG_VALUE].toString(false);
    int group, element;
    if (!DBManager::strTag2DcmTag(mongoObj, group, element))
        return nullptr;
    
    DcmTag dcmTag(group, element);
    DcmElement* dcmElem = nullptr;
    int multiplicity = mongoObj[MONGO_TAG_VM].Int();
    if (vrValue == "LO" || vrValue == "LT" || vrValue == "PN" || vrValue == "SH" || vrValue == "ST" ||
        vrValue == "UT" || vrValue == "CS" || vrValue == "DS" ||
        vrValue == "IS" || vrValue == "UI")
    {
        // checking multplicity        
        string value;
        if (multiplicity == 1)
            value = mongoObj[MONGO_TAG_VALUE].str();
        else
        {
            vector<mongo::BSONElement> values = mongoObj[MONGO_TAG_VALUE].Array();
            int mult = multiplicity;
            for (vector<mongo::BSONElement>::iterator it = values.begin(), end = values.end(); it < end; it++)
            {
                value += *it;
                if (mult > 0)
                {
                    value+= "/";
                    mult -= 1;
                }
            }            
        }
        // creating the DcmElement and setting the value
        if (vrValue == "LO")
            dcmElem = new DcmLongString(dcmTag);
        else if (vrValue == "LT")
            dcmElem = new DcmLongText(dcmTag);
        else if (vrValue == "PN")
            dcmElem = new DcmPersonName(dcmTag);
        else if (vrValue == "SH")
            dcmElem = new DcmShortString(dcmTag);
        else if (vrValue == "ST")
            dcmElem = new DcmShortText(dcmTag);
        else if (vrValue == "UT")
            dcmElem = new DcmUnlimitedText(dcmTag);
        else if (vrValue == "CS")
            dcmElem = new DcmCodeString(dcmTag);
        else if (vrValue == "DS")
            dcmElem = new DcmDecimalString(dcmTag);
        else if (vrValue == "IS")
            dcmElem = new DcmIntegerString(dcmTag);
        else if (vrValue == "UI")
            dcmElem = new DcmUniqueIdentifier(dcmTag);
        else 
            return nullptr;
        
        OFString dcmValue(value.c_str());
        dcmElem->putOFStringArray(dcmValue);
    }
    else if (vrValue == "FD" || vrValue == "FL")
    {
        double value;
        double* valueArr;
        
        if (vrValue == "FD")
            dcmElem = new DcmFloatingPointDouble(dcmTag);
        else if (vrValue == "FL")            
            dcmElem = new DcmFloatingPointSingle(dcmTag);
        
        if (multiplicity == 1)
            value = mongoObj[MONGO_TAG_VALUE].Double();
        else if (multiplicity >= 1)
        {
            vector<mongo::BSONElement> values = mongoObj[MONGO_TAG_VALUE].Array();
            valueArr = new double[multiplicity];
            vector<mongo::BSONElement>::iterator it = values.begin(), end = values.end();
            for(int i = 0; it < end; i++, it++)
                valueArr[i] = (*it).Double();
        }
        else
            return nullptr;
        
        // setting the value
        if (vrValue == "FD")
        {
            if (multiplicity == 1)
                dcmElem->putFloat64(value);
            else if (multiplicity > 1)
                dcmElem->putFloat64Array(valueArr, multiplicity);
            else
                return nullptr;
        }
        else if (vrValue == "FL")
        {
            if (multiplicity == 1)
                dcmElem->putFloat64(value);
            else if (multiplicity > 1)
                dcmElem->putFloat64Array(valueArr, multiplicity);
            return nullptr;
        }
        
    }
    else if (vrValue == "SL" || vrValue == "SS" || vrValue == "AT" ||
        vrValue == "US" || vrValue == "UL")
    {
        if (vrValue == "SL")
            dcmElem = new DcmSignedLong(dcmTag);
        else if (vrValue == "SS")
            dcmElem = new DcmSignedShort(dcmTag);
        else if (vrValue == "AT")
            dcmElem = new DcmAttributeTag(dcmTag);
        else if (vrValue == "US")
            dcmElem = new DcmUnsignedShort(dcmTag);
        else if (vrValue == "UL")
            dcmElem = new DcmUnsignedLong(dcmTag);
        else 
            return nullptr;
        
        int value;
        int* valueArr;
        if (multiplicity == 1)
        {
            value = mongoObj[MONGO_TAG_VALUE].Int();
            switch(dcmElem->getVR())
            {
                case EVR_SL:
                case EVR_AT:
                {
                    dcmElem->putSint32(value);
                    break;
                }
                case EVR_SS:
                {
                    dcmElem->putSint16(value);
                    break;
                }
                case EVR_UL:
                {
                    dcmElem->putUint32(value);
                    break;
                }
                case EVR_US:
                {
                    dcmElem->putUint16(value);
                    break;
                }
            }
        }
        else if (multiplicity >= 1)
        {
            vector<mongo::BSONElement> values = mongoObj[MONGO_TAG_VALUE].Array();
            valueArr = new int[multiplicity];
            vector<mongo::BSONElement>::iterator it = values.begin(), end = values.end();
            for (int i = 0; it < end; i++, it++)
                valueArr[i] = (*it).Int();
            
            switch (dcmElem->getVR())
            {
                case EVR_SL:
                case EVR_AT:
                {
                    dcmElem->putSint32Array(valueArr, multiplicity);
                    break;
                }
                case EVR_SS:
                {
                    dcmElem->putSint16Array((Sint16*)valueArr, multiplicity);
                    break;
                }
                case EVR_UL:
                {
                    dcmElem->putUint32Array((Uint32*)valueArr, multiplicity);
                    break;
                }
                case EVR_US:
                {
                    dcmElem->putUint16Array((Uint16*)valueArr, multiplicity);
                    break;
                }
                default:
                {
                    return nullptr;
                }
            }
        }
        else
            return nullptr;
    }
    
    else if (vrValue == "DA" || vrValue == "DT" || vrValue == "TM")
    {
        DcmElement* dcmElem = nullptr;
        long long int value;
        long long int* valueArr;
        if (vrValue == "TM")
            dcmElem = new DcmTime(dcmTag);
        else if (vrValue == "DA")
            dcmElem = new DcmDate(dcmTag);
        else if (vrValue == "DT")
            dcmElem = new DcmDateTime(dcmTag);
        
        // checking multiplicity
        stringstream data;
        if (multiplicity == 1)
        {
            data << mongoObj[MONGO_TAG_VALUE].Long();
            dcmElem->putString(data.str().c_str());
        }
        else if (multiplicity >= 1)
        {
            string strVal;
            for (auto arrVal : mongoObj[MONGO_TAG_VALUE].Array())
                data << arrVal.Long() << '/';
            strVal = std::move(data.str());
            dcmElem->putString(strVal.substr(0, strVal.length() - 1).c_str());
        }
    }
    else
        return nullptr;
    
    return dcmElem;
}

bool DBManager::strTag2DcmTag(mongo::BSONObj& mongoObj, int& group, int& element)
{
    set<string> setField;
    mongoObj.getFieldNames(setField);
    set<string>::iterator begin = setField.begin();
 
    bool conversion = false;
    string strTag = *begin, groupStr, elementStr;
    if (strTag.size() != 11)
        return false;
    
    for (int i = 0; i < 11; i++)
    {
        if (strTag[i] == '(' ||
            strTag[i] == ')' ||
            strTag[i] == ',')
            continue;
        else if (i < 5)
            groupStr += strTag[i];
        else
            elementStr += strTag[i];
    }
    // Group and Element are ready
    QByteArray groupBA(groupStr.c_str()); 
    QByteArray elemBA(elementStr.c_str());
    // converting
    group = groupBA.toInt(&conversion, 16);
    if (!conversion)
        return false;
    element = elemBA.toInt(&conversion, 16);
    if (!conversion)
        return false;
    
    return true;
}


DBManager::~DBManager()
{
    if (this->m_isConnected)
        this->closeConnection();
    
    delete this->m_mongoConn;
    delete this->m_mongoServer;
}
