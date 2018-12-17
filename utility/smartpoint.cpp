#include<iostream>
using namespace std;

class CCount{
public:
    template<typename T>
	friend class CShared_Ptr;
	CCount(int n) :count(n){}
private:
	int count;
};

template<typename T>
class CShared_Ptr{
public:
	CShared_Ptr() : ptr(new T), use(new CCount(1))
	{
		cout << "Call constructor function1, use count :" << use->count << endl;
	}
	
	CShared_Ptr(T p) : use(new CCount(1))
	{
		ptr = new T(p);
		cout << "Call constructor function2, use count :" << use->count << endl;
	}

	CShared_Ptr(const CShared_Ptr<T> &rhs)
	{
		ptr = rhs.ptr;
		use = rhs.use;
		use->count++;
		cout << "Call copy constructor function, use count :" << use->count << endl;
	}

	CShared_Ptr<T>& operator= (const CShared_Ptr<T> &other)
	{
	    if (this == &other)
		    return *this;
            
        use->count--;
        if (use->count == 0)
        {
            delete ptr;
            delete use;
        }

		ptr = other.ptr;
		use = other.use;
		use->count++;
		cout << "Call operator=, use count :" << use->count << endl;
		return *this;
	}
	~CShared_Ptr()
	{
		use->count--;
		cout << "Call destructor function, use count : " << use->count << endl;
		if (use->count == 0)
		{
			delete ptr;
			delete use;
		}
	}
private:
	T *ptr;
	CCount *use;
};

int main()
{
	CShared_Ptr<int> a(3);
	auto b = a;
	return 0;
}