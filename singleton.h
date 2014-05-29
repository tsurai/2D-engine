#ifndef SINGLETON_H
#define SINGLETON_H

#include <assert.h>

// WARNING: not threadsafe
template <typename T> class Singleton
{
protected:
    static T *m_instance;
public:
    Singleton() {
        assert(!m_instance);

        if(!m_instance)
            m_instance = static_cast <T*> (this);
    }

    static T *instance() {
        assert(m_instance);
        return m_instance;
    }

    virtual ~Singleton() {
        m_instance = 0;
    }

private:
    Singleton(const Singleton<T> &);
    Singleton& operator=(const Singleton<T> &);

};

#endif // SINGLETON_H
