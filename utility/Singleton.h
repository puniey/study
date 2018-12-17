//单件类，一个程序中只有一个实例
#ifndef SINGLETON_H
#define SINGLETON_H

#include <boost/thread.hpp>

boost::mutex gMutex;

class Singleton
{
public:
    static Singleton* GetInstance()
    {
		if (m_instance == NULL)
		{
			boost::mutex::scoped_lock lock(gMutex);
			if (m_instance == NULL)
				m_instance = new Singleton();
		}
        
		return m_instance;
	}

private:
	class CGarbo //私有内嵌类 它的唯一工作用于销毁单件类实例
	{
	public:
		~CGarbo()
		{
			if (m_instance != NULL)
			{
				delete m_instance;
				m_instance = NULL;
			}
		}
	};

private:
	//防止实例的拷贝
	Singleton(){}
	Singleton(Singleton &other){}
	Singleton& operator=(Singleton &other){}
	~Singleton(){}

private:
	static Singleton *m_instance;
	static CGarbo Garbo;   //程序结束时自动调用析构函数
};

Singleton* Singleton::m_instance = NULL;
Singleton::CGarbo Singleton::Garbo;

#endif