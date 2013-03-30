#include "DcmNetSCP.h"



DcmNetSCP::DcmNetSCP ( const DcmAET& aet ) : m_aet(aet)
{
    this->m_stop = false;
}

void DcmNetSCP::start()
{
    T_ASC_Network* network;
    if (ASC_initializeNetwork(NET_ACCEPTOR, this->m_aet.port(), 0, &network).good())
    {        
        T_ASC_Association* assoc;
        while (ASC_receiveAssociation(network, &assoc, ASC_DEFAULTMAXPDU).good())
        {
            // Checking the called and calling AET 
            if (strcmp(assoc->params->DULparams.calledAPTitle, this->m_aet.aet().c_str()) == 0)
            {
                // Checking the presentation contexts 
                char** sopClassList = SOPClass::instance()->sopClassUIDList();
                char** tsList = TransferSyntax::instance()->dcmtkXFerList();
                ASC_acceptContextsWithPreferredTransferSyntaxes(assoc->params, 
                                                                (const char**)sopClassList, 
                                                                SOPClass::instance()->count(),
                                                                (const char**)tsList,
                                                                TransferSyntax::instance()->count()
                                                               );
                
                
                ASC_acknowledgeAssociation(assoc);
                
                // receiving DIMSE messages
                T_ASC_PresentationContextID idPC;
                T_DIMSE_Message msg;
                DcmDataset* statusDetail;
                OFCondition cond = EC_Normal;
                while (cond.good())
                {
                    cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 3, &idPC, &msg, &statusDetail, NULL);
                    cout << cond.text() << endl;
                    if (cond == DUL_PEERREQUESTEDRELEASE)
                    {
                        ASC_acknowledgeRelease(assoc);
                        cout << "entre" <<endl;
                    }
                    
                    
                    else
                    {
                        T_ASC_PresentationContext pc;
                        ASC_findAcceptedPresentationContext(assoc->params, idPC, &pc);
                        
                        if (dcmIsaStorageSOPClassUID(pc.abstractSyntax))
                        {
                            cout << "esto es para almacenar ficheros" << endl;
                        }
                        else
                        {
                            // Verification SOP Class
                            if (strcmp(pc.abstractSyntax, (*SOPClass::instance())[SOPClass::UID_VerificationSOPClassUID].c_str()) == 0)
                            {
                                T_DIMSE_Message rsp;
                                rsp.CommandField = DIMSE_C_ECHO_RSP;
                                rsp.msg.CEchoRSP.MessageIDBeingRespondedTo = msg.msg.CEchoRQ.MessageID;
                                strcpy(rsp.msg.CEchoRSP.AffectedSOPClassUID, msg.msg.CEchoRQ.AffectedSOPClassUID);
                                rsp.msg.CEchoRSP.opts = O_ECHO_AFFECTEDSOPCLASSUID;
                                rsp.msg.CEchoRSP.DataSetType = DIMSE_DATASET_NULL;
                                rsp.msg.CEchoRSP.DimseStatus = STATUS_Success;
                                
                                DIMSE_sendMessageUsingMemoryData(assoc, idPC, &rsp, NULL, NULL, NULL, NULL, NULL);
                            }
                            cout << "esto es para no almacenar ficheros" << endl;
                        }
                    }                    
                }
//                 else
//                     cout << "Comando no recibido" << endl;
                // TODO: Throw exception because the command wasn't sent to our server
            }
            else
            {
                ASC_rejectAssociation(assoc, NULL, NULL, NULL);
                // TODO: Throw exception if there is any error with the the association parameters
            }
        }
        ASC_dropAssociation(assoc);
    }
    else;
        // TODO: Throw exception if there is any error with the network
    
    
}
void DcmNetSCP::stop()
{
    
}

DcmNetSCP::~DcmNetSCP()
{
   
}