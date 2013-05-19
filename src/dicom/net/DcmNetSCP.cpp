#include "DcmNetSCP.h"
#include <QCryptographicHash>




DcmNetSCP::DcmNetSCP ()
{
    this->loadSettings();
    this->m_stop = false;
}

void DcmNetSCP::loadSettings()
{
    // the AET setting is saved like: aet=aet@localhost:port
    MPSSystemSettings* setting = Singleton<MPSSystemSettings>::instance()->instance();
    string aet, hostname;
    bool valResult;
    int port;
    
    if (setting->hasSetting(MPSSetting_LOCAL_AET))
        this->m_aet.setAet(setting->value(MPSSetting_LOCAL_AET).toString().toStdString());
    else
    {
        setting->settings()->setValue(MPSSetting_LOCAL_AET, "MPS");
        this->m_aet.setAet("MPS");
        setting->settings()->sync();
    }
    
    if (setting->hasSetting(MPSSetting_LOCAL_HOSTNAME))
        this->m_aet.setHostname(setting->value(MPSSetting_LOCAL_HOSTNAME).toString().toStdString());
    else
    {
        setting->settings()->setValue(MPSSetting_LOCAL_HOSTNAME, "localhost");
        this->m_aet.setHostname("localhost");
        setting->settings()->sync();
    }
    
    if (setting->hasSetting(MPSSetting_LOCAL_PORT))
    {
        port = setting->value(MPSSetting_LOCAL_PORT).toInt(&valResult);
        if (port <= 0 && !valResult)
        {
            port = 1104;
            setting->settings()->setValue(MPSSetting_LOCAL_PORT, 1104);
            setting->settings()->sync();
        }        
        this->m_aet.setPort(port);
    }
    else
    {
        port = 1104;
        setting->settings()->setValue(MPSSetting_LOCAL_PORT, 1104);
        setting->settings()->sync();
    }   
    
    if (setting->hasSetting(MPSSetting_LOCAL_SCP_FOLDER))
        this->m_rootFolder = setting->value(MPSSetting_LOCAL_SCP_FOLDER).toString().toStdString();
    else
    {
        QCryptographicHash md5sum(QCryptographicHash::Md5);
        QString datetime = QDateTime::currentDateTime().toString(Qt::ISODate);
        md5sum.addData(datetime.toStdString().c_str(), datetime.size());
        QString homeFolder = QDir::homePath() + QDir::separator() + QString("MPS") + md5sum.result().toHex();
        QDir::home().mkdir(homeFolder);
        this->m_rootFolder = homeFolder.toStdString();
        setting->settings()->setValue(MPSSetting_LOCAL_SCP_FOLDER, this->m_rootFolder.c_str());
        setting->settings()->sync();
    }
}

void DcmNetSCP::saveSettings()
{

}


Status DcmNetSCP::cechoSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    // Sending response
    T_DIMSE_Message rspMsg;
    Status result;
    
    if ((result = DIMSEMessajeFactory::newCEchosRSP(&rspMsg, assoc->nextMsgID)).good())
        DIMSE_sendMessageUsingMemoryData(assoc, idPC, &rspMsg, NULL, NULL, NULL, NULL);
    return result;
}

Status DcmNetSCP::cfindSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    // TODO: To make this, we need the MongoDb databse implementation
}

Status DcmNetSCP::cmoveSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    // TODO: To make this, we need the MongoDb databse implementation
}

