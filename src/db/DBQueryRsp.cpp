/*
 *
 */

#include "DBQueryRsp.h"
#include "DBManager.h"



DBQueryRsp::DBQueryRsp(auto_ptr< mongo::DBClientCursor >& cursor, 
                       QueryLevel queryLevel, 
                       DBQueryType type)
{
    this->m_cursor = cursor;
    this->m_seriesPos = this->m_instancePos = 0;
    this->m_currentResult = nullptr;
    this->m_type = type;
    this->m_dcmQueryLevel = queryLevel;
    this->getNext();
}

DBResultContainer* DBQueryRsp::next()
{
    switch(this->m_type)
    {
        case DBQueryType::DICOM_QUERY_RETRIEVE:
        {
            DcmDataset* ds = new DcmDataset();
            switch(this->m_dcmQueryLevel)
            {
                case QueryLevel::STUDY_LEVEL:
                case QueryLevel::PATIENT_LEVEL:
                {
                    this->studyLvl2Dcm(&ds);
                    if (ds->card() == 0)
                        return new DBResultContainer(ds, DBResultContainerType::INVALID);
                    return new DBResultContainer(ds, DBResultContainerType::DCMTK_DATASET);
                }
                
                case QueryLevel::SERIES_LEVEL:
                {
                    this->seriesLvl2Dcm(&ds);
                    if (ds->card() == 0)
                        return new DBResultContainer(ds, DBResultContainerType::INVALID);
                    return new DBResultContainer(ds, DBResultContainerType::DCMTK_DATASET);
                }
                
                case QueryLevel::IMAGE_LEVEL:
                {
                    this->instanceLvl2Dcm(&ds);
                    if (ds->card() == 0)
                        return new DBResultContainer(ds, DBResultContainerType::INVALID);
                    return new DBResultContainer(ds, DBResultContainerType::DCMTK_DATASET);
                }
            }
            break;
        }
    }
    
    return new DBResultContainer(nullptr, DBResultContainerType::INVALID);
}


bool DBQueryRsp::getNext()
{
    if (this->m_currentResult != nullptr)
    {
        delete this->m_currentResult;
        this->m_currentResult = nullptr;
    }
    
    if (!this->m_cursor->more())
        return false;
    
    this->m_currentResult = new mongo::BSONObj(this->m_cursor->next());
    return true;
}


void DBQueryRsp::instanceLvl2Dcm(DcmDataset** ds)
{
    if (this->m_currentResult == nullptr)
        return;
    // getting the instances in pos
    // checking the series position
    if (this->m_currentResult->hasField("series"))
    {
        vector<mongo::BSONElement> seriesElement = (*this->m_currentResult)["series"].Array();
        if (this->m_seriesPos >= seriesElement.size())
            return;
        
        mongo::BSONElement seriesPos = seriesElement[this->m_seriesPos];
        // checking the instances position
        if (seriesPos.Obj().hasField("instances"))
        {
            vector<mongo::BSONElement> instancesElement = seriesPos.Obj()["instances"].Array();
            if (this->m_instancePos >= instancesElement.size())
            {
                this->m_instancePos = 0;
                this->m_seriesPos++;
                // checking the range of m_seriesPos
                if (this->m_seriesPos >= seriesElement.size() && this->getNext())
                    this->m_seriesPos = 0;
                
                this->instanceLvl2Dcm(ds);
                return;
            }
            
            // everything is fine
            // converting instances array to dcmdataset
            this->arrayBson2Dcm(instancesElement, ds, this->m_instancePos);
            this->m_instancePos++;
            // converting series array to dcmdataset
            this->arrayBson2Dcm(seriesElement, ds, this->m_seriesPos);
            this->studyLvl2Dcm(ds);
        }
    }
}

