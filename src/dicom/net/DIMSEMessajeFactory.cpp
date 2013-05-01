#include "DIMSEMessajeFactory.h"

Status DIMSEMessajeFactory::newCEchosRSP(T_DIMSE_Message* msg, DIC_US msgIDToRespond)
{
    
    if (msgIDToRespond < 0 || msgIDToRespond % 2 == 0)
        return Status(Status::Error, "Bad Message ID.");
    
    // Everything is fine
    strcpy(msg->msg.CEchoRSP.AffectedSOPClassUID, UID_VerificationSOPClass);
    msg->CommandField = DIMSE_C_ECHO_RSP;
    msg->msg.CEchoRSP.DataSetType = DIMSE_DATASET_NULL;
    msg->msg.CEchoRSP.DimseStatus = STATUS_Success;
    msg->msg.CEchoRSP.MessageIDBeingRespondedTo = msgIDToRespond;
    
    return Status();
}

Status DIMSEMessajeFactory::newCEchosRQ(T_DIMSE_Message* msg, DIC_US msgID)
{
    if (msgID < 0 || msgID % 2 == 0)
        return Status(Status::Error, "Bad Message ID.");
    
    // Everything is fine
    strcpy(msg->msg.CEchoRQ.AffectedSOPClassUID, UID_VerificationSOPClass);
    msg->msg.CEchoRQ.DataSetType = DIMSE_DATASET_NULL;
    msg->msg.CEchoRQ.MessageID = msgID;
    msg->CommandField = DIMSE_C_ECHO_RQ;
    
    return Status();
}

Status DIMSEMessajeFactory::newCStoreRQ(T_DIMSE_Message* msg, 
                                        DIC_US msgID,
                                        char* affectedSOPClassUID,
                                        char* affectedSOPinstanceUID)
{
    if (msgID < 0 || msgID % 2 == 0)
        return Status(Status::Error, "Bad Message ID.");
    if (affectedSOPClassUID == NULL || affectedSOPinstanceUID == NULL ||
        strlen(affectedSOPClassUID) == 0 || strlen(affectedSOPinstanceUID)  == 0)
        return Status(Status::Error, "Bad SOP Class UID or SOP Instance UID.");
    
    // Everything is fine
    strcpy(msg->msg.CStoreRQ.AffectedSOPClassUID, affectedSOPClassUID);
    strcpy(msg->msg.CStoreRQ.AffectedSOPInstanceUID, affectedSOPinstanceUID);
    msg->msg.CStoreRQ.MessageID = msgID;
    msg->msg.CStoreRQ.DataSetType = DIMSE_DATASET_PRESENT;
    msg->msg.CStoreRQ.Priority = DIMSE_PRIORITY_MEDIUM;
    msg->CommandField = DIMSE_C_STORE_RQ;
    
    return Status();
}

Status DIMSEMessajeFactory::newCStoreRSP(T_DIMSE_Message* msg, 
                                         DIC_US msgIDToRespond, 
                                         char* affectedSOPClassUID, 
                                         char* affectedSOPinstanceUID)
{
    if (affectedSOPClassUID == NULL || affectedSOPinstanceUID == NULL ||
        strlen(affectedSOPClassUID) == 0 || strlen(affectedSOPinstanceUID)  == 0)
        return Status(Status::Error, "Bad SOP Class UID or SOP Instance UID.");
    
    // Everything is fine
    strcpy(msg->msg.CStoreRSP.AffectedSOPClassUID, affectedSOPClassUID);
    strcpy(msg->msg.CStoreRSP.AffectedSOPInstanceUID, affectedSOPinstanceUID);
    msg->msg.CStoreRSP.DataSetType = DIMSE_DATASET_NULL;
    msg->msg.CStoreRSP.DimseStatus = STATUS_Success;
    msg->msg.CStoreRSP.MessageIDBeingRespondedTo = msgIDToRespond;
    msg->CommandField = DIMSE_C_STORE_RSP;
    msg->msg.CStoreRSP.MessageIDBeingRespondedTo = msgIDToRespond;
    
    return Status();
}

