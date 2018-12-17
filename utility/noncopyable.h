//禁止复制的类
//继承此类的对象不可拷贝和赋值，noncopyable的构造函数和析构函数是protected说明noncopyable不可生成对象
#ifndef __NONCOPYABALE_H__
#define __NONCOPYABALE_H__

class noncopyable
{
protected:
#if !defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && !defined(BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS)
    BOOST_CONSTEXPR noncopyable() = default;
    ~noncopyable() = default;
#else
    noncopyable() {}
    ~noncopyable() {}
#endif
#if !defined(BOOST_NO_CXX11_DELETED_FUNCTIONS)
    noncopyable( const noncopyable& ) = delete;
    noncopyable& operator=( const noncopyable& ) = delete;
#else
private:  // emphasize the following members are private
    noncopyable( const noncopyable& );
    noncopyable& operator=( const noncopyable& );
#endif
};

#endif