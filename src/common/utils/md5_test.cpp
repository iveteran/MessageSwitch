#if defined(__UNITTEST__)

#include <iostream>
#include "md5.h"
 
using std::cout; using std::endl;
 
int main(int argc, char *argv[])
{
    const char* content = "Terminal Root";
    cout << "md5 of '" << content << "': " << md5(content) << endl;
    return 0;
}

#endif
// Refer: https://www.cnblogs.com/nanfei/p/16967340.html
