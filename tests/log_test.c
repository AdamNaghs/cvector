#include "../logger/logger.h"
#include "../mem_debug.h"


int main(void)
{
    LogFile* l = LogOpen("Log.txt");
    Log(l,"Test");

    Logf(l,"This should be 100 : %d",100);
    LogClose(l);

    MEM_DEBUG_END(stderr);
    return 0;
}