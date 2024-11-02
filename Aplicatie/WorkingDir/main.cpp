#include "Server.h"

#define NUM_THREADS 10  
#define PORT 8080

//milea tesh branch

int main(int argc, char* argv[])
{

    Server sv(PORT,NUM_THREADS);
    sv.start();

    return 0;
}