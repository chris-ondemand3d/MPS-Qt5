#ifndef ASSOCIATIONHANDLER_H
#define ASSOCIATIONHANDLER_H


#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmdata/dcuid.h>

#include <settings/aet/AetTableManager.h>
#include <dicom/net/DcmAET.h>
#include <dicom/UID.h>
#include <system/SystemManager.h>
#include <QThread>


class AssociationHandler : public QThread
{
    Q_OBJECT
    
private:
    void acceptPC(T_ASC_Association* assoc, T_ASC_PresentationContext* pc);
protected:    
    DcmAET* m_aet;    
    void startSCP();
    void C_ECHO_SCP(T_ASC_Association* assoc);
    void C_STORE_RQ(T_ASC_Association* assoc);
    
public:
    explicit AssociationHandler(DcmAET* aet, QObject* parent = 0);
    virtual void run();    
    virtual ~AssociationHandler();
};

#endif // ASSOCIATIONHANDLER_H
