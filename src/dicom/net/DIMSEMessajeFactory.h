#ifndef DIMSEMESSAJEFACTORY_H
#define DIMSEMESSAJEFACTORY_H

#include <dcmtk/dcmnet/dimse.h>

#include <utils/Status.h>


class DIMSEMessajeFactory
{
public:
    static Status newCEchosRSP(T_DIMSE_Message* msg, DIC_US msgIDToRespond);
    static Status newCEchosRQ(T_DIMSE_Message* msg, DIC_US msgID);
    
    static Status newCStoreRQ(T_DIMSE_Message* msg, 
                              DIC_US msgID,
                              char* affectedSOPClassUID,
                              char* affectedSOPinstanceUID);
    static Status newCStoreRSP(T_DIMSE_Message* msg, 
                               DIC_US msgIDToRespond,
                               char* affectedSOPClassUID,
                               char* affectedSOPinstanceUID);
    
    static Status newCFindRQ(T_DIMSE_Message* msg, 
                             DIC_US msgID,
                             char* affectedSOPClassUID);
    static Status newCFindRSP(T_DIMSE_Message* msg, 
                              DIC_US msgIDToRespond,
                              char* affectedSOPClassUID,
                              DIC_US status);
    
    static Status newCMoveRQ(T_DIMSE_Message* msg, 
                             DIC_US msgID,
                             char* affectedSOPClassUID,
                             char* moveDestination);
    static Status newCMoveRSP(T_DIMSE_Message* msg, 
                              DIC_US msgIDToRespond,
                              char* affectedSOPClassUID,
                              DIC_US status,
                              DIC_US numberOfCompletedSubOperations,
                              DIC_US numberOfFailedSubOperations,
                              DIC_US numberOfRemainingSubOperations,
                              DIC_US numberOfWarningSubOperations);
};
#endif