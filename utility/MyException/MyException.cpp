#include "MyException.h"
#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>

Exception::Exception(const char* msg) : message_(msg)
{
    fillStackTrace();
}

Exception::Exception(const std::string& msg) : message_(msg)
{
    fillStackTrace();
}

Exception::~Exception() throw ()
{}

const char* Exception::what() const throw()
{
    return message_.c_str();
}

const char* Exception::stackTrace() const throw()
{
    return stack_.c_str();
}

void Exception::fillStackTrace()
{
    void* buffer[STACK_DEPTH];
    int depth = ::backtrace(buffer, STACK_DEPTH);
    char** strings = ::backtrace_symbols(buffer, depth);
    
    if (strings)
    {
        for (int i = 2; i < depth; ++i)
        {
            stack_.append("\t");
            stack_.append(demangle(strings[i]));
            stack_.push_back('\n');
        }
        free(strings);
    }
}

std::string Exception::demangle(const char* symbol)
{
    size_t size;
    int status;
    char temp[128];
    char* demangled;
    if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp))
    {
        if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, &size, &status)))
        {
            std::string result(demangled);
            free(demangled);
            return result;
        }
    }

    if (1 == sscanf(symbol, "%127s", temp))
    {
        return temp;
    }
    return symbol;
}
