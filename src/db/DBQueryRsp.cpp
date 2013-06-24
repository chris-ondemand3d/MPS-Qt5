/*
 *
 */

#include "DBQueryRsp.h"
#include "DBManager.h"



DBQueryRsp::DBQueryRsp(unique_ptr<mongo::DBClientCursor>& cursor)
{
    this->m_cursor = std::move(cursor);
}

bool DBQueryRsp::next(mongo::BSONObj& bsonObject)
{
    if (this->m_cursor.get() != nullptr &&
        this->m_cursor->more())
    {
        bsonObject = this->m_cursor->next();
        return true;
    }
    
    return false;
}

DcmDataset* DBQueryRsp::nextDcmQR(mongo::BSONObj& bsonObject)
{
    if (this->m_cursor.get() != nullptr &&
        this->m_cursor->more())
    {
        bsonObject = this->m_cursor->next();
        return DBManager::bson2DcmDataset(bsonObject);
    }
    
    return nullptr;
}

bool DBQueryRsp::hasNext()
{
    return this->m_cursor->more();
}


DBQueryRsp::~DBQueryRsp()
{
    this->m_cursor.release();
}
