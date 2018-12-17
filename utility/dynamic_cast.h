#include <iostream>  
  
using namespace std;  
class Base  
{  
    friend bool operator==(const Base& lhs,const Base& rhs)  
    {  
        return (typeid(lhs) == typeid(rhs)) && (lhs.equal(rhs));  
    }  
public:  
    Base(int i = 0):ival(i){}  
    virtual bool equal(const Base& rhs)const  
    {  
        return ival == rhs.ival;  
    }  
protected:  
    int ival;  
};  
  
class Derived:public Base  
{  
public:  
    Derived(int i = 0,double d = 1.1):Base(i),dval(d){}  
    bool equal(const Base& rhs)const  
    {  
        if(const Derived *dp = dynamic_cast<const Derived*>(&rhs))  
        {  
            return Base::equal(rhs) && dval == dp->dval;           
        }  
        else  
            return false;  
     }  
private:  
    double dval;  
};  
int main()  
{  
      
    Base b1;  
    Base b2;  
    Derived d1(1,1.1);  
    Derived d2(2,2.2);  
    if(d1 == d2)  
        cout<<"yes"<<endl;  
    else  
        cout<<"no"<<endl;  
    return 0;  
}  