Status DIMSEMessajeFactory::newCFindRQ(T_DIMSE_Message* msg, DIC_US msgID, char* affectedSOPClassUID)
{
    if (msgID < 0 || msgID % 2 == 0)
        return Status(Status::Error, "Bad Message ID.");
    if (affectedSOPClassUID == NULL || strlen(affectedSOPClassUID) == 0)
        return Status(Status::Error, "Bad SOP Class UID or SOP Instance UID.");
    
    // Everything is fine
    strcpy(msg->msg.CFindRQ.AffectedSOPClassUID, affectedSOPClassUID);
    msg->msg.CFindRQ.DataSetType = DIMSE_DATASET_PRESENT;
    msg->msg.CFindRQ.MessageID = msgID;
    msg->msg.CFindRQ.Priority = DIMSE_PRIORITY_MEDIUM;
    msg->CommandField = DIMSE_C_FIND_RQ;
    
    return Status();
}

Status DIMSEMessajeFactory::newCFindRSP(T_DIMSE_Message* msg, 
                                        DIC_US msgIDToRespond, 
                                        char* affectedSOPClassUID, 
                                        DIC_US status)
{
    if (msgIDToRespond < 0 || msgIDToRespond % 2 == 0)
        return Status(Status::Error, "Bad Message ID.");
    if (affectedSOPClassUID == NULL || strlen(affectedSOPClassUID) == 0)
        return Status(Status::Error, "Bad SOP Class UID or SOP Instance UID.");
    
    // Everything is find
    strcpy(msg->msg.CFindRSP.AffectedSOPClassUID, affectedSOPClassUID);
    msg->msg.CFindRSP.DataSetType = DIMSE_DATASET_PRESENT;
    msg->msg.CFindRSP.DimseStatus = status;
    msg->msg.CFindRSP.MessageIDBeingRespondedTo = msgIDToRespond;
    msg->CommandField = DIMSE_C_FIND_RSP;
    
    return Status();
}


Status DIMSEMessajeFactory::newCMoveRQ(T_DIMSE_Message* msg, 
                                       DIC_US msgID, 
                                       char* affectedSOPClassUID, 
                                       char* moveDestination)
{
    if (msgID < 0 || msgID % 2 == 0)
        return Status(Status::Error, "Bad Message ID.");
    if (affectedSOPClassUID == NULL || strlen(affectedSOPClassUID) == 0)
        return Status(Status::Error, "Bad SOP Class UID or SOP Instance UID.");
    
    // Everything is fine
    strcpy(msg->msg.CMoveRQ.AffectedSOPClassUID, affectedSOPClassUID);
    strcpy(msg->msg.CMoveRQ.MoveDestination, moveDestination);
    msg->msg.CMoveRQ.DataSetType = DIMSE_DATASET_PRESENT;
    msg->msg.CMoveRQ.MessageID = msgID;
    msg->msg.CMoveRQ.Priority = DIMSE_PRIORITY_MEDIUM;
    msg->CommandField = DIMSE_C_MOVE_RQ;
    
    return Status();
}


Status DIMSEMessajeFactory::newCMoveRSP(T_DIMSE_Message* msg, 
                                        DIC_US msgIDToRespond, 
                                        char* affectedSOPClassUID, 
                                        DIC_US status, 
                                        DIC_US numberOfCompletedSubOperations, 
                                        DIC_US numberOfFailedSubOperations, 
                                        DIC_US numberOfRemainingSubOperations, 
                                        DIC_US numberOfWarningSubOperations)
{
    if (msgIDToRespond < 0 || msgIDToRespond % 2 == 0)
        return Status(Status::Error, "Bad Message ID.");
    if (affectedSOPClassUID == NULL || strlen(affectedSOPClassUID) == 0)
        return Status(Status::Error, "Bad SOP Class UID or SOP Instance UID.");
    
    // Everything is find
    strcpy(msg->msg.CMoveRSP.AffectedSOPClassUID, affectedSOPClassUID);
    msg->msg.CMoveRSP.DataSetType = DIMSE_DATASET_NULL;
    msg->msg.CMoveRSP.DimseStatus = status;
    msg->msg.CMoveRSP.MessageIDBeingRespondedTo = msgIDToRespond;
    msg->msg.CMoveRSP.NumberOfCompletedSubOperations = numberOfCompletedSubOperations;
    msg->msg.CMoveRSP.NumberOfFailedSubOperations = numberOfFailedSubOperations;
    msg->msg.CMoveRSP.NumberOfRemainingSubOperations = numberOfRemainingSubOperations;
    msg->msg.CMoveRSP.NumberOfWarningSubOperations = numberOfWarningSubOperations;
    msg->CommandField = DIMSE_C_MOVE_RSP;
    
    return Status();
}
