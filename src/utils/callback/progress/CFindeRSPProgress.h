#ifndef CFINDERSPPROGRESS_H
#define CFINDERSPPROGRESS_H

#include <utils/callback/progress/Progress.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <QObject>


class CFindeRSPProgress : public Progress
{
private:
    DcmDataset* m_queryRsp;
    QObject* m_obj;
public:
    CFindeRSPProgress(const QObject* obj = nullptr);
    virtual Status notifyProgress(void** params);
    virtual ~CFindeRSPProgress();
};

#endif // CFINDERSPPROGRESS_H
