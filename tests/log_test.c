#include "../logger/logger.h"

int main(void)
{
    LogFile* l = LogOpen("Log.txt");
    Log(l,"Test");

    Logf(l,"This should be 100 : %d",100);
    LogClose(l);
    return 0;
}