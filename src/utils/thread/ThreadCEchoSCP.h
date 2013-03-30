#ifndef THREADCECHOSCP_H
#define THREADCECHOSCP_H

#include <utils/thread/Thread.h>
#include <dcmtk/dcmnet/assoc.h>

class ThreadCEchoSCP : public Thread
{
    Q_OBJECT
private:
    T_ASC_Association* m_association;
    
public:
    ThreadCEchoSCP(T_ASC_Association*& assoc);
    virtual void run();
    virtual ~ThreadCEchoSCP();
};

#endif // THREADCECHOSCP_H
