#include "FilesManager.h"
using namespace std;

FileManager::FileManager(const QString dirname)
{
    this->m_dirname = dirname;
    this->m_itDir = new QDirIterator(this->m_dirname, QDirIterator::Subdirectories);
    this->m_countDcmFiles = this->m_countOtherFiles = 0;
}

void FileManager::reset()
{
    delete this->m_itDir;
    this->m_countDcmFiles = this->m_countOtherFiles = 0;
    this->m_itDir = new QDirIterator(this->m_dirname, QDirIterator::Subdirectories);
}

Status FileManager::nextDcmFile(DcmFileFormat** dcmFile)
{
    QString filename;
    while(this->m_itDir->hasNext())
    {
        filename =  this->m_itDir->next();
        if (!this->m_itDir->fileInfo().isFile())
        {
            filename.clear();
            continue;
        }

        // is a file
        *dcmFile = new DcmFileFormat();
        if ((*dcmFile)->loadFile(filename.toLocal8Bit().data()).good())
        {
            cout << "File: " << filename.toLocal8Bit().data();
            return Status(StatusResult::ExistDicomFile, "Success");
        }

        delete *dcmFile;
        filename.clear();
    }
    return Status(StatusResult::NotExistDicomFile, "End of file list.");
}

void FileManager::setDirname(const QString& dirname)
{
    this->m_dirname = dirname;
}

FileManager::~FileManager()
{
    delete this->m_itDir;
}

