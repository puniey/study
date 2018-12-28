#include <stdarg.h>
#include <stdio.h>
#include <string>
using namespace std;

//¸ñÊ½»¯×Ö·û´®
string FormateString(size_t size, const char* fmt, ...)
{
	char* buffer = (char*)malloc(size);
	va_list arg;
	int		cnt = 0;
	va_start(arg, fmt);
	cnt = vsnprintf(buffer, size, fmt, arg);
	va_end(arg);

	string ret;
	if (cnt >= 0)
	{
		ret.assign(buffer, cnt);
	}
	free(buffer);
	buffer = NULL;
	return ret;
}