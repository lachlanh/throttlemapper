#include <unity.h>
#include <PedalAssist.h>

void test_function_addition(void) {
    TEST_ASSERT_EQUAL(32, (25+7));
}

void test_calculate_cadence(void) {
    long edgeInterval = 100; //milliseconds
    float cadence = calculateCadence(edgeInterval, 12);
    TEST_ASSERT_EQUAL_FLOAT(50.0, cadence);
    cadence = calculateCadence(0.0, 12);
    TEST_ASSERT_EQUAL_FLOAT(0.0, cadence); 
    cadence = calculateCadence(50.0, 12);
    TEST_ASSERT_EQUAL_FLOAT(100.0, cadence);
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_function_addition);
    RUN_TEST(test_calculate_cadence);
    UNITY_END();
}

int main(int argc, char **argv) {
    process();
    return 0;
}
