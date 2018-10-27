#include<iostream>
#include <thread>
#include <unistd.h>
#include <functional>
class Test
{
    public:
        Test(int m):n(m)//,testThread_(std::bind(&Test::print,this))
        {
            std::thread t1(std::bind(&Test::print,this));
            t1.join();
        }
        void print(void)
        {
            std::cout<<n<<std::endl;
        }
    private:
        int n;
        std::thread testThread_;
};

int main(int argc,char **argv)
{
  Test test(8);
  sleep(1000); 
  return 0;
}

