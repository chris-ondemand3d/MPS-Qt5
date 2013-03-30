#include "DcmQuery.h"
#include <dcmtk/dcmdata/dcelem.h>
#include <dcmtk/dcmdata/dcvrcs.h>
#include <dcmtk/dcmdata/dcdeftag.h>

DcmQuery::DcmQuery()
{
    
}

void DcmQuery::addKey(const DcmElement* key)
{
    if (key != NULL)
        this->m_query.push_back((DcmElement*)key);
}

char* DcmQuery::queryLevel2Str(DcmQuery::QueryLevel queryLevel)
{
    char* result = NULL;
    switch(queryLevel)
    {
        case PATIENT_LEVEL:
            return (char*)"PATIENT";
            
        case STUDY_LEVEL:
            return (char*)"STUDY";
            
        case SERIES_LEVEL:
            return (char*)"SERIES";
            
        case IMAGE_LEVEL:
            return (char*)"IMAGE";
    }
    
    return (char*)"";
}


DcmDataset* DcmQuery::toDataset()
{
    DcmDataset* dsQuery = new DcmDataset();
    OFIterator<DcmElement*> itEnd = this->m_query.end();
    for (OFIterator<DcmElement*> it = this->m_query.begin(); it != itEnd; it++)
        dsQuery->insert((*it));
    
    DcmCodeString* queryLevel = new DcmCodeString(DCM_QueryRetrieveLevel);
    queryLevel->putString(this->queryLevel2Str(this->m_level));
    dsQuery->insert(queryLevel, true);

    return dsQuery;
}

DcmQuery::~DcmQuery()
{

}


