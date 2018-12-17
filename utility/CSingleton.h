//单例化工具模板类，用来单例其他类，保证整个程序中只有一个实例,适用于单线程模式程序
#ifndef CSINGLETONTOOL_H
#define CSINGLETONTOOL_H

template<typename T>
class CSingletonTool
{
public:
    static T& instance()
    {
        if (m_instance == NULL)
        {
            init();
        }
        
        return *m_instance;
    }

private:
    static void init()
    {
        m_instance = new T();
        atexit(destory);  //在main函数结束之后，调用destory()函数销毁单件类的实例T::m_instance
    }
    
    static void destory()
    {
        if (m_instance != NULL)
        {
            delete m_instance;
            m_instance = NULL;
        }
    }

private:
    static T* volatile m_instance;  //volatile:禁止编译器优化，多线程编程，每次都从内存地址读取变量
}

template<typename T>
T* volatile CSingletonTool<T>::m_instance = NULL;

#endif