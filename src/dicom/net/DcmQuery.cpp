#include "DcmQuery.h"
#include <db/DBDcmTagTable.h>
#include <dcmtk/dcmdata/dcelem.h>
#include <dcmtk/dcmdata/dcvrcs.h>
#include <dcmtk/dcmdata/dcdeftag.h>

DcmQuery::DcmQuery()
{
    
}

DcmQuery::DcmQuery(DcmDataset* ds)
{
    int card = ds->card();
    this->m_level = QueryLevel::INVALID_VALUE;
    
    for (int i = 0; i < card; i++)
    {
        DcmElement* elem = ds->getElement(i);
        if (elem->getTag() == DCM_QueryRetrieveLevel)
        {
            char* value;
            elem->getString(value);
            this->m_level = this->str2QueryLevel(value);
        }
        else
            this->addKey(elem);
    }
}


bool DcmQuery::validateDcmQuery(DcmDataset* ds)
{
    if (ds == nullptr)
        return false;
    
    OFString strValue;
    if (ds->findAndGetOFString(DCM_QueryRetrieveLevel, strValue).bad())
        return false;
    
    // checking query level value
    QueryLevel queryLevel = DcmQuery::str2QueryLevel(strValue.c_str());
    if (queryLevel == QueryLevel::INVALID_VALUE)
        return false;
    
    // checking each tag level
    int card = ds->card();
    for (int i = 0; i < card; i++)
    {
        if(ds->getElement(i)->getTag() == DCM_QueryRetrieveLevel)
            continue;
        
        DcmElement* elem = ds->getElement(i);
        QueryLevel tagLevel = DBDcmTagTable::instance()->getTagLevel(elem->getTag().toString().c_str());
        if (tagLevel > queryLevel)
        {
            elem = ds->remove(i);
            delete elem;
        }
    }
    
    if (ds->card() == 0)
        return false;
    
    return true;
}


bool DcmQuery::isGood()
{
    if (this->m_level == QueryLevel::INVALID_VALUE)
        return false;
    
    // checking the tags level
    for (OFIterator<DcmElement*> it = this->m_query.begin(), end = this->m_query.end(); it != end; it++)
    {
        DcmElement* elem = *it;
        QueryLevel queryLevel = DBDcmTagTable::instance()->getTagLevel(elem->getTag().toString().c_str());
        if (this->m_level < queryLevel)
            this->m_query.remove(*it);        
    }
    
    if (this->m_query.empty())
        return false;
    
    return true;
}


void DcmQuery::addKey(const DcmElement* key)
{
    if (key != nullptr)
        this->m_query.push_back((DcmElement*)key);
}

QueryLevel DcmQuery::str2QueryLevel(string queryLevel)
{
    if (queryLevel == "PATIENT")
        return QueryLevel::PATIENT_LEVEL;
    
    if (queryLevel == "STUDY")
        return QueryLevel::STUDY_LEVEL;
    
    if (queryLevel == "SERIES")
        return QueryLevel::SERIES_LEVEL;
    
    if (queryLevel == "IMAGE")
        return QueryLevel::IMAGE_LEVEL;
    
    return QueryLevel::INVALID_VALUE;
}


string DcmQuery::queryLevel2Str(QueryLevel queryLevel)
{
    switch(queryLevel)
    {
        case QueryLevel::PATIENT_LEVEL:
            return "PATIENT";
            
        case QueryLevel::STUDY_LEVEL:
            return "STUDY";
            
        case QueryLevel::SERIES_LEVEL:
            return "SERIES";
            
        case QueryLevel::IMAGE_LEVEL:
            return "IMAGE";
        
        case QueryLevel::QUERY_RETRIEVE_LEVEL:
            return "QUERY_RETRIEVE";
    }
    
    return "";
}


DcmDataset* DcmQuery::toDataset()
{
    DcmDataset* dsQuery = new DcmDataset();
    OFIterator<DcmElement*> itEnd = this->m_query.end();
    for (OFIterator<DcmElement*> it = this->m_query.begin(); it != itEnd; it++)
        dsQuery->insert((*it));
    
    DcmCodeString* queryLevel = new DcmCodeString(DCM_QueryRetrieveLevel);
    queryLevel->putString(this->queryLevel2Str(this->m_level).c_str());
    dsQuery->insert(queryLevel, true);
    
    return dsQuery;
}

DcmQuery::~DcmQuery()
{
    this->m_query.clear();
}


