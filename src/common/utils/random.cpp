#include <random>
#include <iostream>

unsigned int generate_random_integer(unsigned int begin=1, unsigned int end=65535)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<unsigned int> dist(begin, end);
    return dist(mt);
}

#if defined(__UNITTEST__)
int main()
{
    for (int i=0; i<16; ++i) {
        std::cout << generate_random_integer() << std::endl;
    }
    return 0;
}
#endif
