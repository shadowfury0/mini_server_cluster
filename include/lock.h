#ifndef __L_MUTEX__
#define __L_MUTEX__

#include <iostream>
#include <pthread.h>

class Mutex  {
public:
    Mutex(){
        if(pthread_mutex_init(&mid,NULL) != 0){
            throw std::exception();
        }
        std::cout << " pthread_mutex_init  ------------- " << std::endl;
    }
    ~Mutex(){
        if(pthread_mutex_destroy(&mid) != 0){
            throw std::exception();
        }
        std::cout << " pthread_mutex_destroy --------------" << std::endl;
    }
    void lock(){
        pthread_mutex_lock(&mid);
        std::cout << " mutex_lock --------------" << std::endl;
    }
    void unlock(){
        pthread_mutex_unlock(&mid);
        std::cout << " mutex_unlock --------------" << std::endl;
    }
    void trylock(){
        if (pthread_mutex_trylock(&mid) != 0){
            std::cout << " trylock failed ----->  " << std::endl;
            return;
        }
        std::cout << " trylock  success ----->  " << std::endl;
    }
    pthread_mutex_t getId()
    {
        return mid;
    }
    
private:
    mutable  pthread_mutex_t mid;
};

class  SpinLock
{
public:
    SpinLock(){
        if (pthread_spin_init(&mid,NULL) != 0){
            throw std::exception();
        }
        std::cout << " pthread_mutex_init  ------------- " << std::endl;
    }
    virtual ~SpinLock(){
        if (pthread_spin_destroy(&mid) != 0){
            throw std::exception();
        }
        std::cout << " pthread_spin_destroy  ------------- " << std::endl;
    }

    void lock(){
        pthread_spin_lock(&mid);
        std::cout << " pthread_spin_lock  ------------- " << std::endl;
    }
    void unlock(){
        pthread_spin_unlock(&mid);
        std::cout << " pthread_spin_unlock  ------------- " << std::endl;
    }
    void trylock(){
        if (pthread_spin_trylock(&mid) != 0){
            std::cout << " trylock failed ----->  " << std::endl;
            return;
        }
        std::cout << " trylock  success ----->  " << std::endl;
    }
    pthread_spinlock_t getId()
    {
        return mid;
    }
private:
    mutable pthread_spinlock_t mid;
};

class  RWLock
{
public:
    RWLock(){
        if ( pthread_rwlock_init(&mid,NULL) != 0 ){
            throw std::exception();
        }
        std::cout << " pthread_rwlock_init  ------------- " << std::endl;
    }
    virtual ~RWLock(){
        if ( pthread_rwlock_destroy(&mid) != 0 ){
            throw std::exception();
        }
        std::cout << " pthread_rwlock_destroy  ------------- " << std::endl;
    }
    //默认加写锁
    void lock(){
        writeLock();
    }
    void readLock(){
        pthread_rwlock_rdlock(&mid);
        std::cout << " pthread_rwlock_rdlock  ------------- " << std::endl;
    }
    void writeLock(){
        pthread_rwlock_wrlock(&mid);
        std::cout << " pthread_rwlock_wrlock  ------------- " << std::endl;
    }

    void unlock(){
        pthread_rwlock_unlock(&mid);
        std::cout << " pthread_rwlock_unlock  ------------- " << std::endl;
    }
    void trylock(){
        if (pthread_rwlock_trywrlock(&mid) != 0){
            std::cout << " trylock failed ----->  " << std::endl;
            return;
        }
        std::cout << " trylock  success ----->  " << std::endl;
    }

    pthread_rwlock_t getId()
    {
        return mid;
    }
private:
    mutable pthread_rwlock_t mid;
};

template<typename T = Mutex>
class Lock {
public:
    Lock(){}
    ~Lock(){}
    inline static T* getLock(){
        if (m_lock == NULL){
            m_lock = new T();
            std::cout << " allocate lock ... " << std::endl;
        }
        return m_lock;
    }
    inline static void lock(){
        m_lock->lock();
    }
    inline static void unlock(){
        m_lock->unlock();
    }
    inline static void trylock(){
        m_lock->trylock();
    }
private:
    static T* m_lock ;
};
template<class T>
T* Lock<T>::m_lock = NULL;


#endif