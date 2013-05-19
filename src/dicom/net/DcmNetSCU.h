#ifndef DCMNETSCU_H
#define DCMNETSCU_H

// DCMTK Includes
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcxfer.h>
#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/ofstd/oflist.h>
// Application Include
#include <dicom/net/DcmAET.h>
#include "DcmQuery.h"
#include <utils/Status.h>
#include <dicom/UID.h>
#include <utils/filesystem/FilesManager.h>
#include <utils/numeric.h>
#include <utils/callback/Callback.h>
#include <utils/callback/progress/Progress.h>

// Other Includes
#include <QFileInfo>
#include <QDir>
#include <boost/concept_check.hpp>



class DcmNetSCU
{
private:
    DcmAET * m_localAet; 
    
protected:
    Status negociateAllPresentationContext(T_ASC_Association** assoc,
                                           T_ASC_Parameters** params,
                                           T_ASC_Network** network,
                                           int timeout = 0
    );
    
public:
    DcmNetSCU(DcmAET& localAet);
    Status cecho_RQ(DcmAET& remoteAet, int timeout = 0);
    Status cstore_RQ(DcmAET& remoteAet, FileManager& directory, int timeout = 0);
    Status cstore_RQ(DcmAET& remoteAet, const OFList<string>& files, int timeout = 0);
    Status cstore_RQ(DcmAET& remoteAet, const string& file, int timeout = 0);
    Status cfind_RQ(DcmAET& remoteAet, DcmQuery& query, Callback<Progress>* proggres = NULL);
    Status cmove_RQ(DcmAET& findAET, DcmAET& moveAET, 
                    DcmQuery& query,
                    Callback<Progress>* progress = NULL);
    void rejectAssoc(T_ASC_Association*& assoc,
                     T_ASC_RejectParametersReason reason, 
                     T_ASC_RejectParametersResult result,
                     T_ASC_RejectParametersSource source);
    
    inline DcmAET& getAET() {return *(this->m_localAet);}
    virtual ~DcmNetSCU();
};

#endif // DCMNETSCU_H
