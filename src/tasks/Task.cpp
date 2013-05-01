#include "Task.h"
#include <utils/Singleton.h>
#include <system/SystemManager.h>


Task::Task(Task::TaskType taskType, QObject* parent): QThread(parent)
{
    this->m_type = taskType;
    TaskManager* taskManager = Singleton<SystemManager>::instance()->mpsSystem()->taskManager();
    this->connect(this, &Task::started, taskManager, &TaskManager::addTask, Qt::UniqueConnection);
    this->connect(this, &Task::finished, taskManager, &TaskManager::removeTask, Qt::UniqueConnection);
}


Task::~Task()
{

}

