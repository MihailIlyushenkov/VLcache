#include "stdio.h"

struct MYPage
{
    int data[10];
};

MYPage SlowGetPage(int Key);
void printpage(MYPage* p);
int checkpage(MYPage p, int val);
int* readtestsdata(const char* filename, size_t* testcount);
int testfile(const char* filename);