#include "TaskQRMoveSCU.h"

TaskQRMoveSCU::TaskQRMoveSCU(DcmNetSCU* scu, 
                                  DcmAET* findAET, 
                                  DcmAET* moveAET, 
                                  DcmQuery* query, 
                                  char* savFolder, 
                                  QObject* parent): 
                                  Task(TaskType::C_MOVE_SCU, parent)
{
    this->m_scu = scu;
    this->m_findAET = findAET;
    this->m_moveAET = moveAET;
    this->m_query = query;
    strcpy(this->m_saveFolder, savFolder);
}

void TaskQRMoveSCU::run()
{
    this->m_scu->cmove_RQ(*(this->m_findAET), 
                          *(this->m_moveAET),
                          *(this->m_query));
}

TaskQRMoveSCU::~TaskQRMoveSCU()
{

}

