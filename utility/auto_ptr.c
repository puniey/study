template<typename Y>
struct auto_ptr_ref
{
	Y *yp;
	auto_ptr_ref(Y *rhs)
		: yp(rhs){}
}

template<typename T>
class auto_ptr
{
private:
    T *ap;
public:
    typedef T element_type;

	explicit auto_ptr(T *ptr = 0) throw()
		: ap(ptr)
	{

	}

	auto_ptr(auto_ptr &rhs) throw()
		: ap(rhs.release())
	{

	}

	template<typename Y>
	auto_ptr(auto_ptr<Y> &rhs) throw()
		: ap(rhs.release())
	{

	}

	auto_ptr &operator=(auto_ptr &rhs) throw()
	{
		reset(rhs.release());
		return *this;
	}

	template<typename Y>
	auto_ptr &operator=(auto_ptr<Y> &rhs) throw()
	{
		reset(rhs.release());
		return *this;
	}

	~auto_ptr() throw()
	{
		delete ap;
	}

	T *get() const throw()
	{
		return ap;
	}

	T &operator*() const throw()
	{
		return *ap;
	}

	T *operator->() const throw()
	{
		return ap;
	}

	T *release() throw()
	{
		T *tmp(ap);
		ap = 0;
		return tmp;
	}

	void reset(T *ptr = 0) throw()
	{
		if (ap != ptr)
		{
			delete ap;
			ap = ptr;
		}
	}

	auto_ptr(auto_ptr_ref<T> rhs) throw()
		: ap(rhs.yp)
	{

	}

	auto_ptr &operator= (auto_ptr_ref<T> rhs) throw()
	{
		reset(rhs.yp);
		return *this;
	}

	template<typename Y>
	operator auto_ptr_ref<Y> throw()
	{
		return auto_ptr_ref<Y>(release());
	}

	template<typename Y>
	operator auto_ptr<Y>() throw()
	{
		return auto_ptr<Y>(release());
	}
}