void DBQueryRsp::seriesLvl2Dcm(DcmDataset** ds)
{
    if (!this->m_currentResult->hasField("series"))
        return;
    
    try
    {
        vector<mongo::BSONElement> seriesArr = (*this->m_currentResult)["series"].Array();
        if (this->m_seriesPos >= seriesArr.size() && this->getNext())
        {
            this->m_seriesPos = 0;
            this->seriesLvl2Dcm(ds);
            return;
        }
        this->arrayBson2Dcm(seriesArr, ds, this->m_seriesPos);
        this->m_seriesPos++;
        this->studyLvl2Dcm(ds);
    }
    catch(...)
    {
        // TODO: Notify this error
        cout << "Error in: void DBQueryRsp::arrayBson2Dcm(mongo::BSONElement& bsonArr, DcmDataset** ds, int pos)";
    }
    
}

void DBQueryRsp::studyLvl2Dcm(DcmDataset** ds)
{   
    if (this->m_currentResult == nullptr || 
        this->m_currentResult->nFields() == 0)
        return;
    
    set<string> fieldNames;
    this->m_currentResult->getFieldNames(fieldNames);
    for(auto fieldName : fieldNames)
    {
        mongo::BSONElement bsonElement = this->m_currentResult->getField(fieldName);
        
        if (bsonElement.type() == mongo::BSONType::Array)
            continue;
        
        DcmElement* dcmElement = DBManager::bson2DcmElement(bsonElement);
        if (dcmElement == nullptr)
            continue;
        
        (*ds)->insert(dcmElement);
    }
    
    /*
     * checking if the query level is STUDY. In this case, 
     * we are the responsible to put the next element in the 
     * m_currentResult class attribute.
     */
    if (this->m_dcmQueryLevel == QueryLevel::STUDY_LEVEL || 
        this->m_dcmQueryLevel == QueryLevel::PATIENT_LEVEL)
        this->getNext();
}


void DBQueryRsp::arrayBson2Dcm(vector< mongo::BSONElement >& bsonArr, DcmDataset** ds, int pos)
{
    try
    {
        // everything is fine
        if (bsonArr.empty() ||
            pos >= bsonArr.size())
            return;
        
        mongo::BSONObj bsonElement = bsonArr[pos].Obj();
        set<string> fieldNames;
        bsonElement.getFieldNames(fieldNames);
        
        for (auto fieldName : fieldNames)
        {
            mongo::BSONElement elem = bsonElement[fieldName];
            if (elem.type() == mongo::BSONType::Array)
                continue;
            
            DcmElement* dcmElement = DBManager::bson2DcmElement(elem);
            if (dcmElement == nullptr)
                continue;
            
            (*ds)->insert(dcmElement);
        }
    }
    catch(...)
    {
        // TODO: Notify this error
        cout << "Error in: void DBQueryRsp::arrayBson2Dcm(mongo::BSONElement& bsonArr, DcmDataset** ds, int pos)";
    }
}


DcmDataset* DBQueryRsp::nextDcmQRRsp()
{    
    if (this->m_currentResult == nullptr)
    {
        if (!this->getNext())
            return nullptr;
    }
    // checking the result by query level
    DcmDataset* queryResult = new DcmDataset();
    switch(this->m_dcmQueryLevel)
    { 
        case QueryLevel::STUDY_LEVEL:
        case QueryLevel::PATIENT_LEVEL:
        {
            
            // there is pending result
            int length = this->m_currentResult->nFields();
            for (int i = 0; i < length; i++)
            {
                mongo::BSONElement bsonElement = (*this->m_currentResult)[i];
                DcmElement* dcmElement = DBManager::bson2DcmElement(bsonElement);
                if (dcmElement != nullptr)
                    queryResult->insert(dcmElement);
            }
            if (queryResult->card() == 0)
            {
                delete queryResult;
                queryResult = nullptr;
            }
            break;
        }
        
        case QueryLevel::SERIES_LEVEL:
        {
            // getting the result in the series level
            break;
        }
        
        case QueryLevel::IMAGE_LEVEL:
        {
            break;
        }
    }
    
    return queryResult;
}

bool DBQueryRsp::hasNext()
{
    return this->m_currentResult != nullptr;
}


DBQueryRsp::~DBQueryRsp()
{
    this->m_cursor.release();
}
