/*
 *
 */

#include "DBDcmTagTable.h"
#include <QVariant>

DBDcmTagTable::DBDcmTagTable()
{
    // Patient Level
    this->m_table["(0010,0010)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,0020)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,0030)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,0040)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,0021)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,0032)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,1000)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,1001)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,2160)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0010,4000)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0020,1200)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0020,1202)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0020,1204)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0020,1200)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0020,1204)"] = QueryLevel::PATIENT_LEVEL;
    this->m_table["(0020,1202)"] = QueryLevel::PATIENT_LEVEL;
    
    // Study Level
    this->m_table["(0008,0020)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0008,0030)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0008,0050)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0008,0090)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0008,1030)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0020,000D)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0020,000d)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0020,0010)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0008,1060)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0008,1080)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0010,1010)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0010,1020)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0010,1030)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0010,2180)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0010,21B0)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0020,1070)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0020,1206)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0020,1208)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0020,1206)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0020,1208)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0008,0061)"] = QueryLevel::STUDY_LEVEL;
    this->m_table["(0008,0062)"] = QueryLevel::STUDY_LEVEL;
        
    // Series Level
    this->m_table["(0008,0060)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0008,0021)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0008,0031)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0008,0070)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0008,0080)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0008,1010)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0008,1050)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0008,1090)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0020,000E)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0020,000e)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0020,0011)"] = QueryLevel::SERIES_LEVEL;
    this->m_table["(0020,1209)"] = QueryLevel::SERIES_LEVEL;
    
    // Query/Retrieve Level
    this->m_table["(0008,0052)"] = QueryLevel::QUERY_RETRIEVE_LEVEL;
}

QueryLevel DBDcmTagTable::getTagLevel(const string& tag)
{
    if (this->m_table.contains(tag))
        return this->m_table[tag];
    
    return QueryLevel::IMAGE_LEVEL;
}
