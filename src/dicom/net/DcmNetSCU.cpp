#include "DcmNetSCU.h"


DcmNetSCU::DcmNetSCU(DcmAET& localAet)
{
    this->m_localAet = new DcmAET(localAet);
}


Status DcmNetSCU::cecho_RQ(DcmAET& remoteAet, int timeout)
{
    this->m_localAet->print();
    remoteAet.print();
    OFCondition cond;
    T_ASC_Network * network;
    Status result(StatusResult::Success, "Success");
    if ((cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 0, &network)).good())
    {
        T_ASC_Parameters* params;
        ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);
        ASC_setAPTitles(params,
                        this->m_localAet->aet().c_str(),
                        remoteAet.aet().c_str(),
                        nullptr);

        char* remoteAet4Dcmtk = strdup((remoteAet.hostname() +
                                        ":" +
                                        int2str(remoteAet.port())).c_str()
                                        );
        ASC_setPresentationAddresses(params,
                                     this->m_localAet->hostname().c_str(),
                                     remoteAet4Dcmtk
                                    );
        
        char** tsList = TransferSyntax::instance()->dcmtkXFerList();
        ASC_addPresentationContext(params,
                                   1,
                                   (*SOPClass::instance())[SOPClass::UID_VerificationSOPClassUID].c_str(),
                                   ((const char**)(tsList)),
                                   TransferSyntax::instance()->count()
                                  );
        // Request DICOM Association
        T_ASC_Association* assoc;
        if ((cond = ASC_requestAssociation(network, params, &assoc)).good())
        {
            if (ASC_countAcceptedPresentationContexts(params) == 1)
            {
                DIC_US msgID = assoc->nextMsgID;
                DIC_US status;
                DcmDataset* echoDataset = nullptr;
                cond =  DIMSE_echoUser(assoc, msgID, DIMSE_BLOCKING, 0, &status, &echoDataset);
                if (cond.bad())
                {
                    result = Status(StatusResult::Error, cond.text());
                    ASC_abortAssociation(assoc);
                }
                else
                    ASC_releaseAssociation(assoc);
                
                ASC_dropAssociation(assoc);
            }
            else
                result = Status(StatusResult::Error, "Bad Presentation Context.");
        }
        else
            result = Status(StatusResult::AssociationError, cond.text());
    }
    else
        result = Status(StatusResult::Error, cond.text());
        
    ASC_dropNetwork(&network);
    return result;
}



Status DcmNetSCU::negociateAllPresentationContext(T_ASC_Association** assoc,
                                                  T_ASC_Parameters** params,
                                                  T_ASC_Network** network,
                                                  int timeout
                                                 )
{
    char** ts = TransferSyntax::instance()->dcmtkXFerList();
    char** sopClass = SOPClass::instance()->sopClassUIDList();
    Status result(StatusResult::Success, "Association Negotiation Success.");
    OFCondition cond;
    
    int pcID = 1;
    int sopClassCount = SOPClass::instance()->count();
    int tsCount = TransferSyntax::instance()->count();
    for (int i = 0; i < sopClassCount; i++)
    {
        for (int j = 0; j < tsCount; j++)
        {
            char** tsPC = new char*[1];
            tsPC[0] = strdup(ts[j]);
            cond = ASC_addPresentationContext(*params,
                                              pcID,
                                              const_cast<char*>(sopClass[i]),
                                              (const char**)tsPC,
                                              1);
            if (cond.bad())
                cout << cond.text() << endl;
            delete tsPC[0];
            delete [] tsPC;
            pcID += 2;
        }
    }

    cond = ASC_requestAssociation(*network, *params, assoc);
    if (cond.bad())
        result = Status(StatusResult::AssociationError, cond.text());
    
    cout << "Acepted Presentation Context:" << ASC_countAcceptedPresentationContexts(*params) << endl;
    return result;
}



