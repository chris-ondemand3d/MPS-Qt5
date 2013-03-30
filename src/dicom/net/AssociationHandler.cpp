#include "AssociationHandler.h"
#include <system/MPSSystem.h>

AssociationHandler::AssociationHandler(DcmAET* aet,  QObject* parent): QThread(parent)
{
    this->m_aet = aet;
}

void AssociationHandler::startSCP()
{
    T_ASC_Network* network;
    // Initializing network
    if (ASC_initializeNetwork(NET_ACCEPTOR, this->m_aet->port(), 0, &network).good())
    {
        T_ASC_Association* assoc;
        if (ASC_receiveAssociation(network, &assoc, ASC_DEFAULTMAXPDU).good())
        {
            // Notifying to all the system that a new association was arrived
            SystemManager::instance()->m_system->emitAssReceived();
            // Checking the presentation context
            
            // Checking connection parameters like AET
            char* remoteAET = assoc->params->DULparams.calledAPTitle;
            AetTableManager::PermissionList permissions = AetTableManager::instance()->getPermissions(remoteAET);
            AetTableManager::PermissionList::iterator it = permissions.begin(), itEnd = permissions.end();
            
            if (permissions.count() == 1 && !permissions.contains(AetTableManager::NO_PERMISSION))
            {
                T_ASC_RejectParameters rejParamns;
                rejParamns.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
                rejParamns.result = ASC_RESULT_REJECTEDPERMANENT;
                rejParamns.source = ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED;
                ASC_rejectAssociation(assoc, &rejParamns);
                // TODO: Throw an exception to notify that was an error with this association
            }
            
            // Everything is fine
            else
            {
                int countPC = ASC_countPresentationContexts(assoc->params);            
                T_ASC_PresentationContext* pc = NULL;
                for (int i = 0; i < countPC; i++)
                {
                    ASC_getPresentationContext(assoc->params, i, pc);
                    
                    if (dcmIsaStorageSOPClassUID(pc->abstractSyntax))
                    {
                        this->acceptPC(assoc, pc);
                        
                        // Accepting association
                        ASC_acknowledgeRelease(assoc);
                        
                        // Waiting for commands
                        this->C_STORE_RQ(assoc);
                    }
                    else
                    {
                        // Checking that is a Verification Service Class
                        if (strcmp(pc->abstractSyntax, UID_VerificationSOPClass) == 0)
                        {
                            this->acceptPC(assoc, pc);
                            
                            // Accepting association
                            ASC_acknowledgeRelease(assoc);
                            
                            // Waiting for commands
                            this->C_ECHO_SCP(assoc);
                        }
                        
                        else
                        {
                            T_ASC_RejectParameters rejParamns;
                            rejParamns.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
                            rejParamns.result = ASC_RESULT_REJECTEDPERMANENT;
                            rejParamns.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
                            
                            ASC_rejectAssociation(assoc, &rejParamns);
                        }
                    }
                }
            }
        }
        else;
            // TODO: Notify that was an error receiving the association
    }
    else;
        // TODO: Notify that the network is not reaady
    T_ASC_Association* assoc;
}

void AssociationHandler::acceptPC(T_ASC_Association* assoc, T_ASC_PresentationContext* pc)
{
    for (int i = 0; i < pc->transferSyntaxCount; i++)
        ASC_acceptPresentationContext(assoc->params, pc->presentationContextID, pc->proposedTransferSyntaxes[i]);
}



void AssociationHandler::C_ECHO_SCP(T_ASC_Association* assoc)
{
    T_DIMSE_Message msg;
    T_ASC_PresentationContextID idPC;
    DcmDataset* statusDetail = NULL;
    OFCondition cond;
    while(true)
    {
        cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 10, &idPC, &msg, &statusDetail);
        if (cond == DUL_PEERREQUESTEDRELEASE)
        {
            ASC_acknowledgeRelease(assoc);
            // TODO: Notify that association was released
        }
        else if (cond != EC_Normal)
        {
            // TODO: Notify that peer was aborted the association
            break;
        }
        
        else if (cond == EC_Normal)
        {
            T_ASC_PresentationContext pc;
            ASC_findAcceptedPresentationContext(assoc->params, idPC, &pc);
            if (strcmp(pc.abstractSyntax, UID_VerificationSOPClass) == 0)
            {
                T_DIMSE_Message msg;
                
                strcpy(msg.msg.CEchoRSP.AffectedSOPClassUID, UID_VerificationSOPClass);
                msg.msg.CEchoRSP.DataSetType = DIMSE_DATASET_NULL;
                msg.msg.CEchoRSP.MessageIDBeingRespondedTo = assoc->nextMsgID;
                msg.msg.CEchoRSP.DimseStatus = STATUS_Success;
                
                // Checking that the message was sent successfully
                if (DIMSE_sendMessageUsingMemoryData(assoc, idPC, &msg, NULL, NULL, NULL, NULL, NULL).bad())
                {
                    // TODO: Notify that was an error sending the C-ECHO-RSP
                    ASC_abortAssociation(assoc);
                }
            }
            else
            {
                // Bad SOP Class Command
                T_ASC_RejectParameters rejParamns;
                rejParamns.reason = ASC_REASON_SP_ACSE_NOREASON;
                rejParamns.result = ASC_RESULT_REJECTEDPERMANENT;
                rejParamns.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
                
                ASC_rejectAssociation(assoc, &rejParamns);
                break;
            }
        }
        else
        {
            
        }
    }    
    ASC_destroyAssociation(&assoc);
}

void AssociationHandler::C_STORE_RQ(T_ASC_Association* assoc)
{

}


void AssociationHandler::run()
{
    this->startSCP();
}

AssociationHandler::~AssociationHandler()
{

}