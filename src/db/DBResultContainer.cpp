/*
 *
 */

#include "DBResultContainer.h"

DBResultContainer::DBResultContainer(void* dataValue, DBResultContainerType type)
{
    switch(type)
    {
        case DBResultContainerType::MONGO_BSON_OBJECT:
        {
            this->m_data.bsonObject = (mongo::BSONObj*)dataValue;
            break;
        }
        case DBResultContainerType::DCMTK_DATASET:
        {
            this->m_data.ds = (DcmDataset*)dataValue;
            break;
        }
        case DBResultContainerType::DB_QR_MOVE:
        {
            this->m_data.moveDcmFile = (DBQRMoveContainer*)dataValue;
            break;
        }
    }
    
    this->m_type = type;
}

void* DBResultContainer::value()
{
    switch(this->m_type)
    {
        case DBResultContainerType::DCMTK_DATASET:
            return this->m_data.ds;
            
        case DBResultContainerType::MONGO_BSON_OBJECT:
            return this->m_data.bsonObject;
            
        case DBResultContainerType::DB_QR_MOVE:
            return this->m_data.moveDcmFile;
    }
    
    return nullptr;
}


DBResultContainer::~DBResultContainer()
{

}
