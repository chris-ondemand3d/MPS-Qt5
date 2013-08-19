#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include <tasks/Task.h>
#include <tasks/TaskStoreSCP.h>
#include <tasks/TaskEchoSCU.h>
#include <tasks/TaskStoreSCU.h>
#include <tasks/TaskQRFindSCU.h>
#include <tasks/TaskQRMoveSCU.h>
#include <tasks/TaskEchoSCP.h>
#include <tasks/TaskHandleRqDCMConn.h>
#include <tasks/TaskQRFindSCP.h>
#include <dicom/net/DcmNetSCU.h>
#include <tasks/TaskQRMoveSCP.h>

class TaskFactory
{
public:
    // DICOM SCU Services
    static Task* newEchoSCUTask(DcmNetSCU* scu, DcmAET* remoteAET);
    static Task* newStoreSCU(DcmNetSCU* scu, DcmAET* remoteAET, FileManager* fileManager);
    static Task* newQRFindSCU(DcmNetSCU* scu, DcmAET* remoteAET, DcmQuery* query);
    static Task* newQRMoveSCU(DcmNetSCU* scu, DcmAET* findAET, 
                              DcmAET* moveAET, DcmQuery* query, char* saveFolder);
    
    // DICOM SCP Services
    static Task* newEchoSCPTask(DcmNetSCP* scp, T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    static Task* newStoreSCP(DcmNetSCP* scp, T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    static Task* newQRFindSCP(DcmNetSCP* scp, T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    static Task* newQRMoveSCP(DcmNetSCP* scp, T_ASC_Association* assoc, T_ASC_PresentationContextID idPC, string moveDestination);
    
    static Task* newHandleRqDCMConn(DcmNetSCP* scp, T_ASC_Network* net, T_ASC_Association* assoc);
    
    // DICOM SCP Start
    static Task* newDcmSCPInstance(DcmNetSCP* scp);
};

#endif // TASKFACTORY_H
