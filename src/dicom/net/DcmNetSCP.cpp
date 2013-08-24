#include "DcmNetSCP.h"
#include <db/DBQueryRsp.h>
#include <QCryptographicHash>
#include <stdlib.h>
#include <dcmtk/dcmdata/dcvrul.h>
#include <dcmtk/dcmdata/dcvrsh.h>
#include <dcmtk/dcmdata/dcvrae.h>
#include <dcmtk/dcmdata/dcvrui.h>
#include <dcmtk/dcmdata/dcvrobow.h>
#include <dcmtk/dcmdata/dcvrcs.h>
#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmdata/dcrleerg.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmjpeg/djencode.h>
#include <dcmtk/dcmjpls/djdecode.h>
#include <dcmtk/dcmjpls/djencode.h>


DcmNetSCP::DcmNetSCP ()
{
    this->loadSettings();
    this->m_stop = false;
    
    // register codecs
    DcmRLEDecoderRegistration::registerCodecs();
    DcmRLEEncoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs();
    DJEncoderRegistration::registerCodecs();
    DJLSDecoderRegistration::registerCodecs();
    DJLSEncoderRegistration::registerCodecs();
}

void DcmNetSCP::registerCodecs()
{
    
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
    
    this->registerCodecs();
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



Status DcmNetSCP::cfindSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    T_DIMSE_Message resp;
    OFCondition cond;
    DcmDataset* dsQuery = nullptr;
    Status result;
    DBManager* dbManager = DBManager::instance();
    bool canceled = false;
    
    if ((cond = DIMSE_receiveDataSetInMemory(assoc, DIMSE_NONBLOCKING, 5, &idPC, &dsQuery, nullptr, nullptr)).good())
    {
        // validating the query
        if (!DcmQuery::validateDcmQRFindQuery(dsQuery))
        {
            ASC_abortAssociation(assoc);
            return Status(StatusResult::CFindRSPError, "Bad query received from SCU.");
        }
        
        // Query is fine
        DcmDataset* dsRspQuery;
        OFCondition cond;
        if (dsQuery != nullptr)
        {
            DBQueryRsp* queryRsp = dbManager->dcmQRFind(dsQuery);
            T_DIMSE_Message* rsp;
            while(queryRsp != nullptr &&  queryRsp->hasNext() && !canceled)
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
                DIMSEMessajeFactory::newCFindRSP(rsp, 
                                                 assoc->nextMsgID, 
                                                 assoc->params->theirImplementationClassUID,
                                                 DIMSE_DATASET_PRESENT,
                                                 STATUS_Pending);
                // Sending response
                cond = DIMSE_sendMessageUsingMemoryData(assoc, idPC, rsp, nullptr, dsRspQuery, nullptr, nullptr, nullptr);
                delete rsp;
                if (cond.bad())
                {
                    delete rsp;
                    rsp = nullptr;
                    break;
                }
                
                T_DIMSE_Message* msgReceived = nullptr;
                // receiving responses
                cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 0, &idPC, msgReceived, nullptr);
                if (cond.good())
                {
                    if (msgReceived->CommandField == DIMSE_C_CANCEL_RQ)
                    {
                        // canceling C-FIND operation
                        rsp = new T_DIMSE_Message;
                        DIMSEMessajeFactory::newCFindRSP(rsp, 
                                                         assoc->nextMsgID, 
                                                         assoc->params->theirImplementationClassUID,
                                                         DIMSE_DATASET_NULL,
                                                         STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest);
                        
                        DIMSE_sendMessageUsingMemoryData(assoc, idPC, rsp, nullptr, nullptr, nullptr, nullptr);
                        canceled = true;
                        delete rsp;
                        rsp = nullptr;
                        break;
                    }
                }
                
                if (msgReceived != nullptr)
                    delete msgReceived;
                delete resultContainer;
                delete dsRspQuery;
            }
            
            delete queryRsp;
           
            if (!canceled)
            {
                // sending last response
                rsp = new T_DIMSE_Message;
                DIMSEMessajeFactory::newCFindRSP(rsp, 
                                                 assoc->nextMsgID, 
                                                 assoc->params->theirImplementationClassUID,
                                                 DIMSE_DATASET_NULL,
                                                 STATUS_Success);
               
                cond = DIMSE_sendMessageUsingMemoryData(assoc, idPC, rsp, nullptr, nullptr, nullptr, nullptr);
                
                if (cond.bad())
                    // TODO: notify error sending C-FIND-RSP<
                    result.setStatus(StatusResult::CFindRSPError, (string)"Error sending C-FIND-RSP: " + cond.text());
            }
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

Status&& DcmNetSCP::negociateMoveSubOperationPC(T_ASC_Network** network, 
                                                T_ASC_Association** subOpAssoc, 
                                                T_ASC_Parameters* params, 
                                                list< pair< string, string > >& sopClassTSPairs)
{
    Status result;
    T_ASC_PresentationContextID idPC = 1;
    for (auto sopClassPair : sopClassTSPairs)
    {
        char** ts = new char*[1];
        cout << "SOP Class - TS Pair: " << sopClassPair.first << " - " << sopClassPair.second << endl; 
        ts[0] = strdup("1.2.840.10008.1.2");
        ASC_addPresentationContext(params, 
                                   idPC, 
                                   const_cast<char*>(sopClassPair.first.c_str()), 
                                   (const char**)ts, 
                                   1);
        idPC += 2;
        
        delete ts[0];
        delete [] ts;
    }
    
    // negociating the presentation context
    OFCondition cond = ASC_requestAssociation(*network, params, subOpAssoc);
    cout << "Condition result: " << cond.text() << endl;
    int acceptedPC = ASC_countAcceptedPresentationContexts(params);
    if (cond.bad() || acceptedPC == 0)
        result.setStatus(StatusResult::AssociationError, "Error negociating the presentation context for C-MOVE suboperation.");
    
    return std::move(result);
}


Status DcmNetSCP::cmoveSCP(T_ASC_Association* assoc, 
                           T_ASC_PresentationContextID idPC, 
                           string& moveDestination)
{
    Status result;
    T_DIMSE_Message* moveRSP = nullptr;
    T_ASC_PresentationContext pc;
    ASC_findAcceptedPresentationContext(assoc->params, idPC, &pc);
    // checking if the AET for move is defined in the system.
    if (!MPSSystemSettings::instance()->existRemoteAET(const_cast<char*>(moveDestination.c_str())))
    {
        moveRSP = new T_DIMSE_Message;
        DIMSEMessajeFactory::newCMoveRSP(moveRSP,
                                         assoc->nextMsgID,
                                         assoc->params->theirImplementationClassUID,
                                         STATUS_MOVE_Failed_MoveDestinationUnknown,
                                         0,
                                         0,
                                         0,
                                         0);
        
        DIMSE_sendMessageUsingMemoryData(assoc,
                                         idPC,
                                         moveRSP,
                                         nullptr,
                                         nullptr,
                                         nullptr,
                                         nullptr);
        delete moveRSP;
        result.setStatus(StatusResult::CMoveRQError, "Unknow remote AET.");
        return result;
    }
    
    else
    {        
        OFCondition cond;
        DcmDataset* dsQuery = nullptr;
        Status result;
        DBManager* dbManager = DBManager::instance();
        
        if ((cond = DIMSE_receiveDataSetInMemory(assoc, DIMSE_NONBLOCKING, 5, &idPC, &dsQuery, nullptr, nullptr)).good())
        {
            QueryLevel moveQueryLevel = DcmQuery::validateDcmQRMoveQuery(dsQuery);
            if(moveQueryLevel == QueryLevel::INVALID_VALUE)
            {
                result.setStatus(StatusResult::CMoveRSPError, "Bad C-MOVE query.");
                ASC_abortAssociation(assoc);
            }
            else
            {
                // The query is fine
                this->delTagsDiferentToKeyLevels(dsQuery);
                DcmCodeString* queryLevel = new DcmCodeString(DCM_QueryRetrieveLevel);
                queryLevel->putString(DcmQuery::queryLevel2Str(moveQueryLevel).c_str());
                dsQuery->insert(queryLevel, true);
                
                // removing all tags diferent to the key levels
                
                DBQueryRsp* moveQueryResult = dbManager->dcmQRMove(dsQuery);
                moveRSP = new T_DIMSE_Message;
                
                DBResultContainer* resultContainer = moveQueryResult->next();
                if (resultContainer->type() == DBResultContainerType::INVALID)
                {
                    delete resultContainer;
                    delete dsQuery;
                    
                    DIMSEMessajeFactory::newCMoveRSP(moveRSP,
                                                     assoc->nextMsgID,
                                                     assoc->params->theirImplementationClassUID,
                                                     STATUS_MOVE_Failed_UnableToProcess,
                                                     0,
                                                     0,
                                                     0,
                                                     0);
                    
                    DIMSE_sendMessageUsingMemoryData(assoc,
                                                     idPC,
                                                     moveRSP,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr);
                    delete moveRSP;
                    result.setStatus(StatusResult::CMoveRQError, "Unable to process the C-MOVE operation.");
                    return result;
                }
                else // everything is fine
                {
                    // making a store scu for move
                    T_ASC_Association* assocStore = nullptr;
                    T_ASC_Parameters* params = nullptr;
                    ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);
                    ASC_setAPTitles(params,
                                    strdup(this->m_aet.aet().c_str()),
                                    strdup(moveDestination.c_str()),
                                    nullptr);
                    
                    DcmAET* remoteAET = MPSSystemSettings::instance()->aet(const_cast<char*>(moveDestination.c_str()));
                    stringstream remoteAETAddr;
                    remoteAETAddr << remoteAET->hostname() << ":" << remoteAET->port();
                    ASC_setPresentationAddresses(params,
                                                 strdup(this->m_aet.hostname().c_str()),
                                                 strdup(remoteAETAddr.str().c_str()));
                    
                    // initializing network for STORE SCU associationdicomDir
                    T_ASC_Network* storeNetwork = nullptr;
                    if (ASC_initializeNetwork(NET_REQUESTOR, remoteAET->port(), DICOME_NETWORK_TIMEOUT, &storeNetwork).good())
                    {
                        // negociating presentation context for C-STORE-RQ
                        bool canceled = false;
                        DBQRMoveContainer* dbMoveRSP = (DBQRMoveContainer*)resultContainer->value();
                        list<pair<string,string>> sopClassTSPairs = dbMoveRSP->sopClassTSPairs();
                        if (dbMoveRSP->countInstances() > 0 && this->negociateMoveSubOperationPC(&storeNetwork, &assocStore, params, sopClassTSPairs).good())
                        {
                            DcmNetSCU* storeSCU = new DcmNetSCU(this->m_aet);
                            int completeSubOperations = 0;
                            int failedSubOperations = 0;
                            int remainingSubOperations = dbMoveRSP->filenames().size();
                            int warningSubOperations = 0;
                            set<string> filenames = dbMoveRSP->filenames();
                            for (string filename : filenames)
                            {
                                // sending file;T_ASC_PresentationContext pc;
                                ASC_findAcceptedPresentationContext(assoc->params, idPC, &pc);
                                cond = storeSCU->cstore_RQ(*remoteAET, filename, assocStore, pc.presentationContextID, 5);
                                switch(cond.code())
                                {
                                    case STATUS_STORE_Warning_CoercionOfDataElements:
                                    case STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass:
                                    case STATUS_STORE_Warning_ElementsDiscarded:
                                    {
                                        warningSubOperations++;
                                        break;
                                    }
                                    
                                    case STATUS_Success:
                                    {
                                        completeSubOperations++;
                                        break;
                                    }
                                    
                                    default:
                                    {
                                        failedSubOperations++;
                                        break;
                                    }
                                }
                                remainingSubOperations--;
                                moveRSP = new T_DIMSE_Message;
                                DIMSEMessajeFactory::newCMoveRSP(moveRSP,
                                                                 assoc->nextMsgID,
                                                                 assoc->params->theirImplementationClassUID,
                                                                 STATUS_Pending,
                                                                 completeSubOperations,
                                                                 failedSubOperations,
                                                                 remainingSubOperations,
                                                                 warningSubOperations);
                                
                                cond = DIMSE_sendMessageUsingMemoryData(assoc,idPC, moveRSP, nullptr, nullptr, nullptr, nullptr);                                    
                                delete moveRSP;
                                moveRSP = nullptr;
                                
                                // receiving messages
                                if (DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING,0, &idPC, moveRSP, nullptr).good())
                                {
                                    if (moveRSP->CommandField == DIMSE_C_CANCEL_RQ)
                                        // canceling the C-MOVE operation
                                    {
                                        canceled = true;
                                        moveRSP = new T_DIMSE_Message;
                                        
                                        // stoping the store sub-association
                                        ASC_abortAssociation(assocStore);
                                        ASC_dropAssociation(assocStore);
                                        ASC_dropNetwork(&storeNetwork);
                                        
                                        DIMSEMessajeFactory::newCMoveRSP(moveRSP, 
                                                                         assoc->nextMsgID, 
                                                                         assoc->params->theirImplementationClassUID,
                                                                         STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest,
                                                                         completeSubOperations,
                                                                         failedSubOperations,
                                                                         remainingSubOperations,
                                                                         warningSubOperations);
                                        
                                        DIMSE_sendMessageUsingMemoryData(assoc, idPC, moveRSP, nullptr, nullptr, nullptr, nullptr);
                                        result.setStatus(StatusResult::CMoveRSPError, "C-MOVE operation canceled by the peer.");
                                        break;
                                    }
                                }
                                else
                                {
                                    if (moveRSP != nullptr)
                                    {
                                        delete moveRSP;
                                        moveRSP = nullptr;
                                    }
                                }
                            }
                            
                            // sending the STATUS_Success message
                            if (!canceled)
                            {
                                moveRSP = new T_DIMSE_Message;
                                
                                DIMSEMessajeFactory::newCMoveRSP(moveRSP, 
                                                                 assoc->nextMsgID,
                                                                 assoc->params->theirImplementationClassUID,
                                                                 STATUS_Success,
                                                                 completeSubOperations, 
                                                                 failedSubOperations,
                                                                 remainingSubOperations,
                                                                 warningSubOperations);
                                
                                DIMSE_sendMessageUsingMemoryData(assoc, idPC, moveRSP, nullptr, nullptr, nullptr, nullptr);
                                result.setStatus(StatusResult::Success, "MOVE Operation Success");
                                ASC_releaseAssociation(assocStore);
                                
                                delete moveRSP;
                            }
                            
                            delete storeSCU;
                        }
                        else
                        {
                            if (assocStore != nullptr)
                                ASC_dropAssociation(assocStore);                            
                            ASC_dropNetwork(&storeNetwork);
                            
                            T_ASC_PresentationContext pc;
                            ASC_findAcceptedPresentationContext(assoc->params, idPC, &pc);
                            moveRSP = new T_DIMSE_Message;
                            DIMSEMessajeFactory::newCMoveRSP(moveRSP, 
                                                             assoc->nextMsgID,
                                                             assoc->params->theirImplementationClassUID,
                                                             ASC_BADPRESENTATIONCONTEXTID.theStatus,
                                                             0,0,0,0);
                            
                            DIMSE_sendMessageUsingMemoryData(assoc,
                                                             idPC,
                                                             moveRSP,
                                                             nullptr,
                                                             nullptr,
                                                             nullptr,
                                                             nullptr);
                            delete moveRSP;
                            result.setStatus(StatusResult::CMoveRQError, "Unable to process the C-MOVE operation. Bad C-STORE suboperation Presentation Context.");
                            return result;
                        }
                            
                    }
                    else
                    {
                        if (storeNetwork != nullptr)
                            delete storeNetwork;
                        result.setStatus(StatusResult::CMoveRSPError, "Network for C-STORE suboperation in C-MOVE DIMSE Service is unreachable.");
                    }
                
                    delete remoteAET;
                }
            }
        }
        else
        {
            result.setStatus(StatusResult::CMoveRSPError, "Error receiving C-MOVE query dataset.");
            if (dsQuery != nullptr)
                delete dsQuery;
        }
    }
    
    return result;
}

