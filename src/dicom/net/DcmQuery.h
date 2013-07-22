#ifndef DCMQUERY_H
#define DCMQUERY_H

#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcelem.h>

using namespace std;


enum class QueryLevel 
{
    PATIENT_LEVEL = 0,
    STUDY_LEVEL = 1,
    SERIES_LEVEL = 2,
    IMAGE_LEVEL = 3,
    QUERY_RETRIEVE_LEVEL = 4,
    INVALID_VALUE= 10000
};

class DcmQuery
{
private:
    OFList<DcmElement*> m_query;
    QueryLevel m_level;
    
    
public:
    DcmQuery();
    DcmQuery(DcmDataset* ds);
    DcmDataset* toDataset();
    inline void setQueryLevel(QueryLevel level){this->m_level = level;}
    inline QueryLevel queryLevel() const {return this->m_level;}
    void addKey(const DcmElement* key);
    static bool validateDcmQuery(DcmDataset* ds);
    static string queryLevel2Str(QueryLevel queryLevel);
    static QueryLevel str2QueryLevel(string queryLevel);
    bool isGood(); // this method validate the query object
    virtual ~DcmQuery();
};

#endif // DCMQUERY_H
