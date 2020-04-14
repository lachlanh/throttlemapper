#include <unity.h>

void test_function_addition(void) {
    TEST_ASSERT_EQUAL(32, (25+8));
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_function_addition);
    UNITY_END();
}

int main(int argc, char **argv) {
    process();
    return 0;
}