void DcmNetSCP::delTagsDiferentToKeyLevels(DcmDataset* dsQuery)
{
    if (dsQuery == nullptr || 
        dsQuery->isEmpty())
        return;
    
    // everything is fine
    for (int i = 0; i < dsQuery->card(); i++)
    {
        DcmElement* elem = dsQuery->getElement(i);
        
        if (elem->getTag() != DCM_StudyInstanceUID &&
            elem->getTag() != DCM_SeriesInstanceUID &&
            elem->getTag() != DCM_SOPInstanceUID)
        {
            // deleting tag
            dsQuery->remove(elem);
            i--;
        }
    }
}


QFileInfo* DcmNetSCP::dirFromDS(DcmDataset* ds)
{
    QFileInfo* dir4save = new QFileInfo();
    char* baseDir = strdup(this->m_rootFolder.c_str());
    OFString studyInstanceUID, seriesInstanceUID, sopInstanceUID, patientName, patientID, studyDate;
    // getting patientID
    
    ds->findAndGetOFString(DCM_PatientName, patientName);
    ds->findAndGetOFString(DCM_StudyDate, studyDate);
    ds->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
    ds->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
    
    QFileInfo fileInfo(baseDir);
    if (fileInfo.isDir() &&
        !studyDate.empty() &&
        !studyInstanceUID.empty() &&
        !seriesInstanceUID.empty())
    {
        // baseDir is valid directory
        // building the new path
        QString realPath(baseDir);
        realPath += QDir::separator();
        
        realPath += studyDate.c_str();
        realPath += QDir::separator();
        // checking the patientName and patientID values
        QString patName = patientName.empty() ? "PATIENT_NAME_UNKNOW" : patientName.c_str();
        realPath += patName;
        realPath += QDir::separator();
        
        // setting the study and series path
        realPath += studyInstanceUID.c_str();
        realPath += QDir::separator();
        
        realPath += seriesInstanceUID.c_str();
        realPath += QDir::separator();
        
        // checking if exist the directory
        fileInfo.setFile(realPath);
        if (!fileInfo.exists()) // creating the directory
        {
            if (QDir().mkpath(realPath))
                dir4save->setFile(realPath);
            else
                dir4save->setFile("");
        }
        else
            dir4save->setFile(realPath);
    }
    
    delete baseDir;
    return dir4save;
}


