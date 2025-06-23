/**
 * @file test_temp_task.c
 * @brief Unit testing the temperature related parameters of rtdb.
 * 
 */

#include "unity.h"
#include "rtdb.h"


/**
 * @brief Test that updates and receives rtdb.current_temp. 
 * 
 * This test checks that setting the temperature works as intended. 
 * That the temperature value in the RTDB can be set from an extral source, 
 * for example a temperature sensor. 
 * 
 * It uses Mutex locks to simulate real world application access patterns. 
 */
void test_set_temp_rtdb(void) {

    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.current_temp = 25;
    k_mutex_unlock(&rtdb_mutex);

    TEST_ASSERT_EQUAL_INT(25, rtdb.current_temp);

    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.current_temp = -7;
    k_mutex_unlock(&rtdb_mutex);

    TEST_ASSERT_EQUAL_INT(-7, rtdb.current_temp);
}

/**
 * @brief Unity setup function. Runs before each test.
 */
void setUp(void) {
    rtdb.current_temp = 0;
}

/**
 * @brief Unity teardown function. Runs after each test.
 */
void tearDown(void) {
}



/**
 * @brief Unity test runner.
 *
 * Registers and executes the given test case.
 */
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_set_temp_rtdb);

    return UNITY_END();
}
 