//可变参数模板 实现打印不定数量参数转为字符串

template <typename T>
string debug_msg(const T& val)
{
    ostringstream os;
    os << val;
    return os.str();
}
//递归终点
template <typename T>
string print(const T& val)
{
    ostringstream os;
    os << val;
    return os.str();
}
//递归解包可变参数
template <typename T, typename ... Args>
string print(const T& val, const Args& ... args)
{
    ostringstream os;
    os << val;
    return os.str() + print(args ...);
}
//对外调用接口
template <typename ... Args>
string errmsg(const Args& ... args)
{
    return print(debug_msg(args)...);
}

//用法：errmsg("hello", 2017, 0, 3, 1, 4)

//C语言可变参数 返回string
string OutPringMsg(const char* fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf_s(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	return string(buf);
}