//单例模式 线程安全 可继承  class SingletonInstance : public Singleton<SingletonInstance>…
#include <boost/thread.hpp>
boost::mutex gMutex;

template <typename T>
class Singleton
{
public:
    static T& Instance()
    {
        if (m_pInstance == NULL)
        {
            //Lock lock;
            boost::mutex::scoped_lock lock(gMutex);
            if (m_pInstance == NULL)
            {
                m_pInstance = new T();
                atexit(Destroy);
            }
            return *m_pInstance;
        }
        return *m_pInstance;
    }

protected:
    Singleton(void) {}
    virtual ~Singleton(void) {}

private:
    Singleton(const Singleton& rhs) {}
    Singleton& operator = (const Singleton& rhs) {}

    static void Destroy()
    {
        if (m_pInstance != NULL)
            delete m_pInstance;
        m_pInstance = NULL;
    }

    static T* volatile m_pInstance;
};

template <typename T>
T* volatile Singleton<T>::m_pInstance = NULL;