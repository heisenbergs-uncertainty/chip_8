#include "unity.h"
#include "chip8.h"

void setUp(void) {}
void tearDown(void) {}

// Example test - add your tests here
void test_placeholder(void)
{
    TEST_PASS();
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_placeholder);
    return UNITY_END();
}
