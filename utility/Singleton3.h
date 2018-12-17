//见到过奇妙的单件模式，线程不安全，不可重入，优点：不用担心，内存忘记释放，在调用的时候才会初始化静态局部对象

class CSingleton
{
public:
    static CSingleton& Instance()
    {
        static CSingleton obj;
        return obj;
    }

protected:
    CSingleton(){}
    ~CSingleton(){}

private:
    CSingleton(const CSingleton& rhs){}
    CSingleton& operator= (const CSingleton& rhs){}
}