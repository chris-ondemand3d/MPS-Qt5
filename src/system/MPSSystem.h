#ifndef MPSSYSTEM_H
#define MPSSYSTEM_H

#include <qt5/QtCore/qobject.h>
#include <dicom/net/AssociationHandler.h>

class MPSSystem : public QObject
{
    Q_OBJECT
private:
public:
    explicit MPSSystem(QObject* parent = 0);
    void emitAssReceived();
    virtual ~MPSSystem();
    
Q_SIGNALS:
    void associationReceived();
};

#endif // SYSTEM_H
