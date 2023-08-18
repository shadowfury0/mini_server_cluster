#ifndef __L_CONTITION__
#define __L_CONTITION__

#include "lock.h"

template<class L = Mutex>
class Condition{
public:
    Condition(){
        std::cout << "  condition init ------- " << std::endl;
        m_mutex.getLock();
        if(pthread_cond_init(&m_cond,NULL) != 0){
            throw std::exception();
        }
    }
    virtual ~Condition(){
        std::cout << "  condition destroy --------- " << std::endl;
        if (pthread_cond_destroy(&m_cond) != 0) {
            throw std::exception();
        }
    }
    bool wait(){
        int ret = 0;
        m_mutex.lock();
        pthread_mutex_t p = m_mutex.getLock()->getId();
        ret = pthread_cond_wait(&m_cond,&p);
        m_mutex.unlock();
        return ret == 0;
    }
    bool signal(){
        return pthread_cond_signal(&m_cond) == 0;
    }

private:
    Lock<L> m_mutex;
    pthread_cond_t m_cond;
};


#endif