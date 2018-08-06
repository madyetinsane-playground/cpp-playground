#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <random>
#include <chrono>
#include <type_traits>

class Barrier
{
    unsigned int threshold, count, generation;
    std::mutex m;
    std::condition_variable cv;
public:
    Barrier(unsigned int _count)
        : threshold(_count), count(_count), generation(0)
    {
          if(count == 0) throw std::logic_error("Barrier constructor: count cannot be equal to zero.");
    }
    
    bool wait()
    {
        std::unique_lock<std::mutex> lk(m);
        
        unsigned int gen = generation;
        
        if(--count == 0)
        {
            ++generation;
            count = threshold;
            
            cv.notify_all();
            return true;
        }
        while(gen == generation)
            cv.wait(lk);
        
        return false;
    }
};

std::once_flag flag;

void print(Barrier& r, int& current, int id)
{
    ++current;    
    r.wait();
    
    std::call_once(flag, [&]() { std::cout << current << std::endl; });
}


int main(int argc, char **argv)
{
    Barrier r(3);
    int cur = 0;
    
    std::thread th1(print, std::ref(r), std::ref(cur), 0);
    std::thread th2(print, std::ref(r), std::ref(cur), 1);
    std::thread th3(print, std::ref(r), std::ref(cur), 2);
    
    th1.join();
    th2.join();
    th3.join();
    
    return 0;
}
