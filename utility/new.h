//重载全局operator new
void* operator new(size_t size)
{
	void *pReturn = NULL;
	if (size == 0)
	{
		size = 1; //参数为0也要能返回一个有效的地址
	}
	//cout << "allocate memory " << size << " bytes" << endl;
	/*if (void* mem = malloc(size))
	{
		return mem;
	} 
	else
	{
		throw std::bad_alloc();
	}*/
	while (1)
	{
		pReturn = malloc(size);
		if (pReturn != NULL)
		{
			return pReturn;
		}

		new_handler globalhandler = set_new_handler(0);
		set_new_handler(globalhandler);
		if (globalhandler)
		{
			(*globalhandler)();
		}
		else
		{
			throw std::bad_alloc();
		}
	}
}
//重载全局operator new[]
void* operator new[](size_t size)//前4个byte保存了对象个数 placement new返回(void*)((char*)mem + 4)
{
	if (size == 0)
	{
		size = 1; //参数为0也要能返回一个有效的地址
	}
	cout << "allocate memory " << size << " bytes" << endl;
	if (void* mem = malloc(size))
	{
		return mem;
	}
	else
	{
		throw std::bad_alloc();
	}
}
//重载全局operator delete
void operator delete(void *mem) throw() //delete 是如何判断数据类型的？(猜测编译器做了处理)
{
	cout << "call operator delete " << endl;
	if (mem == nullptr)
	{
		return; //能处理delete 空指针
	}
	free(mem);
}
//重载全局operator delete[]
void operator delete[](void *mem) throw() //delete 是如何判断数据类型的？(猜测编译器做了处理)
{
	cout << "call operator delete " << endl;
	if (mem == nullptr)
	{
		return; //能处理delete 空指针
	}
	free(mem);
}
//placement new实现
void* operator new(size_t size, void* ptr)
{
	//只是简单返回指针
	return ptr;
}
//placement delete实现
void* operator delete(void*, void*)
{
	//不做任何处理 但必须和placement new 配套出现
}
//placement new[]实现
void* operator new[](size_t size, void* ptr)
{
	//只是简单返回指针
	return ptr;
}
//placement delete实现
void* operator delete[](void*, void*)
{
	//不做任何处理 但必须和placement new[] 配套出现
}

class CBase
{
public:
	CBase(int v = 0, int t = 0) : m(v), p(t)
	{ 
		cout << "CBase()" << endl; 
	}
	virtual ~CBase(){ cout << "~CBase()" << endl; }
	CBase(const CBase &other)
	{
		cout << "copy CBase()" << endl;
	}
	CBase &operator=(const CBase &other)
	{
		cout << "operator= CBase()" << endl;
		return *this;
	}

	static void* operator new(size_t size)//size为需要分配的内存大小
	{
		if (size == 0)
		{
			size = 1; //参数为0也要能返回一个有效的地址
		}
		cout << "CBase allocate memory " << size << " bytes" << endl;
		if (void *mem = malloc(size))
		{
			return mem;
		} 
		else
		{
			throw std::bad_alloc();
		}
	}
	static void* operator new[](size_t size)//size为需要分配的内存大小
	{
		if (size == 0)
		{
			size = 1; //参数为0也要能返回一个有效的地址
		}
		cout << "CBase allocate memory " << size << " bytes" << endl;
		if (void *mem = malloc(size))
		{
			return mem;
		}
		else
		{
			throw std::bad_alloc();
		}
	}
	//size不是释放内存的大小 区别operator new的size 表示第一个形参所指对象的字节数
	static void operator delete(void* mem, size_t size) throw()
	{
		if (mem == nullptr)
		{
			return; //能处理delete 空指针
		}
		cout << "CBase free memory " << size << " bytes" << endl;
		free(mem);
	}
	//size不是释放内存的大小 区别operator new的size 表示第一个形参所指对象的字节数
	static void operator delete[](void* mem, size_t size) throw()
	{
		if (mem == nullptr)
		{
			return; //能处理delete 空指针
		}
		cout << "CBase free memory " << size << " bytes, " << *(int*)mem << "blocks" << endl;
		free(mem);
	}
private:
	int m;
	int p;
};
class CDerived :public CBase
{
public:
	CDerived(int o = 0, int p = 0, int q = 0) :CBase(o, p), n(q)
	{ 
		cout << "CDerived()" << endl; 
	}
	virtual ~CDerived(){ cout << "~CDerived()" << endl; }
	CDerived(const CDerived &other) : CBase(other)
	{
		cout << "copy CDerived()" << endl;
	}
	CDerived &operator=(const CDerived &other)
	{
		CBase::operator=(other);
		cout << "operator= CDerived()" << endl;
		return *this;
	}
private:
	int n;
};

int main(int argc, char **argv)
{
	CBase *b = (CBase*)::operator new(sizeof(CBase));
	::new(b) CBase; //定位new的操作符 先调用placement new函数，再调用构造函数
	//b->CBase::CBase();//手动调用构造函数
	b->~CBase();//手动调用析构函数 没有对应操作符调用 因为可以直接调用析构函数 构造函数不能直接调用 所有有操作符调用
	operator delete(b);
	system("pause");
	return 0;
}