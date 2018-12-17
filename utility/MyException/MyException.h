#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <string>
#include <exception>

#define STACK_DEPTH 10

class Exception : public std::exception
{
public:
    explicit Exception(const char* what);
    explicit Exception(const std::string& what);
    virtual ~Exception() throw();
    virtual const char* what() const throw();
    const char* stackTrace() const throw();

private:
    void fillStackTrace();
    std::string demangle(const char* symbol);

    std::string message_;
    std::string stack_;
};

#endif
