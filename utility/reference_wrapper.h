#ifndef __REFERENCE_WRAPPER_H__
#define __REFERENCE_WRAPPER_H__
//================================================================================
//将对象包装成引用类型的对象 减小对象拷贝的代价 把对象的拷贝转为对reference_wrapper对象的拷贝
//但是相比直接传引用 代价还是比较大的
//还有以包装可调用对象 针对STL需要传可调用对象的算法
template <typename T>
class reference_wrapper
{
public:
    explicit reference_wrapper(T& t) : t_(&t){} //显示调用构造函数
    operator T& () const {return *t_;} //类型转换为T& 可以隐式转换为T或者T&类型

    decltype(T()()) operator()() const //重载无参数函数调用功能
    {
        return (*t_)();
    }
#ifndef C++11
    template <typename T0>
    decltype(T()(T0)) operator()(T0 t0) const //重载一个参数函数调用功能
    {
        return (*t_)(t0);
    }

    template <typename T0, typename T1>
    decltype(T()(T0, T1)) operator()(T0 t0, T1 t1) const //重载二个参数函数调用功能
    {
        return (*t_)(t0, t1);
    }
#else
    //C++11 支持重载多个参数
    template <typename ... Args>
	auto operator()(Args&& ... args) const -> decltype(T()(std::forward<Args>(args) ...))
	{
		return (*t_)(std::forward<Args>(args) ...);
	}
#endif
    T& get() const {return *t_;}
    T* get_pointer() const {return t_;}

private:
    T* t_;
};
//================================================
//===========工厂函数===========================
template <typename T>
reference_wrapper<T> ref(T& t)
{
    return reference_wrapper<T>(t);
}

template <typename T>
reference_wrapper<T const> ref(T const& t)
{
    return reference_wrapper<T const>(t);
}
//=============================================
//检测reference_wrapper对象
template <typename T>
struct is_reference_wrapper
{
    static const bool value = false;
};

template<typename T>
const bool is_reference_wrapper<T>::value;

//偏特化
template <typename T>
struct is_reference_wrapper< reference_wrapper<T> >
{
    static const bool value = true;
};

template <typename T>
const bool is_reference_wrapper< reference_wrapper<T> >::value;

template <typename T>
struct is_reference_wrapper< reference_wrapper<T> const >
{
    static const bool value = true;
};

template <typename T>
const bool is_reference_wrapper< reference_wrapper<T> const >::value;

template <typename T>
struct is_reference_wrapper< reference_wrapper<T> volatile >
{
    static const bool value = true;
};

template <typename T>
const bool is_reference_wrapper< reference_wrapper<T> volitale >::value;

template <typename T>
struct is_reference_wrapper< reference_wrapper<T> const volatile >
{
    static const bool value = true;
};

template <typename T>
const bool is_reference_wrapper< reference_wrapper<T> const volatile >::value;

//解包装 返回实际类型引用
template <typename T>
struct unwrap_reference
{
    typedef T type;
};

template <typename T>
struct unwrap_reference< reference_wrapper<T> >
{
    typedef T type;
};

template <typename T>
struct unwrap_reference< reference_wrapper<T> const >
{
    typedef T type;
};

template <typename T>
struct unwrap_reference< reference_wrapper<T> volatile >
{
    typedef T type;
};

template <typename T>
struct unwrap_reference< reference_wrapper<T> const volatile >
{
    typedef T type;
};

//自由函数 解包装
template <typename T>
typename unwrap_reference<T>::type& unwrap_ref(T& t)
{
    return t;
}
//========================================================
#endif