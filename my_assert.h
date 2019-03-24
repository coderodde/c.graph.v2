#ifndef MY_ASSERT_H
#define MY_ASSERT_H

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(CONDITION) assert(CONDITION, #CONDITION, __FILE__, __LINE__)
#define PRINT_TEST_RESULTS print_test_results(total_assertions, total_assertion_errors)

extern size_t total_assertions;
extern size_t total_assertion_errors;

int assert(int cond, char* err_msg, char* file_name, int line)
{
    if (!cond)
    {
        total_assertion_errors++;

        fprintf(stderr,
            "'%s' is not true in file '%s' at line %d.\n",
            err_msg,
            file_name,
            line);
    }

    total_assertions++;
    return cond;
}

void print_test_results()
{
    if (total_assertions == 0)
    {
        puts("Nothing tested.");
    }
    else
    {
        printf(
            "Assertions: %zu, errors: %zu, pass percentage: %f.\n",
            total_assertions,
            total_assertion_errors,
            100.0f - (100.0f * total_assertion_errors) / total_assertions);

        if (total_assertion_errors == 0)
        {
            puts("TESTS PASSED!");
        }
        else
        {
            puts("Some tests failed.");
        }
    }
}

#endif /* MY_ASSERT_H */