Status DcmNetSCU::cstore_RQ(DcmAET& remoteAet, FileManager& directory, int timeout)
{
    OFCondition cond;
    Status result(StatusResult::Success, "C-STORE Success.");

    T_ASC_Network* network;
    if (ASC_initializeNetwork(NET_REQUESTOR, 0, timeout, &network).good())
    {
        T_ASC_Parameters* params;
        ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);

        ASC_setAPTitles(params,
                        this->m_localAet->aet().c_str(),
                        remoteAet.aet().c_str(),
                        nullptr);
        char* remoteHost = strdup((remoteAet.hostname() + ":" + int2str(remoteAet.port())).c_str());
        ASC_setPresentationAddresses(params,
                                     this->m_localAet->hostname().c_str(),
                                     remoteHost);

        // Adding presentation context
        int sopClassCount = SOPClass::instance()->count();
        int xferCount = TransferSyntax::instance()->count();
        char** sopClassUIDList = SOPClass::instance()->sopClassUIDList();
        char** xferList = TransferSyntax::instance()->dcmtkXFerList();
        T_ASC_PresentationContextID pcID = 1;
        for (int i = 0; i < sopClassCount; i++, pcID+=2)
            ASC_addPresentationContext(params, pcID, sopClassUIDList[i], (const char**)xferList, xferCount);

        T_ASC_Association* assoc;
        if ((cond = ASC_requestAssociation(network, params, &assoc)).good())
        {
            DcmFileFormat* dcmfile = nullptr;
            while(directory.nextDcmFile(&dcmfile).good())
            {
                DcmDataset* ds = dcmfile->getDataset();
                OFString sopClassUID, sopInstanceUID;
                DcmXfer* xfer = new DcmXfer(ds->getOriginalXfer());
                ds->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
                ds->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);

                //  Checking the accepted presentation context
                char* xferID = strdup(xfer->getXferID());
                pcID =ASC_findAcceptedPresentationContextID(assoc, sopClassUID.c_str(), xferID);
                if (pcID != 0)
                {
                    // Preparing message to send
                    T_DIMSE_Message msgReq;
                    DcmDataset* statusDetail = nullptr;
                    msgReq.CommandField = DIMSE_C_STORE_RQ;
                    strcpy(msgReq.msg.CStoreRQ.AffectedSOPClassUID, sopClassUID.c_str());
                    strcpy(msgReq.msg.CStoreRQ.AffectedSOPInstanceUID, sopInstanceUID.c_str());
                    msgReq.msg.CStoreRQ.DataSetType = DIMSE_DATASET_PRESENT;
                    msgReq.msg.CStoreRQ.Priority = DIMSE_PRIORITY_MEDIUM;
                    msgReq.msg.CStoreRQ.MessageID = assoc->nextMsgID++;
                    // Sending the mesage
                    if ((cond = DIMSE_sendMessageUsingMemoryData(assoc, pcID, &msgReq, statusDetail, ds, nullptr, nullptr)).good())
                    {
                        T_DIMSE_Message msgRsp;
                        DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 0, &pcID, &msgRsp, &statusDetail);
                    }
                }
                else
                    cout << "not sent." << endl;
                sopClassUID.clear();
                sopInstanceUID.clear();
                delete xfer;
                delete dcmfile;
            }
            ASC_releaseAssociation(assoc);
        }
        else
        {
            result = Status(StatusResult::AssociationError, "Can not associate with the server.");
            ASC_abortAssociation(assoc);
        }

        cout << "End transmission" << endl;
        // Cleaning the memory
    }
    else
        result = Status(StatusResult::CStoreRQError, "Network is not ready.");

    return result;
}

