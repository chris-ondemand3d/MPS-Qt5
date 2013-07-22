
#ifndef DBRESULTCONTAINER_H
#define DBRESULTCONTAINER_H

#include <dcmtk/dcmdata/dcdatset.h>
#include <mongo/bson/bson.h>

enum class DBResultContainerType
{
    DCMTK_DATASET,
    MONGO_BSON_OBJECT,
    INVALID,
};

union DBResultData
{
    DcmDataset* ds = nullptr;
    mongo::BSONObj* bsonObject;
};

class DBResultContainer
{
    
private:
    DBResultContainerType m_type;
    DBResultData m_data;
    
public:
    DBResultContainer(void* dataValue, DBResultContainerType type);
    void* value();
    inline DBResultContainerType  type () const {return this->m_type;}
    ~DBResultContainer();

};

#endif // DBRESULTCONTAINER_H
