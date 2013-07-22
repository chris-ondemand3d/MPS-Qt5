#include "DcmNetSCP.h"
#include <db/DBQueryRsp.h>
#include <QCryptographicHash>
#include <stdlib.h>




DcmNetSCP::DcmNetSCP ()
{
    this->loadSettings();
    this->m_stop = false;
}

void DcmNetSCP::loadSettings()
{
    // the AET setting is saved like: aet=aet@localhost:port
    MPSSystemSettings* setting = MPSSystemSettings::instance();
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
    MPSSystemSettings::instance()->settings()->sync();
}


Status DcmNetSCP::cechoSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    // Sending response
    cout << "entre a echo scu" << endl;
    T_DIMSE_Message rspMsg;
    Status result = DIMSEMessajeFactory::newCEchosRSP(&rspMsg, assoc->nextMsgID);
    if (result.good())
        DIMSE_sendMessageUsingMemoryData(assoc, idPC, &rspMsg, nullptr, nullptr, nullptr, nullptr);
    
    
    return result;
}

void DcmNetSCP::dcmDataset2BSON(const DcmDataset* ds)
{
    
}


Status DcmNetSCP::cfindSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    T_DIMSE_Message resp;
    OFCondition cond;
    DcmDataset* dsQuery = nullptr;
    Status result;
    DBManager* dbManager = DBManager::instance();
    
    if ((cond = DIMSE_receiveDataSetInMemory(assoc, DIMSE_NONBLOCKING, 5, &idPC, &dsQuery, nullptr, nullptr)).good())
    {
        // validating the query
        dsQuery->print(cout);
        if (!DcmQuery::validateDcmQuery(dsQuery))
            return Status(StatusResult::CFindRSPError, "Bad query received from SCU.");
        
        // Query is fine
        DcmDataset* dsRspQuery;
        OFCondition cond;
        if (dsQuery != nullptr)
        {
            DBQueryRsp* queryRsp = dbManager->dcmFind(dsQuery);
            T_DIMSE_Message* rsp;
            while(queryRsp != nullptr &&  queryRsp->hasNext())
            {
                DBResultContainer* resultContainer = queryRsp->next();
                if (resultContainer->type() == DBResultContainerType::INVALID)
                {
                    delete resultContainer;
                    delete dsQuery;
                    break;
                }
                
                dsRspQuery = (DcmDataset*)resultContainer->value();                
                rsp = new T_DIMSE_Message;
                rsp->CommandField = DIMSE_C_FIND_RSP;
                strcpy(rsp->msg.CFindRSP.AffectedSOPClassUID,assoc->params->theirImplementationClassUID);
                rsp->msg.CFindRSP.DataSetType = DIMSE_DATASET_PRESENT;
                rsp->msg.CFindRSP.DimseStatus = STATUS_Pending;
                rsp->msg.CFindRSP.MessageIDBeingRespondedTo = assoc->nextMsgID;
                
                // Sending response
                cond = DIMSE_sendMessageUsingMemoryData(assoc, idPC, rsp, nullptr, dsRspQuery, nullptr, nullptr, nullptr);
                if (cond.bad())
                {
                    delete rsp;
                    break;
                }
                delete resultContainer;
                delete dsRspQuery;
                delete rsp;
            }
            
            delete queryRsp;
            
            // sending last response
            rsp = new T_DIMSE_Message;
            rsp->CommandField = DIMSE_C_FIND_RSP;
            strcpy(rsp->msg.CFindRSP.AffectedSOPClassUID, assoc->params->theirImplementationClassUID);
            rsp->msg.CFindRSP.DataSetType = DIMSE_DATASET_NULL;
            rsp->msg.CFindRSP.DimseStatus = STATUS_Success;
            rsp->msg.CFindRSP.MessageIDBeingRespondedTo = assoc->nextMsgID;
            cond = DIMSE_sendMessageUsingMemoryData(assoc, idPC, rsp, nullptr, nullptr, nullptr, nullptr);
            
            if (cond.good())
            {
                
            }
            else
                // TODO: notify error sending C-FIND-RSP
                result.setStatus(StatusResult::CFindRSPError, (string)"Error sending C-FIND-RSP: " + cond.text());
        }
        else
            // TODO: notify that the query is empty and that is an error.
            result.setStatus(StatusResult::CFindRSPError, (string)"Query object is nullptr: " + cond.text());
    }
    else
        // TODO: throw exception to notify the error
        result.setStatus(StatusResult::CFindRSPError, (string)"Error receiving the C-FIND-RQ data: " + cond.text());
    
    if (cond.bad())
        ASC_abortAssociation(assoc);
    
    return result;
}

Status DcmNetSCP::cmoveSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    // TODO: To make this, we need the MongoDb databse implementation
}