Status DcmNetSCU::cstore_RQ(DcmAET& remoteAet, const list< string >& files, int timeout)
{
    OFCondition cond;
    T_ASC_Association* assoc;
    T_ASC_Network* network;
    Status result (StatusResult::Success, "C-STORE Success.");

    cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 0, &network);
    if (cond.good())
    {
        // Reading the file list to build the presentation context
        T_ASC_Parameters* params;
        ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);
        ASC_setAPTitles(params,
                        strdup(this->m_localAet->aet().c_str()),
                        strdup(remoteAet.aet().c_str()),
                        nullptr);
        ASC_setPresentationAddresses(params,
                                     strdup(this->m_localAet->hostname().c_str()),
                                     strdup((remoteAet.hostname() + ":" + int2str(remoteAet.port())).c_str()));
        char** sopClassUIDList = SOPClass::instance()->sopClassUIDList();
        char** xferList = TransferSyntax::instance()->dcmtkXFerList();

        T_ASC_PresentationContextID pcID = 1;
        int tsCount = TransferSyntax::instance()->count();
        int sopClassCount = SOPClass::instance()->count();

        for (int i = 0; i < sopClassCount; i++, pcID+=2)
        {
            char* sopClassUID = strdup(sopClassUIDList[i]);
            ASC_addPresentationContext(params, pcID, sopClassUID , (const char**)xferList, tsCount);
        }
        
        // Associating with the DICOM Server
        if (ASC_requestAssociation(network, params, &assoc).good())
        {
            OFString sopClassUID;
            OFString sopInstanceUID;
            for (string filename : files)
            {
                DcmFileFormat* dcmfile = new DcmFileFormat();                                
                T_ASC_PresentationContextID pcID;
                if (dcmfile->loadFile(filename.c_str()).good())
                {
                    DcmDataset* ds = dcmfile->getDataset();
                    ds->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
                    ds->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
                    char* xferUID = strdup(DcmXfer(ds->getOriginalXfer()).getXferID());                    
                    char* abstractSyntax = strdup(sopClassUID.c_str());
                    pcID = ASC_findAcceptedPresentationContextID(assoc, abstractSyntax, const_cast<char*>(xferUID));
                    if (pcID != 0)
                    {
                        
                        T_DIMSE_Message reqMsg;
                        reqMsg.CommandField = DIMSE_C_STORE_RQ;
                        T_DIMSE_C_StoreRQ* req = &(reqMsg.msg.CStoreRQ);
                        DcmDataset* statusDetail = nullptr;
                        req->MessageID = assoc->nextMsgID++;
                        strcpy(req->AffectedSOPClassUID, sopClassUID.c_str());
                        strcpy(req->AffectedSOPInstanceUID, sopInstanceUID.c_str());
                        req->Priority = DIMSE_PRIORITY_MEDIUM;
                        req->DataSetType = DIMSE_DATASET_PRESENT;
                        if ((cond = DIMSE_sendMessageUsingMemoryData(assoc, pcID, &reqMsg, statusDetail, ds, nullptr, nullptr)).good())
                        {
                            T_DIMSE_Message respMsg;
                            cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, timeout, &pcID, &respMsg, nullptr);
                            // TODO: Checking response status
                        }
                    }                    
                    delete xferUID;
                    delete abstractSyntax;
                }
                delete dcmfile;
                
            }
            ASC_releaseAssociation(assoc);
        }
        else
        {
            result = Status(StatusResult::AssociationError, "Bad Presentation Context.");
            ASC_abortAssociation(assoc);
        }
    }
    ASC_destroyAssociation(&assoc);
    ASC_dropNetwork(&network);

    return result;
}


OFCondition DcmNetSCU::cstore_RQ(DcmAET& remoteAET, const string& file, T_ASC_Association* assoc, int timeout)
{
    Status result(StatusResult::Success, "C-STORE Success");
    OFCondition cond;
    
    DcmFileFormat* dcmFile = new DcmFileFormat();
    OFString sopClassUID, xferUID, sopInstanceUID;
    if (dcmFile->loadFile(file.c_str()).good())
    {
        DcmDataset* ds = dcmFile->getDataset();
        ds->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
        ds->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
        xferUID = OFString(DcmXfer(ds->getOriginalXfer()).getXferID());
        T_ASC_PresentationContextID pcID = ASC_findAcceptedPresentationContextID(assoc,
                                                                                 const_cast<char*>(sopClassUID.c_str()),
                                                                                 const_cast<char*>(xferUID.c_str()));
        if (pcID == 0)
            return OFCondition(2000, 2000, OF_error, "Bad Presentation Context");
        if (ASC_countAcceptedPresentationContexts(assoc->params) > 0) // Checking accepted presentation context
        {
            
            if (pcID != 0) // Presentation Context was accepted
            {
                T_DIMSE_Message reqMsg;
                T_DIMSE_C_StoreRQ* req = &(reqMsg.msg.CStoreRQ);
                DcmDataset* statusDetail = nullptr;
                reqMsg.CommandField = DIMSE_C_STORE_RQ;
                req->MessageID = assoc->nextMsgID++;
                strcpy(req->AffectedSOPClassUID, sopClassUID.c_str());
                strcpy(req->AffectedSOPInstanceUID, sopInstanceUID.c_str());
                req->Priority = DIMSE_PRIORITY_MEDIUM;
                req->DataSetType = DIMSE_DATASET_PRESENT;
                cond = DIMSE_sendMessageUsingMemoryData(assoc, pcID, &reqMsg, statusDetail, ds, nullptr, nullptr);
                if (cond.good())
                {
                    T_DIMSE_Message rsp;
                    cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 0, &pcID, &rsp, nullptr, nullptr); 
                    // TODO: Checking response status
                    cout << "Response Status for C-MOVE Suboperation: " << cond.text() << endl;
                }
            }
        }
    }
    
    delete dcmFile;
    return cond;;
}



