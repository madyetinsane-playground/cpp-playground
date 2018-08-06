#include <iostream>
#include "thread_pool.h"

int test1()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "\ntest1!\n" << std::endl;
    
    return 42;
}

void test2(std::string str)
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "\n" << str << "\n" << std::endl;
}

void test3(int x, double d)
{
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << "\n" << x << ", " << d << "\n" << std::endl;
}

int main(int argc, char **argv)
{
    ThreadPool pool(2);

    auto f1 = pool.submit(test1);
    auto f2 = pool.submit(test2, "Bamp!");
    auto f3 = pool.submit(test3, 65536, 15.25);
    
    //f1.wait();
    std::cout << f1.get() << std::endl;

    return 0;
}