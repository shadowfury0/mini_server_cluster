#ifndef __L_SEM__
#define __L_SEM__
//这个类暂时用不到
#include <exception>
#include <semaphore.h>
//信号量类
class Sem{
public:
    Sem(){
        if( sem_init(&m_sem,0,0) != 0){
            throw std::exception();
        }
    }
    ~Sem(){
        sem_destroy(&m_sem);
    }

    bool wait(){
        return sem_wait(&m_sem) == 0;
    }
    bool post(){
        return sem_post( &m_sem ) == 0;
    }
private:
    sem_t m_sem;
};


#endif