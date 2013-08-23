/*
 *
 */

#ifndef DBQUERYRSP_H
#define DBQUERYRSP_H

#include <mongo/client/dbclient.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <db/DBManager.h>
#include "DBResultContainer.h"


using namespace std;

enum class DBQueryType
{
    DICOM_QR_FIND,
    DICOM_QR_MOVE,
};

class DcmMoveQueryData
{
private:
    QueryLevel m_qeryLevel;
    string m_studyInstanceUID;
    string m_seriesInstancesUID;
    string m_sopInstancesUID;
    
public:
    DcmMoveQueryData(QueryLevel queryLevel, 
                     string studyInstancesUID = "",
                     string seriesInstancesUID = "",
                     string sopInstancesUID = "");
    
    inline string studyInstancesUID() const {return this->m_studyInstanceUID;}
    inline string seriesInstancesUID() const {return this->m_seriesInstancesUID;}
    inline string sopInstancesUID() const {return this->m_sopInstancesUID;}
    inline QueryLevel queryLevel() const {return this->m_qeryLevel;}
    
    ~DcmMoveQueryData();
};

class DBQueryRsp
{
protected:
    auto_ptr<mongo::DBClientCursor> m_cursor;
    QueryLevel m_dcmQueryLevel;
    int m_seriesPos;
    int m_instancePos;
    mongo::BSONObj* m_currentResult;
    DcmMoveQueryData* m_moveData;
    DBQueryType m_type;
    
    bool getNext();
    void seriesLvl2Dcm(DcmDataset** ds);
    void instanceLvl2Dcm(DcmDataset** ds);
    void studyLvl2Dcm(DcmDataset** ds);
    void arrayBson2Dcm(vector<mongo::BSONElement>& bsonArr, DcmDataset** ds, int pos);
public:
    DBQueryRsp( auto_ptr< mongo::DBClientCursor >& cursor, 
                QueryLevel queryLevel, 
                DBQueryType type,
                DcmMoveQueryData* moveData = nullptr);
    DBResultContainer* next();
    inline QueryLevel dcmQueryLevel() const{return this->m_dcmQueryLevel;}
    bool hasNext();
    
    ~DBQueryRsp();
};

#endif // DBRESPONSE_H