Status DcmNetSCU::cstore_RQ(DcmAET& remoteAet, const string& file, int timeout)
{
    Status result(StatusResult::Success, "C-STORE Success");
    OFCondition cond;
    T_ASC_Network* network;
    T_ASC_Association* assoc;

    cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 0, &network);
    if (cond.good())
    {
        // Creating presentation context
        DcmFileFormat* dcmfile = new DcmFileFormat();
        OFString sopClassUID, xferUID, sopInstanceUID;
        if (dcmfile->loadFile(file.c_str()).good())
        {
            DcmDataset* ds = dcmfile->getDataset();
            ds->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
            ds->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
            xferUID = OFString(DcmXfer(ds->getOriginalXfer()).getXferID());
            // The file is a dicom file
            // Creating params
            T_ASC_Parameters* params;
            ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);
            ASC_setAPTitles(params,
                            this->m_localAet->aet().c_str(),
                            remoteAet.aet().c_str(),
                            nullptr);

            char* remoteAet4Dcmtk = strdup((remoteAet.hostname() +
                                            ":" +
                                            int2str(remoteAet.port())).c_str());
            ASC_setPresentationAddresses(params,
                                         this->m_localAet->hostname().c_str(),
                                         remoteAet4Dcmtk
            );
            
            char** xferList = new char*[1];
            xferList[0] = strdup(DcmXfer(EXS_LittleEndianImplicit).getXferID());
            ASC_addPresentationContext(params, 1, sopClassUID.c_str(), (const char**)xferList, 1);
            if ((cond = ds->chooseRepresentation(EXS_LittleEndianImplicit, nullptr)).good() && 
                (cond = ASC_requestAssociation(network, params, &assoc)).good())
            {
                // Checking
                if (ASC_countAcceptedPresentationContexts(params) > 0) // Checking accepted presentation context
                {
                    T_ASC_PresentationContextID pcID = ASC_findAcceptedPresentationContextID(assoc,
                                                                                             const_cast<char*>(sopClassUID.c_str()),
                                                                                             const_cast<char*>(xferUID.c_str()));
                    if (pcID != 0) // Presentation Context was accepted
                    {
                        T_DIMSE_Message reqMsg;
                        T_DIMSE_C_StoreRQ* req = &(reqMsg.msg.CStoreRQ);
                        DcmDataset* statusDetail = nullptr;
                        reqMsg.CommandField = DIMSE_C_STORE_RQ;
                        req->MessageID = assoc->nextMsgID++;
                        strcpy(req->AffectedSOPClassUID, sopClassUID.c_str());
                        strcpy(req->AffectedSOPInstanceUID, sopInstanceUID.c_str());
                        req->Priority = DIMSE_PRIORITY_MEDIUM;
                        req->DataSetType = DIMSE_DATASET_PRESENT;
                        cond = DIMSE_sendMessageUsingMemoryData(assoc, pcID, &reqMsg, statusDetail, ds, nullptr, nullptr);
                        if (cond.good())
                        {
                            // Message was send it successfully
                            T_DIMSE_Message respMsg;
                            cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, timeout, &pcID, &respMsg, nullptr);

                            if (cond.good())
                            {
                                ASC_releaseAssociation(assoc);
                                result = Status(StatusResult::Success, "C-STORE Success.");
                            }
                            else
                                result = Status(StatusResult::CStoreRSPError, "Error receiving C-STORE-RSP message.");
                        }
                        else
                            result = Status(StatusResult::CStoreRQError, "Error sending C-STORE-RQ messgae.");
                    }
                    else
                        result = Status(StatusResult::BadPresentationContext, "Presentation context was not accepted.");
                }
                else
                    result = Status(StatusResult::BadPresentationContext, "Presentation context was not accepted.");
            }
            else
            {
                result = Status(StatusResult::AssociationError, cond.text());
                ASC_abortAssociation(assoc);                
            }
