/*
 *
 */

#ifndef DBQUERYRSP_H
#define DBQUERYRSP_H

#include <mongo/client/dbclient.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <db/DBManager.h>


using namespace std;


class DBQueryRsp
{
protected:
    unique_ptr<mongo::DBClientCursor> m_cursor;
    
public:
    DBQueryRsp( unique_ptr< mongo::DBClientCursor >& cursor);
    bool next(mongo::BSONObj& bsonObject);
    DcmDataset* nextDcmQR(mongo::BSONObj& bsonObject);
    bool hasNext();
    
    ~DBQueryRsp();
};

#endif // DBRESPONSE_H
