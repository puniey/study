//一个简单的内存池
#ifndef __SIMPLE_OBJECT_POOL_H__
#define __SIMPLE_OBJECT_POOL_H__

#include <list>
#include <boost/thread/mutex.hpp>

template <typename T>
class CSimpleObjectPool
{
public:
    CSimpleObjectPool()
    {
        m_dwInitCount = 0;
        m_pBlockHeader = NULL;
    }

    ~CSimpleObjectPool()
    {
        delete[] m_pBlockHeader;
    }

    bool Init(UINT32 dwCount)
    {
        m_dwInitCount = dwCount;
        m_pBlockHeader = new T[m_dwInitCount];
        if (m_pBlockHeader == NULL)
            return false;

        for (UINT32 i = 0; i < dwCount; ++i)
            m_ListFree.push_back(&m_pBlockHeader[i]);

        return true;
    }

    T* FetchObj()
    {
        T* pObj = NULL;
        boost::mutex::scoped_lock lock(m_mutex);
        if (m_ListFree.size() > 0)
        {
            pObj = m_ListFree.front();
            m_ListFree.pop_front;
        }
        return pObj;
    }

    void ReleaseObj(T* pObj)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_ListFree.push_back(pObj);
    }

    size_t GetFreeCount()
    {
        size_t n = 0;
        boost::mutex::scoped_lock lock(m_mutex);
        n = m_ListFree.size();

        return n;
    }

    T* GetHeader()
    {
        return m_pBlockHeader;
    }

private:
    std::list<T*>   m_ListFree;
    T*              m_pBlockHeader;
    UINT32          m_dwInitCount;
    boost::mutex    m_mutex;
}

#endif //__SIMPLE_OBJECT_POOL_H__