//             ASC_destroyAssociation(&assoc);
               ASC_dropAssociation(assoc);
               delete xferList[0];
               delete [] xferList;
               delete remoteAet4Dcmtk;
        }
        else
            result = Status(StatusResult::NotExistDicomFile, file + " is not a valid DICOM file.");;
        
        delete dcmfile;
        ASC_dropNetwork(&network);
    }
    else
        result = Status(StatusResult::NetworkIsNotReady, "Network is not ready.");    

    return result;
}

Status DcmNetSCU::cfind_RQ(DcmAET& remoteAet, DcmQuery& query, Callback<Progress>* progress)
{
    OFCondition cond;
    Status status(StatusResult::Success, "C-FIND Success.");
    T_ASC_Network* network;

    if ((cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 0, &network)).good())
    {
        T_ASC_Parameters* params;
        ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);
        // Setting the connections parammeters
        ASC_setAPTitles(params,
                        this->m_localAet->aet().c_str(),
                        remoteAet.aet().c_str(),
                        nullptr
        );
        string remoteAddr = remoteAet.hostname() + ":" + int2str(remoteAet.port());
        ASC_setPresentationAddresses(params,
                                     this->m_localAet->hostname().c_str(),
                                     remoteAddr.c_str()
                                    );

        // Adding Presentation Context
        char** xferList = TransferSyntax::instance()->dcmtkXFerList();
        char* findPatientRoot = strdup((*SOPClass::instance())[SOPClass::UID_FINDPatientRootQueryRetrieveInformationModelUID].c_str());
        char* findStudyRoot = strdup((*SOPClass::instance())[SOPClass::UID_FINDStudyRootQueryRetrieveInformationModelUID].c_str());
        
        T_ASC_PresentationContextID studyRootID = 1;
        T_ASC_PresentationContextID patientRootID = 3;
        
        ASC_addPresentationContext(params,
                                   studyRootID,
                                   findStudyRoot,
                                   (const char**)xferList,
                                   TransferSyntax::instance()->count()
                                  );
        ASC_addPresentationContext(params,
                                   patientRootID,
                                   findPatientRoot,
                                   (const char**)xferList,
                                   TransferSyntax::instance()->count()
                                );

        T_ASC_Association* assoc;
        if ((cond = ASC_requestAssociation(network, params, &assoc)).good())
        {
            // Checking the list of accepted presentation context
            if (ASC_countAcceptedPresentationContexts(params) > 0)
            {
                T_ASC_PresentationContextID idPC;
                // Checking if the server support STUDY-ROOT Information Model
                if (ASC_findAcceptedPresentationContextID(assoc, findStudyRoot) == studyRootID)
                    idPC = studyRootID;
                // The information model supported by the server is the Patient Root.
                else 
                    idPC = patientRootID;
                
                // Building the C-FIND message
                T_DIMSE_Message reqMsg;
                strcpy(reqMsg.msg.CFindRQ.AffectedSOPClassUID, findStudyRoot);
                reqMsg.msg.CFindRQ.DataSetType = DIMSE_DATASET_PRESENT;
                reqMsg.msg.CFindRQ.MessageID = assoc->nextMsgID++;
                reqMsg.msg.CFindRQ.Priority = DIMSE_PRIORITY_MEDIUM;
                reqMsg.CommandField = DIMSE_C_FIND_RQ;
                DcmDataset* statusDetail = nullptr;
                
                // Sending the C-FIND message
                cond = DIMSE_sendMessageUsingMemoryData(assoc, 
                                                        idPC,
                                                        &reqMsg,
                                                        statusDetail, 
                                                        query.toDataset(),
                                                        nullptr,
                                                        nullptr,
                                                        nullptr);
                if (cond.good())
                {
                    // Receiving the responses
                    bool pendingRsp = true;
                    T_DIMSE_Message rspMsg;
                    if (statusDetail != nullptr)
                        delete statusDetail;
                    
                    while (pendingRsp)
                    {
                        if ((cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &idPC, &rspMsg, &statusDetail, nullptr)).good())
                        {
                            cout << "Message ID: " << idPC << endl;
                            DcmDataset* dsRsp = nullptr;
                            if (rspMsg.msg.CFindRSP.DimseStatus == 0)
                                break;
                            
                            if ((cond = DIMSE_receiveDataSetInMemory(assoc, DIMSE_BLOCKING, 0, &idPC, &dsRsp, nullptr, nullptr)).good())
                            {
                                if (progress != nullptr)
                                {
                                    void** params = new void*[1];
                                    params[0] = dsRsp;
//                                     progress->exec(params);
                                    
                                }
                                
                                dsRsp->print(cout);
                            }
                            delete dsRsp;
                        }
                        else
                        {
                            status = Status(StatusResult::CFindRSPError, cond.text());
                            ASC_abortAssociation(assoc);
                            break;
                        }
                    }
                }
                else
                    status = Status(StatusResult::CFindRQError, cond.text());
            }
            else
                status = Status(StatusResult::BadPresentationContext, "Presentation context was not accepted.");
            
            if (status.good())
                ASC_releaseAssociation(assoc);
        }
        else
        {
            status = Status(StatusResult::AssociationError, cond.text());
            ASC_abortAssociation(assoc);
        }
        ASC_destroyAssociation(&assoc);
    }
    else
        status = Status(StatusResult::NetworkIsNotReady, "Network is not ready.");
    
    ASC_dropNetwork(&network);

    return status;
}