Status DcmNetSCP::cstoreSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    // receiving dataset
        DcmDataset* ds = NULL;
        Status result(Status::Succes, "Store file Success.");
        OFCondition cond;
        
        if ((cond = DIMSE_receiveDataSetInMemory(assoc, DIMSE_NONBLOCKING, 10, &idPC, &ds, NULL, NULL)).good())
        {
            T_DIMSE_Message msg;
            if (ds == NULL)
            {
                result.setStatus(Status::Error, (string)"Bad Dataset. Dataset can not be NULL: " + cond.text());
                DIMSEMessajeFactory::newCStoreRSP(&msg, 
                                                  assoc->nextMsgID++, 
                                                  const_cast<char*>("No SOP Class Affected"), 
                                 const_cast<char*>("NO SOP Instance Affected"));
                                 msg.msg.CStoreRSP.DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                                 DIMSE_sendMessageUsingMemoryData(assoc, idPC, &msg, NULL, NULL, NULL, NULL);
                                 return result;
            }
            // Everything is fine
            // Creating the message
            OFString sopClassUID, sopInstanceUID;        
            ds->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
            ds->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
            
            // saving the file
            DcmFileFormat* dcmFile = new DcmFileFormat(ds);
            char* filename = strdup((this->m_rootFolder.c_str() + (string)sopInstanceUID.c_str() + (string)".dcm").c_str());
            ds->saveFile(filename);
            Singleton<SystemManager>::instance()->mpsSystem()->dbManager()->store(ds, filename);
            delete filename;
            delete ds;
            T_DIMSE_Message rsp;
            DIMSEMessajeFactory::newCStoreRSP(&rsp, 
                                              assoc->nextMsgID++,
                                              const_cast<char*>(sopClassUID.c_str()),
                                              const_cast<char*>(sopInstanceUID.c_str()));
            // Sending response messgae
            DIMSE_sendMessageUsingMemoryData(assoc, idPC, &rsp, NULL, NULL, NULL, NULL);
        }
        
        result.setStatus(Status::CStoreRSPError, (string)"Error waiting the datatset: " + cond.text());
}

Status DcmNetSCP::receiveCommand(T_ASC_Association* assoc, T_DIMSE_Message& message)
{

}


void DcmNetSCP::acceptingPresentationContext(T_ASC_Association* assoc)
{
    // Checking the Proposed Presentation Context
    int proposedPCCount = ASC_countPresentationContexts(assoc->params);
    for (int i = 0; i < proposedPCCount; i++)
    {
        T_ASC_PresentationContext pc;
        ASC_getPresentationContext(assoc->params, i, &pc);
        if (dcmIsaStorageSOPClassUID(pc.abstractSyntax))
        {
            /* we are going to accept the first transefer syntax for 
             * each presentation contex
             */ 
            ASC_acceptPresentationContext(assoc->params, 
                                          pc.presentationContextID,
                                          pc.proposedTransferSyntaxes[0]);
        }
        else
        {
            /* 
             * Checking if is Verification SOP Class UID, FINDStudyRootSOPClass,
             * MOVEStudyRootSOPClass, FINDPatientRootSOPClass or MOVEPatientRootSOPClass
             */
            if (strcmp(pc.abstractSyntax, UID_VerificationSOPClass) == 0 ||
                strcmp(pc.abstractSyntax, UID_FINDPatientRootQueryRetrieveInformationModel) == 0 ||
                strcmp(pc.abstractSyntax, UID_FINDStudyRootQueryRetrieveInformationModel) == 0 ||
                strcmp(pc.abstractSyntax, UID_MOVEPatientRootQueryRetrieveInformationModel) == 0 ||
                strcmp(pc.abstractSyntax, UID_MOVEStudyRootQueryRetrieveInformationModel) == 0)
            {
                // Checking the TransferSyntax. It must be Little endian Implicit
                bool accepted = false;
                for (int j = 0; j < pc.transferSyntaxCount; j++)
                {
                    if (strcmp(pc.proposedTransferSyntaxes[j], UID_LittleEndianImplicitTransferSyntax) == 0)
                    {
                        ASC_acceptPresentationContext(assoc->params, 
                                                      pc.presentationContextID, 
                                                      UID_LittleEndianImplicitTransferSyntax, 
                                                      ASC_SC_ROLE_SCP);
                        accepted = true;
                        break;
                    }
                }
                if (!accepted) // Little Endian Implicit was not found
                {
                    ASC_refusePresentationContext(assoc->params, pc.presentationContextID, ASC_P_TRANSFERSYNTAXESNOTSUPPORTED);
                }
            }
        }
    }
}



/*
 * handleIncomingConnection(T_ASC_Network* network, T_ASC_Association* assoc):
 *  This method handle the incoming connections to the server and determine
 *  what service ther server must use, and for each connection the server 
 *  raise one thread to handle the incoming request.
 */
