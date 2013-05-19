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


using namespace std;



class DcmNetSCP
{
protected:
    DcmAET m_aet;
    bool m_stop;
    string m_rootFolder;
    // DICOM operations
    
    void acceptingPresentationContext(T_ASC_Association* assoc);
    Status receiveCommand(T_ASC_Association* assoc, T_DIMSE_Message& message);
    void loadSettings();
    void saveSettings();
    
public:
    DcmNetSCP();
    void start();
    void handleIncomingConnection(T_ASC_Network* network, T_ASC_Association* assoc);
    Status cechoSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    Status cstoreSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    Status cfindSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    Status cmoveSCP(T_ASC_Association* assoc, T_ASC_PresentationContextID idPC);
    void stop();
    virtual ~DcmNetSCP();
    
};

#endif // DCMNETSCP_H
