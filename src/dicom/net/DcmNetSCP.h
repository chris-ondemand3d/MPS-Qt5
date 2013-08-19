#ifndef DCMNETSCP_H
#define DCMNETSCP_H

#include <dicom/net/DcmAET.h>
#include <dicom/net/DcmQuery.h>
#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dicom/UID.h>
#include <utils/Status.h>
#include <iostream>
#include <system/SystemManager.h>
#include <tasks/TaskFactory.h>
#include <tasks/TaskHandleRqDCMConn.h>
#include <utils/exception/exceptions.h>
#include <dicom/net/DIMSEMessajeFactory.h>
#include <settings/MPSSystemSettings.h>
#include <db/DBManager.h>
#include <regex>


#define DICOME_NETWORK_TIMEOUT                      5

using namespace std;
#include <dicom/net/DcmNetSCU.h>

class DcmNetSCP
{
protected:
    DcmAET m_aet;
    bool m_stop;
    string m_rootFolder;
    // DICOM operations
    
    void acceptingPresentationContext(T_ASC_Association* assoc);
    void loadSettings();
    void saveSettings();
    void abortAssociation(T_ASC_Association* assoc);
    void registerCodecs();
    Status&& negociateMoveSubOperationPC(T_ASC_Network** network, 
                                         T_ASC_Association** subOpAssoc, 
                                         T_ASC_Parameters* params, 
                                         list< pair< string, string > >& sopClassTSPairs);
    
public:
    DcmNetSCP();
    void start();
    void handleIncomingConnection(T_ASC_Network* network, T_ASC_Association* assoc);
    Status cechoSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    Status cstoreSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    Status cfindSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    Status cmoveSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC, string& moveDestination);
    void stop();
    virtual ~DcmNetSCP();
    
};


#endif // DCMNETSCP_H
