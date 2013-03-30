#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class Thread : public QThread
{
    Q_OBJECT
    
public:
    explicit Thread(QObject* parent = 0);
    virtual void run() = 0;
    virtual ~Thread();
};

#endif // THREAD_H