Status DcmNetSCP::cstoreSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    // receiving dataset
        DcmDataset* ds = nullptr;
        Status result(StatusResult::Success, "Store file Success.");
        OFCondition cond;
        
        if ((cond = DIMSE_receiveDataSetInMemory(assoc, DIMSE_NONBLOCKING, 10, &idPC, &ds, nullptr, nullptr)).good())
        {
            T_DIMSE_Message msg;
            if (ds == nullptr)
            {               
                result.setStatus(StatusResult::Error, (string)"Bad Dataset. Dataset can not be nullptr: " + cond.text());
                DIMSEMessajeFactory::newCStoreRSP(&msg, 
                                                  assoc->nextMsgID++, 
                                                  const_cast<char*>("No SOP Class Affected"), 
                                                  const_cast<char*>("NO SOP Instance Affected"));
                msg.msg.CStoreRSP.DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                DIMSE_sendMessageUsingMemoryData(assoc, idPC, &msg, nullptr, nullptr, nullptr, nullptr);
                return result;
            }
            // Everything is fine
            // Creating the message
            OFString sopClassUID, sopInstanceUID;        
            ds->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
            ds->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
            
            // saving the file
            QDir dir4save(this->m_rootFolder.c_str());
            if (!dir4save.exists())
                dir4save.mkpath(this->m_rootFolder.c_str());
            char* filename = strdup((this->m_rootFolder.c_str() + (string)"/" + (string)sopInstanceUID.c_str() + (string)".dcm").c_str());            
            ds->saveFile(filename);
            SystemManager::instance()->mpsSystem()->dbManager()->store(ds, filename);
            
            delete ds;
            delete [] filename;
            T_DIMSE_Message* rsp = new T_DIMSE_Message;
            DIMSEMessajeFactory::newCStoreRSP(rsp, 
                                              assoc->nextMsgID++,
                                              const_cast<char*>(sopClassUID.c_str()),
                                              const_cast<char*>(sopInstanceUID.c_str()));
            // Sending response messgae
            DIMSE_sendMessageUsingMemoryData(assoc, idPC, rsp, nullptr, nullptr, nullptr, nullptr);
            
            delete rsp;
        }
        
        result.setStatus(StatusResult::CStoreRSPError, (string)"Error waiting the datatset: " + cond.text());
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
        MPSSystemSettings* systemSettings = SystemManager::instance()->mpsSystem()->settings();
        DUL_ASSOCIATESERVICEPARAMETERS* dulAssParams = &(assoc->params->DULparams);
        cout << dulAssParams->callingAPTitle << endl;
        if (!systemSettings->existRemoteAET(dulAssParams->callingAPTitle, dulAssParams->callingPresentationAddress))
        {
            T_ASC_RejectParameters rejParams;            
            rejParams.reason = ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED;
            rejParams.result = ASC_RESULT_REJECTEDPERMANENT;
            rejParams.source = ASC_SOURCE_SERVICEUSER;
            OFCondition cond = ASC_rejectAssociation(assoc, &rejParams);
            cout << cond.text() << endl;
            ASC_dropAssociation(assoc);
//             ASC_destroyAssociation(&assoc);
            DUL_ASSOCIATESERVICEPARAMETERS k = assoc->params->DULparams;
            cout << "association rejected" << endl;
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
            
            cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 10, &idPC, &msgRQ, nullptr, nullptr);
            switch(cond.code())
            {
                case DULC_PEERREQUESTEDRELEASE:
                {
                    ASC_acknowledgeRelease(assoc);
                    ASC_dropSCPAssociation(assoc);
                    cout << "Releasing association...";
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
                    Status* result = new Status;
                    switch(msgRQ.CommandField)
                    {
                        case DIMSE_C_ECHO_RQ:
                        {
                            *result = this->cechoSCP(assoc, idPC);                             
                            break;
                        }
                            
                        case DIMSE_C_FIND_RQ:
                        {
                            *result = this->cfindSCP(assoc, idPC);
                            break;
                        }
                            
                        case DIMSE_C_MOVE_RQ:
                        {
                            *result = this->cmoveSCP(assoc, idPC);
                            break;
                        }
                            
                        case DIMSE_C_STORE_RQ:
                        {
                            *result = this->cstoreSCP(assoc, idPC);
                            break;
                        }
                    }
                    if (result->status() == StatusResult::AssociationError)
                    {
                        this->abortAssociation(assoc);
                        // TODO: Notify this error to the system.
                        return;
                    }
                }
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
        // TODO: Notify this error
    }
}

void DcmNetSCP::abortAssociation(T_ASC_Association* assoc)
{
    ASC_abortAssociation(assoc);
    ASC_dropAssociation(assoc);
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
        cout << (string)"Error: Network port is already in use: " + cond.text() << endl;
        exit(EXIT_FAILURE);
    }
}
void DcmNetSCP::stop()
{
    
}

DcmNetSCP::~DcmNetSCP()
{
   
}