Status DcmNetSCU::cmove_RQ(DcmAET& findAET, DcmAET& moveAET, DcmQuery& query, Callback< Progress >* progress)
{
    OFCondition cond;
    Status status;
    T_ASC_Network* network;
    if ((cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 10, &network)).good())
    {
        T_ASC_Parameters* params;
        ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);
        
        ASC_setAPTitles(params, 
                        this->m_localAet->aet().c_str(), 
                        findAET.aet().c_str(), 
                        nullptr);
        string remoteAddr = findAET.hostname() + ":" + int2str(findAET.port());
        ASC_setPresentationAddresses(params, 
                                     this->m_localAet->hostname().c_str(), 
                                     remoteAddr.c_str());
        
        // Adding presentation context
        char* tsList[] = {const_cast<char*>(UID_LittleEndianImplicitTransferSyntax)};
        ASC_addPresentationContext(params, 1, UID_MOVEPatientRootQueryRetrieveInformationModel, (const char**)tsList, 1);
        ASC_addPresentationContext(params, 3, UID_MOVEStudyRootQueryRetrieveInformationModel, (const char**)tsList, 1);
        
        T_ASC_Association* assoc;
        if ((cond = ASC_requestAssociation(network, params, &assoc)).good())
        {
            // association success.
            // checking the presentation context            
            if (ASC_countAcceptedPresentationContexts(params) == 0)
            {
                status.setStatus(StatusResult::BadPresentationContext, "Error with proposed presentation context.");
                ASC_abortAssociation(assoc);
                ASC_dropNetwork(&network);
                return status;
            }
            /* 
             * There are some presentation context accepted.
             * Now we need to check if the Abstract Syntax accepted was for use 
             * Patient Root or Study Root Query/Retrieve model. If Patient and Study 
             * Root model are accepted, we'll be used Study Root by default as Abstract 
             * Syntax.
             */
            // Checking that Study Root is supported by the server
            T_ASC_PresentationContext pc;
            T_ASC_PresentationContextID idPC;
            T_DIMSE_Message msgMOVE;
            if (ASC_findAcceptedPresentationContext(params, 3, &pc).good())
            {
                // Using Study Root                
                strcpy(msgMOVE.msg.CMoveRQ.AffectedSOPClassUID,(char*)UID_MOVEStudyRootQueryRetrieveInformationModel);  
                idPC = 3;
            }
            else if (ASC_findAcceptedPresentationContext(params, 1, &pc).good())
            {
                // Using Patient Root
                strcpy(msgMOVE.msg.CMoveRQ.AffectedSOPClassUID,(char*)UID_MOVEPatientRootQueryRetrieveInformationModel);
                idPC = 1;
            }
            else
            {
                ASC_abortAssociation(assoc);
                ASC_dropNetwork(&network);
                status.setStatus(StatusResult::AssociationError, "Unsupported presentation context.");
            }
            
            msgMOVE.msg.CMoveRQ.MessageID = assoc->nextMsgID;
            msgMOVE.CommandField = DIMSE_C_MOVE_RQ;
            msgMOVE.msg.CMoveRQ.Priority = DIMSE_PRIORITY_MEDIUM;
            msgMOVE.msg.CMoveRQ.DataSetType = DIMSE_DATASET_PRESENT;
            strcpy(msgMOVE.msg.CMoveRQ.MoveDestination, moveAET.aet().c_str());
            
            if ((cond = DIMSE_sendMessageUsingMemoryData(assoc, 
                                                         idPC, 
                                                         &msgMOVE, 
                                                         nullptr, 
                                                         query.toDataset(), 
                                                         nullptr, 
                                                         nullptr, 
                                                         nullptr)).good())
            {
                
                // Handling C-MOVE response
                T_DIMSE_Message msgMOVERsp;
                T_ASC_PresentationContextID idPCMoveRsp;
                
                
                while(true)
                {
                    // Receiving C-MOVE-RSP command
                    cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 10, &idPCMoveRsp, &msgMOVERsp, nullptr, nullptr);
                    if (cond.good())
                    {
                        switch(msgMOVERsp.msg.CMoveRSP.DimseStatus)
                        {
                            case STATUS_Success:
                            case STATUS_STORE_Warning_CoersionOfDataElements:
                            case STATUS_STORE_Warning_ElementsDiscarded:
                            case STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass:
                            {
                                ASC_releaseAssociation(assoc);
                                ASC_dropAssociation(assoc);
                                return Status(StatusResult::Success, (string)"C-MOVE Success: " + cond.text());
                                break;
                            }
                            
                            case STATUS_Pending:
                            {
                                cout << "Remaining: " << msgMOVERsp.msg.CMoveRSP.NumberOfRemainingSubOperations << endl;
                                cout << "Fail: " << msgMOVERsp.msg.CMoveRSP.NumberOfFailedSubOperations << endl;
                                cout << "Completed: " << msgMOVERsp.msg.CMoveRSP.NumberOfCompletedSubOperations << endl;
                                cout << "Warnings: " << msgMOVERsp.msg.CMoveRSP.NumberOfWarningSubOperations << endl;
                                break;
                            }
                            
                            default:
                            {
                                // An error occur
                                ASC_abortAssociation(assoc);
                                ASC_dropAssociation(assoc);
                                ASC_dropNetwork(&network);
                                return Status(StatusResult::CMoveRQError, (string)"C-MOVE operation error: " + cond.text());
                            }
                        }
                        
                        cout << "Remain: " << msgMOVERsp.msg.CMoveRSP.NumberOfRemainingSubOperations << endl;
                        cout << cond.text() << endl;
                        cout << msgMOVERsp.msg.CMoveRSP.DimseStatus << endl;
                    }
                    else
                    {
                        // receiving the last message                        
                        status.setStatus(StatusResult::CMoveRSPError, (string)"Error waiting C-MOVE-RSP: " + cond.text());
                        ASC_abortAssociation(assoc);
                        ASC_dropNetwork(&network);
                        return status;
                    }
                }
            }
            else
            {
                ASC_abortAssociation(assoc);
                status.setStatus(StatusResult::CMoveRQError, "Error sending query: " + string(cond.text()));
            }
        }
        else
        {
            status.setStatus(StatusResult::AssociationError, "Error during the association process: " + string(cond.text()));
        }
        ASC_releaseAssociation(assoc);
        ASC_dropAssociation(assoc);
    }
    else
        status.setStatus(StatusResult::NetworkIsNotReady, "Network error. Maybe the port is busy: " + string(cond.text()));
    
    ASC_dropNetwork(&network);
    
    return status;
}

void DcmNetSCU::rejectAssoc(T_ASC_Association*& assoc, 
                            T_ASC_RejectParametersReason reason, 
                            T_ASC_RejectParametersResult result, 
                            T_ASC_RejectParametersSource source)
{
    T_ASC_RejectParameters rejParams;
    rejParams.reason = reason;
    rejParams.result = result;
    rejParams.source = source;
    
    ASC_rejectAssociation(assoc, &rejParams);
}


DcmNetSCU::~DcmNetSCU()
{
    delete this->m_localAet;
}

// #include "DcmNetSCU.moc"