Status DcmNetSCP::cstoreSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC)
{
    // receiving dataset
        DcmDataset* ds = nullptr;
        T_DIMSE_Message* rsp = nullptr;
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
            T_ASC_PresentationContext pc;
            ASC_findAcceptedPresentationContext(assoc->params, idPC, &pc);
            
            // saving the file
            QDir dir4save(this->m_rootFolder.c_str());
            if (!dir4save.exists())
                dir4save.mkpath(this->m_rootFolder.c_str());
            
//             char* filename = strdup((this->m_rootFolder.c_str() + (string)"/" + (string)sopInstanceUID.c_str() + (string)".dcm").c_str());
            QFileInfo* dirPath = this->dirFromDS(ds);
            if (!dirPath->exists())
            {
                // Error with the dataset values
                rsp = new T_DIMSE_Message;
                DIMSEMessajeFactory::newCStoreRSP(rsp,
                                                  assoc->nextMsgID++,
                                                  const_cast<char*>(sopClassUID.c_str()),
                                                  const_cast<char*>(sopInstanceUID.c_str()));
                rsp->msg.CStoreRSP.DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                ASC_abortAssociation(assoc);
                result.setStatus(StatusResult::CStoreRSPError, "Error creating directory tree from dataset.");
            }
            else
            {
                char* filename = strdup((dirPath->absolutePath() + QDir::separator() + sopInstanceUID.c_str() + ".dcm").toStdString().c_str());
                SystemManager::instance()->mpsSystem()->dbManager()->store(ds, filename);
                DcmFileFormat* dcmFile = new DcmFileFormat();
                dcmFile->getDataset()->copyFrom(*ds);
                delete ds;
                ds = nullptr;
                this->buildFileMetaInfo(dcmFile);
                dcmFile->saveFile(filename);
                delete dcmFile;
                delete [] filename;
                rsp = new T_DIMSE_Message;
                DIMSEMessajeFactory::newCStoreRSP(rsp, 
                                                  assoc->nextMsgID++,
                                                  const_cast<char*>(sopClassUID.c_str()),
                                                  const_cast<char*>(sopInstanceUID.c_str()));
                // Sending response messgae
                DIMSE_sendMessageUsingMemoryData(assoc, idPC, rsp, nullptr, nullptr, nullptr, nullptr);
            }
            
            delete rsp;
            if (ds != nullptr)
                delete ds;
        }
        else
            result.setStatus(StatusResult::CStoreRSPError, (string)"Error waiting the datatset: " + cond.text());
        
        return result;
}

