#include "TaskFactory.h"
#include "TaskDcmSCP.h"

Task* TaskFactory::newDcmSCPInstance(DcmNetSCP* scp)
{
    return new TaskDcmSCP(scp);
}


Task* TaskFactory::newEchoSCPTask(DcmNetSCP* scp, 
                                            T_ASC_Association* assoc, 
                                            T_ASC_PresentationContextID idPC)
{
    return new TaskEchoSCP(scp, assoc, idPC);
}

Task* TaskFactory::newEchoSCUTask(DcmNetSCU* scu, 
                                            DcmAET* remoteAET)
{
    return new TaskEchoSCU(scu, remoteAET);
}

Task* TaskFactory::newQRFindSCP(DcmNetSCP* scp, 
                                          T_ASC_Association* assoc, 
                                          T_ASC_PresentationContextID idPC)
{
    return new TaskQRFindSCP(scp, assoc, idPC);    
}

Task* TaskFactory::newQRFindSCU(DcmNetSCU* scu, 
                                          DcmAET* remoteAET, 
                                          DcmQuery* query)
{
    return new TaskQRFindSCU(scu, remoteAET, query);
}

Task* TaskFactory::newQRMoveSCP(DcmNetSCP* scp, 
                                          T_ASC_Association* assoc, 
                                          T_ASC_PresentationContextID idPC)
{
    return new TaskQRMoveSCP(scp, assoc, idPC);
}

Task* TaskFactory::newQRMoveSCU(DcmNetSCU* scu, 
                                          DcmAET* findAET, 
                                          DcmAET* moveAET, 
                                          DcmQuery* query, 
                                          char* saveFolder)
{
    return new TaskQRMoveSCU(scu, findAET, moveAET, query, saveFolder);
}


Task* TaskFactory::newStoreSCP(DcmNetSCP* scp,
                                         T_ASC_Association* assoc, 
                                         T_ASC_PresentationContextID idPC)
{
    return new TaskStoreSCP(scp, assoc, idPC);
}

Task* TaskFactory::newHandleRqDCMConn(DcmNetSCP* scp, 
                                                T_ASC_Network* net, 
                                                T_ASC_Association* assoc)
{
    return new TaskHandleRqDCMConn(scp, net, assoc);
}

Task* TaskFactory::newStoreSCU(DcmNetSCU* scu, 
                                         DcmAET* remoteAET, 
                                         FileManager* fileManager)
{

}