void DcmNetSCP::handleIncomingConnection(T_ASC_Network* network, T_ASC_Association* assoc)
{
    // Checking that called AET is correct
    if (strcmp(assoc->params->DULparams.calledAPTitle, this->m_aet.aet().c_str()) == 0)
    {
        
        // Checking that the calling AET has permission to communicate with our server
        MPSSystemSettings* systemSettings = Singleton<SystemManager>::instance()->mpsSystem()->settings();
        if (!systemSettings->existRemoteAET(assoc->params->DULparams.callingAPTitle))
        {
            T_ASC_RejectParameters rejParams;            
            rejParams.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
            rejParams.result = ASC_RESULT_REJECTEDPERMANENT;
            rejParams.source = ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED;
            ASC_rejectAssociation(assoc, &rejParams);
            ASC_dropSCPAssociation(assoc);
            DUL_ASSOCIATESERVICEPARAMETERS k = assoc->params->DULparams;
            return;
        }    
        
        // Everything is fine
        this->acceptingPresentationContext(assoc);
        
        ASC_acknowledgeAssociation(assoc);
        // waiting for command
        T_ASC_PresentationContextID idPC;
        T_ASC_PresentationContext pc;
        OFCondition cond = EC_Normal;
        T_DIMSE_Message msgRQ;
        
        while(true)
        {
            
            cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 10, &idPC, &msgRQ, NULL, NULL);
            switch(cond.code())
            {
                case DULC_PEERREQUESTEDRELEASE:
                {
                    ASC_acknowledgeRelease(assoc);
                    ASC_dropSCPAssociation(assoc);
                    return;
                }
                case DULC_LISTERROR:
                case DULC_UNEXPECTEDPDU:
                case DULC_UNRECOGNIZEDPDUTYPE:
                case DULC_ILLEGALREQUEST:
                case DULC_WRONGDATATYPE:
                case DULC_READTIMEOUT:
                case DULC_TCPIOERROR:
                case DULC_PEERABORTEDASSOCIATION:
                {
                    cout << "Aborting association: " << cond.text() << endl;
                    ASC_abortAssociation(assoc);
                    ASC_dropSCPAssociation(assoc);
                    return;
                }
                
                default: // everything is fine
                {
                    ASC_findAcceptedPresentationContext(assoc->params, idPC, &pc);
                    
                    switch(msgRQ.CommandField)
                    {
                        case DIMSE_C_ECHO_RQ:
                            this->cechoSCP(assoc, idPC);
                            break;
                            
                        case DIMSE_C_FIND_RQ:
                            this->cfindSCP(assoc, idPC);
                            break;
                            
                        case DIMSE_C_MOVE_RQ:
                            this->cmoveSCP(assoc, idPC);
                            break;
                            
                        case DIMSE_C_STORE_RQ:
                            this->cstoreSCP(assoc, idPC);
                            break;
                    }
                }
                
                //             else
                //             {
                //                 cout << "Condition: " << cond.text() << endl << cond.status() << endl;
                //                 ASC_abortAssociation(assoc);
                //                 ASC_dropSCPAssociation(assoc, 0);
                //                 throw WaitingMessageErrorExcep((string)"Error receiving message from this association: " + cond.text());
                //             }
            }
        }
    }
    else
    {
        T_ASC_RejectParameters rejParams;
        rejParams.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
        rejParams.result = ASC_RESULT_REJECTEDPERMANENT;
        rejParams.source = ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED;
        // Rejecting this association
        ASC_rejectAssociation(assoc, &rejParams);
        ASC_dropSCPAssociation(assoc, 0);
        
        throw CalledAETErrorExcep("Invalid Called AET.");
    }
}



void DcmNetSCP::start()
{
    cout << "probando esta paja " << endl;
    T_ASC_Network* network;
    OFCondition cond;
    cout << "Port number: " << this->m_aet.port() << endl;
    if ((cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, this->m_aet.port(), 0, &network)).good())
    {        
        T_ASC_Association* assoc;
        cout << "Port number: " << this->m_aet.port() << endl;
        while (true)
        {
            if (ASC_receiveAssociation(network, &assoc, ASC_DEFAULTMAXPDU).good())
            {
                // create new handle connection thread
                cout << "request received" << endl; 
                Task* task = TaskFactory::newHandleRqDCMConn(this, network, assoc);                
                task->start();
            }
        }
    }
    else
    {
        cout << (string)"Error: Network is already in use: " + cond.text() << endl;
    }
}
void DcmNetSCP::stop()
{
    
}

DcmNetSCP::~DcmNetSCP()
{
   
}