//  Shallow copying technique to avoid possible pitfalls due to 
//  synthesized versions of copy constructor and the overload assignment 
//  operator (if the class contains nonstatic pointer member)
//  Why shallow copying? Sharing is caring. (=sarcasm)

#include <iostream> 

struct IntPtr
{
    int* pInt; 
    int ref_count; 
}; 

class HasPtr 
{
    private: 
        int n; 
        struct IntPtr* ptr; 

    public: 

        HasPtr(int _n, int _m) : n(_n), ptr(new struct IntPtr)
        {
            this->ptr->pInt = new int(_m); 
            this->ptr->ref_count = 1; 
        }

        HasPtr(const HasPtr& other) : n(other.n), ptr(other.ptr)
        {
            this->ptr->ref_count += 1; 
        }

        HasPtr& operator=(const HasPtr& other)
        {
            this->n = other.n; 
            this->ptr->ref_count = this->ptr->ref_count - 1; 
            if(this->ptr->ref_count == 0)
            {
                std::cout << "operator=():Deleting ptr->pInt and then ptr" << std::endl; 
                delete this->ptr->pInt; 
                delete this->ptr; 
            }
            this->ptr = other.ptr; 
            this->ptr->ref_count = this->ptr->ref_count + 1; 
            return *this; 
        }

        ~HasPtr()
        {
            this->ptr->ref_count =  this->ptr->ref_count - 1; 
            if(this->ptr->ref_count == 0)
            {
                std::cout << "~HasPtr():Deleting ptr->pInt and then ptr" << std::endl; 
                delete ptr->pInt; 
                delete ptr; 
            }
        }

        int get_ref_count() const 
        {
            return this->ptr->ref_count; 
        }
}; 

void test_func(void); 

int main(void)
{
    HasPtr hp1(100, 200); 
    std::cout << "hp1.get_ref_count()=" << hp1.get_ref_count() << std::endl; // 1
    HasPtr hp2(hp1); 
    std::cout << "hp1.get_ref_count()=" << hp1.get_ref_count() << std::endl; // 2 
    HasPtr hp3(hp1); 

    std::cout << "hp1.get_ref_count()=" << hp1.get_ref_count() << std::endl; // 3
    std::cout << "hp2.get_ref_count()=" << hp2.get_ref_count() << std::endl; // 3 
    std::cout << "hp3.get_ref_count()=" << hp3.get_ref_count() << std::endl; // 3 

    test_func(); 

    return (0); 
}

// 3: ~HasPtr():deleting ptr->pInt, ptr 

void test_func(void)
{
    HasPtr* php1 = new HasPtr(1000, 2000); 
    HasPtr* php2 = new HasPtr(*php1); 
    HasPtr* php3 = new HasPtr(*php2); 
    HasPtr* php4 = new HasPtr(3000, 4000); 
    HasPtr* php5 = new HasPtr(*php4); 

    *php4 = *php1; 
    *php5 = *php1; // 1: operator=():deleting ptr->pInt, ptr 
   
///////////////////////////////////////////////
    std::cout << "php1->get_ref_count()=" << php1->get_ref_count() << std::endl; // 5
    delete php1; 
    std::cout << "php4->get_ref_count()=" << php4->get_ref_count() << std::endl; // 4
    delete php4; 
    std::cout << "php2->get_ref_count()=" << php2->get_ref_count() << std::endl; // 3
    delete php2; 
    std::cout << "php5->get_ref_count()=" << php5->get_ref_count() << std::endl; // 2
    delete php5; 
    std::cout << "php3->get_ref_count()=" << php3->get_ref_count() << std::endl; // 1
    delete php3; 
    // 2: ~HasPtr():deleting ptr->pInt, ptr 
}