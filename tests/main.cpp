#include <cstdlib>
#include "AutoTest.h"


int main(int argc, char *argv[])
{
    int ret = AutoTest::run(argc, argv);

#ifdef WIN32
    system("PAUSE");
#endif

    return ret;
}
