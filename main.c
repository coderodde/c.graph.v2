#include "my_assert.h"
#include "queue.h"
#include "unordered_map.h"
#include "unordered_set.h"
#include <stdio.h>
#define _C_GRAPH_V2_TEST

#define FALSE 0

static void test_all()
{
    puts("--- Testing ---");
    queue_test();
    list_test();
    unordered_map_test();
    unordered_set_test();
    PRINT_TEST_RESULTS;
    puts("--- Done testing ---");
}

int main(int argc, char* argv[])
{
#ifdef _C_GRAPH_V2_TEST
    test_all();
#elif defined YEAH
    puts("yeah");
#endif

    return 0;
}