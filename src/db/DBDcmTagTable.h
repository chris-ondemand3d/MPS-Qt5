/*
 *
 */

#ifndef DBDCMTAGTABLE_H
#define DBDCMTAGTABLE_H

#include <QMap>
#include <dcmtk/ofstd/ofstring.h>
#include <dicom/net/DcmQuery.h>
#include <utils/Singleton.h>

using namespace std;


class DBDcmTagTable : public Singleton<DBDcmTagTable>
{
    friend class Singleton;
    
private:
    QMap<string, QueryLevel> m_table;
    DBDcmTagTable();
    
public:
    QueryLevel getTagLevel(const string& tag);
    
};
#endif // DBDCMTAGTABLE_H
