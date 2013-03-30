#ifndef FILESMANAGER_H
#define FILESMANAGER_H

#include <QDirIterator>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <utils/Status.h>

class FileManager
{
private:
    QDirIterator* m_itDir;
    QString m_dirname;
    quint64 m_countDcmFiles;
    quint64 m_countOtherFiles;

public:
    FileManager(const QString dirname);
    inline QString dirname() {return this->m_dirname;}
    void setDirname(const QString& dirname);
    void reset();
    Status nextDcmFile(DcmFileFormat** dcmFile);
    virtual ~FileManager();
};

#endif // FILESMANAGER_H
