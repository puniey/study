#ifndef AUTO_PTR_H
#define AUTO_PTR_H

template<typename T>
class auto_ptr
{
    public :
        //使用explicit关键字避免隐式转换
        explicit auto_ptr(T* p=0);

        ~auto_ptr();

        //使用另一个类型兼容的auto_ptr来初始化一个新的auto_ptr
        template<typename U>
        auto_ptr(auto_ptr<U>& rhs);

        template<typename U>
        auto_ptr<T>& operator=(auto_ptr<U>& rhs);

        T& operator*() const;
        T* operator->() const;

        //返回原始对象的指针
        T* get() const;
        //放弃指针的所以权
        T* release();
        //删除原有指针并获得指针的p的所有权
        void reset(T* p=0);

    private:
        T* pointee;

};

template<typename T>
auto_ptr<T>::auto_ptr(T* p)
    :pointee(p)
{}

template<typename T>
    template<typename U>
auto_ptr<T>::auto_ptr(auto_ptr<U>& rhs)
    :pointee(rhs.release())
{}

template<typename T>
auto_ptr<T>::~auto_ptr()
{
    delete pointee;
}

template<typename T>
    template<typename U>
auto_ptr<T>& auto_ptr<T>::operator=(auto_ptr<U>& rhs)
{
    if(this!=&rhs)
        reset(rhs.release());
    return *this;
}

template<typename T>
T& auto_ptr<T>::operator*() const
{
    return *pointee;
}

template<typename T>
T* auto_ptr<T>::operator->() const
{
    return pointee;
}

template<typename T>
T* auto_ptr<T>::get() const
{
    return pointee;
}

template<typename T>
T* auto_ptr<T>::release()
{
    T* oldpointee=pointee;
    pointee=0;
    return oldpointee;
}

template<typename T>
void auto_ptr<T>::reset(T* p)
{
    if(pointee!=p)
    {
        delete pointee;
        pointee=p;
    }
}

#endif