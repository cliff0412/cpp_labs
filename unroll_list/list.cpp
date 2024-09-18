#include "plf_list.h"
#include <iostream>

int main()
{
    std::cout << "hello world" << std::endl;

    plf::list<double> list;

    for (int counter = 0; counter != 50; ++counter)
    {
        list.push_back(counter);
    }

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