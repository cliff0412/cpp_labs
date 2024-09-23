#include "plf_list.h"
#include <iostream>

template <bool is_const>
class list_iterator
{
};

class Parent
{
public:
    class Child
    {
    public:
        double a;
    };
};

int main()
{
    std::allocator<double> double_alloc;
    double a,b,c;
    a=b=c= *double_alloc.allocate(1,0);
    std::cout << a << b << c << std::endl;

    size_t double_alloc_size = sizeof(double_alloc);
    std::cout << "double_alloc_size is " << double_alloc_size << std::endl;

    struct MyStruct
    {
        double a;
        double b;
        double c;
        unsigned short d;
    };
    size_t size_my_struct = sizeof(MyStruct);
    std::cout << "size_my_struct is " << size_my_struct << std::endl;

    size_t parent_size = sizeof(Parent);
    std::cout << "parent_size is " << parent_size << std::endl;

    size_t child_size = sizeof(Parent::Child);
    std::cout << "child_size is " << child_size << std::endl;

    plf::list<double> list;

    list.push_back(0);
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_front(4);
    list.remove(2);

    unsigned int size = list.size();

    for (plf::list<double>::iterator it = list.begin(); it != list.end();)
    {
        if ((rand() & 7) == 0)
        {
            it = list.erase(it);
            --size;
        }
        else
        {
            ++it;
        }
    }
}