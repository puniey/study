//定义一个不能继承的类
//一种方法是将类的构造函数和析构函数定义成私有的，定义静态成员构造对象，但是局限是只能在堆上创建对象
//技巧 利用一个辅助类 友元和虚继承 定义一个不能继承的类
template <typename T>
class base_class
{
	friend T;
private:
	base_class(){}
	~base_class(){}
};
//虚继承base_class 保证首先调用虚基类的构造函数
//该类既可以在堆上生成对象 也可在栈上生成对象 不能继承
class nonderived : virtual public base_class<nonderived>
{
public:
	nonderived(){}
	~nonderived(){}
};

class derived : public nonderived //生成对象时编译会报错 不能访问 base_class的构造函数
{};