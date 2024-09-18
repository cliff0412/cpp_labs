#include <forward_list>
#include <iostream>

using namespace std;

int main()
{
    // Declaring forward list
    forward_list<int> flist1;

    // Declaring another forward list
    forward_list<int> flist2;

    // Declaring another forward list
    forward_list<int> flist3;

    flist1.push_front(1);
    flist1.push_front(2);

       // Inserting after the first element
    auto it = flist1.begin();
    flist1.insert_after(it, 4);

    // // Assigning values using assign()
    // flist1.assign({1, 2, 3});

    // // Assigning repeating values using assign()
    // // 5 elements with value 10
    // flist2.assign(5, 10);

    // // Assigning values of list 1 to list 3
    // flist3.assign(flist1.begin(), flist1.end());

    // Displaying forward lists
    cout << "The elements of first forward list are : ";
    for (int &a : flist1)
        cout << a << " ";
    cout << endl;

    // cout << "The elements of second forward list are : ";
    // for (int &b : flist2)
    //     cout << b << " ";
    // cout << endl;

    // cout << "The elements of third forward list are : ";
    // for (int &c : flist3)
    //     cout << c << " ";
    // cout << endl;

    return 0;
}