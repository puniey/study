//可动态生长的内存池 线程安全
#ifndef __DYNAMIC_POOL_EX_H__
#define __DYNAMIC_POOL_EX_H__

#include <list>
#include <set>
#include <boost/thread/mutex.hpp>

template <class T>
class CDynamicPoolEx
{
public:
	explicit CDynamicPoolEx(void)
		: m_dwInitCount(0), m_dwGrowCount(0)
	{
		m_dwMaxPoolSize = 0;
		m_dwUsedSize = 0;
	}

	~CDynamicPoolEx(void)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		typename set<T*>::iterator itr;
		typename list<T*>::iterator itr2;

		for (itr = m_setUse.begin(); itr != m_setUse.end(); ++itr)
		{
			T* pObj = *itr;
			pObj->~T();
		}

		m_ListFree.clear();
		m_setUse.clear();

		for (itr2 = m_ListAll.begin(); itr2 != m_ListAll.end(); ++itr2)
		{
			T* pObj = *itr2;
			::operator delete(pObj);
		}

		m_ListAll.clear();
	}

	bool Init(UINT32 dwInitCount, UINT32 dwGrowCount, UINT32 dwPoolSize = 0)
	{
		m_dwInitCount = dwInitCount;
		m_dwGrowCount = dwGrowCount;
		m_dwMaxPoolSize = dwPoolSize;

		return _AllocT(m_dwInitCount);
	}

	T* FetchObj(void)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		T* pT = NULL;
		if ((m_dwMaxPoolSize > 0) && (m_dwUsedSize >= m_dwMaxPoolSize))
			return NULL;

		if(!m_ListFree.empty())
		{
			pT = m_ListFree.front();
			m_ListFree.pop_front();
			m_setUse.insert(pT);
			if (m_dwMaxPoolSize > 0)
			{
				m_dwUsedSize++;
			}
			pT = new(pT) T();
			return pT;
		}

		if(!_AllocT(m_dwGrowCount))
		{
			return NULL;
		}

		if (m_dwMaxPoolSize > 0)
		{
			m_dwUsedSize++;
		}
		pT = m_ListFree.front();
		m_ListFree.pop_front();
		m_setUse.insert(pT);
		pT = new(pT) T();
		return pT;
	}

	void ReleaseObj(T* pObj)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		if (m_dwMaxPoolSize > 0)
		{
			m_dwUsedSize-- ;
		}
		typename set<T*>::iterator itr = m_setUse.find(pObj);
		if (itr != m_setUse.end())
		{
			m_setUse.erase(pObj);
			pObj->~T();
			m_ListFree.push_back(pObj);
		}
		else
			delete pObj;
	}

	size_t GetFreeCount(void)
	{
		size_t n = 0;
		boost::mutex::scoped_lock lock(m_mutex);
		n = m_ListFree.size();
		return n;
	}

	size_t GetUseCount(void)
	{
		size_t n = 0;
		boost::mutex::scoped_lock lock(m_mutex);
		n = m_setUse.size();
		return n;
	}

protected:

	bool _AllocT(UINT32 dwCount)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		if (!m_ListFree.empty())
			return true;

		T* pT = (T*)::operator new(dwCount * sizeof(T));

		if (pT == NULL)
			return false;

		for (UINT32 i = 0; i < dwCount; ++i)
		{
			m_ListFree.push_back(&pT[i]);
		}

		m_ListAll.push_back(pT);
		return true;
	}

private:
	boost::mutex			m_mutex;
	list<T*>				m_ListFree;
	set<T*>					m_setUse;
	list<T*>				m_ListAll;

	UINT32					m_dwInitCount;
	UINT32					m_dwGrowCount;

	UINT32                  m_dwMaxPoolSize;
	UINT32                  m_dwUsedSize;
};

#endif //__DYNAMIC_POOL_EX_H__