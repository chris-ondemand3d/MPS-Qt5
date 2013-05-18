/*
 *
 */

#include "DBDcmTagTable.h"
#include <QVariant>

DBDcmTagTable::DBDcmTagTable()
{
    // Patient Level
    this->m_table["(0010,0010)"] = DcmQuery::PATIENT_LEVEL;
    this->m_table["(0010,0020)"] = DcmQuery::PATIENT_LEVEL;
    this->m_table["(0010,0030)"] = DcmQuery::PATIENT_LEVEL;
    this->m_table["(0010,0040)"] = DcmQuery::PATIENT_LEVEL;
    
    // Study Level
    this->m_table["(0008,0020)"] = DcmQuery::STUDY_LEVEL;
    this->m_table["(0008,0030)"] = DcmQuery::STUDY_LEVEL;
    this->m_table["(0008,0050)"] = DcmQuery::STUDY_LEVEL;
    this->m_table["(0008,0090)"] = DcmQuery::STUDY_LEVEL;
    this->m_table["(0008,1030)"] = DcmQuery::STUDY_LEVEL;
    this->m_table["(0020,000D)"] = DcmQuery::STUDY_LEVEL;
    this->m_table["(0020,000d)"] = DcmQuery::STUDY_LEVEL;
    this->m_table["(0020,0010)"] = DcmQuery::STUDY_LEVEL;
    
    // Series Level
    this->m_table["(0008,0060)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0008,0021)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0008,0031)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0008,0070)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0008,0080)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0008,1010)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0008,1050)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0008,1090)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0020,000E)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0020,000e)"] = DcmQuery::SERIES_LEVEL;
    this->m_table["(0020,0011)"] = DcmQuery::SERIES_LEVEL;
    
}

DcmQuery::QueryLevel DBDcmTagTable::getTagLevel(const QString& tag)
{
    if (this->m_table.contains(tag))
        return this->m_table[tag];
    
    return DcmQuery::IMAGE_LEVEL;
}