void DcmNetSCP::buildFileMetaInfo(DcmFileFormat* dcmFile)
{
    OFString sopClassUId, sopInstanceUID;
    
    DcmDataset* datset = dcmFile->getDataset();
    
    datset->findAndGetOFString(DCM_SOPClassUID, sopClassUId);
    datset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
    DcmElement* elem;
    
    elem = new DcmUnsignedLong(DCM_FileMetaInformationGroupLength);
    Uint32 tmp = 0;
    elem->putUint32Array(&tmp, 1);
    dcmFile->getMetaInfo()->insert(elem, OFTrue);
    
    elem = new DcmOtherByteOtherWord(DCM_FileMetaInformationVersion);
    Uint8 version[2] = {0, 1};
    elem->putUint8Array(version,2);
    dcmFile->getMetaInfo()->insert(elem, OFTrue);
    
    elem = new DcmUniqueIdentifier(DCM_MediaStorageSOPClassUID);
    elem->putString(sopClassUId.c_str());
    dcmFile->getMetaInfo()->insert(elem, OFTrue); 
    
    elem = new DcmUniqueIdentifier(DCM_MediaStorageSOPInstanceUID);
    elem->putString(sopInstanceUID.c_str());
    dcmFile->getMetaInfo()->insert(elem, OFTrue);
    
    elem = new DcmUniqueIdentifier(DCM_TransferSyntaxUID);
    elem->putString(DcmXfer(datset->getOriginalXfer()).getXferID());
    dcmFile->getMetaInfo()->insert(elem, OFTrue);
    
    elem = new DcmUniqueIdentifier(DCM_ImplementationClassUID);
    elem->putString(sopClassUId.c_str());
    dcmFile->getMetaInfo()->insert(elem, true);
    
    elem = new DcmShortString(DCM_ImplementationVersionName);
    elem->putString(OFFIS_DTK_IMPLEMENTATION_VERSION_NAME);
    dcmFile->getMetaInfo()->insert(elem, true);
    
    elem = new DcmApplicationEntity(DCM_SourceApplicationEntityTitle);
    elem->putString("MPS");
    dcmFile->getMetaInfo()->insert(elem, true);
    
    elem = new DcmUniqueIdentifier(DCM_PrivateInformationCreatorUID);
    dcmFile->getMetaInfo()->insert(elem, true);
    
    elem = new DcmOtherByteOtherWord(DCM_PrivateInformation);
    dcmFile->getMetaInfo()->insert(elem, true);
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
        if (!systemSettings->existRemoteAET(dulAssParams->callingAPTitle))
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
                            string moveDestination(msgRQ.msg.CMoveRQ.MoveDestination);
                            *result = this->cmoveSCP(assoc, idPC, moveDestination);
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
    T_ASC_Network* network;
    OFCondition cond;
    
    if ((cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, this->m_aet.port(), 0, &network)).good())
    {        
        T_ASC_Association* assoc;
        
        while (true)
        {
            if (ASC_receiveAssociation(network, &assoc, ASC_DEFAULTMAXPDU).good())
            {
                // create new handle connection thread
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
    DcmRLEDecoderRegistration::cleanup();
    DcmRLEEncoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();
    DJEncoderRegistration::cleanup();
    DJLSDecoderRegistration::cleanup();
    DJLSEncoderRegistration::cleanup();
}