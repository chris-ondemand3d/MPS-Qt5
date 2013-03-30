#ifndef DCMQUERY_H
#define DCMQUERY_H

#include <dcmtk/dcmdata/dcdatset.h>


class DcmQuery
{
public:
    typedef enum {
        PATIENT_LEVEL,
        STUDY_LEVEL,
        SERIES_LEVEL,
        IMAGE_LEVEL
    } QueryLevel;       

private:
    OFList<DcmElement*> m_query;
    QueryLevel m_level;
    char* queryLevel2Str(QueryLevel queryLevel);
    
public:
    DcmQuery();
    DcmDataset* toDataset();
    inline void setQueryLevel(QueryLevel level){this->m_level = level;}
    inline QueryLevel queryLevel(){return this->m_level;}
    void addKey(const DcmElement* key);
    virtual ~DcmQuery();
};

#endif // DCMQUERY_H
