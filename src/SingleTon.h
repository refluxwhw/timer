#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>

#define SINGLETON(CName)                          \
public:                                           \
    static CName& getInstance();                  \
    static void destoryInstance();                \
private:                                          \
    CName();                                      \
    ~CName();                                     \
    CName(CName const&);                          \
    void operator=(CName const&);                 \
    static std::mutex m_singletonMutex;           \
    static CName* m_singletonInstance;


#define SINGLETON_IMPL(CName)                     \
CName* CName::m_singletonInstance = NULL;         \
std::mutex CName::m_singletonMutex;               \
CName& CName::getInstance() {                     \
    if (m_singletonInstance == NULL) {            \
        m_singletonMutex.lock();                  \
        if (m_singletonInstance == NULL) {        \
            m_singletonInstance = new CName();    \
        }                                         \
        m_singletonMutex.unlock();                \
    }                                             \
    return *m_singletonInstance;                  \
}                                                 \
void CName::destoryInstance(){                    \
    m_singletonMutex.lock();                      \
    if (m_singletonInstance != NULL){             \
        delete m_singletonInstance;               \
        m_singletonInstance = NULL;               \
    }                                             \
    m_singletonMutex.unlock();                    \
}


#endif // SINGLETON_H
