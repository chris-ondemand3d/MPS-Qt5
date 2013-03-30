#ifndef DCMNETSCP_H
#define DCMNETSCP_H

#include <dicom/net/DcmAET.h>
#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dicom/UID.h>
#include <utils/Status.h>


class DcmNetSCP
{
protected:
    DcmAET m_aet;
    bool m_stop;    
    // DICOM operations
    Status cechoSCP();
    
public:
    DcmNetSCP(const DcmAET& aet);
    void start();
    void stop();
    virtual ~DcmNetSCP();
    
};

#endif // DCMNETSCP_H
