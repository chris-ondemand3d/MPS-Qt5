/*
 *
 */

#ifndef DBDCMTAGTABLE_H
#define DBDCMTAGTABLE_H

#include <QHash>
#include <dcmtk/ofstd/ofstring.h>
#include <dicom/net/DcmQuery.h>
#include <utils/Singleton.h>
using namespace std;

class DBDcmTagTable : public Singleton<DBDcmTagTable>
{
    friend class Singleton;
private:
    QHash<QString, DcmQuery::QueryLevel> m_table;
    DBDcmTagTable();
    
public:
    DcmQuery::QueryLevel getTagLevel(const QString& tag);
    
};

#endif // DBDCMTAGTABLE_H
